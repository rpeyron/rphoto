///////////////////////////////////////////////////////////////////////////////
// Name:        TablessNotebook.cpp
// Purpose:     implementation of wxTablessNotebook
// Author:      Alex Thuering (a modifiecation of generic/notebook.h)
// Created:     06.10.03
// RCS-ID:      $Id: TablessNotebook.cpp,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include  "TablessNotebook.h"

#include  "wx/string.h"
#include  "wx/log.h"
#include  "wx/settings.h"
#include  "wx/generic/imaglist.h"
#include  "wx/dcclient.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxTablessNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxTablessNotebook::OnSelChange)
    EVT_SIZE(wxTablessNotebook::OnSize)
    EVT_PAINT(wxTablessNotebook::OnPaint)
    EVT_MOUSE_EVENTS(wxTablessNotebook::OnMouseEvent)
    EVT_SET_FOCUS(wxTablessNotebook::OnSetFocus)
    EVT_NAVIGATION_KEY(wxTablessNotebook::OnNavigationKey)
//    EVT_IDLE(wxTablessNotebook::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTablessNotebook, wxControl)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTablessNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxTablessNotebook::Init()
{
    m_nSelection = -1;
}

// default for dynamic class
wxTablessNotebook::wxTablessNotebook()
{
    Init();
}

// the same arguments as for wxControl
wxTablessNotebook::wxTablessNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();

    Create(parent, id, pos, size, style, name);
}

// Create() function
bool wxTablessNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    // base init
    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    // It's like a normal window...
    if (!wxWindow::Create(parent, id, pos, size, style|wxNO_BORDER, name))
        return FALSE;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    return TRUE;
}

// dtor
wxTablessNotebook::~wxTablessNotebook()
{
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------
int wxTablessNotebook::GetRowCount() const
{
    // TODO
    return 0;
}

int wxTablessNotebook::SetSelection(int nPage)
{
    if (nPage == -1)
      return 0;

    wxASSERT( IS_VALID_PAGE(nPage) );
	
	if (m_nSelection != nPage)
	  ChangePage(m_nSelection, nPage);
	
    return 0;
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook and delete it
bool wxTablessNotebook::DeletePage(int nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), FALSE );

    if (m_nSelection != -1)
    {
        m_pages[m_nSelection]->Show(FALSE);
        m_pages[m_nSelection]->Lower();
    }

    wxNotebookPage* pPage = GetPage(nPage);

    m_pages.Remove(pPage);
    delete pPage;
	
	if (m_pages.GetCount() == 0)
    {
      m_nSelection = -1;
    }
    else if (m_nSelection > -1)
    {
      // Only change the selection if the page we
      // deleted was the selection.
      if (nPage == m_nSelection)
      {
         m_nSelection = -1;
         // Select the first tab. Generates a ChangePage.
         SetSelection(0);
      }
      else
      {
		// We must adjust which tab we think is selected.
        // If greater than the page we deleted, it must be moved down
        // a notch.
        if (m_nSelection > nPage)
          m_nSelection -- ;
      }
    }

    RefreshLayout(FALSE);

    return TRUE;
}

bool wxTablessNotebook::DeletePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return DeletePage(pagePos);
    else
        return FALSE;
}

// remove one page from the notebook
bool wxTablessNotebook::RemovePage(int nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), FALSE );

    m_pages[nPage]->Show(FALSE);
    //    m_pages[nPage]->Lower();

    wxNotebookPage* pPage = GetPage(nPage);
    m_pages.Remove(pPage);

    if (m_pages.GetCount() == 0)
    {
      m_nSelection = -1;
    }
    else if (m_nSelection > -1)
    {
      // Only change the selection if the page we
      // deleted was the selection.
      if (nPage == m_nSelection)
      {
         m_nSelection = -1;
         // Select the first tab. Generates a ChangePage.
         SetSelection(0);
      }
      else
      {
		// We must adjust which tab we think is selected.
        // If greater than the page we deleted, it must be moved down
        // a notch.
        if (m_nSelection > nPage)
          m_nSelection -- ;
      }
    }

    RefreshLayout(FALSE);

    return TRUE;
}

bool wxTablessNotebook::RemovePage(wxNotebookPage* page)
{
    int pagePos = FindPagePosition(page);
    if (pagePos > -1)
        return RemovePage(pagePos);
    else
        return FALSE;
}

// Find the position of the wxNotebookPage, -1 if not found.
int wxTablessNotebook::FindPagePosition(wxNotebookPage* page) const
{
    int nPageCount = GetPageCount();
    int nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        if (m_pages[nPage] == page)
            return nPage;
    return -1;
}

// remove all pages
bool wxTablessNotebook::DeleteAllPages()
{
    int nPageCount = GetPageCount();
    int nPage;
    for ( nPage = 0; nPage < nPageCount; nPage++ )
        delete m_pages[nPage];

    m_pages.Clear();

    return TRUE;
}

