/* ****************************************************************************
 * RectTracker.cpp                                                            *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */

#include "RectTracker.h"

#ifdef wxUSE_GRAPHICS_CONTEXT
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#endif

IMPLEMENT_CLASS(wxRectTracker, wxEvtHandler)

static const wxRect NullRect(0, 0, -1, -1);

wxRectTracker::wxRectTracker(wxWindow* wnd, wxFrame* frame) : 
    wxEvtHandler(), m_wnd(wnd), m_frame(frame), m_cGreyOut(0,0,0,0)
{
	m_iHandlerWidth = 5;
	m_cursorMove = new wxCursor(wxCURSOR_SIZING);
	m_state = 0;
	m_Rect = m_maxRect = NullRect;
	m_iHandlerMask = RT_MASK_ALL;
	if (wnd) wnd->PushEventHandler(this);
}

wxRectTracker::~wxRectTracker()
{
	if (m_wnd) m_wnd->RemoveEventHandler(this);
	delete m_cursorMove;
}

BEGIN_EVENT_TABLE(wxRectTracker, wxEvtHandler)
   EVT_PAINT(wxRectTracker::OnPaint)
   EVT_MOTION(wxRectTracker::OnMouseMotion)
   EVT_LEFT_DOWN(wxRectTracker::OnMouseLeftDown)
   EVT_LEFT_UP(wxRectTracker::OnMouseLeftUp)
   EVT_CHAR(wxRectTracker::OnKey)
END_EVENT_TABLE()

const wxEventType wxEVT_TRACKER_CHANGED = wxNewEventType();
const wxEventType wxEVT_TRACKER_CHANGING = wxNewEventType();

void wxRectTracker::OnPaint(wxPaintEvent& event)
{
	this->GetNextHandler()->ProcessEvent(event);

	wxClientDC dc(m_wnd);
	m_wnd->PrepareDC(dc);
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
	}  

	OnDraw(&dc);
}

void wxRectTracker::OnDraw(wxDC* dc)
{
   if ((m_state & RT_STATE_DISABLED) != 0) return;
   int w = m_Rect.width, h = m_Rect.height;
   if ((w >=0) && (h >= 0)) DrawRect(*dc, m_Rect.GetLeft(), m_Rect.GetTop(), w, h);
}

// DrawRect operates with a wxPaintDC => Window coordinates
void wxRectTracker::DrawRect(wxDC & dc, int x, int y, int w, int h)
{

	// Grey out cutted off area
	// Use wxGraphicsContext for transparency support
#ifdef wxUSE_GRAPHICS_CONTEXT
	if ((w != 0) && (h != 0) && (m_cGreyOut.Alpha() != 0))
	{
		if (wxDynamicCast(&dc,wxWindowDC))  // If needed more casting are available to create wxGraphicsContext
		{
			int cw, ch;
			dc.GetSize(&cw,&ch);
			if (cw > GetMaxRect().GetWidth()) cw = GetMaxRect().GetWidth();
			if (ch > GetMaxRect().GetHeight()) ch = GetMaxRect().GetHeight();
			wxGraphicsContext * gc;
			gc = wxGraphicsContext::Create(*wxDynamicCast(&dc,wxWindowDC));
			gc->SetBrush(wxBrush(m_cGreyOut));
			gc->SetPen(wxPen(m_cGreyOut));
			wxGraphicsPath gp = gc->CreatePath();
			gp.AddRectangle(0  ,0  ,cw ,y );
			gp.AddRectangle(0  ,y  ,x  ,h  );
			gp.AddRectangle(x+w,y  ,cw-x-w,h  );
			gp.AddRectangle(0  ,y+h,cw ,ch-y-h );
			gc->FillPath(gp);
			delete gc;
		}  
	}
#endif
	
	// Rect
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(x,y,w,h);
	dc.SetPen(wxPen(*wxWHITE,1,wxDOT));  // wxSHORT_DASH  in wxGCDC
	dc.DrawRectangle(x,y,w,h);

	// Handlers
  	int z = m_iHandlerWidth;
	if (m_iHandlerMask != RT_MASK_NONE)
	{
    	dc.SetBrush(*wxBLACK_BRUSH);
    	dc.SetPen(*wxBLACK_PEN);
    	
    	if (m_iHandlerMask & RT_MASK_TOP_LEFT) dc.DrawRectangle(x+1    , y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_TOP_RIGHT) dc.DrawRectangle(x-1+w-z, y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_LEFT) dc.DrawRectangle(x+1    , y-1+h-z, z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_RIGHT) dc.DrawRectangle(x-1+w-z, y-1+h-z, z, z);
    	
    	if (m_iHandlerMask & RT_MASK_TOP_MID) dc.DrawRectangle(x+(w-z)/2, y+1    , z, z);
    	if (m_iHandlerMask & RT_MASK_BOTTOM_MID) dc.DrawRectangle(x+(w-z)/2, y+h-z-1, z, z);
    	if (m_iHandlerMask & RT_MASK_MID_LEFT) dc.DrawRectangle(x+1    , y+(h-z)/2, z, z);
    	if (m_iHandlerMask & RT_MASK_MID_RIGHT) dc.DrawRectangle(x+w-z-1, y+(h-z)/2, z, z);
    }

}

