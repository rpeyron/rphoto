/* ****************************************************************************
 * ListCtrlResize.h                                                           *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+wx@via.ecp.fr>                        *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */


#ifndef __LISTCTRLRESIZE_H__
#define __LISTCTRLRESIZE_H__

#include <wx/listctrl.h>

class wxListCtrlResize : public wxListCtrl
{

public:
	wxListCtrlResize(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("listCtrl"))
		: wxListCtrl(parent, id, pos, size, style, validator, name)
	{
	}
	virtual ~wxListCtrlResize() {}

public:
	void OnSize(wxSizeEvent & event);
	void OnListBeginResize(wxListEvent & event);
	void OnListEndResize(wxListEvent & event);
	void OnMotion(wxMouseEvent & event);
	DECLARE_EVENT_TABLE()

protected:
	int lastWidth;

};


#endif // __LISTCTRLRESIZE_H__