// same as AddPage() but does it at given position
bool wxTablessNotebook::InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxASSERT( pPage != NULL );
    wxCHECK( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), FALSE );

    if (!bSelect)
      pPage->Show(FALSE);

    // save the pointer to the page
    m_pages.Insert(pPage, nPage);

    if (bSelect)
    {
        // This will cause ChangePage to be called, via OnSelPage
        SetSelection(nPage);
    }

    // some page must be selected: either this one or the first one if there is
    // still no selection
    if ( m_nSelection == -1 )
      ChangePage(-1, 0);

    RefreshLayout(FALSE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxTablessNotebook::OnSize(wxSizeEvent& event)
{
    static bool s_bFirstTime = TRUE;
    if ( s_bFirstTime ) {
        // TODO: any first-time-size processing.
        s_bFirstTime = FALSE;
    }

    RefreshLayout();

    // Processing continues to next OnSize
    event.Skip();
}

// This was supposed to cure the non-display of the notebook
// until the user resizes the window.
// What's going on?
void wxTablessNotebook::OnIdle(wxIdleEvent& event)
{
    static bool s_bFirstTime = TRUE;
    if ( s_bFirstTime ) {
      /*
      wxSize sz(GetSize());
      sz.x ++;
      SetSize(sz);
      sz.x --;
      SetSize(sz);
      */

      /*
      wxSize sz(GetSize());
      wxSizeEvent sizeEvent(sz, GetId());
      sizeEvent.SetEventObject(this);
      GetEventHandler()->ProcessEvent(sizeEvent);
      Refresh();
      */
      s_bFirstTime = FALSE;
    }
    event.Skip();
}

// Implementation: calculate the layout of the view rect
// and resize the children if required
bool wxTablessNotebook::RefreshLayout(bool force)
{
	int cw, ch;
	GetClientSize(&cw, &ch);

	wxRect rect(0,0,cw,ch);

	if (!force)
	  return FALSE;

	// fit the notebook page to the tab control's display area
	unsigned int nCount = m_pages.Count();
	for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
		wxNotebookPage *pPage = m_pages[nPage];
		if (pPage->IsShown())
		{
			wxRect clientRect = GetAvailableClientSize();
			pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);
			if ( pPage->GetAutoLayout() )
			   pPage->Layout();
		}
	}
	Refresh();
    return TRUE;
}

void wxTablessNotebook::OnSelChange(wxNotebookEvent& event)
{
    // is it our tab control?
    if ( event.GetEventObject() == this )
    {
        if (event.GetSelection() != m_nSelection)
          ChangePage(event.GetOldSelection(), event.GetSelection());
    }

    // we want to give others a chance to process this message as well
    event.Skip();
}

void wxTablessNotebook::OnSetFocus(wxFocusEvent& event)
{
    // set focus to the currently selected page if any
    if ( m_nSelection != -1 )
        m_pages[m_nSelection]->SetFocus();

    event.Skip();
}

void wxTablessNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        AdvanceSelection(event.GetDirection());
    }
    else {
        // pass to the parent
        if ( GetParent() ) {
            event.SetCurrentFocus(this);
            GetParent()->ProcessEvent(event);
        }
    }
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

// override these 2 functions to do nothing: everything is done in OnSize

void wxTablessNotebook::SetConstraintSizes(bool /* recurse */)
{
    // don't set the sizes of the pages - their correct size is not yet known
    wxControl::SetConstraintSizes(FALSE);
}

bool wxTablessNotebook::DoPhase(int /* nPhase */)
{
    return TRUE;
}

void wxTablessNotebook::Command(wxCommandEvent& WXUNUSED(event))
{
    wxFAIL_MSG("wxTablessNotebook::Command not implemented");
}

// ----------------------------------------------------------------------------
// wxNotebook helper functions
// ----------------------------------------------------------------------------

// hide the currently active panel and show the new one
void wxTablessNotebook::ChangePage(int nOldSel, int nSel)
{
    wxASSERT( nOldSel != nSel ); // impossible

    if ( nOldSel != -1 ) {
        m_pages[nOldSel]->Show(FALSE);
        m_pages[nOldSel]->Lower();
    }

    wxNotebookPage *pPage = m_pages[nSel];

    wxRect clientRect = GetAvailableClientSize();
    pPage->SetSize(clientRect.x, clientRect.y, clientRect.width, clientRect.height);

    Refresh();

    pPage->Show(TRUE);
    pPage->Raise();
    pPage->SetFocus();

    m_nSelection = nSel;
}

void wxTablessNotebook::OnMouseEvent(wxMouseEvent& event)
{
}

void wxTablessNotebook::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
}

wxRect wxTablessNotebook::GetAvailableClientSize()
{
    int cw, ch;
    GetClientSize(& cw, & ch);
    wxRect rect(0,0,cw,ch);

    return rect;
}

