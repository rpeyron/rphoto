/* ****************************************************************************
 * RatioImageBox.cpp                                                          *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * RPhoto : image tool for a quick and efficient use with digital photos      *
 * Copyright (C) 2004 - Rmi Peyronnet <remi+rphoto@via.ecp.fr>               *
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

#include "RatioFrame.h"

#include <math.h>

// wxImageBox does not implement RTTI
//IMPLEMENT_CLASS2(wxRatioImageBox, wxWindow, wxRectTrackerHost)
//IMPLEMENT_CLASS(wxRatioImageBox, wxWindow)
IMPLEMENT_CLASS2(wxRatioImageBox, wxScrolledWindow, wxRectTrackerHost)

wxRatioImageBox::wxRatioImageBox(
			wxWindow * parent, 
			wxWindowID id) 
			: wxImageBox(parent, id), wxRectTrackerHost()
{
	m_image = wxImage(2,2);
	m_buffer = new wxBitmap(m_image);
	rectTracker = new wxRectTrackerRatio(this, -1, wxPoint(0,0), wxSize(0,0), 0, this);
	lineTracker = new wxLineTracker(this, -1, wxPoint(0,0), wxPoint(0,0), 0, this);
	captured = 0;
	m_state = RIB_STATE_CROP;
	rectTracker->Enable();
	lineTracker->Disable();
}

wxRatioImageBox::~wxRatioImageBox(void)
{
	if (rectTracker) delete rectTracker;
	if (lineTracker) delete lineTracker;
}

BEGIN_EVENT_TABLE(wxRatioImageBox, wxImageBox)
	EVT_MOTION(wxRatioImageBox::OnMouseMotion)
	EVT_LEFT_DOWN(wxRatioImageBox::OnMouseLeftDown)
	EVT_LEFT_UP(wxRatioImageBox::OnMouseLeftUp)
	EVT_RIGHT_DOWN(wxRatioImageBox::OnMouseRightDown)
	EVT_RIGHT_UP(wxRatioImageBox::OnMouseRightUp)
	EVT_SIZE(wxRatioImageBox::OnResize)
	EVT_TRACKER_CHANGED(wxID_ANY, wxRatioImageBox::OnTrackerChanged)
	EVT_TRACKER_CHANGING(wxID_ANY, wxRatioImageBox::OnTrackerChanged)
END_EVENT_TABLE()

void wxRatioImageBox::OnMouseMotion(wxMouseEvent & event)
{
	int sx, sy, scr_x, scr_y;
	/*
	if (event.Dragging() && event.LeftIsDown() && ((m_state & RIB_STATE_INCLINAISON) != 0) )
	{
        wxScreenDC dc;
        int cx, cy, cw, ch;
        GetClientSize(&cw, &ch);
        cx = cy = 0;
        ClientToScreen(&cx, &cy);
        dc.SetDeviceOrigin(cx, cy);
        dc.SetClippingRegion(0, 0, cw, ch);
        dc.StartDrawingOnTop(GetParent());
    	dc.SetLogicalFunction(wxINVERT);
    	dc.SetBrush(*wxTRANSPARENT_BRUSH);
    	dc.SetPen(*wxGREY_PEN);
    	dc.DrawLine(firstClick.x, firstClick.y, lastPoint.x, lastPoint.y);
    	dc.DrawLine(firstClick.x, firstClick.y, event.m_x, event.m_y);
		lastPoint.x = event.m_x; lastPoint.y = event.m_y;
        dc.EndDrawingOnTop();
	}    
	*/
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
			wxString::Format(wxT("%d,%d"), 
				(int)(event.m_x / GetScaleValue()), 
				(int)(event.m_y / GetScaleValue())) ,1);
}

void wxRatioImageBox::OnMouseLeftDown(wxMouseEvent & event)
{
	/*
    switch (m_state)
    {
        case RIB_STATE_INCLINAISON:
            firstClick.x = event.m_x;
			firstClick.y = event.m_y;
			lastPoint = firstClick;
            break;
        case RIB_STATE_CROP:
        default:
            // Set up the Tracker
			*/
    wxMouseEvent myEvent = event;
    myEvent.m_x = 0;
    myEvent.m_y = 0;
	if (rectTracker && rectTracker->IsEnabled())
	{
		rectTracker->SetSize(wxRect(event.m_x, event.m_y, 0, 0));
		rectTracker->AddPendingEvent(myEvent);
	}
	if (lineTracker && lineTracker->IsEnabled())
	{
		lineTracker->SetTrackerPosition(wxPoint(event.m_x, event.m_y), wxPoint(event.m_x, event.m_y));
		lineTracker->AddPendingEvent(myEvent);
	}
	/*
    break;
	}
	*/
	SetFocus();
}

