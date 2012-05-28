/* ****************************************************************************
 * LineTracker.cpp                                                            *
 *                                                                            *
 * (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 * LineTracker was originally designed for RPhoto, but can be used separately *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */


#include "LineTracker.h"
#include <wx/dcbuffer.h>

/// Helpers

static wxRect NormalizeRect(wxRect rect)
{
	if (rect.width < 0) { rect.x += rect.width ; rect.width = -rect.width; }
	if (rect.height < 0) { rect.y += rect.height ; rect.height = -rect.height; }
	return rect;
}

/// wxLineTracker

IMPLEMENT_CLASS(wxLineTracker, wxRectTracker)

wxLineTracker::wxLineTracker(wxWindow * parent, wxFrame * frame) 
			: wxRectTracker(parent, frame)
{
}

wxLineTracker::~wxLineTracker()
{
}


void wxLineTracker::OnPaint(wxPaintEvent & event)
{
	if ((m_state & RT_STATE_DISABLED) != 0) return;
	if ((m_state & RT_STATE_DRAGGING) != 0) return;
	this->GetNextHandler()->ProcessEvent(event);
	wxClientDC dc(m_wnd);
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
	}   
	DrawLine(dc, GetPosBegin(), GetPosEnd());
}

// DrawLine operates with a wxPaintDC => Window coordinates
void wxLineTracker::DrawLine(wxDC & dc, wxPoint begin, wxPoint end)
{
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
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxLineTracker::DrawTracker(wxDC & dc, wxPoint begin, wxPoint end)
{

	int x1, y1, x2, y2;
	x1 = begin.x; y1 = begin.y;
	x2 = end.x;   y2 = end.y;
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow) != NULL)
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x1, y1, &x1, &y1);
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x2, y2, &x2, &y2);
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
    
	int incr = 0, dx = 0, dy = 0;
	int handler;
	wxRect tmpRect;
	tmpRect = GetUnscrolledRect();
	
	if (event.ControlDown()) incr = 10; else incr = 1;

	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE :
			if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
			{
				m_wnd->ReleaseMouse();
				m_state ^= RT_STATE_MOUSE_CAPTURED;
				Update();
			}
            else
            {
                Hide();
            }
			break;
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
			m_pCurEnd.x += dx;
			m_pCurEnd.y += dy;
			handler = RT_LINE_HANDLER_END;
		}
		else
		{
			m_pCurBegin.x += dx;	m_pCurEnd.x += dx;
			m_pCurBegin.y += dy;	m_pCurEnd.y += dy;
			handler = 0;
		}
		AdjustLineTracker(m_pCurBegin, m_pCurEnd, handler);
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
		Update();
	}

}

void wxLineTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;

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
			m_wnd->SetCursor(*m_cursorMove);
			break;
		default:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
            break;
		}
	}
	else if (event.LeftIsDown())
	{
		// Dragging

		wxASSERT(m_wnd!=NULL);

		// Drawing Tracker Rect
		wxClientDC dc(m_wnd);

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
		AdjustLineTracker(m_pCurBegin, m_pCurEnd, hit);

		// Draw current Tracker
		DrawTracker(dc, m_pCurBegin, m_pCurEnd);

		// Update Parent's Status Bar
		wxCommandEvent evt(wxEVT_TRACKER_CHANGING, m_wnd->GetId());
		m_wnd->GetEventHandler()->ProcessEvent(evt);

        //dc.EndDrawingOnTop();
	}
	
	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((m_state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}

	// Update prev_move
    m_prevMove = event.GetPosition();
}

void wxLineTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	if (HitTest(event.m_x, event.m_y) == RT_LINE_HANDLER_NONE)
	{
		m_pCurBegin = wxPoint(event.m_x, event.m_y);
		m_pCurEnd = m_pCurBegin;
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
	}
	wxRectTracker::OnMouseLeftDown(event);
}

void wxLineTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetTrackerPosition(m_pCurBegin, m_pCurEnd);
		m_state ^= RT_STATE_DRAGGING;
		Update();
	}
}

