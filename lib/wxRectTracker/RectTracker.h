/* ****************************************************************************
 * RectTracker.h                                                              *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */

#ifndef __RECTTRACKER_H__
#define __RECTTRACKER_H__

#include <wx/wx.h>

// TODO : Separate common source code into a wxTracker / wxHostTracker superclass

/// Handler position enum.
enum RT_HANDLER
{
   RT_HANDLER_OUTSIDE       = -1,
   RT_HANDLER_NONE          =  0,
   RT_HANDLER_TOP_MID       =  1,
   RT_HANDLER_MID_RIGHT     =  2,
   RT_HANDLER_BOTTOM_MID    =  4,
   RT_HANDLER_MID_LEFT      =  8,
   RT_HANDLER_TOP_LEFT      = RT_HANDLER_TOP_MID + RT_HANDLER_MID_LEFT,
   RT_HANDLER_TOP_RIGHT     = RT_HANDLER_TOP_MID + RT_HANDLER_MID_RIGHT,
   RT_HANDLER_BOTTOM_RIGHT  = RT_HANDLER_BOTTOM_MID + RT_HANDLER_MID_RIGHT,
   RT_HANDLER_BOTTOM_LEFT   = RT_HANDLER_BOTTOM_MID + RT_HANDLER_MID_LEFT
};

/// Current state of the wxRectTracker control. Currently only for internal use.
enum RT_STATE
{
   RT_STATE_NONE = 0,
   RT_STATE_DRAGGING = 1,
   RT_STATE_MOUSE_CAPTURED = 2,
   RT_STATE_DISABLED = 4
};

/// Mask to use with SetHandlerMask() to specify which handlers will be displayed.
enum RT_MASK
{
   /// Use this to hide handlers.
   RT_MASK_NONE         = 0x00,
   RT_MASK_TOP_LEFT     = 0x01,
   RT_MASK_TOP_MID      = 0x02,
   RT_MASK_TOP_RIGHT    = 0x04,
   RT_MASK_MID_RIGHT    = 0x08,
   RT_MASK_BOTTOM_RIGHT = 0x10,
   RT_MASK_BOTTOM_MID   = 0x20,
   RT_MASK_BOTTOM_LEFT  = 0x40,
   RT_MASK_MID_LEFT     = 0x80,
   /// Use this to show all handlers (default).
   RT_MASK_ALL          = 0xFF
};

/** GTK Hack
 *
 * There is a big problem with wxGTK for transparency, as background of controls 
 * will always be cleared by the system before any call to the control. Where 
 * with wxWin32, only a redefinition of OnEraseBackground() was needed, we must 
 * completly redraw the background in wxGTK.
 * 
 * Waiting for a better solution (maybe in wxGTK2?), wxRectTrackerHost is a 
 * workaround to this. The goal of this class is to provide a PaintDC function
 * to the wxRectTracker. Basically, as this kind of tracker is often used on 
 * owner-drawn surfaces (images, graphics, ...) this should not be too hard.
 * The wxDC given in parameter is exprimated in "Parent" units, which should 
 * allow to use the same code for OnPaint() and PaintDC().
 * 
 * If you only want it to work on Microsoft Windows, do not care of all that stuff,
 * and pass NULL as wxRectTrackerHost to wxRectTracker constructor.
 *
 * If you want a GTK support, you should define a local class which derivates 
 * of wxRectTrackerHost in the class you use the wxRectTracker (the one which handles
 * the background of the control, and in most case, the parent of wxRectTracker)
 * In this inner class, you will be able to refer protected functions of the parent
 * class, and thus, the code for drawing the background.
 *
 * Code example :
 * <pre>
 * class myHostControlClass : wxWindow
 * {
 *   ...
 *   protected: 
 *     wxRectTracker myTracker; // constructed with myMainClass as parent window
 *     void OnPaint(wxPaintEvent & event) { ... PaintDC(dc); ... } // Owner-drawn
 *     void PaintDC(wxDC & dc) { ... } // Actually draw stuff (ex : dc.DrawBitmap)
 *     // wxRectTrackerHost workaround
 *     class myRectTrackerHost : public wxRectTrackerHost
 *     {
 *     public:
 *       myRectTrackerHost(wxRatioImageBox & owner) : m_pOwner(owner) {};
 *       virtual ~myRectTrackerHost() {};
 *       virtual void PaintDC(wxDC & dc) { m_pOwner.PaintDC(dc); }
 *     protected:
 *       myHostControlClass & m_pOwner;
 *     };
 *     myRectTrackerHost m_Host;  // Should be instanciated in the constructor by m_Host(this)
 * }
 * </pre>
 *
 * Warning : you MUST NOT use PrepareDC in your own PaintDC. 
 * This is because on wxGTK, PrepareDC redefines from scratch the DeviceOrigin,
 *  which has already been altered in the Tracker OnPaint function (works on wxMSW).
 *  You should use GetViewStart and translate everything yourself, as shown below :
 *  (or call yourself SetDeviceOrigin with the view start AND the tracker position)
 *
 * <pre>
 * void PaintDC(wxDC & dc) 
 * { 
 *      int vx, vy;
 *      GetViewStart(&vx, &vy); // Do not use PrepareDC(dc); 
 *      dc.DrawBitmap(*m_buffer, m_bufferX - vx, m_bufferY - vy); 
 * }
 * </pre>
 *
 * As this is maybe not very clear, a full featured example is available at :
 * http://people.via.ecp.fr/~remi/soft/rphoto/rphoto.php3
 * I will try also to make spppppppomething like a wiki on the subject.
 */
 
