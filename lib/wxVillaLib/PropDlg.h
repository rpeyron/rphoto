/////////////////////////////////////////////////////////////////////////////
// Name:        PropDlg.h
// Purpose:     Base class for property dialogs
// Author:      Alex Thuering
// Created:     18.11.2003
// RCS-ID:      $Id: PropDlg.h 365 2008-06-21 23:15:43Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifndef PROP_DLG_H
#define PROP_DLG_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/dynarray.h>

#ifndef wxFD_OPEN
#define wxFD_OPEN wxOPEN
#define wxFD_DEFAULT_STYLE 0
#endif

/** Base class for property dialogs.
  * Allow you to simply create properties dialogs without any GUI designer.
  * Using this class, you can also create dynamic property dialog.
  */
class wxPropDlg: public wxDialog
{
  public:
    wxPropDlg(wxWindow *parent);
    virtual ~wxPropDlg();
	
  protected:
	wxArrayInt m_types;
	wxArrayPtrVoid m_controls;
	wxArrayInt m_groupIds;
	int m_updateIndex; /** index of property by update (reset) */
	int m_currGroupId;
	void* m_currGroupChecker;
	
	void Create(bool resetButton = false);
	/**
	 * Creates properties panel. 
	 * if sizer != NULL adds properties, else update values
	 */
	virtual void CreatePropPanel(wxSizer* sizer) = 0;
	/**
	 * Creates panel with buttons
	 */
	virtual void CreateButtonPane(wxSizer* sizer, bool resetButton = false);

	virtual void Reset();
	
	/**
	 * Populate values on the model.
	 */
	virtual bool SetValues() = 0;

	// Add properties
	wxWindow* propWindow; // default this

	void AddText(wxSizer* sizer, wxString text);

	void AddTextProp(wxSizer* sizer, wxString text, wxString defValue,
	  bool readonly = false);
	void AddStaticTextProp(wxSizer* sizer, wxString text, wxString defValue);
	void AddSpinProp(wxSizer* sizer, wxString text, int defValue,
	  int min = 0, int max = 100);
	void AddCheckProp(wxSizer* sizer, wxString text, bool defValue,
	  bool readonly = false, int id = -1);
	void AddRadioProp(wxSizer* sizer, wxString text, bool defValue,
	  long style = wxRB_SINGLE, bool readonly = false, int id = -1);
	void AddRadioGroupProp(wxSizer* sizer, const wxArrayString& text,
	  int defValue, bool readonly = false);
	void AddComboProp(wxSizer* sizer, const wxString& text,
	  const wxString& defValue, const wxArrayString& choices, long style = 0);
	void SetComboData(wxComboBox* ctrl,
	  const wxString& defValue, const wxArrayString& choices);
	void AddChoiceProp(wxSizer* sizer, const wxString& text,
	  const wxString& defValue, const wxArrayString& choices, int id = -1);
	void SetChoiceData(wxChoice* ctrl,
	  const wxString& defValue, const wxArrayString& choices);
	void AddGridProp(wxSizer* sizer, const wxArrayPtrVoid& data,
	  const wxString& rowTitle, bool editable);
	void SetGridData(wxGrid* grid, const wxArrayPtrVoid& data,
      const wxString& rowTitle, bool editable);
	void AddFontProp(wxSizer* sizer, wxString text,
	  wxFont font, wxString caption = _T("..."));
	void AddFontProp(wxSizer* sizer, wxString text,
	  wxFontData font, wxString caption = _T("..."));
	void AddColourProp(wxSizer* sizer, wxString text,
	  wxColour colour, wxString caption = _T("..."));
	void AddFileProp(wxSizer* sizer, wxString text, const wxString& defValue,
	  int dlgStyle = wxFD_OPEN, wxString caption = _T("..."));
	void AddDirectoryProp(wxSizer* sizer, wxString text,
	  const wxString& defValue, wxString caption = _T("..."));

	wxSizer* BeginGroup(wxSizer* sizer, wxString title = wxEmptyString,
	  wxString checkTitle = wxEmptyString, bool defValue = false,
	  bool readonly = false);
	void EndGroup();

	// Get values
	wxString GetString(int index);
	int GetInt(int index);
	bool GetBool(int index);
	void* GetClientData(int index);
	wxArrayPtrVoid GetGrid(int index);
	wxFontData GetFont(int index);
	wxColour GetColour(int index);
	
	void* m_currObject;
	int m_currObjectItem;
  	virtual void OnCancel(wxCommandEvent& event);
    virtual void OnOk(wxCommandEvent& event);
    virtual void OnReset(wxCommandEvent& event);
	virtual void OnSelectFont(wxCommandEvent& event);
	virtual void OnSelectColour(wxCommandEvent& event);
	virtual void OnSelectFile(wxCommandEvent& event);
	virtual void OnSelectDir(wxCommandEvent& event);
	virtual void OnGroupCheck(wxCommandEvent& event);
	virtual void OnCellLeftClick(wxGridEvent& event);
	virtual void OnCellRightClick(wxGridEvent& event);
	virtual void OnCellChange(wxGridEvent& event);
	virtual void OnRowDelete(wxCommandEvent& event);
	
  private:
    DECLARE_EVENT_TABLE()
};

#endif // PROP_DLG_H
