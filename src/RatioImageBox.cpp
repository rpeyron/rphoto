/* ****************************************************************************
 * RatioImageBox.cpp                                                          *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * RPhoto : image tool for a quick and efficient use with digital photos      *
 * Copyright (C) 2004 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>               *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * http://www.gnu.org/copyleft/gpl.html                                       *
 * ************************************************************************** */

#include "RatioPrecomp.h"
#include "RatioImageBox.h"


wxRatioImageBox::wxRatioImageBox(
			wxWindow * parent, 
			wxWindowID id) 
			: wxImageBox(parent, id)
{
	rectTracker = new wxRectTracker(this, -1, wxPoint(0,0), wxSize(0,0) );
	captured = 0;
}

wxRatioImageBox::~wxRatioImageBox(void)
{
	if (rectTracker) delete rectTracker;
}

// wxImageBox does not implement RTTI
// IMPLEMENT_CLASS2(wxRatioImageBox, wxImageBox, wxRectTracker_Window)
//IMPLEMENT_CLASS2(wxRatioImageBox, wxWindow, wxRectTracker_Window)
IMPLEMENT_CLASS(wxRatioImageBox, wxWindow)

BEGIN_EVENT_TABLE(wxRatioImageBox, wxImageBox)
	EVT_MOTION(wxRatioImageBox::OnMouseMotion)
	EVT_LEFT_DOWN(wxRatioImageBox::OnMouseLeftDown)
	EVT_RIGHT_DOWN(wxRatioImageBox::OnMouseRightDown)
	EVT_RIGHT_UP(wxRatioImageBox::OnMouseRightUp)
	EVT_SIZE(wxRatioImageBox::OnResize)
	EVT_TRACKER_CHANGED(wxID_ANY, wxRatioImageBox::OnTrackerChanged)
	EVT_TRACKER_CHANGING(wxID_ANY, wxRatioImageBox::OnTrackerChanged)
END_EVENT_TABLE()

void wxRatioImageBox::OnMouseMotion(wxMouseEvent & event)
{
	int sx, sy, scr_x, scr_y;
	if (event.Dragging() && event.RightIsDown() )
	{
		GetScrollPixelsPerUnit(&scr_x, &scr_y);
		GetViewStart(&sx, &sy);
		if ( (scr_x > 0) && (scr_y > 0) )
			Scroll(
			 sx + (int)((event.m_x - firstClick.x) * GetScaleValue() / scr_x),
			 sy + (int)((event.m_y - firstClick.y) * GetScaleValue() / scr_y)
			);
		firstClick.x = event.m_x;
		firstClick.y = event.m_y;
	}
	CalcUnscrolledPosition(event.m_x, event.m_y, &event.m_x, &event.m_y);
	if ((GetScaleValue() > 0)  && (wxDynamicCast(GetParent(),wxFrame) != NULL))
		((wxFrame *)GetParent())->SetStatusText(
			wxString::Format("%d,%d", 
				(int)(event.m_x / GetScaleValue()), 
				(int)(event.m_y / GetScaleValue())) ,1);
}

void wxRatioImageBox::OnMouseLeftDown(wxMouseEvent & event)
{
	rectTracker->SetSize(wxRect(event.m_x, event.m_y, 0, 0));
	wxMouseEvent myEvent = event;
	myEvent.m_x = 0;
	myEvent.m_y = 0;
	rectTracker->AddPendingEvent(myEvent);
}

void wxRatioImageBox::OnMouseRightDown(wxMouseEvent & event)
{
	firstClick.x = event.m_x;
	firstClick.y = event.m_y;
	// ((wxFrame *)GetParent())->SetStatusText(wxString::Format(" First %d, %d", firstClick.x, firstClick.y) ,0);
	// Do not capture if in tracking process
	if (!event.LeftIsDown())
	{
		CaptureMouse();
		captured = 1;
	}
}

void wxRatioImageBox::OnMouseRightUp(wxMouseEvent & event)
{
	if (captured == 1) ReleaseMouse();
	captured = 0;
	Refresh();
}

void wxRatioImageBox::OnResize(wxSizeEvent & event)
{
	double prevScale, curScale;
	
	prevScale = GetScaleValue();
	wxImageBox::OnResize(event);
	if (rectTracker) 
		rectTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)(GetImage().GetWidth()*GetScaleValue()), 
				   (int)(GetImage().GetHeight()*GetScaleValue())
				   ));
	curScale = GetScaleValue();
	if ((curScale > 0 ) && (prevScale > 0) && (prevScale != curScale))
	{
		wxRect rect;
		rect = rectTracker->GetUnscrolledRect();
		rect.x = (int) (rect.x * (curScale/prevScale));
		rect.y = (int)(rect.y * (curScale/prevScale));
		rect.width = (int)(rect.width * (curScale/prevScale));
		rect.height = (int)(rect.height * (curScale/prevScale));
		rectTracker->SetUnscrolledRect(rect);
		rectTracker->Update();
	}
}