#ifdef __WXGTK__
#define RT_GTK_HACK
#endif

// Well, it seems finally that Windows now needs the GTK Hack...
#define RT_GTK_HACK

// First attempt with defining wxRectTrackerHost as an interface,
//  but making interfaces with C++ and Abstact Base Class is awfull
// Usefull Black Magic from : http://wiki.wxwidgets.org/wiki.pl?Multiple_Inheritance

/// Definition of the PaintDC "callback", for transparency on GTK. 
/// Please see documentation on RT_GTK_HACK define. @see RT_GTK_HACK
class wxRectTrackerHost
{
public:
   DECLARE_CLASS(wxRectTrackerHost)
   wxRectTrackerHost() {};
   virtual ~wxRectTrackerHost() {};
   virtual void PaintDC(wxDC & dc) = 0;
};    

/** wxRectTracker control
 * 
 * This control aims at providing same functionnalies as the MFC CRectTracker.
 * It is basically a selection rectangle with dragging capabilites, 
 *  to set its size and position.
 * It is highly recommanded to use directly contained in the wxWindow that will handle 
 * its background. (See RT_GTK_HACK for any related issue on backgrounds).
 *
 * Well nothing much to be said about this... or maybe :
 *  - The control defines two events EVT_TRACKER_CHANGED and EVT_TRACKER_CHANGING to be
 *     used in the parent hierarchy (for size and position display in a status bar, for instance)
 *  - The virtual function AdjustTrackerRect can be used to implement some 
 *     rules about the way the tracker should behave (typically, keeping a constant 
 *     width/height ratio, as in wxRectTrackerRatio)
 *  - You can set a max area with SetMaxRect()
 *  - You can get the current size/position with GetUnscrolledRect()
 *
 *  @see RT_GTK_HACK
 */ 
class wxRectTracker : public wxWindow
{
	DECLARE_CLASS(wxRectTracker)
public:
    /** wxRectTracker constructor
     * The first five parameters as like in every wxWindow class.
     * The last one is dedicated to background support in GTK (see RT_GTK_HACK define)
     *  if you do not intend to use GTK, juste keep it to NULL. @see RT_GTK_HACK
     */
	wxRectTracker(wxWindow* parent, 
                  wxWindowID id, 
                  const wxPoint& pos = wxDefaultPosition, 
                  const wxSize& size = wxDefaultSize, 
                  long style = 0,
                  wxRectTrackerHost * rectTrackerHost = NULL);
	~wxRectTracker();

public:
    /// Returns true if the provided coordinates are in the tracker. (Parent coo.)
	int HitTest(int x, int y);
	
