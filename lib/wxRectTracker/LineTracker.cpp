/* ****************************************************************************
 * LineTracker.cpp                                                            *
 *                                                                            *
 * LineTracker was originally designed for RPhoto, but can be used separately *
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


#include "LineTracker.h"
#include <wx/dcbuffer.h>

/// Helpers

wxRect NormalizeRect(wxRect rect)
{
	if (rect.width < 0) { rect.x += rect.width ; rect.width = -rect.width; }
	if (rect.height < 0) { rect.y += rect.height ; rect.height = -rect.height; }
	return rect;
}

/// wxLineTracker

IMPLEMENT_CLASS(wxLineTracker, wxRectTracker)

wxLineTracker::wxLineTracker(
			wxWindow * parent, 
			wxWindowID id,  
			const wxPoint & begin, 
			const wxPoint & end, 
			long style,
            wxRectTrackerHost * rectTrackerHost) 
			: wxRectTracker(parent, id, begin, wxSize((end - begin).x, (end - begin).y), style, rectTrackerHost)
{
}

wxLineTracker::~wxLineTracker()
{
}


void wxLineTracker::OnPaint(wxPaintEvent & event)
{
//    wxPaintDC dc(this);
    wxAutoBufferedPaintDC dc(this);

	if ((m_state & RT_STATE_DRAGGING) != 0) return;

#ifdef RT_GTK_HACK
    if (m_pRectTrackerHost)
    {
		dc.SetDeviceOrigin(-GetPosition().x, -GetPosition().y);
        m_pRectTrackerHost->PaintDC(dc);
        dc.SetDeviceOrigin(0, 0);
    }    
#endif

	DrawLine(dc, GetPosBegin(), GetPosEnd());
}

// DrawLine operates with a wxPaintDC => Window coordinates
void wxLineTracker::DrawLine(wxDC & dc, wxPoint begin, wxPoint end)
{
	dc.SetDeviceOrigin(-GetPosition().x, -GetPosition().y);

	// Rect
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(begin.x, begin.y, end.x, end.y);
	dc.SetPen(wxPen(*wxWHITE,1,wxDOT));
	dc.DrawLine(begin.x, begin.y, end.x, end.y);

	// Handlers
  	int z = m_iHandlerWidth;
	if (m_iHandlerMask != RT_MASK_NONE)
	{
    	dc.SetBrush(*wxBLACK_BRUSH);
    	dc.SetPen(*wxBLACK_PEN);
    	
    	if (m_iHandlerMask & RT_LINE_MASK_BEGIN) dc.DrawRectangle(begin.x-(z/2), begin.y-(z/2), z, z);
    	if (m_iHandlerMask & RT_LINE_MASK_END  ) dc.DrawRectangle(end.x-(z/2), end.y-(z/2), z, z);
    }   	
    dc.SetDeviceOrigin(0, 0);
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxLineTracker::DrawTracker(wxDC & dc, wxPoint begin, wxPoint end)
{

	int x1, y1, x2, y2;
	x1 = begin.x; y1 = begin.y;
	x2 = end.x;   y2 = end.y;
	// Convert coordinates if scrolled
	if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
	{
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(x1, y1, &x1, &y1);
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(x2, y2, &x2, &y2);
	}

	// Inverted Line
	dc.SetLogicalFunction(wxINVERT);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawLine(begin.x,begin.y,end.x,end.y);
	dc.SetLogicalFunction(wxCOPY);
}


void wxLineTracker::OnKey(wxKeyEvent & event)
{
    
    if ((m_state & RT_STATE_DISABLED) != 0) return;
    
	/*
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
	*/

}

void wxLineTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;
	
	wxMouseEvent evt(event);
	ForwardMouseEventToParent(evt);

    if ((m_state & RT_STATE_DISABLED) != 0) return;

	// Just moving ?
	if (!event.Dragging())
	{
		hit = HitTest(event.m_x, event.m_y);
		switch (hit)
		{
		case RT_LINE_HANDLER_END:
		case RT_LINE_HANDLER_BEGIN:
		case RT_LINE_HANDLER_ON_LINE:
			SetCursor(*m_cursorMove);
			break;
		default:
			SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	}
	else if (event.LeftIsDown())
	{
		// Dragging

		wxASSERT(GetParent()!=NULL);

		// Drawing Tracker Rect
		wxClientDC dc(GetParent());
        //wxScreenDC dc;
        int cx, cy, cw, ch;
        GetParent()->GetClientSize(&cw, &ch);
        cx = cy = 0;
        //GetParent()->ClientToScreen(&cx, &cy);
        dc.SetDeviceOrigin(cx, cy);
        dc.SetClippingRegion(0, 0, cw, ch);
        //dc.StartDrawingOnTop(GetParent());

		if ((m_state & RT_STATE_DRAGGING) == 0)
		{
			m_state |= RT_STATE_DRAGGING;
			m_pCurBegin = GetPosBegin();
			m_pCurEnd = GetPosEnd();
		}
		else
		{
			// Erase previous Tracker
			DrawTracker(dc, m_pCurBegin, m_pCurEnd);
		}

		// Update the new position
		// - Which Tracker ?
		hit = HitTest(m_leftClick.x, m_leftClick.y);
		// - Default Rect values
		
		dx = (event.m_x - m_leftClick.x);
		dy = (event.m_y - m_leftClick.y);

		if ( (hit & RT_LINE_HANDLER_BEGIN) != 0)
		{
			m_pCurBegin.x = GetPosBegin().x + dx;
			m_pCurBegin.y = GetPosBegin().y + dy;
		}
		if ( (hit & RT_LINE_HANDLER_END) != 0)
		{
			m_pCurEnd.x = GetPosEnd().x + dx;
			m_pCurEnd.y = GetPosEnd().y + dy;
		}

		// Adjust current Tracker size
		//AdjustTrackerLine(m_pCurBegin, m_pCurEnd, hit);

		// Draw current Tracker
		DrawTracker(dc, m_pCurBegin, m_pCurEnd);

		// Update Parent's Status Bar
		wxCommandEvent evt(EVT_TRACKER_CHANGING, this->GetId());
		GetParent()->GetEventHandler()->ProcessEvent(evt);

        //dc.EndDrawingOnTop();
	}
	
	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((m_state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}

	// Update prev_move
	m_prevMove.x = event.m_x;
	m_prevMove.y = event.m_y;

}


void wxLineTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	if (HitTest(event.m_x, event.m_y) == RT_LINE_HANDLER_NONE)
	{
		SetTrackerPosition(wxPoint(event.m_x + GetPosition().x, event.m_y + GetPosition().y), 
						   wxPoint(event.m_x + GetPosition().x, event.m_y + GetPosition().y));
		event.m_x = 0;
		event.m_y = 0;
	}
	wxRectTracker::OnMouseLeftDown(event);
}

void wxLineTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
		m_state ^= RT_STATE_DRAGGING;
		Update();
	}
}

int wxLineTracker::HitTest(int x, int y)
{
	wxRect curRect;
  	int z = m_iHandlerWidth;
	int hit;

	hit = wxRectTracker::HitTest(x, y);

	// if ( (y < 0) || (y > h) || (x < 0) || (x > w) ) return RT_HANDLER_OUTSIDE;
	
	if ( hit == m_iEndHandler)		return RT_LINE_HANDLER_END;
	if ( hit == m_iBeginHandler)	return RT_LINE_HANDLER_BEGIN;

	// Distance entre le point et le segment (a = begin ; b = end ; c = x,y )
	int xac = x - GetPosBegin().x + GetPosition().x;
	int yac = y - GetPosBegin().y + GetPosition().y;
	int xab = GetPosEnd().x - GetPosBegin().x;
	int yab = GetPosEnd().y - GetPosBegin().y;

	if ( (xab != 0) || (yab != 0) )
		if ( ( (((double) - xac * yab + yac * xab ) * ((double)  - xac * yab + yac * xab )) 
			 / ((double) (xab * xab) + (yab * yab))
				 ) < ((double) z * z) )
				return RT_LINE_HANDLER_ON_LINE;

/*
#define SQR(x) ((x)*(x))
#define DELTA 3

   double xn = -(yb-ya);
   double yn = (xb-xa);

   double xac = xc-xa;
   double yac = yc-ya;
   double xab = xb-xa;
   double yab = yb-ya;

   double acdotnsq = SQR(xac*xn + yac*yn);

   double normnsq = SQR(xn) + SQR(yn);

   double dsq = acdotnsq / normnsq;

   double acdotab = xac*xab + yac*yab;
   double normabsq = SQR(xab) + SQR(yab);

   return dsq <= SQR(DELTA) &&
         acdotab >= 0 &&
         acdotab <= normabsq; 
*/
	
	return RT_HANDLER_NONE;

/*	
	// Split vertically, then horizontally
	if ( (y <= h) && (y >= h-z) )
	{
		// Bottom line
		if ( (x >= w-z) && (x <= w) )
		{
			// Bottom Right
			return RT_HANDLER_BOTTOM_RIGHT;
		}
		else if ( (x >= 0) && (x <= z) )
		{
			// Bottom left
			return RT_HANDLER_BOTTOM_LEFT;
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
		else if ( (x >= 0) && (x <= z) )
		{
			// Top left
			return RT_HANDLER_TOP_LEFT;
		}
	}
	return RT_HANDLER_NONE;
*/
}

