/* ****************************************************************************
 * ListCtrlResize.h                                                           *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * Copyright (C) 2006 - Rémi Peyronnet <remi+wx@via.ecp.fr>                   *
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
