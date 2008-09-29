/* ****************************************************************************
 * RectTrackerRatio.h                                                         *
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


#ifndef __RECTTRACKERRATIO_H__
#define __RECTTRACKERRATIO_H__

#include "RectTracker.h"

/** Tracker with Ratio constraint.
 * 
 * This class adds a width/height constraint in the tracker. The user can only
 *  drag a tracker which respect this constraint. It uses basically :
 * - ratio : the selected ratio (0 if none)
 * - orientation : the desired orientation (0 : automatic, -1 : portrait, 1 : landscape)
 */
class wxRectTrackerRatio : public wxRectTracker
{
	DECLARE_CLASS(wxRectTrackerRatio)
public:
	wxRectTrackerRatio(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, wxRectTrackerHost * rectTrackerHost = NULL);
	~wxRectTrackerRatio();

public:
	// Manipulation function
	double GetRatio() { return ratio; };
	int GetOrientation() { return orientation; };
	int GetFixedWidth() { return fixedWidth; }
	int GetFixedHeight() { return fixedHeight; }
	void SetRatio(double ratio);
	void SetOrientation(int orientation);
	void SetFixedSize(int width, int height);

protected:
	// Behaviour Functions
	long CalcRectDist(wxRect r1, wxRect r2);
	wxRect CalcRectNearer(wxRect rr, wxRect r1, wxRect r2);
	virtual void AdjustTrackerRect(wxRect & curRect, int handler);
	void AdjustTrackerRectRatio(wxRect & curRect, int handler, bool expand);
	void AdjustTrackerRectFixed(wxRect & curRect, int handler);

protected:
	/// ratio : the tracker should keep this ratio (0 : don't use)
	double ratio;
	int fixedWidth;
	int fixedHeight;
	/// orientation : 0 : automatic, -1 : portrait, 1 : landscape
	int orientation;
};

#endif // __RECTTRACKERRATIO_H__
