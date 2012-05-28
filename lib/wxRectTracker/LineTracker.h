/* ****************************************************************************
 * LineTracker.h                                                              *
 *                                                                            *
 * (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 * LineTracker was originally designed for RPhoto, but can be used separately *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */

#ifndef __LINETRACKER_H__
#define __LINETRACKER_H__

#include <wx/wx.h>
#include "RectTracker.h"

/// Handler position enum.
enum RT_LINE_HANDLER
{
   RT_LINE_HANDLER_OUTSIDE       = -1,
   RT_LINE_HANDLER_NONE          =  0,
   RT_LINE_HANDLER_BEGIN         =  1,
   RT_LINE_HANDLER_END           =  2,
   RT_LINE_HANDLER_ON_LINE       =  3,
};

/// Mask to use with SetHandlerMask() to specify which handlers will be displayed.
enum RT_LINE_MASK
{
   /// Use this to hide handlers.
   RT_LINE_MASK_NONE         = 0x00,
   RT_LINE_MASK_BEGIN        = 0x01,
   RT_LINE_MASK_END          = 0x02,
   /// Use this to show all handlers (default).
   RT_LINE_MASK_ALL          = 0xFF
};

/** wxLineTracker control
 * 
 * This control aims at providing same functionnalies as wxRectTracker, but for lines.
 * It is basically a line with dragging capabilites, to set its size and position.
 *
 *  @see wxRectTracker
 */ 
class wxLineTracker : public wxRectTracker
{
	DECLARE_CLASS(wxLineTracker)
public:
    /** wxLineTracker constructor
     */
	wxLineTracker(wxWindow* parent, wxFrame * frame = NULL);
	virtual ~wxLineTracker();

public:
    /// Returns true if the provided coordinates are in the tracker. (Parent coo.)
	int HitTest(int x, int y) const;

	/// Update the tracker position and size (usefull for initialisation)
	void Update();
	/// Get the list of handlers to be displayed (See RT_LINE_MASK enum)
	int GetHandlerMask() const { return m_iHandlerMask; }
	/// Set which handlers will be displayed (See RT_MASK enum)
	void SetHandlerMask(int iMask = RT_LINE_MASK_ALL) { m_iHandlerMask = iMask; }
	
	/// Get the current position of the tracker, without taking in account any scroll area.
	wxPoint GetPosBegin() const; 
	wxPoint GetPosEnd() const; 
	wxPoint GetPosHandler(enum RT_HANDLER handler) const;
	/// Set a new position for the tracker
	virtual void SetTrackerPosition(wxPoint begin, wxPoint end);

protected:
	// Internals : Events
	// - Misc Events
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnKey(wxKeyEvent & event);
	virtual void OnMouseMotion(wxMouseEvent & event);
	virtual void OnMouseLeftDown(wxMouseEvent & event);
	virtual void OnMouseLeftUp(wxMouseEvent & event);

	// Helper Functions
	virtual void DrawLine(wxDC & dc, wxPoint begin, wxPoint end);
	virtual void DrawTracker(wxDC & dc, wxPoint begin, wxPoint end);

protected:
	// Behaviour Functions
	virtual void AdjustLineTracker(wxPoint & begin, wxPoint & end, int handler);
	void AdjustLineTrackerMax(wxPoint & begin, wxPoint & end, int handler);

protected:
	wxPoint m_pCurBegin; /// Coordinates of the previous first point
	wxPoint m_pCurEnd;   /// Coordinates of the previous last point

	enum RT_HANDLER m_iBeginHandler;	// Handler of Begin Point
	enum RT_HANDLER m_iEndHandler;		// Handler of End Point

};


#endif // __LINETRACKER_H__