void wxRatioImageBox::OnTrackerChanged(wxCommandEvent &event)
{
	wxRect myRect;
	if (event.GetEventType() != EVT_TRACKER_CHANGING)  
			 myRect = GetRectTracker().GetUnscrolledRect();
		else 
		{
			myRect = GetRectTracker().GetTrackerRect();
		}
	if ((GetScaleValue() > 0)  && (wxDynamicCast(GetParent(),wxFrame) != NULL))
		((wxFrame *)GetParent())->GetStatusBar()->SetStatusText(
		wxString::Format("%dx%d+%d,%d",
			(int)(myRect.width / GetScaleValue()),
			(int)(myRect.height / GetScaleValue()),
			(int)(myRect.x / GetScaleValue()),
			(int)(myRect.y / GetScaleValue()))
			,2);
}

void wxRatioImageBox::SetScale(double scale)
{
	double prevScale, newScale, ratio;
	wxRect rect;
	ratio = 0;
	prevScale = GetScaleValue();
	if (rectTracker != NULL) rect = rectTracker->GetUnscrolledRect();
	wxImageBox::SetScale(scale);
	newScale = GetScaleValue();
	SetVirtualSize((int)(GetImage().GetWidth()*newScale), (int)(GetImage().GetHeight()*newScale));
	if (prevScale > 0) ratio = newScale / prevScale;
	if ((rectTracker != NULL) && (ratio > 0))
	{
		// * Update Max Rect
		rectTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)(GetImage().GetWidth()*GetScaleValue()), 
				   (int)(GetImage().GetHeight()*GetScaleValue())
				   ));
		rect.x = (int)(rect.x * ratio);
		rect.y = (int)(rect.y * ratio);
		rect.width = (int)(rect.width * ratio);
		rect.height = (int)(rect.height * ratio);
		rectTracker->SetUnscrolledRect(rect);
		rectTracker->Update();
		// * Scroll to the tracker
		int sx, sy;
		GetScrollPixelsPerUnit(&sx, &sy);
		Scroll(
			(int)((rect.x-((GetSize().GetWidth() - rect.width)/2)) / sx) ,
			(int)((rect.y-((GetSize().GetHeight() - rect.height)/2)) / sy)
			);
	}
}

void wxRatioImageBox::UpdatePaintBox()
{
	if (m_scaleV == scFIT_TO_PAGE_IF_LARGER)
	{
		m_scaleV = scFIT_TO_PAGE;
		wxImageBox::UpdatePaintBox();
		if (m_scale > 1)
		{
			m_scale = 1;
			Refresh();
		}
		m_scaleV = scFIT_TO_PAGE_IF_LARGER;
	}
	else wxImageBox::UpdatePaintBox();
}

wxRect wxRatioImageBox::GetSelectedZone()
{
    wxRect rect(0,0,0,0);
    double ratio = GetScaleValue();
    if (ratio < 0) ratio = - ratio;
    if (rectTracker != NULL)
    {
        rect = rectTracker->GetUnscrolledRect();
        rect.x = (int)(rect.x / ratio);
        rect.y = (int)(rect.y / ratio);
        rect.width = (int)(rect.width / ratio);
        rect.height = (int)(rect.height / ratio);
    }
    return rect;
}

bool wxRatioImageBox::LoadFile(wxString fname)
{
	bool rc;
	rc = wxImageBox::LoadFile(fname);
    UpdateTracker();
	return rc;
}

void wxRatioImageBox::SetImage(const wxImage & image)
{
    m_image = image;
    Update();
    UpdateTracker();
    /*
    wxString name;
    wxBeginBusyCursor();
    name = wxFileName::CreateTempFileName("mem");
    image.SaveFile(name, wxBITMAP_TYPE_BMP);
    LoadFile(name);
    wxRemoveFile(name);
    wxEndBusyCursor();
    */
}

void wxRatioImageBox::UpdateTracker()
{
	if (rectTracker) 
		rectTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)(GetImage().GetWidth()*GetScaleValue()), 
				   (int)(GetImage().GetHeight()*GetScaleValue())
				   ));
}