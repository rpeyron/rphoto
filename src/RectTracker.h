/* ****************************************************************************
 * RectTracker.h                                                              *
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


#ifndef __RECTTRACKER_H__
#define __RECTTRACKER_H__

#define RT_HANDLER_TOP_MID		1
#define RT_HANDLER_MID_RIGHT	2
#define RT_HANDLER_BOTTOM_MID	4
#define RT_HANDLER_MID_LEFT		8
#define RT_HANDLER_TOP_LEFT		9
#define RT_HANDLER_TOP_RIGHT	3
#define RT_HANDLER_BOTTOM_RIGHT 6
#define RT_HANDLER_BOTTOM_LEFT	12
#define RT_HANDLER_NONE			0
#define RT_HANDLER_OUTSIDE		-1

#define RT_STATE_NONE				0
#define RT_STATE_DRAGGING			1
#define RT_STATE_MOUSE_CAPTURED		2

//#ifdef __WXGTK12__
#define RT_GTK_HACK
//#endif

class wxRectTracker : public wxWindow
{
	DECLARE_CLASS(wxRectTracker)
public:
	int HitTest(int x, int y);
	wxRectTracker(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	~wxRectTracker();

	// Manipulation function
	void Update();
	double GetRatio() { return ratio; };
	wxRect GetMaxRect() { return maxRect; };
	int GetOrientation() { return orientation; };
	void SetRatio(double ratio);
	void SetMaxRect(wxRect maxRect);
	void SetOrientation(int orientation);
	// Scrolled Region Function
	wxRect GetUnscrolledRect();
	void SetUnscrolledRect(wxRect rect);
	wxRect GetTrackerRect() { return curRect; };

protected:
	// Events
    DECLARE_EVENT_TABLE()
	// - Misc Events
	void OnEraseBackground(wxEraseEvent & event);
	void OnPaint(wxPaintEvent & event);
	void OnKey(wxKeyEvent & event);
	void OnMouseMotion(wxMouseEvent & event);
	void OnMouseLeftDown(wxMouseEvent & event);
	void OnMouseLeftUp(wxMouseEvent & event);

	// Helper Functions
	virtual void DrawRect(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawRect(wxDC & dc, wxRect rect);
	virtual void DrawTracker(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawTracker(wxDC & dc, wxRect rect);
	long CalcRectDist(wxRect r1, wxRect r2);
	wxRect CalcRectNearer(wxRect rr, wxRect r1, wxRect r2);
	void wxRectTracker::ForwardMouseEventToParent(wxMouseEvent & event);

	// Behaviour Functions
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	void AdjustTrackerRectMax(wxRect & curRect, int handler);
	void AdjustTrackerRectRatio(wxRect & curRect, int handler, bool expand);

protected:
	int m_iHandlerWidth;
	int state;

	wxPoint leftClick;
	wxPoint prevMove;
	wxRect prevRect;
	wxRect curRect;
	wxCursor * cursorMove;

	/// maxRect : the tracker should not go beyond this rect (Parent Coo)
	wxRect maxRect;
	/// ratio : the tracker should keep this ratio (0 : don't use)
	double ratio;
	/// orientation : 0 : automatic, -1 : portrait, 1 : landscape
	int orientation;

};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_TRACKER_CHANGED, 4212)
  DECLARE_EVENT_TYPE(EVT_TRACKER_CHANGING, 4213)
END_DECLARE_EVENT_TYPES()

#define EVT_TRACKER_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_TRACKER_CHANGED, id,\
 wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),
#define EVT_TRACKER_CHANGING(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_TRACKER_CHANGING, id,\
 wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),


#endif // __RECTTRACKER_H__
