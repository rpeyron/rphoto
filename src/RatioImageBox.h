/* ****************************************************************************
 * RatioImageBox.h                                                            *
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


#ifndef __RATIOIMAGEBOX_H__
#define __RATIOIMAGEBOX_H__

#include "../lib/wxVillaLib/ImageBox.h"
#include "RectTracker.h"

const int scFIT_TO_PAGE_IF_LARGER = -4;

class wxRatioImageBox : public wxImageBox 
{
public:
    DECLARE_CLASS(wxRatioImageBox)
	wxRatioImageBox(wxWindow* parent, wxWindowID id);
	~wxRatioImageBox(void);

	// wxRectTracker wrapper
	wxRectTracker & GetRectTracker() { return *rectTracker; }
    wxRect GetSelectedZone();
	double GetRatio() { return rectTracker->GetRatio(); };
	wxRect GetMaxRect() { return rectTracker->GetMaxRect(); };
	int GetOrientation() { return rectTracker->GetOrientation(); };
	void SetRatio(double ratio) { rectTracker->SetRatio(ratio); };
	void SetMaxRect(wxRect maxRect) { rectTracker->SetMaxRect(maxRect); };
	void SetOrientation(int orientation) { rectTracker->SetOrientation(orientation); };
	void SetScale(double scale = scFIT_TO_PAGE);
	void TrackerReset() { rectTracker->SetUnscrolledRect(wxRect(0,0,0,0)); Refresh(); wxCommandEvent evt(0,0); OnTrackerChanged(evt); }

    void SetImage(const wxImage & image);

#ifdef RT_GTK_HACK
    // Berk. This ugly function to provide GTK+ compatibility
    // It aims at providing pseudo support for transparent areas...
    virtual void PaintDC(wxDC & dc) { dc.DrawBitmap(*m_buffer, -m_bufferX, -m_bufferY); }
#endif

    void UpdateTracker();

    // Overrides
	virtual void UpdatePaintBox();
	bool LoadFile(wxString fname);


protected:
	wxRectTracker * rectTracker;
	int captured;
	wxPoint firstClick;

    DECLARE_EVENT_TABLE()
	// Misc Events
	void OnMouseMotion(wxMouseEvent & event);
	void OnMouseLeftDown(wxMouseEvent & event);
	void OnMouseRightDown(wxMouseEvent & event);
	void OnMouseRightUp(wxMouseEvent & event);
	void OnResize(wxSizeEvent & event);
	void OnTrackerChanged(wxCommandEvent & event);
};


#endif // __RATIOIMAGEBOX_H__
