/* ****************************************************************************
 * RectTracker.cpp                                                            *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
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
#include "RectTracker.h"

#ifdef RT_GTK_HACK
#include "RatioImageBox.h"
#endif

IMPLEMENT_CLASS(wxRectTracker, wxWindow)

wxRectTracker::wxRectTracker(
			wxWindow * parent, 
			wxWindowID id, 
			const wxPoint & pos, 
			const wxSize& size, 
			long style) 
			: wxWindow(parent, id, pos, size, style)
{
	m_iHandlerWidth = 5;
	cursorMove = new wxCursor(wxCURSOR_SIZING);
	state = 0;
	ratio = 0;
	orientation = 0;
	maxRect = wxRect(0, 0, -1, -1); 
}

wxRectTracker::~wxRectTracker()
{
	if (cursorMove) delete cursorMove;
}


BEGIN_EVENT_TABLE(wxRectTracker, wxWindow)
   EVT_ERASE_BACKGROUND(wxRectTracker::OnEraseBackground)
   EVT_PAINT(wxRectTracker::OnPaint)
   EVT_MOTION(wxRectTracker::OnMouseMotion)
   EVT_LEFT_DOWN(wxRectTracker::OnMouseLeftDown)
   EVT_LEFT_UP(wxRectTracker::OnMouseLeftUp)
   EVT_RIGHT_DOWN(wxRectTracker::ForwardMouseEventToParent)
   EVT_RIGHT_UP(wxRectTracker::ForwardMouseEventToParent)
   EVT_CHAR(wxRectTracker::OnKey)
END_EVENT_TABLE()

DEFINE_EVENT_TYPE(EVT_TRACKER_CHANGED)
DEFINE_EVENT_TYPE(EVT_TRACKER_CHANGING)

void wxRectTracker::OnEraseBackground(wxEraseEvent & event)
{
    // TODO : Mettre le hack GTK ici ? 
    // et plutot dériver ? 
    // ou créer un évènement envoyé au GetParent() ? (contenant un DC)
}

void wxRectTracker::OnPaint(wxPaintEvent & event)
{
	int w, h ;
    wxPaintDC dc(this);
#ifdef RT_GTK_HACK
    // TODO : Section à déplacer dans OnEraseBackground ?
    // Begin of GTK Hack
    // redraw parent's window
    if (wxDynamicCast(GetParent(), wxRatioImageBox) != NULL)
    {
        int x, y;
        x = y = 0;
        GetParent()->ClientToScreen(&x, &y);
        ScreenToClient(&x, &y);
        dc.SetDeviceOrigin(x, y);
        wxDynamicCast(GetParent(), wxRatioImageBox)->PaintDC(dc);
        dc.SetDeviceOrigin(0, 0);
    }
    // End of GTK Hack
#endif
    GetClientSize(&w, &h);
	DrawRect(dc, 0, 0, w, h);
}

// DrawRect operates with a wxPaintDC => Window coordinates
void wxRectTracker::DrawRect(wxDC & dc, int x, int y, int w, int h)
{
	// Rect
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(x,y,w,h);
	dc.SetPen(wxPen(*wxWHITE,1,wxDOT));
	dc.DrawRectangle(x,y,w,h);

	// Handlers
	int z = m_iHandlerWidth;
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(x+1    , y+1    , z, z);
	dc.DrawRectangle(x-1+w-z, y+1    , z, z);
	dc.DrawRectangle(x+1    , y-1+h-z, z, z);
	dc.DrawRectangle(x-1+w-z, y-1+h-z, z, z);
	
	dc.DrawRectangle(x+(w-z)/2, y+1    , z, z);
	dc.DrawRectangle(x+(w-z)/2, y+h-z-1, z, z);
	dc.DrawRectangle(x+1    , y+(h-z)/2, z, z);
	dc.DrawRectangle(x+w-z-1, y+(h-z)/2, z, z);
}

void wxRectTracker::DrawRect(wxDC & dc, wxRect rect)
{
	DrawRect(dc, rect.x, rect.y, rect.width, rect.height);
}

// DrawRect operates with the parent's wxWindowDC => Parent coordinates
void wxRectTracker::DrawTracker(wxDC & dc, int x, int y, int w, int h)
{
	// Convert coordinates if scrolled
	if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
	{
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
	}
	// Inverted Rect
	dc.SetLogicalFunction(wxINVERT);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRectangle(x,y,w,h);
	dc.SetLogicalFunction(wxCOPY);
}

void wxRectTracker::DrawTracker(wxDC & dc, wxRect rect)
{
	DrawTracker(dc, rect.x, rect.y, rect.width, rect.height);
}


void wxRectTracker::OnKey(wxKeyEvent & event)
{
	int incr = 0, dx = 0, dy = 0;
	int handler;
	wxRect tmpRect;
	tmpRect = GetUnscrolledRect();
	
	if (event.ControlDown()) incr = 10; else incr = 1;

	switch(event.GetKeyCode())
	{
	case WXK_LEFT	: dx -= incr; break;
	case WXK_UP		: dy -= incr; break;
	case WXK_RIGHT	: dx += incr; break;
	case WXK_DOWN	: dy += incr; break;
	default:
		event.Skip();
	}

	if ((dx != 0) || (dy != 0))
	{
		if (event.ShiftDown())
		{
			tmpRect.width += dx;
			tmpRect.height += dy;
			handler = (dx == 0)?RT_HANDLER_BOTTOM_MID:RT_HANDLER_MID_RIGHT;
		}
		else
		{
			tmpRect.x += dx;
			tmpRect.y += dy;
			handler = 0;
		}
		AdjustTrackerRect(tmpRect, handler);
		SetUnscrolledRect(tmpRect);
		Update();
	}

}

void wxRectTracker::ForwardMouseEventToParent(wxMouseEvent & event)
{
	// Forward simple Mouse Motion to parent window
	if (GetParent())
	{
		ClientToScreen(&event.m_x, &event.m_y);
		GetParent()->ScreenToClient(&event.m_x, &event.m_y);
		GetParent()->AddPendingEvent(event);
	}
}

void wxRectTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;
	
	wxMouseEvent evt(event);
	ForwardMouseEventToParent(evt);

	// Just moving ?
	if (!event.Dragging())
	{
		hit = HitTest(event.m_x, event.m_y);
		switch (hit)
		{
		case RT_HANDLER_TOP_MID:
		case RT_HANDLER_BOTTOM_MID:
			SetCursor(wxCursor(wxCURSOR_SIZENS));
			break;
		case RT_HANDLER_MID_LEFT:
		case RT_HANDLER_MID_RIGHT:
			SetCursor(wxCursor(wxCURSOR_SIZEWE));
			break;
		case RT_HANDLER_TOP_LEFT:
		case RT_HANDLER_BOTTOM_RIGHT:
			SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			break;
		case RT_HANDLER_TOP_RIGHT:
		case RT_HANDLER_BOTTOM_LEFT:
			SetCursor(wxCursor(wxCURSOR_SIZENESW));
			break;
		default:
			SetCursor(*cursorMove);
		}
	}
	else if (event.LeftIsDown())
	{
		// Dragging

		wxASSERT(GetParent()!=NULL);

		// Drawing Tracker Rect
		//wxClientDC dc(GetParent());
        wxScreenDC dc;
        int cx, cy, cw, ch;
        GetParent()->GetClientSize(&cw, &ch);
        cx = cy = 0;
        GetParent()->ClientToScreen(&cx, &cy);
        dc.SetDeviceOrigin(cx, cy);
        dc.SetClippingRegion(0, 0, cw, ch);
        dc.StartDrawingOnTop(GetParent());

		if ((state & RT_STATE_DRAGGING) == 0)
		{
			state |= RT_STATE_DRAGGING;
		}
		else
		{
			// Erase previous Tracker
			DrawTracker(dc, prevRect);
		}

		// Update the new position
		// - Which Tracker ?
		hit = HitTest(leftClick.x, leftClick.y);
		// - Default Rect values
		curRect = GetUnscrolledRect();
		
		dx = (event.m_x - leftClick.x);
		dy = (event.m_y - leftClick.y);

		if ( (hit & RT_HANDLER_MID_RIGHT) != 0)
		{
			curRect.width += dx;
		}
		if ( (hit & RT_HANDLER_BOTTOM_MID) != 0)
		{
			curRect.height += dy;
		}
		if ( (hit & RT_HANDLER_MID_LEFT) != 0)
		{
			curRect.x += dx;
			curRect.width -= dx;
		}
		if ( (hit & RT_HANDLER_TOP_MID) != 0)
		{
			curRect.y += dy;
			curRect.height -= dy;
		}
		if (hit == RT_HANDLER_NONE)
		{
			curRect.x += dx;
			curRect.y += dy;
		}

		// Correct Orientation (size and virtual handler)
        if (curRect.width < 0) 
        { 
            curRect.width = - curRect.width; 
            curRect.x -= curRect.width; 
            if ((hit & RT_HANDLER_MID_LEFT) != 0) hit = (hit ^ RT_HANDLER_MID_LEFT) | RT_HANDLER_MID_RIGHT;
            else if ((hit & RT_HANDLER_MID_RIGHT) != 0) hit = (hit ^ RT_HANDLER_MID_RIGHT) | RT_HANDLER_MID_LEFT;
        }
		if (curRect.height < 0) 
        { 
            curRect.height = - curRect.height; 
            curRect.y -= curRect.height; 
            if ((hit & RT_HANDLER_TOP_MID) != 0) hit = (hit ^ RT_HANDLER_TOP_MID) | RT_HANDLER_BOTTOM_MID;
            else if ((hit & RT_HANDLER_BOTTOM_MID) != 0) hit = (hit ^ RT_HANDLER_BOTTOM_MID) | RT_HANDLER_TOP_MID;
        }

		// Adjust current Tracker size
		AdjustTrackerRect(curRect, hit);

		// Draw current Tracker
		DrawTracker(dc, curRect);

		// Update Parent's Status Bar
		prevRect = curRect;
		wxCommandEvent evt(EVT_TRACKER_CHANGING, this->GetId());
		GetParent()->GetEventHandler()->ProcessEvent(evt);

        dc.EndDrawingOnTop();
	}
	
	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		ReleaseMouse();
		state ^= RT_STATE_MOUSE_CAPTURED;
	}

	// Update prev_move
	prevMove.x = event.m_x;
	prevMove.y = event.m_y;

}

void wxRectTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	leftClick.x = event.m_x;
	leftClick.y = event.m_y;
	if ((state & RT_STATE_MOUSE_CAPTURED) == 0)
	{
		CaptureMouse();
		state |= RT_STATE_MOUSE_CAPTURED;
	}
}

void wxRectTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		ReleaseMouse();
		state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((state & RT_STATE_DRAGGING) != 0)
	{
		SetUnscrolledRect(prevRect);
		state ^= RT_STATE_DRAGGING;
		Update();
	}
}


int wxRectTracker::HitTest(int x, int y)
{
	int w, h;
	int z = m_iHandlerWidth;

	GetClientSize(&w, &h);

	if ( (y < 0) || (y > h) || (x < 0) || (x > w) ) return RT_HANDLER_OUTSIDE;
	
	// Split vertically, then horizontally
	if ( (y <= h) && (y >= h-z) )
	{
		// Bottom line
		if ( (x >= w-z) && (x <= w) )
		{
			// Bottom Right
			return RT_HANDLER_BOTTOM_RIGHT;
		}
		else if ( (x >= (w-z)/2) && (x <= (w+z)/2) )
		{
			// Bottom Mid
			return RT_HANDLER_BOTTOM_MID;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Bottom left
			return RT_HANDLER_BOTTOM_LEFT;
		}
	}
	else if ( (y >= (h-z)/2 ) && (y <= (h+z)/2) )
	{
		// Mid line
		if ( (x >= w-z) && (x <= w) )
		{
			// Mid Right
			return RT_HANDLER_MID_RIGHT;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Mid left
			return RT_HANDLER_MID_LEFT;
		}
	}
	else if ( (y >= 0) && (y <= z) )
	{
		// Top line
		if ( (x >= w-z) && (x <= w) )
		{
			// Top Right
			return RT_HANDLER_TOP_RIGHT;
		}
		else if ( (x >= (w-z)/2) && (x <= (w+z)/2) )
		{
			// Top Mid
			return RT_HANDLER_TOP_MID;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Top left
			return RT_HANDLER_TOP_LEFT;
		}
	}
	return RT_HANDLER_NONE;
}

long wxRectTracker::CalcRectDist(wxRect r1, wxRect r2)
{
	int d;
	d = 0;
	d += (r1.x - r2.x) * (r1.x - r2.x);
	d += (r1.y - r2.y) * (r1.y - r2.y);
	d += (r1.width - r2.width) * (r1.width - r2.width);
	d += (r1.height - r2.height) * (r1.height - r2.height);
	return d;
}

wxRect wxRectTracker::CalcRectNearer(wxRect rr, wxRect r1, wxRect r2)
{
	if (CalcRectDist(rr, r2) < CalcRectDist(rr, r1))
	 		 return r2;
		else return r1;
}


void wxRectTracker::AdjustTrackerRectMax(wxRect &curRect, int handler)
{
	// Adjust maxRect 
	if ((maxRect.width < 0) || (maxRect.height < 0)) return;

	// - Left X
	if (curRect.x < maxRect.x)
	{
		if (handler != RT_HANDLER_NONE) curRect.width -= maxRect.x - curRect.x; 
		curRect.x = maxRect.x;
	}
	// - Right X
	if ((curRect.x + curRect.width) > (maxRect.x + maxRect.width))
	{
		if (handler != RT_HANDLER_NONE) curRect.width = maxRect.x + maxRect.width - curRect.x; 
		curRect.x = maxRect.x + maxRect.width - curRect.width;
	}
	// - Top Y
	if (curRect.y < maxRect.y)
	{
		if (handler != RT_HANDLER_NONE) curRect.height -= maxRect.y - curRect.y; 
		curRect.y = maxRect.y;
	}
	// - Bottom Y
	if ((curRect.y + curRect.height) > (maxRect.y + maxRect.height))
	{
		if (handler != RT_HANDLER_NONE) curRect.height = maxRect.y + maxRect.height - curRect.y; 
		curRect.y = maxRect.y + maxRect.height - curRect.height;
	}

}

void wxRectTracker::AdjustTrackerRectRatio(wxRect &curRect, int handler, bool expand)
{
	wxRect rectPort, rectLand, rectWidth, rectHeight, rectTmp;

	if (ratio <= 0) return;

	// Calc the best two rects
	// - rectWidth
	rectPort = rectLand = curRect;
	rectPort.width = (int) (rectPort.height * ratio);
	rectLand.width = (int) (rectLand.height / ratio);
	if ((handler & RT_HANDLER_MID_LEFT) != 0)
	{
		rectPort.x += curRect.width - rectPort.width;
		rectLand.x += curRect.width - rectLand.width;
	}
	//   * Portrait or Landscape ?
	switch(orientation)
	{
	case -1:	rectTmp = rectLand; break;
	case 1:		rectTmp = rectPort; break;
	default:	rectTmp = CalcRectNearer(curRect, rectPort, rectLand); break;
	}
	rectWidth = rectTmp;
	// - rectHeight (ugly cut'n paste)
	rectPort = rectLand = curRect;
	rectPort.height = (int) (rectPort.width / ratio);
	rectLand.height = (int) (rectLand.width * ratio);
	if ((handler & RT_HANDLER_TOP_MID) != 0)
	{
		rectPort.y += curRect.height - rectPort.height;
		rectLand.y += curRect.height - rectLand.height;
	}
	//   * Portrait or Landscape ?
	switch(orientation)
	{
	case -1:	rectTmp = rectLand; break;
	case 1:		rectTmp = rectPort; break;
	default:	rectTmp = CalcRectNearer(curRect, rectPort, rectLand); break;
	}
	rectHeight = rectTmp;

	switch(handler)
	{
	case RT_HANDLER_TOP_MID:
	case RT_HANDLER_BOTTOM_MID:
		curRect = (expand)?rectWidth:rectHeight;
		break;
	case RT_HANDLER_MID_RIGHT:
	case RT_HANDLER_MID_LEFT:
		curRect = (expand)?rectHeight:rectWidth;
		break;
	default:
		if (!expand)
		{
			if (curRect.height < rectHeight.height) rectHeight.height = -12000;
			if (curRect.width < rectWidth.width) rectWidth.width = -12000;
		}
		curRect = CalcRectNearer(curRect, rectHeight, rectWidth);
	}

}


void wxRectTracker::AdjustTrackerRect(wxRect &curRect, int handler)
{
	AdjustTrackerRectMax(curRect, handler);
	if (ratio > 0)
	{
		// First Pass
		AdjustTrackerRectRatio(curRect, handler, true);
		// Second Pass - Adjust if new rect has been out of the max rect.
		AdjustTrackerRectMax(curRect, handler);
		AdjustTrackerRectRatio(curRect, handler, false);
	}
}


void wxRectTracker::SetRatio(double ratio)
{
	if ((ratio > 0) && (ratio < 1)) ratio = 1 / ratio;
	this->ratio = ratio;
}

void wxRectTracker::SetMaxRect(wxRect maxRect)
{
	this->maxRect = maxRect;
	Update();
}

void wxRectTracker::SetOrientation(int orientation)
{
	this->orientation = orientation;
}

void wxRectTracker::Update()
{
	curRect = GetUnscrolledRect();
	AdjustTrackerRect(curRect, -1);
	if (curRect != GetUnscrolledRect())
	{
		SetUnscrolledRect(curRect);
	}
	GetParent()->Refresh();
    wxCommandEvent evt(EVT_TRACKER_CHANGED, this->GetId());
    GetParent()->GetEventHandler()->ProcessEvent(evt);
}

wxRect wxRectTracker::GetUnscrolledRect()
{
	wxRect rect;
	rect = GetRect();
	// Convert coordinates if scrolled
	if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
	{
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcUnscrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	return rect;
}

void wxRectTracker::SetUnscrolledRect(wxRect rect)
{
	// Convert coordinates if scrolled
	if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
	{
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	SetSize(rect);
}
