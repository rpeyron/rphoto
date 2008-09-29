/////////////////////////////////////////////////////////////////////////////
// Name:        TablessNotebook.h
// Purpose:     wxTablessNotebook class
// Author:      Alex Thuering (a modifiecation of generic/notebook.h)
// Created:     06.10.03
// RCS-ID:      $Id: TablessNotebook.h,v 1.8 2004/03/21 12:50:30 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_WX_TABLESSNOTEBOOK_H_
#define WXVILLALIB_WX_TABLESSNOTEBOOK_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/dynarray.h"
#include "wx/event.h"
#include "wx/control.h"
#include "wx/panel.h"
#include "wx/generic/tabg.h"
#include "wx/notebook.h"

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxTablessNotebook
// ----------------------------------------------------------------------------
#if wxCHECK_VERSION(2,5,0)
#define size_tt size_t
#else
#define size_tt int
#endif
/** The widget to display a tabless notebook */
class wxTablessNotebook : public wxNotebookBase
{
public:
  // ctors
  // -----
    // default for dynamic class
  wxTablessNotebook();
    // the same arguments as for wxControl (@@@ any special styles?)
  wxTablessNotebook(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = _T("notebook"));
    // Create() function
  bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = _T("notebook"));
    // dtor
  ~wxTablessNotebook();

  // accessors
  // ---------
  // Find the position of the wxNotebookPage, -1 if not found.
  int FindPagePosition(wxNotebookPage* page) const;

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(size_tt nPage);
    // cycle thru the tabs
  //  void AdvanceSelection(bool bForward = TRUE);
    // get the currently selected page
  int GetSelection() const { return m_nSelection; }

    // set/get the title of a page
  bool SetPageText(size_tt n, const wxString& strText) { return true; };
  wxString GetPageText(size_tt n) const { return wxEmptyString; }

  // get the number of rows for a control with wxNB_MULTILINE style (not all
  // versions support it - they will always return 1 then)
  virtual int GetRowCount() const ;

  // sets/returns item's image index in the current image list
  int GetPageImage(size_tt n) const { return 0; }
  bool SetPageImage(size_tt n, int imageId) { return true; }

  // control the appearance of the notebook pages
    // set the size (the same for all pages)
  void SetPageSize(const wxSize& size) {}
    // set the padding between tabs (in pixels)
  void SetPadding(const wxSize& padding) {}

    // Sets the size of the tabs (assumes all tabs are the same size)
  void SetTabSize(const wxSize& sz) {}

  // operations
  // ----------
    // remove one page from the notebook, and delete the page.
  bool DeletePage(size_tt nPage);
  bool DeletePage(wxNotebookPage* page);
    // remove one page from the notebook, without deleting the page.
  bool RemovePage(size_tt nPage);
  bool RemovePage(wxNotebookPage* page);
    // remove all pages
  bool DeleteAllPages();
    // the same as AddPage(), but adds it at the specified position
  bool InsertPage(size_tt nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1);

  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnIdle(wxIdleEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  void OnSetFocus(wxFocusEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);

  // base class virtuals
  // -------------------
  virtual void Command(wxCommandEvent& event);
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool DoPhase(int nPhase);
  
  // Implementation

  void OnMouseEvent(wxMouseEvent& event);
  void OnPaint(wxPaintEvent& event);

  virtual wxRect GetAvailableClientSize();

  // Implementation: calculate the layout of the view rect
  // and resize the children if required
  bool RefreshLayout(bool force = TRUE);

protected:
  // remove the page and return a pointer to it
  wxWindow *DoRemovePage(size_t page) { return NULL; }
	
  // common part of all ctors
  void Init();

  // helper functions
  void ChangePage(int nOldSel, int nSel); // change pages

  int m_nSelection;           // the current selection (-1 if none)

  DECLARE_DYNAMIC_CLASS(wxTablessNotebook)
  DECLARE_EVENT_TABLE()
};

#endif // WXVILLALIB_WX_TABLESSNOTEBOOK_H_