#define PI 3.141592653589793

void wxRatioImageBox::OnMouseLeftUp(wxMouseEvent & event)
{
	/*
	int dx, dy;
	double angle, degree;
	wxScreenDC dc;
	int cx, cy, cw, ch;
    switch (m_state)
    {
        case RIB_STATE_INCLINAISON:
			// Clean up
			GetClientSize(&cw, &ch);
			cx = cy = 0;
			ClientToScreen(&cx, &cy);
			dc.SetDeviceOrigin(cx, cy);
			dc.SetClippingRegion(0, 0, cw, ch);
			dc.StartDrawingOnTop(GetParent());
    		dc.SetLogicalFunction(wxINVERT);
    		dc.SetBrush(*wxTRANSPARENT_BRUSH);
    		dc.SetPen(*wxGREY_PEN);
    		dc.DrawLine(firstClick.x, firstClick.y, lastPoint.x, lastPoint.y);
			lastPoint.x = event.m_x; lastPoint.y = event.m_y;
			dc.EndDrawingOnTop();
			// Rotate
			dx = event.m_x - firstClick.x;
			dy = event.m_y - firstClick.y;
			if (dx == 0) angle = 0; 
			else angle = atan((double) dy / dx);
			if (angle > (PI / 4.0)) angle -= PI / 2.0;
			if (angle < (-PI / 4.0)) angle += PI / 2.0;
			degree = angle / PI * 180.0;
			if (angle != 0.0) 
			{
				wxBeginBusyCursor();
        		if ((wxDynamicCast(GetParent(),wxFrame) != NULL))
        			((wxFrame *)GetParent())->SetStatusText(wxString::Format(_("Rotating of %f degree. Please wait..."),degree),0);            
				SetImage(GetImage().Rotate(angle, wxPoint(0,0)));
        		if ((wxDynamicCast(GetParent(),wxFrame) != NULL))
        			((wxFrame *)GetParent())->SetStatusText(_("Done."),0);            
				wxEndBusyCursor();
        		if ((wxDynamicCast(GetParent(),RatioFrame) != NULL)) ((RatioFrame *)GetParent())->Dirty();
			}
            break;
        case RIB_STATE_CROP:
        default:
            // Nothing to do
        	break;
	}	
	*/
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

void wxRatioImageBox::AdjustTrackerSize(double ratio)
{
	wxRect rect;

	if (rectTracker != NULL)
	{
		// On remet en taille maximale pour que a passe tout le temps
		rectTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)(GetImage().GetWidth()), 
				   (int)(GetImage().GetHeight())
				   ));
		// On adapte  la nouvelle taille
		if (ratio != 0)
		{
			rect = rectTracker->GetUnscrolledRect();
			rect.x = (int) ((double)rect.x * ratio);
			rect.y = (int)((double)rect.y * ratio);
			rect.width = (int)((double)rect.width * ratio);
			rect.height = (int)((double)rect.height * ratio);
			rectTracker->SetUnscrolledRect(rect);
			rectTracker->Update();
		}
		// Et on restaure les nouvelles limites
		rectTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)(GetImage().GetWidth()*GetScaleValue()), 
				   (int)(GetImage().GetHeight()*GetScaleValue())
				   ));
	}


	if (lineTracker != NULL)
	{
		// Reset Max Rect
		lineTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)((double)GetImage().GetWidth()), 
				   (int)((double)GetImage().GetHeight())
				   ));
		if (ratio > 0)
		{
			// Update Position
			lineTracker->SetTrackerPosition(
					wxPoint((int)((double)lineTracker->GetPosBegin().x * ratio),(int)((double)lineTracker->GetPosBegin().y * ratio)),
					wxPoint((int)((double)lineTracker->GetPosEnd().x * ratio),(int)((double)lineTracker->GetPosEnd().y * ratio))
				);
		}
		// Update Max Rect
		lineTracker->SetMaxRect(
			wxRect(0,
				   0,
				   (int)((double)GetImage().GetWidth()*GetScaleValue()), 
				   (int)((double)GetImage().GetHeight()*GetScaleValue())
				   ));
		lineTracker->Update();
	}

}

