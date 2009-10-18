/* ****************************************************************************
 * LineTracker.h                                                              *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
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
 * It is highly recommanded to use directly contained in the wxWindow that will handle 
 * its background. (See RT_GTK_HACK for any related issue on backgrounds).
 *
 *  @see RT_GTK_HACK
 *  @see wxRectTracker
 */ 
class wxLineTracker : public wxRectTracker
{
	DECLARE_CLASS(wxLineTracker)
public:
    /** wxLineTracker constructor
     * The first five parameters as like in every wxWindow class.
     * The last one is dedicated to background support in GTK (see RT_GTK_HACK define)
     *  if you do not intend to use GTK, juste keep it to NULL. @see RT_GTK_HACK
     */
	wxLineTracker(wxWindow* parent, 
                  wxWindowID id, 
                  const wxPoint& begin = wxDefaultPosition, 
                  const wxPoint& end = wxDefaultPosition, 
                  long style = 0,
                  wxRectTrackerHost * rectTrackerHost = NULL);
	~wxLineTracker();

public:
    /// Returns true if the provided coordinates are in the tracker. (Parent coo.)
	int HitTest(int x, int y);

	/// Update the tracker position and size (usefull for initialisation)
	void Update();
	/// Get the list of handlers to be displayed (See RT_LINE_MASK enum)
	int GetHandlerMask() { return m_iHandlerMask; }
	/// Set which handlers will be displayed (See RT_MASK enum)
	void SetHandlerMask(int iMask = RT_LINE_MASK_ALL) { m_iHandlerMask = iMask; }
	
	/// Get the current position of the tracker, without taking in account any scroll area.
	wxPoint GetPosBegin(); 
	wxPoint GetPosEnd(); 
	wxPoint GetPosHandler(enum RT_HANDLER handler); 
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
	//virtual void AdjustTracker(wxPoint & begin, wxPoint & end, int handler);

protected:
	//int m_iHandlerWidth; /// Width of drawn handler
	//int m_iHandlerMask;  /// Mask describing which handlers will be drawn
	//int m_state;         /// Current state of the control (See RT_STATE)

	//wxPoint m_leftClick; /// Coordinates of the last left clic
	//wxPoint m_prevMove;  /// Coordinates of the previous move
	//wxPoint m_pBegin;	 /// Coordinates of the fist point
	//wxPoint m_pEnd;		 /// Coordinates of the last point
	//wxPoint m_pPrevBegin;/// Coordinates of the previous first point
	//wxPoint m_pPrevEnd;  /// Coordinates of the previous last point
	wxPoint m_pCurBegin; /// Coordinates of the previous first point
	wxPoint m_pCurEnd;   /// Coordinates of the previous last point

	enum RT_HANDLER m_iBeginHandler;	// Handler of Begin Point
	enum RT_HANDLER m_iEndHandler;		// Handler of End Point

};


#endif // __LINETRACKER_H__
