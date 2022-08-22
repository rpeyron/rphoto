/////////////////////////////////////////////////////////////////////////////
// Name:        PropDlg.cpp
// Purpose:     Properties dialog (base class for all properties dialogs)
// Author:      Alex Thuering
// Created:     18.11.2003
// RCS-ID:      $Id: PropDlg.cpp 365 2008-06-21 23:15:43Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#include "PropDlg.h"
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/grid.h>
#include <wx/spinctrl.h>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// ColourPanel ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class ColourPanel: public wxPanel
{
  public:
	ColourPanel(wxWindow* parent, wxWindowID id): wxPanel(parent, id,
	  wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL)
	{
	  m_bgColour = GetBackgroundColour();
	}
	
	inline void SetColour(wxColour colour)
	{
	  m_colour = colour;
	  if (colour.Ok())
		SetBackgroundColour(colour);
	  else
		SetBackgroundColour(m_bgColour);
	  Refresh();
	}
	
	inline wxColour GetColour()	{ return m_colour; }
	
  protected:
	wxColour m_colour;
	wxColour m_oldColour;
	wxColour m_bgColour;
	
	void OnPaint(wxPaintEvent &event)
	{
	  wxPaintDC dc(this);  
	  PrepareDC(dc);
	  if (!m_colour.Ok())
	  {
		dc.SetPen(*wxBLACK_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		int w = GetClientSize().GetWidth();
		int h = GetClientSize().GetHeight();
		dc.DrawLine(0, 0, w, h);
		dc.DrawLine(w, 0, 0, h);
	  }
	}
	
	void OnClick(wxMouseEvent &event)
	{
	  if (m_colour.Ok())
	  {
		m_oldColour = GetColour();
		SetColour(wxColour());
	  }
	  else
		SetColour(m_oldColour);
	  Refresh();
	}
	
  private:
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ColourPanel, wxPanel)
  EVT_PAINT(ColourPanel::OnPaint)
  EVT_LEFT_DOWN(ColourPanel::OnClick)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////////
///////////////////////////// wxPropDlg //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum
{
  RESET_BT_ID = 7830,
  SELECT_FONT_BT_ID,
  SELECT_COLOR_BT_ID,
  SELECT_FILE_BT_ID,
  SELECT_DIR_BT_ID,
  GROUP_CHECK_ID,
  ROW_DELETE_ID
};

BEGIN_EVENT_TABLE(wxPropDlg, wxDialog)
  EVT_BUTTON(wxID_CANCEL, wxPropDlg::OnCancel)
  EVT_BUTTON(wxID_OK, wxPropDlg::OnOk)
  EVT_BUTTON(RESET_BT_ID, wxPropDlg::OnReset)
  EVT_BUTTON(SELECT_FONT_BT_ID, wxPropDlg::OnSelectFont)
  EVT_BUTTON(SELECT_COLOR_BT_ID, wxPropDlg::OnSelectColour)
  EVT_BUTTON(SELECT_FILE_BT_ID, wxPropDlg::OnSelectFile)
  EVT_BUTTON(SELECT_DIR_BT_ID, wxPropDlg::OnSelectDir)
  EVT_CHECKBOX(GROUP_CHECK_ID, wxPropDlg::OnGroupCheck)
  EVT_GRID_CELL_LEFT_CLICK(wxPropDlg::OnCellLeftClick)
  EVT_GRID_CELL_RIGHT_CLICK(wxPropDlg::OnCellRightClick)
  EVT_GRID_CELL_CHANGE(wxPropDlg::OnCellChange)
  EVT_MENU(ROW_DELETE_ID, wxPropDlg::OnRowDelete)
END_EVENT_TABLE()

enum PropertyType
{
  propTEXT = 0,
  propSTAT_TEXT,
  propSPIN,
  propCOMBO,
  propCHOICE,
  propCHECK,
  propRADIO,
  propRADIO_GROUP,
  propGRID,
  propCOLOUR,
  propFONT
};

wxPropDlg::wxPropDlg(wxWindow* parent): wxDialog(parent, -1,
  _("Properties"), wxDefaultPosition, wxDefaultSize,
  wxDEFAULT_DIALOG_STYLE| wxRESIZE_BORDER)
{
  m_currGroupId = 0;
}

void wxPropDlg::Create(bool resetButton)
{
  propWindow = this;
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* propSizer = new wxBoxSizer(wxVERTICAL);
  CreatePropPanel(propSizer);
  sizer->Add(propSizer, 1, wxEXPAND|wxALL, 10);
  CreateButtonPane(sizer, resetButton);
  SetAutoLayout(true);
  SetSizer(sizer);
  sizer->Fit(this);
  sizer->SetSizeHints(this);
  Layout();
  Centre();
}

wxPropDlg::~wxPropDlg()
{
  for (int i=0; i<(int)m_types.GetCount(); i++)
	switch(m_types[i])
	{
	  case propFONT:
		delete (wxFontData*) m_controls[i];
		break;
	  case propRADIO_GROUP:
	    delete (wxArrayPtrVoid*) m_controls[i];
	    break;
	}
}

void wxPropDlg::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

void wxPropDlg::OnOk(wxCommandEvent &WXUNUSED(event))
{
  if (SetValues())
    EndModal(wxID_OK);
}

void wxPropDlg::OnReset(wxCommandEvent& event)
{
  Reset();
}

void wxPropDlg::CreatePropPanel(wxSizer* sizer)
{
}

void wxPropDlg::CreateButtonPane(wxSizer* sizer, bool resetButton)
{
  wxBoxSizer* buttonPane = new wxBoxSizer(wxHORIZONTAL);
  buttonPane->Add(10, 10, 1, wxEXPAND);
  wxButton* okBt = new wxButton(this, wxID_OK, _("OK"));
  okBt->SetDefault();
#if defined(__WXGTK__) && wxCHECK_VERSION(2,5,2) && !wxCHECK_VERSION(2,5,4)
  okBt->SetSizeHints(-1, okBt->GetSize().GetY()+8);
#endif
  buttonPane->Add(okBt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
  buttonPane->Add(8, 0);
  wxButton* cancelBt = new wxButton(this, wxID_CANCEL, _("Cancel"));
  buttonPane->Add(cancelBt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
  if (resetButton)
  {
    buttonPane->Add(8, 0);
    wxButton* resetBt = new wxButton(this, RESET_BT_ID, _("Reset"));
    buttonPane->Add(resetBt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
  }
  buttonPane->Add(10, 10, 1, wxEXPAND);
  sizer->Add(buttonPane, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 10);
}

void wxPropDlg::Reset()
{
  m_updateIndex = 0;
  CreatePropPanel(NULL);
}

void wxPropDlg::AddText(wxSizer* sizer, wxString text)
{
  if (!sizer)
	return;
  // add static text
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  else
	sizer->Add(10,10);
}

void wxPropDlg::AddTextProp(wxSizer* sizer, wxString text, wxString defValue,
  bool readonly)
{
  if (!sizer) // only update value
  {
	wxTextCtrl* ctrl = (wxTextCtrl*) m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	ctrl->SetEditable(!readonly);
	m_updateIndex++;
	return;
  }
  // add text control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxTextCtrl* ctrl = new wxTextCtrl(propWindow, -1, defValue);
  ctrl->SetEditable(!readonly);
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propTEXT);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddStaticTextProp(wxSizer* sizer, wxString text, wxString defValue)
{
  if (!sizer) // only update value
  {
	((wxStaticText*)m_controls[m_updateIndex])->SetLabel(defValue);
	m_updateIndex++;
	return;
  }
  // add static text
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxStaticText* ctrl = new wxStaticText(propWindow, -1, defValue);
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propSTAT_TEXT);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddSpinProp(wxSizer* sizer, wxString text, int defValue,
  int min, int max)
{
  if (!sizer) // only update value
  {
	wxSpinCtrl* ctrl = (wxSpinCtrl*) m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	m_updateIndex++;
	return;
  }
  // add text control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxSpinCtrl* ctrl = new wxSpinCtrl(propWindow);
  ctrl->SetRange(min, max);
  ctrl->SetValue(defValue);
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propSPIN);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddCheckProp(wxSizer* sizer, wxString text, bool defValue,
  bool readonly, int id)
{
  if (!sizer) // only update value
  {
	wxCheckBox* ctrl = (wxCheckBox*)m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	ctrl->Enable(!readonly);
	m_updateIndex++;
	return;
  }
  // add checkbox control
  wxCheckBox* ctrl = new wxCheckBox(propWindow, id, text);
  ctrl->SetValue(defValue);
  ctrl->Enable(!readonly);
  ctrl->SetName(wxString::Format(_T("%d"), m_controls.GetCount()));
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propCHECK);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddRadioProp(wxSizer* sizer, wxString text, bool defValue,
	  long style, bool readonly, int id)
{
  if (!sizer) // only update value
  {
	wxRadioButton* ctrl = (wxRadioButton*)m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	ctrl->Enable(!readonly);
	m_updateIndex++;
	return;
  }
  // add radio button
  wxRadioButton* ctrl = new wxRadioButton(propWindow, id, text,
	 wxDefaultPosition, wxDefaultSize, style);
  ctrl->SetValue(defValue);
  ctrl->Enable(!readonly);
  ctrl->SetName(wxString::Format(_T("%d"), m_controls.GetCount()));
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propRADIO);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddRadioGroupProp(wxSizer* sizer, const wxArrayString& text,
	int defValue, bool readonly)
{
  if (!sizer) // only update value
  {
  	wxArrayPtrVoid* radioControls = (wxArrayPtrVoid*) m_controls[m_updateIndex];
  	for (int i=0; i<(int)radioControls->GetCount(); i++)
  	{
	  wxRadioButton* ctrl = (wxRadioButton*)(*radioControls)[i];
	  ctrl->SetValue(i == defValue);
	  ctrl->Enable(!readonly);
  	}
	m_updateIndex++;
	return;
  }
  wxArrayPtrVoid* radioControls = new wxArrayPtrVoid();
  for (int i=0; i<(int)text.GetCount(); i++) {
	  // add checkbox control
	  wxRadioButton* ctrl = new wxRadioButton(propWindow, -1, text[i],
		 wxDefaultPosition, wxDefaultSize, i == 0 ? wxRB_GROUP : wxRB_SINGLE);
	  ctrl->SetValue(i == defValue);
	  ctrl->Enable(!readonly);
	  ctrl->SetName(wxString::Format(_T("%d_%d"), m_controls.GetCount(), i));
	  sizer->Add(ctrl, 1, wxEXPAND, 0);
	  radioControls->Add(ctrl);
  }
  m_types.Add(propRADIO_GROUP);
  m_controls.Add(radioControls);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::AddComboProp(wxSizer* sizer, const wxString& text,
  const wxString& defValue, const wxArrayString& choices, long style)
{
  if (!sizer) // only update value
  {
	wxComboBox* ctrl = (wxComboBox*)m_controls[m_updateIndex];
	ctrl->Clear();
	SetComboData(ctrl, defValue, choices);
	m_updateIndex++;
	return;
  }
  // add combobox control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxString strings[1];
  wxComboBox* ctrl = new wxComboBox(propWindow, -1, defValue,
    wxDefaultPosition, wxDefaultSize, 0, strings, style);
  SetComboData(ctrl, defValue, choices);
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propCOMBO);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::SetComboData(wxComboBox* ctrl, const wxString& defValue,
  const wxArrayString& choices)
{
  int sel = -1;
  for (int i=0; i<(int)choices.GetCount(); i++)
  {
	ctrl->Append(choices[i]);
	if (choices[i] == defValue)
	  sel = i;
  }
  ctrl->SetSelection(sel);
  ctrl->SetValue(defValue);
}

void wxPropDlg::AddChoiceProp(wxSizer* sizer, const wxString& text,
  const wxString& defValue, const wxArrayString& choices, int id)
{
  if (!sizer) // only update value
  {
	wxChoice* ctrl = (wxChoice*)m_controls[m_updateIndex];
	ctrl->Clear();
	SetChoiceData(ctrl, defValue, choices);
	m_updateIndex++;
	return;
  }
  // add combobox control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxString strings[1];
  wxChoice* ctrl = new wxChoice(propWindow, id,
    wxDefaultPosition, wxDefaultSize, 0, strings);
  SetChoiceData(ctrl, defValue, choices);
  sizer->Add(ctrl, 1, wxEXPAND, 0);
  m_types.Add(propCHOICE);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::SetChoiceData(wxChoice* ctrl, const wxString& defValue,
  const wxArrayString& choices)
{
  int sel = -1;
  for (int i=0; i<(int)choices.GetCount(); i++)
  {
	ctrl->Append(choices[i]);
	if (choices[i] == defValue)
	  sel = i;
  }
  ctrl->SetSelection(sel);
}

//--- Grid property ---//
void wxPropDlg::AddGridProp(wxSizer* sizer, const wxArrayPtrVoid& data,
  const wxString& rowTitle, bool editable)
{
  if (!sizer) // only update value
  {
	wxGrid* grid = (wxGrid*) m_controls[m_updateIndex];
	grid->ClearGrid();
	SetGridData(grid, data, rowTitle, editable);
	m_updateIndex++;
	return;
  }
  wxGrid* grid = new wxGrid(propWindow, -1, wxPoint( 0, 0 ), wxSize(100, 100));
  grid->CreateGrid(0,0);
  SetGridData(grid, data, rowTitle, editable);

  // todo: autosize RowLableSize
  int width = 50;
  grid->SetRowLabelSize(50);  
  //grid->SetRowSize(0, 60 );
  //grid->SetColSize(0, 120);
  //grid->AutoSize();
  grid->Fit();
  wxArrayString* cols = (wxArrayString*) data.Item(0);
  for (int c = 0; c<(int)cols->GetCount(); c++)
  	width += grid->GetColSize(c);
  grid->SetSize(width+20,100);

  sizer->Add(grid, 1, wxEXPAND);
  m_types.Add(propGRID);
  m_controls.Add(grid);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::SetGridData(wxGrid* grid, const wxArrayPtrVoid& data,
  const wxString& rowTitle, bool editable)
{
  grid->EnableEditing(editable);

  wxArrayString* cols = (wxArrayString*) data.Item(0);
  int cnt = cols->GetCount()-grid->GetNumberCols();
  if (cnt>0)
    grid->AppendCols(cnt);
  else if (cnt<0)
	grid->DeleteCols(0, -cnt);
  grid->SetColLabelSize(grid->GetDefaultRowSize());
  for (int c = 0; c<(int)cols->GetCount(); c++)
    grid->SetColLabelValue(c, cols->Item(c));
  
  cnt = data.GetCount()-(editable?0:1)-grid->GetNumberRows();
  if (cnt>0)
    grid->AppendRows(cnt);
  else if (cnt<0)
	grid->DeleteRows(0, -cnt);
  for (int r = 1; r<=(int)grid->GetNumberRows(); r++)
  {
	grid->SetRowLabelValue(r-1, rowTitle + wxString::Format(_T("%d"), r));
	if (r < (int)data.GetCount())
	{
	  wxArrayString* row = (wxArrayString*) data.Item(r);
      for (int c = 0; c<(int)cols->GetCount(); c++)
        grid->SetCellValue(r-1, c, row->Item(c));
	}
  }
}

void wxPropDlg::OnCellLeftClick(wxGridEvent& event)
{
  wxGrid* grid = (wxGrid*)event.GetEventObject();
  grid->ClearSelection();
  grid->SetGridCursor(event.GetRow(), event.GetCol());
  grid->EnableCellEditControl();
}

void wxPropDlg::OnCellChange(wxGridEvent& event)
{
  wxGrid* grid = (wxGrid*)event.GetEventObject();
  if (event.GetRow() == grid->GetNumberRows()-1)
    grid->AppendRows();
  wxString rowTitle = grid->GetRowLabelValue(0);
  rowTitle = rowTitle.Mid(0, rowTitle.length()-1);
  grid->SetRowLabelValue(grid->GetNumberRows()-1,
	rowTitle + wxString::Format(_T("%d"), grid->GetNumberRows()));
}

void wxPropDlg::OnCellRightClick(wxGridEvent& event)
{
  wxGrid* grid = (wxGrid*)event.GetEventObject();
  if (grid->IsEditable())
  {
	m_currObject = grid;
	m_currObjectItem = event.GetRow();
	wxMenu* menu = new wxMenu;
    menu->Append(ROW_DELETE_ID, _("&Delete"));
	menu->GetMenuItems()[0]->Enable(event.GetRow()<grid->GetNumberRows()-1);
	grid->PopupMenu(menu, event.GetPosition());
  }
}

void wxPropDlg::OnRowDelete(wxCommandEvent& event)
{
  wxGrid* grid = (wxGrid*)m_currObject;
  grid->DeleteRows(m_currObjectItem);

}

//--- Font property ---//
void wxPropDlg::AddFontProp(wxSizer* sizer, wxString text,
  wxFont font, wxString caption)
{
  wxFontData data;
  data.SetInitialFont(font);
  AddFontProp(sizer, text, data, caption);
}

void wxPropDlg::AddFontProp(wxSizer* sizer, wxString text,
  wxFontData font, wxString caption)
{
  if (!sizer) // only update value
  {
	delete (wxFontData*)m_controls[m_updateIndex];
	m_controls[m_updateIndex] = new wxFontData(font);
	m_updateIndex++;
	return;
  }
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxButton* button = new wxButton(propWindow, SELECT_FONT_BT_ID, caption);
  if (caption == _T("..."))
	button->SetSizeHints(24, 24, 24, 24);
  button->SetName(wxString::Format(_T("%d"), m_controls.GetCount()));
  sizer->Add(button);
  m_types.Add(propFONT);
  m_controls.Add(new wxFontData(font));
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::OnSelectFont(wxCommandEvent& event)
{
  long index = -1;
  ((wxButton*)event.GetEventObject())->GetName().ToLong(&index);
  wxFontData* data = (wxFontData*)m_controls[index];
  wxFontDialog dialog(propWindow, data);
  if (dialog.ShowModal() == wxID_OK)
	*data = dialog.GetFontData();
}

void wxPropDlg::AddColourProp(wxSizer* sizer, wxString text,
  wxColour colour, wxString caption)
{
  if (!sizer) // only update value
  {
	((ColourPanel*)m_controls[m_updateIndex])->SetColour(colour);
	m_updateIndex++;
	return;
  }
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
  ColourPanel* panel = new ColourPanel(propWindow, -1);
  panel->SetSize(wxSize(20, 20));
  panel->SetColour(colour);
  sizer2->Add(panel, 1, wxALL, 2);
  wxButton* button = new wxButton(propWindow, SELECT_COLOR_BT_ID, caption);
  if (caption == _T("..."))
	button->SetSizeHints(24, 24, 24, 24);
  button->SetName(wxString::Format(_T("%d"), m_controls.GetCount()));
  sizer2->Add(button);
  sizer->Add(sizer2);
  m_types.Add(propCOLOUR);
  m_controls.Add(panel);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::OnSelectColour(wxCommandEvent& event)
{
  long index = -1;
  ((wxButton*)event.GetEventObject())->GetName().ToLong(&index);
  ColourPanel* panel = ((ColourPanel*)m_controls[index]);
  wxColourData data;
  data.SetColour(panel->GetColour());
  wxColourDialog dialog(propWindow, &data);
  if (dialog.ShowModal() == wxID_OK)
	panel->SetColour(dialog.GetColourData().GetColour());
}

void wxPropDlg::AddFileProp(wxSizer* sizer, wxString text,
  const wxString& defValue, int dlgStyle, wxString caption)
{
  if (!sizer) // only update value
  {
	wxTextCtrl* ctrl = (wxTextCtrl*) m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	m_updateIndex++;
	return;
  }
  // add text control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
  wxTextCtrl* ctrl = new wxTextCtrl(propWindow, -1, defValue);
  sizer2->Add(ctrl, 1, wxEXPAND);
  wxButton* button = new wxButton(propWindow, SELECT_FILE_BT_ID, caption);
  int h = ctrl->GetSize().GetHeight() > 24 ? ctrl->GetSize().GetHeight() : 24;
  if (caption == _T("..."))
	button->SetSizeHints(h, h, h, h);
  button->SetName(wxString::Format(_T("%d:%d"), m_controls.GetCount(), dlgStyle));
  sizer2->Add(button);
  sizer->Add(sizer2, 1, wxEXPAND);
  m_types.Add(propTEXT);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::OnSelectFile(wxCommandEvent& event)
{
  wxString name = ((wxButton*)event.GetEventObject())->GetName();
  long index = -1;
  name.BeforeFirst(wxT(':')).ToLong(&index);
  long dlgStyle = wxFD_DEFAULT_STYLE;
  name = name.AfterFirst(wxT(':'));
  name.BeforeFirst(wxT(':')).ToLong(&dlgStyle);
  wxTextCtrl* ctrl = ((wxTextCtrl*)m_controls[index]);
  wxFileDialog dialog(propWindow, _("Choose a file"), ctrl->GetValue(), wxT(""),
    wxT("*.*"), dlgStyle);
  dialog.SetPath(ctrl->GetValue());
  if (dialog.ShowModal() == wxID_OK)
	ctrl->SetValue(dialog.GetPath());
}

void wxPropDlg::AddDirectoryProp(wxSizer* sizer, wxString text,
  const wxString& defValue, wxString caption)
{
  if (!sizer) // only update value
  {
	wxTextCtrl* ctrl = (wxTextCtrl*) m_controls[m_updateIndex];
	ctrl->SetValue(defValue);
	m_updateIndex++;
	return;
  }
  // add text control
  if (text.length())
	sizer->Add(new wxStaticText(propWindow, -1, text), 0, wxALIGN_CENTER_VERTICAL);
  wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
  wxTextCtrl* ctrl = new wxTextCtrl(propWindow, -1, defValue);
  sizer2->Add(ctrl, 1, wxEXPAND, 0);
  wxButton* button = new wxButton(propWindow, SELECT_DIR_BT_ID, caption);
  int h = ctrl->GetSize().GetHeight() > 24 ? ctrl->GetSize().GetHeight() : 24;
  if (caption == _T("..."))
	button->SetSizeHints(h, h, h, h);
  button->SetName(wxString::Format(_T("%d"), m_controls.GetCount()));
  sizer2->Add(button);
  sizer->Add(sizer2, 1, wxEXPAND);
  m_types.Add(propTEXT);
  m_controls.Add(ctrl);
  m_groupIds.Add(m_currGroupId);
}

void wxPropDlg::OnSelectDir(wxCommandEvent& event)
{
  long index = -1;
  ((wxButton*)event.GetEventObject())->GetName().ToLong(&index);
  wxTextCtrl* ctrl = ((wxTextCtrl*)m_controls[index]);
  wxDirDialog dialog(propWindow, _("Choose a directory"), ctrl->GetValue(),
    wxDD_DEFAULT_STYLE | wxDD_NEW_DIR_BUTTON);
  dialog.SetPath(ctrl->GetValue());
  if (dialog.ShowModal() == wxID_OK)
	ctrl->SetValue(dialog.GetPath());
}

////////////////////////////// Group /////////////////////////////////////////

wxSizer* wxPropDlg::BeginGroup(wxSizer* sizer,
  wxString title, wxString checkTitle, bool defValue, bool readonly)
{
  if (!sizer) // only update value
  {
	m_currGroupChecker = NULL;
	if (checkTitle.length())
	{
	  AddCheckProp(NULL, wxEmptyString, defValue, readonly, GROUP_CHECK_ID);
	  m_currGroupChecker = (wxCheckBox*)m_controls[m_updateIndex-1];
	}
	return NULL;
  }
  m_currGroupId++;
  wxStaticBox* sb = new wxStaticBox(propWindow, -1, title);
  wxStaticBoxSizer* sbsizer = new wxStaticBoxSizer(sb, wxVERTICAL);
  sizer->Add(sbsizer, 0, wxEXPAND);
  if (checkTitle.length())
  {
    wxFlexGridSizer* groupSizer = new wxFlexGridSizer(2, 2, 8, 4);
    groupSizer->AddGrowableCol(1);
    sbsizer->Add(groupSizer, 0, wxEXPAND|wxALL, 4);
    AddCheckProp(groupSizer, wxEmptyString, defValue, readonly, GROUP_CHECK_ID);
    m_currGroupChecker = m_controls[m_controls.GetCount()-1];
	AddText(groupSizer, checkTitle);
	AddText(groupSizer, wxEmptyString);
    return groupSizer;
  }
  m_currGroupChecker = NULL;
  return sbsizer;
}

void wxPropDlg::EndGroup()
{
  if (m_currGroupChecker)
  {
	wxCommandEvent evt;
	evt.SetEventObject((wxObject*)m_currGroupChecker);
	OnGroupCheck(evt);
  }
  m_currGroupChecker = NULL;
  m_currGroupId++;
}

void wxPropDlg::OnGroupCheck(wxCommandEvent& event)
{
  long index = -1;
  wxCheckBox* ctrl = (wxCheckBox*)event.GetEventObject();
  ctrl->GetName().ToLong(&index);
  int groupId = m_groupIds[index];
  for (int i = 0; i<(int)m_groupIds.GetCount(); i++)
    if (i != index && m_groupIds[i] == groupId && m_types[i] != propFONT)
	  ((wxWindow*) m_controls[i])->Enable(ctrl->GetValue());
}

///////////////////////// Get Values /////////////////////////////////////////

wxString wxPropDlg::GetString(int index)
{
  switch(m_types[index])
  {
    case propTEXT:
      return ((wxTextCtrl*)m_controls[index])->GetValue();
    case propSTAT_TEXT:
	  return ((wxStaticText*)m_controls[index])->GetLabel();
    case propCOMBO:
	  return ((wxComboBox*)m_controls[index])->GetValue();
  }
  return wxEmptyString;
}

int wxPropDlg::GetInt(int index)
{
  switch(m_types[index])
  {
	case propSPIN:
      return ((wxSpinCtrl*)m_controls[index])->GetValue();
    case propCOMBO:
      return ((wxComboBox*)m_controls[index])->GetSelection();
	case propCHOICE:
      return ((wxChoice*)m_controls[index])->GetSelection();
    case propRADIO_GROUP:
    {
      wxArrayPtrVoid* radioControls = (wxArrayPtrVoid*) m_controls[index];
  	  for (int i=0; i<(int)radioControls->GetCount(); i++)
  	  {
	    wxRadioButton* ctrl = (wxRadioButton*)(*radioControls)[i];
	    if (ctrl->GetValue())
	    	return i;
  	  }
  	  return -1;
    }
  }
  return -1;
}

bool wxPropDlg::GetBool(int index)
{
  switch(m_types[index])
  {
    case propCHECK:
      return ((wxCheckBox*)m_controls[index])->GetValue();
	case propRADIO:
      return ((wxRadioButton*)m_controls[index])->GetValue();
  }
  return true;
}

void* wxPropDlg::GetClientData(int index)
{
  switch(m_types[index])
  {
    case propCOMBO:
	{
	  wxComboBox* ctrl = (wxComboBox*) m_controls[index];
	  if (ctrl->GetSelection() >= 0)
		return ctrl->GetClientData(ctrl->GetSelection());
	}
	case propCHOICE:
	{
	  wxChoice* ctrl = (wxChoice*) m_controls[index];
      if (ctrl->GetSelection() >= 0)
		return ctrl->GetClientData(ctrl->GetSelection());
	}
  }
  return NULL;
}

wxArrayPtrVoid wxPropDlg::GetGrid(int index)
{
  wxArrayPtrVoid data;
  if (m_types[index] != propGRID)
	return data;
  wxGrid* grid = (wxGrid*)m_controls[index];
  grid->SaveEditControlValue();
  int count = grid->GetNumberRows() - (grid->IsEditable() ? 1 : 0);
  for (int r = 0; r<count; r++)
  {
    wxArrayString* row = new wxArrayString;
    for (int c = 0; c<(int)grid->GetNumberCols(); c++)
      row->Add(grid->GetCellValue(r,c));
	data.Add(row);
  }
  return data;
}

wxFontData wxPropDlg::GetFont(int index)
{
  if (m_types[index] != propFONT)
	return wxFontData();
  wxFontData* data = (wxFontData*)m_controls[index];
  if (!data->GetChosenFont().Ok())
	data->SetChosenFont(data->GetInitialFont());
  return *data;
}

wxColour wxPropDlg::GetColour(int index)
{
  if (m_types[index] != propCOLOUR)
	return wxColour();
  return ((ColourPanel*)m_controls[index])->GetColour();
}

