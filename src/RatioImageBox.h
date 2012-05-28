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

#include <wxVillaLib/ImageBox.h>
#include <wxRectTracker/RectTrackerRatio.h>
#include <wxRectTracker/LineTracker.h>

const int scFIT_TO_PAGE_IF_LARGER = -4;

const int RIB_STATE_NONE = 0;
const int RIB_STATE_CROP = 1;
const int RIB_STATE_INCLINAISON = 2;

class wxRatioImageBox : public wxImageBox
{
public:
    DECLARE_CLASS(wxRatioImageBox)
	wxRatioImageBox(wxWindow* parent, wxWindowID id);
	~wxRatioImageBox(void);

	// wxRectTracker wrapper
	wxRectTrackerRatio & GetRectTracker() { return *rectTracker;  }
	wxLineTracker & GetLineTracker() { return *lineTracker; }
    wxRect GetSelectedZone();
	double GetRatio() { return rectTracker->GetRatio(); };
	double GetGuideRatio() { return rectTracker->GetGuideRatio(); };
	int GetFixedWidth() { return rectTracker->GetFixedWidth(); }
	int GetFixedHeight() { return rectTracker->GetFixedHeight(); }
	wxRect GetMaxRect() { return rectTracker->GetMaxRect(); };
	int GetOrientation() { return rectTracker->GetOrientation(); };
	void SetRatio(double ratio) { rectTracker->SetRatio(ratio); rectTracker->Update(); };
	void SetGuideRatio(double ratio) { rectTracker->SetGuideRatio(ratio); rectTracker->Update(); };
	void SetFixedSize(int width, int height) { rectTracker->SetFixedSize(width, height); rectTracker->Update(); };
	void SetMaxRect(wxRect maxRect) { rectTracker->SetMaxRect(maxRect); if (lineTracker) lineTracker->SetMaxRect(maxRect); };
	void SetOrientation(int orientation) { if (rectTracker) rectTracker->SetOrientation(orientation); rectTracker->Update(); };
	void SetScale(double scale = scFIT_TO_PAGE);
	void TrackerReset() 
	{ 
		if (rectTracker) rectTracker->SetUnscrolledRect(wxRect(0,0,0,0)); 
		if (lineTracker) lineTracker->SetUnscrolledRect(wxRect(0,0,0,0)); 
		Refresh(); 
		wxCommandEvent evt(0,0); 
		OnTrackerChanged(evt); 
	}
	void SetModeInclinaison(bool state);
	bool IsModeInclinaison() { return ((m_state & RIB_STATE_INCLINAISON) != 0); }

    void SetImage(const wxImage & image);

    void UpdateTracker();

    // Overrides
	virtual void UpdatePaintBox();
	bool LoadFile(wxString fname);


protected:
	wxRectTrackerRatio * rectTracker;
	wxLineTracker * lineTracker;
	int captured;
	wxPoint firstClick;
	wxPoint lastPoint;
	int m_state;

    DECLARE_EVENT_TABLE()
	// Misc Events
	void OnMouseMotion(wxMouseEvent & event);
	void OnMouseLeftDown(wxMouseEvent & event);
	void OnMouseLeftUp(wxMouseEvent & event);
	void OnMouseRightDown(wxMouseEvent & event);
	void OnMouseRightUp(wxMouseEvent & event);
	void OnResize(wxSizeEvent & event);
	void OnTrackerChanged(wxCommandEvent & event);

protected:

	void AdjustTrackerSize(double ratio);

    void PaintDC(wxDC & dc) 
    { 
      int vx, vy;
      GetViewStart(&vx, &vy); /*PrepareDC(dc);*/ 
      dc.DrawBitmap(*m_buffer, m_bufferX - vx, m_bufferY - vy); 
    }

	/*
    class ratioImageBoxHost : public wxRectTrackerHost
    {
    public:
        ratioImageBoxHost(wxRatioImageBox & owner) : m_pOwner(owner) {};
        virtual ~ratioImageBoxHost() {};
        virtual void PaintDC(wxDC & dc) { m_pOwner.PaintDC(dc); }
    protected:
        wxRatioImageBox & m_pOwner;
    };
    ratioImageBoxHost m_Host;
	friend class ratioImageBoxHost;
	*/
};


#endif // __RATIOIMAGEBOX_H__