	// Manipulation function -----------------------------------------------------
	/// Update the tracker position and size (usefull for initialisation)
	void Update();
	/// Get the coordinates of the area the tracker should not go beyond.
	wxRect GetMaxRect() { return m_maxRect; };
	/// Set the maximum boundaries of the available space the tracker.
	void SetMaxRect(wxRect maxRect);
	/// Get the list of handlers to be displayed (See RT_MASK enum)
	int GetHandlerMask() { return m_iHandlerMask; }
	/// Set which handlers will be displayed (See RT_MASK enum)
	void SetHandlerMask(int iMask = RT_MASK_ALL) { m_iHandlerMask = iMask; }
	/// Enable the Tracker
	void Enable();
	/// Disable the Tracker
	void Disable();
	/// Get the Status
	bool IsEnabled() { return ((m_state & RT_STATE_DISABLED) == 0); };
	
	// Scrolled Region Function --------------------------------------------------
	/** Get the current size and position of the tracker. 
     * If the tracker is on a wxScrolledWindow component, this is the absolute 
     * position (as if the control was not scrolled) 
     */
	wxRect GetUnscrolledRect();
	/// Set a new position for the tracker
	virtual void SetUnscrolledRect(wxRect rect);
	/// Get the current position of the tracker, without taking in account any scroll area.
	wxRect GetTrackerRect() { return m_curRect; };

protected:
	// Internals : Events
    DECLARE_EVENT_TABLE()
	// - Misc Events
	virtual void OnEraseBackground(wxEraseEvent & event);
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnKey(wxKeyEvent & event);
	virtual void OnMouseMotion(wxMouseEvent & event);
	virtual void OnMouseLeftDown(wxMouseEvent & event);
	virtual void OnMouseLeftUp(wxMouseEvent & event);

	// Helper Functions
	virtual void DrawRect(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawRect(wxDC & dc, wxRect rect);
	virtual void DrawTracker(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawTracker(wxDC & dc, wxRect rect);
	void ForwardMouseEventToParent(wxMouseEvent & event);

protected:
	// Behaviour Functions
	/** Adjust the behaviour of the tracker.
     * This virtual function allow you to specify special behaviour of the tracker
     *  as maintaining a constant ratio, keeping into a special area,...
     *
     * @param curRect [in, out] the asked position
     * @param handler [in] the handler used to ask this new position
     *
     * You get the new coordinates the user ask in curRect, with the handler he 
     * used. You can now process this information, and decide a new position that
     * you indicated as output in curRect (that is why this is not a const wxRect &).
	 */
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	/// Example of AdjustTrackerRect function
	void AdjustTrackerRectMax(wxRect & curRect, int handler);

protected:
	int m_iHandlerWidth; /// Width of drawn handler
	int m_iHandlerMask;  /// Mask describing which handlers will be drawn
	int m_state;         /// Current state of the control (See RT_STATE)

	wxPoint m_leftClick; /// Coordinates of the last left clic
	wxPoint m_prevMove;  /// Coordinates of the previous move
	wxRect m_prevRect;   /// Coordinates of the previous calculated tracker
	wxRect m_curRect;    /// Coordinates of the current tracker
	wxCursor * m_cursorMove;

	/// maxRect : the tracker should not go beyond this rect (Parent Coo)
	wxRect m_maxRect;
	/// For GTK transparency, See RT_GTK_HACK documentation.  @see RT_GTK_HACK
	wxRectTrackerHost * m_pRectTrackerHost;
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_TRACKER_CHANGED, 4212) 
  DECLARE_EVENT_TYPE(EVT_TRACKER_CHANGING, 4213)
END_DECLARE_EVENT_TYPES()

/// Event fired when the user has decided a new position for the tracker (dragging is finished)
#define EVT_TRACKER_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_TRACKER_CHANGED, id,\
 wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

/// Event fired when the user is being moving or resizing the tracker (dragging in process)
#define EVT_TRACKER_CHANGING(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_TRACKER_CHANGING, id,\
 wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),


#endif // __RECTTRACKER_H__