void wxRatioImageBox::OnResize(wxSizeEvent & event)
{
	double prevScale, curScale;
	
	prevScale = GetScaleValue();
	wxImageBox::OnResize(event);
	curScale = GetScaleValue();
	if ((curScale > 0 ) && (prevScale > 0) && (prevScale != curScale))
	{
		AdjustTrackerSize((double)curScale / prevScale);
	}

}

void wxRatioImageBox::OnTrackerChanged(wxCommandEvent &event)
{
	wxRect myRect;
	wxPoint begin, end;
	wxString msg;
	double angle;

	if (!IsModeInclinaison())
	{
		if (event.GetEventType() != EVT_TRACKER_CHANGING)  
				  myRect = GetRectTracker().GetUnscrolledRect();
			else  myRect = GetRectTracker().GetTrackerRect();
		msg = wxString::Format(wxT("%dx%d+%d,%d"),
				(int)(myRect.width / GetScaleValue()),
				(int)(myRect.height / GetScaleValue()),
				(int)(myRect.x / GetScaleValue()),
				(int)(myRect.y / GetScaleValue()));
	}
	else
	{
		#define PI 3.141592653589793
		begin = GetLineTracker().GetPosBegin();
		end = GetLineTracker().GetPosEnd();
		if (begin.x == end.x) angle = 0;
		else angle = atan((double) (end.y - begin.y) / (end.x - begin.x));
		if (angle > (PI / 4.0)) angle -= PI / 2.0;
		if (angle < (-PI / 4.0)) angle += PI / 2.0;
		msg = wxString::Format(wxT("%.2f degres"), (angle / PI * 180.0));
	}
	if ((GetScaleValue() > 0)  
		 && (wxDynamicCast(GetParent()->GetParent(),wxFrame) != NULL)
		 && (((wxFrame *)GetParent()->GetParent())->GetStatusBar() != NULL) )
		((wxFrame *)GetParent()->GetParent())->GetStatusBar()->SetStatusText(msg,2);
}

void wxRatioImageBox::SetScale(double scale)
{
	double prevScale, newScale, ratio;
	int sx, sy;
	wxRect rect;

	ratio = 0;
	rect = wxPanel::GetRect();
	prevScale = GetScaleValue();
	wxImageBox::SetScale(scale);
	newScale = GetScaleValue();

	if (prevScale > 0)
	{
		ratio = newScale / prevScale;
		AdjustTrackerSize(ratio);
	}

	SetVirtualSize((int)((double)GetImage().GetWidth()*newScale), (int)((double)GetImage().GetHeight()*newScale));

	// Scroll to the tracker
	if (rectTracker != NULL)
	{
		GetScrollPixelsPerUnit(&sx, &sy);
		rect = rectTracker->GetUnscrolledRect();
		Scroll(
			(int)((rect.x-((GetSize().GetWidth()*newScale - rect.width)/2)) / sx) ,
			(int)((rect.y-((GetSize().GetHeight()*newScale - rect.height)/2)) / sy)
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
	((wxFrame *) ::wxGetApp().GetTopWindow())->SetTitle(wxFileName(fname).GetFullName() + wxT(" - ") + RPHOTO_NAME);
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
	SetMaxRect(wxRect(0, 0,
				   (int)(GetImage().GetWidth()*GetScaleValue()), 
				   (int)(GetImage().GetHeight()*GetScaleValue())
				   ));
}

void wxRatioImageBox::SetModeInclinaison(bool state)
{
    if (state)
    {
        m_state = RIB_STATE_INCLINAISON;
        if (rectTracker) rectTracker->Disable();
        if (lineTracker) lineTracker->Enable();
    }
    else
    {
        m_state = RIB_STATE_CROP;
        if (rectTracker) rectTracker->Enable();
        if (lineTracker) lineTracker->Disable();
    }        
}    