/*
void wxRectTracker::AdjustTrackerRectMax(wxRect &m_curRect, int handler)
{
	// Adjust m_maxRect 
	if ((m_maxRect.width < 0) || (m_maxRect.height < 0)) return;

	// - Left X
	if (m_curRect.x < m_maxRect.x)
	{
		if (handler != RT_HANDLER_NONE) m_curRect.width -= m_maxRect.x - m_curRect.x; 
		m_curRect.x = m_maxRect.x;
	}
	// - Right X
	if ((m_curRect.x + m_curRect.width) > (m_maxRect.x + m_maxRect.width))
	{
		if (handler != RT_HANDLER_NONE) m_curRect.width = m_maxRect.x + m_maxRect.width - m_curRect.x; 
		m_curRect.x = m_maxRect.x + m_maxRect.width - m_curRect.width;
	}
	// - Top Y
	if (m_curRect.y < m_maxRect.y)
	{
		if (handler != RT_HANDLER_NONE) m_curRect.height -= m_maxRect.y - m_curRect.y; 
		m_curRect.y = m_maxRect.y;
	}
	// - Bottom Y
	if ((m_curRect.y + m_curRect.height) > (m_maxRect.y + m_maxRect.height))
	{
		if (handler != RT_HANDLER_NONE) m_curRect.height = m_maxRect.y + m_maxRect.height - m_curRect.y; 
		m_curRect.y = m_maxRect.y + m_maxRect.height - m_curRect.height;
	}

}

void wxRectTracker::AdjustTrackerRect(wxRect &m_curRect, int handler)
{
	AdjustTrackerRectMax(m_curRect, handler);
}
*/

void wxLineTracker::SetTrackerPosition(wxPoint begin, wxPoint end)
{
	if (begin.x <= end.x)
	{
		if (begin.y <= end.y)
		{
			m_iBeginHandler = RT_HANDLER_TOP_LEFT;
			m_iEndHandler = RT_HANDLER_BOTTOM_RIGHT;
		}
		else
		{
			m_iBeginHandler = RT_HANDLER_BOTTOM_LEFT;
			m_iEndHandler = RT_HANDLER_TOP_RIGHT;
		}
	}
	else
	{
		if (begin.y <= end.y)
		{
			m_iBeginHandler = RT_HANDLER_TOP_RIGHT;
			m_iEndHandler = RT_HANDLER_BOTTOM_LEFT;
		}
		else
		{
			m_iBeginHandler = RT_HANDLER_BOTTOM_RIGHT;
			m_iEndHandler = RT_HANDLER_TOP_LEFT;
		}
	}
	SetUnscrolledRect(NormalizeRect(wxRect(begin, end)));
}


void wxLineTracker::Update()
{
	/*
	m_curRect = GetUnscrolledRect();
	AdjustTrackerRect(m_curRect, -1);
	if (m_curRect != GetUnscrolledRect())
	{
		SetUnscrolledRect(m_curRect);
	}
	*/
	GetParent()->Refresh();
    wxCommandEvent evt(EVT_TRACKER_CHANGED, this->GetId());
    GetParent()->GetEventHandler()->ProcessEvent(evt);
}

wxPoint wxLineTracker::GetPosBegin() 
{
	return GetPosHandler(m_iBeginHandler);
}

wxPoint wxLineTracker::GetPosEnd()
{
	return GetPosHandler(m_iEndHandler);
}

wxPoint wxLineTracker::GetPosHandler(enum RT_HANDLER handler)
{
	wxPoint pos;
	switch(handler)
	{
	case RT_HANDLER_TOP_MID:		pos = wxPoint(GetSize().GetWidth() / 2, 0); break;
	case RT_HANDLER_MID_RIGHT:		pos = wxPoint(GetSize().GetWidth() - 1, GetSize().GetHeight() / 2); break;
	case RT_HANDLER_BOTTOM_MID:		pos = wxPoint(GetSize().GetWidth() / 2, GetSize().GetHeight() - 1); break;
	case RT_HANDLER_MID_LEFT:		pos = wxPoint(0, GetSize().GetHeight() / 2); break;
	case RT_HANDLER_TOP_LEFT:		pos = wxPoint(0, 0); break;
	case RT_HANDLER_TOP_RIGHT:		pos = wxPoint(GetSize().GetWidth() - 1, 0); break;
	case RT_HANDLER_BOTTOM_RIGHT:	pos = wxPoint(GetSize().GetWidth() - 1, GetSize().GetHeight() - 1); break;
	case RT_HANDLER_BOTTOM_LEFT:	pos = wxPoint(0, GetSize().GetHeight() - 1); break;
	case RT_HANDLER_OUTSIDE:
	case RT_HANDLER_NONE:
	default:
		return wxPoint(-1,-1);
	}
	return pos + GetPosition();
}