void wxRectTracker::DrawRect(wxDC & dc, wxRect rect)
{
	DrawRect(dc, rect.x, rect.y, rect.width, rect.height);
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxRectTracker::DrawTracker(wxDC & dc, int x, int y, int w, int h)
{
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow) != NULL)
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
	}
	// Inverted Rect
	dc.SetLogicalFunction(wxINVERT);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRectangle(x,y,w,h);
	dc.SetLogicalFunction(wxCOPY);
}

void wxRectTracker::DrawTracker(wxDC & dc, const wxRect& rect)
{
	DrawTracker(dc, rect.x, rect.y, rect.width, rect.height);
}


void wxRectTracker::OnKey(wxKeyEvent & event)
{
    if ( ((m_state & RT_STATE_DISABLED) != 0) || !IsShown()) { event.Skip(); return; } 
    
	int incr = 0, dx = 0, dy = 0;
	int handler;
	wxRect tmpRect;
	tmpRect = GetUnscrolledRect();

	if ((tmpRect.width <= 0) || (tmpRect.height <= 0)) { event.Skip(); return; } 
	
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

void wxRectTracker::OnMouseMotion(wxMouseEvent & event)
{
	int hit;
	int dx, dy;

    if ((m_state & RT_STATE_DISABLED) != 0) return;

	wxMouseEvent mouse(event);

	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(mouse.m_x, mouse.m_y, &mouse.m_x, &mouse.m_y);
	}
	
    if (m_frame && IsShown())
    {
	    m_frame->SetStatusText(wxString::Format(_("Mouse position : %d, %d / RectSize : %d,%d->%d,%d(+%d,%d)"),
				event.m_x, event.m_y, 
				m_Rect.GetLeft(), m_Rect.GetTop(), m_Rect.GetRight(), m_Rect.GetBottom(), m_Rect.GetWidth(), m_Rect.GetHeight()));
    }
	
	// Just moving ?
	if (!event.Dragging())
	{
		hit = HitTest(mouse.m_x, mouse.m_y);
		switch (hit)
		{
		case RT_HANDLER_TOP_MID:
		case RT_HANDLER_BOTTOM_MID:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENS));
			break;
		case RT_HANDLER_MID_LEFT:
		case RT_HANDLER_MID_RIGHT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZEWE));
			break;
		case RT_HANDLER_TOP_LEFT:
		case RT_HANDLER_BOTTOM_RIGHT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			break;
		case RT_HANDLER_TOP_RIGHT:
		case RT_HANDLER_BOTTOM_LEFT:
			m_wnd->SetCursor(wxCursor(wxCURSOR_SIZENESW));
			break;
		case RT_HANDLER_NONE:
			m_wnd->SetCursor(wxCursor(wxCURSOR_HAND /* *m_cursorMove */));
			break;
		case RT_HANDLER_OUTSIDE:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
			break;
		default:
			m_wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	}
	else if ((m_state & RT_STATE_DRAGGING))
	{
		// Dragging

		wxASSERT(m_wnd!=NULL);

		// Drawing Tracker Rect
	    wxClientDC dc(m_wnd);
		if (wxDynamicCast(m_wnd,wxScrolledWindow))
		{
			wxDynamicCast(m_wnd,wxScrolledWindow)->DoPrepareDC(dc);
		} 

		dx = 0; dy = 0;
		dc.SetDeviceOrigin(dx, dy);


		if ((m_state & RT_STATE_FIRSTDRAG) == 0)
		{
			m_state |= RT_STATE_FIRSTDRAG;
		}
		else
		{
			// Erase previous Tracker
			DrawTracker(dc, m_prevRect);
		}

		// Update the new position
		// - Which Tracker ?
		hit = HitTest(m_leftClick.x, m_leftClick.y);
		// - Default Rect values
		if (hit != RT_HANDLER_OUTSIDE) 
		{
			m_curRect = GetUnscrolledRect();
		}
		else
		{
			m_curRect = wxRect(m_leftClick, m_leftClick);
			hit = RT_HANDLER_BOTTOM_RIGHT;
		}
		
		dx = (mouse.m_x - m_leftClick.x);
		dy = (mouse.m_y - m_leftClick.y);

		if (hit == RT_HANDLER_OUTSIDE)
		{
			m_curRect.width += dx;
			m_curRect.height += dy;
		}
		else 	if (hit == RT_HANDLER_NONE)
		{
			m_curRect.x += dx;
			m_curRect.y += dy;
		}
		else
		{
			// Use bit combination of handler values to update position
			if ( (hit & RT_HANDLER_MID_RIGHT) != 0)
			{
				m_curRect.width += dx;
			}
			if ( (hit & RT_HANDLER_BOTTOM_MID) != 0)
			{
				m_curRect.height += dy;
			}
			if ( (hit & RT_HANDLER_MID_LEFT) != 0)
			{
				m_curRect.x += dx;
				m_curRect.width -= dx;
			}
			if ( (hit & RT_HANDLER_TOP_MID) != 0)
			{
				m_curRect.y += dy;
				m_curRect.height -= dy;
			}
		}
        if (m_frame)
        {
		    m_frame->SetStatusText(wxString::Format(_("Mouse click : %d, %d / Hit : %d / RectSize : %d,%d->%d,%d(+%d,%d)"),
				m_leftClick.x, m_leftClick.y, 
				hit,
				m_Rect.GetLeft(), m_Rect.GetTop(), m_Rect.GetRight(), m_Rect.GetBottom(), m_Rect.GetWidth(), m_Rect.GetHeight()));
        }
		// Correct Orientation (size and virtual handler)
        if (m_curRect.width < 0) 
        { 
            m_curRect.width = - m_curRect.width; 
            m_curRect.x -= m_curRect.width; 
            if ((hit & RT_HANDLER_MID_LEFT) != 0) hit = (hit ^ RT_HANDLER_MID_LEFT) | RT_HANDLER_MID_RIGHT;
            else if ((hit & RT_HANDLER_MID_RIGHT) != 0) hit = (hit ^ RT_HANDLER_MID_RIGHT) | RT_HANDLER_MID_LEFT;
        }
		if (m_curRect.height < 0) 
        { 
            m_curRect.height = - m_curRect.height; 
            m_curRect.y -= m_curRect.height; 
            if ((hit & RT_HANDLER_TOP_MID) != 0) hit = (hit ^ RT_HANDLER_TOP_MID) | RT_HANDLER_BOTTOM_MID;
            else if ((hit & RT_HANDLER_BOTTOM_MID) != 0) hit = (hit ^ RT_HANDLER_BOTTOM_MID) | RT_HANDLER_TOP_MID;
        }

		// Adjust current Tracker size
		AdjustTrackerRect(m_curRect, hit);

		// Draw current Tracker
		DrawTracker(dc, m_curRect);

		// Update Parent's Status Bar
		m_prevRect = m_curRect;
		wxCommandEvent evt(wxEVT_TRACKER_CHANGING, m_wnd->GetId());
		m_wnd->GetEventHandler()->ProcessEvent(evt);
	}
    
	// Check there is no abuse mouse capture
	if (!(event.LeftIsDown()) && ((m_state & RT_STATE_MOUSE_CAPTURED) != 0))
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
    
	// Update prev_move
	m_prevMove = mouse.GetPosition();

   event.Skip();
}

void wxRectTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	m_state |= RT_STATE_DRAGGING;
	m_leftClick = event.GetPosition();
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(m_leftClick.x, m_leftClick.y, &m_leftClick.x, &m_leftClick.y);
	}
	if (HitTest(m_leftClick.x, m_leftClick.y) == RT_HANDLER_OUTSIDE) 
	{
		Hide();
		Update();
	}
	
	if ((m_state & RT_STATE_MOUSE_CAPTURED) == 0)
	{
		m_wnd->CaptureMouse();
		m_state |= RT_STATE_MOUSE_CAPTURED;
	}

    event.Skip();
}

void wxRectTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetUnscrolledRect(m_prevRect);
		m_state ^= RT_STATE_DRAGGING;
		m_state ^= RT_STATE_FIRSTDRAG;
		m_leftClick = wxPoint(0, 0);
		Update();
	}
   event.Skip();
}

int wxRectTracker::HitTest(int x, int y) const
{
	int w, h;
	int z = m_iHandlerWidth;
	
	w = m_Rect.GetWidth();
	h = m_Rect.GetHeight();
	x = x - m_Rect.GetLeft();
	y = y - m_Rect.GetTop();

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

void wxRectTracker::AdjustTrackerRect(wxRect &curRect, int handler)
{
	AdjustTrackerRectMax(curRect, handler);
}

void wxRectTracker::SetMaxRect(const wxRect& maxRect)
{
	this->m_maxRect = maxRect;
	if (this->m_maxRect.height < 0) this->m_maxRect.height = - this->m_maxRect.height;
	if (this->m_maxRect.width < 0)  this->m_maxRect.width  = - this->m_maxRect.width;
	Update();
}

void wxRectTracker::Update()
{
	m_curRect = GetUnscrolledRect();
	AdjustTrackerRect(m_curRect, -1);
	if (m_curRect != GetUnscrolledRect())
	{
		SetUnscrolledRect(m_curRect);
	}
	m_wnd->Refresh();
    wxCommandEvent evt(wxEVT_TRACKER_CHANGED, m_wnd->GetId());
    m_wnd->GetEventHandler()->ProcessEvent(evt);
}


// TODO : rename this function, as it is now returning the scrolled rect
wxRect wxRectTracker::GetUnscrolledRect() const
{
	return m_Rect;
}

void wxRectTracker::SetUnscrolledRect(const wxRect &rect_ref)
{
	m_Rect = rect_ref;
}


/*
wxRect wxRectTracker::GetUnscrolledRect()
{
	wxRect rect;
	rect = m_wnd->GetRect();
   //rect = m_Rect;

	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcUnscrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	return rect;
}

void wxRectTracker::SetUnscrolledRect(const wxRect& rect_ref)
{
   wxRect rect = rect_ref;
   
	// Convert coordinates if scrolled
	if (wxDynamicCast(m_wnd,wxScrolledWindow))
	{
		wxDynamicCast(m_wnd,wxScrolledWindow)->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
	}
	m_wnd->SetSize(rect);
	
   //m_Rect = rect;
}
*/

void wxRectTracker::Enable()
{
	this->SetEvtHandlerEnabled(TRUE);
    m_state &= ~RT_STATE_DISABLED;
}

void wxRectTracker::Disable()
{
	this->SetEvtHandlerEnabled(FALSE);
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		m_wnd->ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
    //SetSize(wxRect(0, 0, 0, 0));
    Update();
    m_state |= RT_STATE_DISABLED;
}

bool wxRectTracker::IsShown() const
{
    return (m_Rect != NullRect);
}

void wxRectTracker::Hide()
{
    m_Rect = NullRect;
    m_prevRect = NullRect;
    m_wnd->Refresh(false);
}