int wxLineTracker::HitTest(int x, int y) const
{
	wxRect curRect;
  	int z = m_iHandlerWidth;
	int hit;

	hit = wxRectTracker::HitTest(x, y);

	// if ( (y < 0) || (y > h) || (x < 0) || (x > w) ) return RT_HANDLER_OUTSIDE;
	
	if ( hit == m_iEndHandler)		return RT_LINE_HANDLER_END;
	if ( hit == m_iBeginHandler)	return RT_LINE_HANDLER_BEGIN;

	// Distance entre le point et le segment (a = begin ; b = end ; c = x,y )
	int xac = x - GetPosBegin().x; // + m_Rect.GetPosition().x;
	int yac = y - GetPosBegin().y; // + m_Rect.GetPosition().y;
	int xab = GetPosEnd().x - GetPosBegin().x;
	int yab = GetPosEnd().y - GetPosBegin().y;

	if ( (xab != 0) || (yab != 0) )
		if ( ( (((double) - xac * yab + yac * xab ) * ((double)  - xac * yab + yac * xab )) 
			 / ((double) (xab * xab) + (yab * yab))
				 ) < ((double) z * z) )
				return RT_LINE_HANDLER_ON_LINE;
	
	return RT_HANDLER_NONE;
}

void wxLineTracker::AdjustLineTracker(wxPoint & begin, wxPoint & end, int handler)
{
	AdjustLineTrackerMax(begin, end, handler);
}

void wxLineTracker::AdjustLineTrackerMax(wxPoint & begin, wxPoint & end, int handler)
{
	
	// Adjust m_maxRect 
	if ((m_maxRect.width < 0) || (m_maxRect.height < 0)) return;

	if (begin.x < m_maxRect.x) begin.x = m_maxRect.x;
	if (begin.x > m_maxRect.x + m_maxRect.width) begin.x = m_maxRect.x + m_maxRect.width;
	if (begin.y < m_maxRect.y) begin.y = m_maxRect.y;
	if (begin.y > m_maxRect.y + m_maxRect.height) begin.y = m_maxRect.x + m_maxRect.height;

	if (end.x < m_maxRect.x) end.x = m_maxRect.x;
	if (end.x > m_maxRect.x + m_maxRect.width) end.x = m_maxRect.x + m_maxRect.width;
	if (end.y < m_maxRect.y) end.y = m_maxRect.y;
	if (end.y > m_maxRect.y + m_maxRect.height) end.y = m_maxRect.x + m_maxRect.height;

}

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
	m_wnd->Refresh();
    wxCommandEvent evt(wxEVT_TRACKER_CHANGED, m_wnd->GetId());
    m_wnd->GetEventHandler()->ProcessEvent(evt);
}

wxPoint wxLineTracker::GetPosBegin() const
{
	return GetPosHandler(m_iBeginHandler);
}

wxPoint wxLineTracker::GetPosEnd() const
{
	return GetPosHandler(m_iEndHandler);
}

wxPoint wxLineTracker::GetPosHandler(enum RT_HANDLER handler) const
{
	wxPoint pos;
	switch(handler)
	{
	case RT_HANDLER_TOP_MID:		pos = wxPoint(m_Rect.GetWidth() / 2, 0); break;
	case RT_HANDLER_MID_RIGHT:		pos = wxPoint(m_Rect.GetWidth() - 1, m_Rect.GetHeight() / 2); break;
	case RT_HANDLER_BOTTOM_MID:		pos = wxPoint(m_Rect.GetWidth() / 2, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_MID_LEFT:		pos = wxPoint(0, m_Rect.GetHeight() / 2); break;
	case RT_HANDLER_TOP_LEFT:		pos = wxPoint(0, 0); break;
	case RT_HANDLER_TOP_RIGHT:		pos = wxPoint(m_Rect.GetWidth() - 1, 0); break;
	case RT_HANDLER_BOTTOM_RIGHT:	pos = wxPoint(m_Rect.GetWidth() - 1, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_BOTTOM_LEFT:	pos = wxPoint(0, m_Rect.GetHeight() - 1); break;
	case RT_HANDLER_OUTSIDE:
	case RT_HANDLER_NONE:
	default:
		return wxPoint(-1,-1);
	}
	return pos + m_Rect.GetPosition();
}

