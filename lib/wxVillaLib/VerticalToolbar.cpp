/////////////////////////////////////////////////////////////////////////////
// Name:        VerticalToolbar.cpp
// Purpose:     wxVerticalToolbar class
// Author:      Alex Thuering
// Created:		11.03.2003
// RCS-ID:      $Id: VerticalToolbar.cpp,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "VerticalToolbar.h"
#include <wx/image.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxVertButtons);

void wxVerticalToolbar::AddTool(
  int toolId, const wxString& label, const wxBitmap& bitmap,
  wxItemKind kind, const wxString& shortHelpString)
{
  m_buttons.Add(wxVertButton(toolId, label, bitmap, kind, shortHelpString));
}
    
void wxVerticalToolbar::Update()
{
  // calc sw, sh
  int i;
  m_sw = m_sh = 0;
  int sw1, sh1;
  for (i=0; i<(int)m_buttons.Count(); i++)
  {
    m_btDc.GetTextExtent(m_buttons[i].label, &sw1, &sh1);
    if (m_buttons[i].bitmap.Ok())
      sw1 += m_buttons[i].bitmap.GetWidth() + 5;
    if (sw1 > m_sw)
     m_sw = sw1;
    if (sh1 > m_sh)
     m_sh = sh1; 
  }
  m_sw += 4; m_sh += 4;
  m_toolbar->SetToolBitmapSize(wxSize(m_sh,m_sw));
  for (i=0; i<(int)m_buttons.Count(); i++)
    InsertVerticalButton(m_toolbar->GetToolsCount(), m_buttons[i]);
}

void wxVerticalToolbar::InsertTool(int pos, int toolId)
{
  for (int i=0; i<(int)m_buttons.Count(); i++)
    if (m_buttons[i].toolId == toolId)
      InsertVerticalButton(pos, m_buttons[i]);
}

void wxVerticalToolbar::InsertVerticalButton(int pos, wxVertButton& button)
{
  wxBitmap btBitmap;
  btBitmap.Create(m_sw, m_sh);
  m_btDc.SelectObject(btBitmap);
  m_btDc.SetBrush(wxBrush(m_toolbar->GetBackgroundColour(),wxSOLID));
  m_btDc.SetPen(wxPen(*wxBLACK,0,wxTRANSPARENT));
  m_btDc.DrawRectangle(0,0,m_sw,m_sh);
  int sw1, sh1;
  m_btDc.GetTextExtent(button.label, &sw1, &sh1);
  if (button.bitmap.Ok())
  {
    sw1 += button.bitmap.GetWidth() + 5;
    wxImage tmp = button.bitmap.ConvertToImage();
    wxColour cmask(tmp.GetRed(0,0), tmp.GetGreen(0,0), tmp.GetBlue(0,0));
    button.bitmap.SetMask(new wxMask(button.bitmap, cmask));
    m_btDc.DrawBitmap(button.bitmap, (m_sw-sw1)/2+2, 2, true);
    sw1 -= (button.bitmap.GetWidth() + 5)*2;
  }
  m_btDc.DrawText(button.label, (m_sw-sw1)/2+2, 2);
  m_btDc.SelectObject(wxNullBitmap);
  btBitmap = wxBitmap(btBitmap.ConvertToImage().Rotate90(false));
  //m_toolbar->AddCheckTool(button.toolId, button.label, btBitmap);
  m_toolbar->InsertTool(pos, button.toolId, button.label, btBitmap,
	wxNullBitmap, button.kind, button.shortHelpString);
}

