/* ****************************************************************************
 * ListCtrlResize.cpp                                                         *
 *                                                                            *
 * (c) 2004-2008 - Rémi Peyronnet <remi+wx@via.ecp.fr>                        *
 *                                                                            *
 * Licence : wxWindows (based on L-GPL)                                       *
 *                                                                            *
 * ************************************************************************** */



#include <wx/wx.h>
#include <wx/tipwin.h>
#include <wx/tooltip.h>
#include "ListCtrlResize.h"


BEGIN_EVENT_TABLE(wxListCtrlResize, wxListCtrl)
  EVT_SIZE(wxListCtrlResize::OnSize)
  EVT_MOTION(wxListCtrlResize::OnMotion)
  // EVT_LIST_COL_DRAGGIND does not seem to work...
  EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, wxListCtrlResize::OnListBeginResize)
  EVT_LIST_COL_END_DRAG(wxID_ANY, wxListCtrlResize::OnListEndResize)
END_EVENT_TABLE()

void wxListCtrlResize::OnSize(wxSizeEvent & event)
{
	int ltot, lcib, i;

    // wxListCtrl::OnSize(event); // Linux
    event.Skip();

	ltot = 0;
	for (i = 0; i < this->GetColumnCount(); i++)
	{
		ltot += this->GetColumnWidth(i);
	}
	// To play with round up...
	lcib = event.GetSize().GetWidth() - this->GetColumnCount() - 2;
	// TODO : Get the proper size of a scrollbar ...
	if (this->GetCountPerPage() < this->GetItemCount()) lcib -= 19; 
	for (i = 0; i < this->GetColumnCount(); i++)
	{
		this->SetColumnWidth(i, this->GetColumnWidth(i) * lcib / ltot);
	}
}

/*
void wxListCtrlResize::OnListResize(wxListEvent & event)
{
	int len, i, curcol;
	len = event.GetPoint().x;
	curcol = event.GetColumn();
	for(i = 0; i < curcol; i++) len -= this->GetColumnWidth(i);
	this->SetColumnWidth(curcol + 1, this->GetColumnWidth(curcol + 1) +
		(len - this->GetColumnWidth(curcol)) );
}
*/

void wxListCtrlResize::OnListBeginResize(wxListEvent & event)
{
	lastWidth = this->GetColumnWidth(event.GetColumn());
}

void wxListCtrlResize::OnListEndResize(wxListEvent & event)
{
	this->SetColumnWidth(event.GetColumn() + 1, 
		this->GetColumnWidth(event.GetColumn() + 1) -
		(this->GetColumnWidth(event.GetColumn()) - lastWidth) );
}

void wxListCtrlResize::OnMotion(wxMouseEvent & event)
{
	int flags, item, col, xpos;
	wxListItem lit;
	item = this->HitTest(event.GetPosition() , flags);
	xpos = event.GetPosition().x;
	col = 0; while((col < this->GetColumnCount()) && ((xpos -= this->GetColumnWidth(col)) > 0)) col++;
	if ((flags & wxLIST_HITTEST_ONITEM) != 0)
	{
		lit.SetId(item);
		lit.SetColumn(col);
		lit.SetMask(wxLIST_MASK_TEXT);
		if (this->GetItem(lit))
		{
			// new wxTipWindow(this, lit.GetText()	);
			if (this->GetToolTip() != NULL)
			{
				if (lit.GetText() != this->GetToolTip()->GetTip())
				{
					this->GetToolTip()->Enable(false);
					this->GetToolTip()->Enable(true);
				}
			}
			this->SetToolTip(lit.GetText());
		}
	}
}
