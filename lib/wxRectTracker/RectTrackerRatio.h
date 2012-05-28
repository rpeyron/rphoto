/* ****************************************************************************
 * RectTrackerRatio.h                                                         *
 *                                                                            *
 * (c) 2004-2012 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>                    *
 *                                                                            *
 * RectTracker was originally designed for RPhoto, but can be used separately *
 * It is a control similar to the CRectTracker of MS MFC.                     *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */


#ifndef __RECTTRACKERRATIO_H__
#define __RECTTRACKERRATIO_H__

#include "RectTracker.h"

/** Tracker with Ratio constraint.
 * 
 * This class adds a width/height constraint in the tracker. The user can only
 *  drag a tracker which respect this constraint. It uses basically :
 * - ratio : the selected ratio (0 if none)
 * - orientation : the desired orientation (0 : automatic, -1 : portrait, 1 : landscape)
 * - fixed size : to set a constant size which cannot be resized
 * - 4 guides line with a given radio (ex : 0 = none; 0.5 = middle ; 0.66 = third)
 *
 */
class wxRectTrackerRatio : public wxRectTracker
{
	DECLARE_CLASS(wxRectTrackerRatio)
public:
	wxRectTrackerRatio(wxWindow* parent, wxFrame * frame = NULL);
	virtual ~wxRectTrackerRatio();

public:
	// Manipulation function
	double GetRatio() { return ratio; };
	double GetGuideRatio() { return guideRatio; };
	int GetOrientation() { return orientation; };
	int GetFixedWidth() { return fixedWidth; }
	int GetFixedHeight() { return fixedHeight; }
	void SetRatio(double ratio);
	void SetGuideRatio(double ratio);
	void SetOrientation(int orientation);
	void SetFixedSize(int width, int height);

protected:
	// Behaviour Functions
	long CalcRectDist(wxRect r1, wxRect r2);
	wxRect CalcRectNearer(wxRect rr, wxRect r1, wxRect r2);
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	void AdjustTrackerRectRatio(wxRect & curRect, int handler, bool expand);
	void AdjustTrackerRectFixed(wxRect & curRect, int handler);
	// Helper Functions
	virtual void DrawRect(wxDC & dc, int x, int y, int w, int h);
	virtual void DrawTracker(wxDC & dc, int x, int y, int w, int h);

protected:
	/// The tracker should keep this ratio (0 : don't use)
	double ratio;
	/// Ratio of the 4 guide lines (ex : 0 = none; 0.5 = middle ; 0.66 = third)
	double guideRatio;
	/// Fixed width
	int fixedWidth;
	/// Fixed height
	int fixedHeight;
	/// Orientation : 0 : automatic, -1 : portrait, 1 : landscape
	int orientation;
};

#endif // __RECTTRACKERRATIO_H__
