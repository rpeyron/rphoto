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
#include <wx/dcbuffer.h>

#ifdef RT_GTK_HACK
#define wxDynamicCastHelper(obj, className, mixInClassName) \
 (obj && wxIsKindOf(((wxObject *)(void *)obj), className))? \
 (mixInClassName *)(className *)(void *)obj: \
 obj
 
#define IMPLEMENT_CLASS0(name) \
 wxClassInfo name::sm_class##name(wxT(#name), 0, 0, (int) sizeof(name), (wxObjectConstructorFn) 0);

// IMPLEMENT_ABSTRACT_CLASS(wxRectTrackerHost, wxObject)
//IMPLEMENT_CLASS0(wxRectTrackerHost)
#endif

IMPLEMENT_ABSTRACT_CLASS(wxRectTrackerHost, wxObject)

IMPLEMENT_CLASS(wxRectTracker, wxWindow)

wxRectTracker::wxRectTracker(
			wxWindow * parent, 
			wxWindowID id,  
			const wxPoint & pos, 
			const wxSize& size, 
			long style,
            wxRectTrackerHost * rectTrackerHost) 
			: wxWindow(parent, id, pos, size, style)
{
	m_iHandlerWidth = 5;
	m_cursorMove = new wxCursor(wxCURSOR_SIZING);
	m_state = 0;
	m_maxRect = wxRect(0, 0, -1, -1); 
	m_iHandlerMask = RT_MASK_ALL;
	m_pRectTrackerHost = rectTrackerHost;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

wxRectTracker::~wxRectTracker()
{
	if (m_cursorMove) delete m_cursorMove;
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
	// Transparency : Do Nothing  @See RT_GTK_HACK
}

void wxRectTracker::OnPaint(wxPaintEvent & event)
{
	int w, h ;
    wxAutoBufferedPaintDC dc(this);
//    wxPaintDC dc(this);

#ifdef RT_GTK_HACK
    if (m_pRectTrackerHost)
    {
		dc.SetDeviceOrigin(-GetPosition().x, -GetPosition().y);
        m_pRectTrackerHost->PaintDC(dc);
        dc.SetDeviceOrigin(0, 0);
    }    
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
	if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
	{
		wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
	}
	// Inverted Rect
	dc.SetLogicalFunction(wxINVERT);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRectangle(x,y,w,h);
	/*
	dc.DrawLine(x  ,y  ,x+w,y  );
	dc.DrawLine(x  ,y  ,x  ,y+h);
	dc.DrawLine(x+w,y  ,x+w,y+h);
	dc.DrawLine(x  ,y+h,x+w,y+h);
	*/
	dc.SetLogicalFunction(wxCOPY);
}

void wxRectTracker::DrawTracker(wxDC & dc, wxRect rect)
{
	DrawTracker(dc, rect.x, rect.y, rect.width, rect.height);
}


void wxRectTracker::OnKey(wxKeyEvent & event)
{

    if ((m_state & RT_STATE_DISABLED) != 0) { event.Skip(); return; } 
    
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
				ReleaseMouse();
				m_state ^= RT_STATE_MOUSE_CAPTURED;
				Update();
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

    if ((m_state & RT_STATE_DISABLED) != 0) return;

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
			SetCursor(*m_cursorMove);
		}
	}
	else if (event.LeftIsDown())
	{
		// Dragging

		wxASSERT(GetParent()!=NULL);

		// Drawing Tracker Rect
		wxClientDC dc(GetParent());
		// Use wxScreenDC instead to avoid clipping
		//wxScreenDC dc;

		dx = 0; dy = 0;
		//if (GetParent()) GetParent()->ClientToScreen(&dx, &dy);
		dc.SetDeviceOrigin(dx, dy);
		//dc.StartDrawingOnTop(GetParent());

		if ((m_state & RT_STATE_DRAGGING) == 0)
		{
			m_state |= RT_STATE_DRAGGING;
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
		m_curRect = GetUnscrolledRect();
		
		dx = (event.m_x - m_leftClick.x);
		dy = (event.m_y - m_leftClick.y);

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
		if (hit == RT_HANDLER_NONE)
		{
			m_curRect.x += dx;
			m_curRect.y += dy;
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

void wxRectTracker::OnMouseLeftDown(wxMouseEvent & event)
{
	m_leftClick.x = event.m_x;
	m_leftClick.y = event.m_y;
	if ((m_state & RT_STATE_MOUSE_CAPTURED) == 0)
	{
		CaptureMouse();
		m_state |= RT_STATE_MOUSE_CAPTURED;
	}
}

void wxRectTracker::OnMouseLeftUp(wxMouseEvent & event)
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
	if ((m_state & RT_STATE_DRAGGING) != 0)
	{
		SetUnscrolledRect(m_prevRect);
		m_state ^= RT_STATE_DRAGGING;
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

void wxRectTracker::SetMaxRect(wxRect m_maxRect)
{
	this->m_maxRect = m_maxRect;
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
	GetParent()->Refresh();
    wxCommandEvent evt(EVT_TRACKER_CHANGED, this->GetId());
    GetParent()->GetEventHandler()->ProcessEvent(evt);
}

/*
wxRect wxRectTracker::GetUnscrolledRect()
{
	wxRect rect;
	rect = GetRect();
	return rect;
}

void wxRectTracker::SetUnscrolledRect(wxRect rect)
{
	SetSize(rect);
}
*/


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


void wxRectTracker::Enable()
{
    m_state &= ~RT_STATE_DISABLED;
}

void wxRectTracker::Disable()
{
	if ((m_state & RT_STATE_MOUSE_CAPTURED) != 0)
	{
		ReleaseMouse();
		m_state ^= RT_STATE_MOUSE_CAPTURED;
	}
    SetSize(wxRect(0, 0, 0, 0));
    Update();
    m_state |= RT_STATE_DISABLED;
}


