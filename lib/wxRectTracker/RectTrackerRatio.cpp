/* ****************************************************************************
 * RectTrackerRatio.cpp                                                       *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */


#include "RectTrackerRatio.h"

IMPLEMENT_CLASS(wxRectTrackerRatio, wxRectTracker)

wxRectTrackerRatio::wxRectTrackerRatio(wxWindow * parent, wxFrame * frame) 
			: wxRectTracker(parent, frame)
{
	SetRatio(0);
	SetGuideRatio(0);
	SetOrientation(0);
}

wxRectTrackerRatio::~wxRectTrackerRatio()
{
}

long wxRectTrackerRatio::CalcRectDist(wxRect r1, wxRect r2)
{
	int d;
	d = 0;
	d += (r1.x - r2.x) * (r1.x - r2.x);
	d += (r1.y - r2.y) * (r1.y - r2.y);
	d += (r1.width - r2.width) * (r1.width - r2.width);
	d += (r1.height - r2.height) * (r1.height - r2.height);
	return d;
}

wxRect wxRectTrackerRatio::CalcRectNearer(wxRect rr, wxRect r1, wxRect r2)
{
	if (CalcRectDist(rr, r2) < CalcRectDist(rr, r1))
	 		 return r2;
		else return r1;
}

void wxRectTrackerRatio::AdjustTrackerRectRatio(wxRect &curRect, int handler, bool expand)
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

void wxRectTrackerRatio::AdjustTrackerRectFixed(wxRect &curRect, int handler)
{
    // Adjust Width
    curRect.x += (curRect.width - fixedWidth);
    curRect.width = fixedWidth;
    if (curRect.x < m_maxRect.x) curRect.x = m_maxRect.x;
    if ((curRect.x + curRect.width) > (m_maxRect.x + m_maxRect.width)) curRect.x = m_maxRect.x + m_maxRect.width - curRect.width;
    // Adjust Height
    curRect.y += (curRect.height - fixedHeight);
    curRect.height = fixedHeight;
    if (curRect.y < m_maxRect.y) curRect.y = m_maxRect.y;
    if ((curRect.y + curRect.height) > (m_maxRect.y + m_maxRect.height)) curRect.y = m_maxRect.y + m_maxRect.height - curRect.height;
}

void wxRectTrackerRatio::AdjustTrackerRect(wxRect &curRect, int handler)
{
	wxRectTracker::AdjustTrackerRect(curRect, handler);
	if ((fixedWidth > 0 ) && (fixedHeight > 0))
	{
	    AdjustTrackerRectFixed(curRect, handler);
	}    
	if (ratio > 0)
	{
		// First Pass
		AdjustTrackerRectRatio(curRect, handler, true);
		// Second Pass - Adjust if new rect has been out of the max rect.
		wxRectTracker::AdjustTrackerRect(curRect, handler);
		AdjustTrackerRectRatio(curRect, handler, false);
	}
}

void wxRectTrackerRatio::SetRatio(double ratio)
{
    this->fixedWidth = this->fixedHeight = -1; // Remove fixed mode
	if ((ratio > 0) && (ratio < 1)) ratio = 1 / ratio;
	this->ratio = ratio;
    SetHandlerMask(RT_MASK_ALL);
}


void wxRectTrackerRatio::SetGuideRatio(double ratio)
{
	if (ratio > 1) ratio = 1 / ratio;
	this->guideRatio = ratio;
}

void wxRectTrackerRatio::SetOrientation(int orientation)
{
	this->orientation = orientation;
	if (this->fixedWidth != -1) SetFixedSize(this->fixedWidth, this->fixedHeight);
}

void wxRectTrackerRatio::SetFixedSize(int width, int height)
{
    int tmp;
    this->fixedWidth = width;
    this->fixedHeight = height;
	// Switch orientation if one was given
    if ( ((orientation == -1) && (this->fixedWidth > this->fixedHeight)) ||
         ((orientation == 1) && (this->fixedWidth < this->fixedHeight))   )
    {
        tmp = this->fixedWidth;
        this->fixedWidth = this->fixedHeight;
        this->fixedHeight = tmp;
    }
    SetHandlerMask(RT_MASK_NONE);
}    


// DrawRect operates with a wxPaintDC => Window coordinates
void wxRectTrackerRatio::DrawRect(wxDC & dc, int x, int y, int w, int h)
{
	wxRectTracker::DrawRect(dc, x, y, w, h);
	// Rect
	if (this->guideRatio != 0)
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawLine(x  ,y + h*this->guideRatio  ,x+w,y + h*this->guideRatio   );
		dc.DrawLine(x  ,y + h*(1-this->guideRatio)  ,x+w,y + h*(1-this->guideRatio));
		dc.DrawLine(x+w*this->guideRatio  ,y  ,x+w*this->guideRatio  ,y+h);
		dc.DrawLine(x+w*(1-this->guideRatio)  ,y  ,x+w*(1-this->guideRatio)  ,y+h);
	}
}

// DrawTracker operates with the parent's wxWindowDC => Parent coordinates
void wxRectTrackerRatio::DrawTracker(wxDC & dc, int x, int y, int w, int h)
{
	wxRectTracker::DrawTracker(dc, x, y, w, h);
	if (this->guideRatio != 0)
	{
		/* useless with wxEvtHandler
		// Convert coordinates if scrolled
		if (wxDynamicCast(GetParent(),wxScrolledWindow) != NULL)
		{
			wxDynamicCast(GetParent(),wxScrolledWindow)->CalcScrolledPosition(x, y, &x, &y);
		}
		*/
		// Inverted Rect
		dc.SetLogicalFunction(wxINVERT);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(*wxGREY_PEN);
		dc.DrawLine(x  ,y + h*this->guideRatio  ,x+w,y + h*this->guideRatio   );
		if (this->guideRatio != 0.5) dc.DrawLine(x  ,y + h*(1-this->guideRatio)  ,x+w,y + h*(1-this->guideRatio));
		dc.DrawLine(x+w*this->guideRatio  ,y  ,x+w*this->guideRatio  ,y+h);
		if (this->guideRatio != 0.5) dc.DrawLine(x+w*(1-this->guideRatio)  ,y  ,x+w*(1-this->guideRatio)  ,y+h);
		dc.SetLogicalFunction(wxCOPY);
	}
}

