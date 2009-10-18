/////////////////////////////////////////////////////////////////////////////
// Name:        VerticalToolbar.h
// Purpose:     wxVerticalToolbar class
// Author:      Alex Thuering
// Created:		11.03.2003
// RCS-ID:      $Id: VerticalToolbar.h 263 2005-11-21 23:14:19Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_VERTICAL_TOOLBAR_H
#define WXVILLALIB_VERTICAL_TOOLBAR_H

#include <wx/wx.h>
#include <wx/toolbar.h>

class wxVertButton
{
  public:
   wxVertButton(int toolId, const wxString& label, const wxBitmap& bitmap,
	 wxItemKind kind = wxITEM_NORMAL, const wxString& shortHelpString = wxT(""))
   {
     this->toolId = toolId; this->label = label;
     this->bitmap = bitmap; this->kind = kind;
	 this->shortHelpString = shortHelpString;
   }
   
   int toolId;
   wxString label;
   wxBitmap bitmap;
   wxItemKind kind;
   wxString shortHelpString;
};

WX_DECLARE_OBJARRAY(wxVertButton, wxVertButtons);

/** The class to emulate vertical toolbar */
class wxVerticalToolbar
{
  public:
    wxVerticalToolbar(wxToolBar* toolbar);
    ~wxVerticalToolbar() { }
    
    void AddTool(int toolId, const wxString& label, const wxBitmap& bitmap,
      wxItemKind kind = wxITEM_NORMAL,
	  const wxString& shortHelpString = wxT(""));
    void Update();
    
    void InsertTool(int pos, int toolId);
  
  protected:
    wxToolBar* m_toolbar;
    wxVertButtons m_buttons;
    int m_sw, m_sh;
    wxMemoryDC m_btDc;
    void InsertVerticalButton(int pos, wxVertButton& button);
};

#endif // WXVILLALIB_VERTICAL_TOOLBAR_H

