/* ****************************************************************************
 * RatioFrame.cpp                                                             *
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


#ifdef __GNUG__
#pragma implementation "RatioFrame.h"
#endif

#include "RatioPrecomp.h"
#include "../resources/resource.h"
#include "../resources/resource.inc" // Include XPM Data
#include "RatioFrame.h"
#include <wx/process.h>
#include <wx/file.h>

IMPLEMENT_CLASS(RatioFrame, wxFrame)

static int dummyDeclForIntelliSense;

RatioFrame::RatioFrame(wxWindow* parent, 
					   wxWindowID id, 
					   const wxString& title, 
					   const wxPoint& pos, 
					   const wxSize& size, 
					   long style, 
					   const wxString& name)
	: wxFrame(parent, id, title, pos, size, style, name)
{

    // Locale Stuff
    m_locale.Init();
    m_locale.AddCatalog("RPhoto");

    // Variables
	m_bTemp = FALSE;
    m_bDirty = FALSE;
    m_bJPEGlossless = FALSE;

	// Icon
	SetIcon(wxIcon(xpm_FilesPhoto));

    InitConfig();
    InitMenu();
    InitToolBar();
    InitAccelerator();
    InitStatusBar();
    InitControls();

    DoConfig();

	Layout();
    SetSize(800,540);

}

void RatioFrame::InitConfig()
{
    // Config
    m_pConfig = new wxConfig("RPhoto");
    m_pConfigDialog = new wxConfigDialog(*m_pConfig, this, -1, _("Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );

    // Setup options
    // * General stuff
    // * JPEG stuff
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, "JPEG", "JPEGTran", _("JPEG"), _("JPEGTran's path"), "jpegtran");
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, "JPEG", "DisplayLosslessWarnings", _("JPEG"), _("Display Lossless warnings"), TRUE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, "JPEG", "Quality", _("JPEG"), _("JPEG Quality (0-100)"), "80");
    
    // * Default settings
    wxString ratios[4];
	ratios[0]=_("0:0 (None)");
	ratios[1]=_("4:3 (Numeric Photo)");
	ratios[2]=_("3:2 (Classical Photo)");
	ratios[3]=_("1:1 (Square)");
    new wxConfigDialog_EntryCombo(*m_pConfigDialog, "Defaults", "Ratio", _("Defaults"), _("Ratio"), 4, ratios, ratios[0]);

    wxString orientation[3];
    orientation[0] = _("Automatic");
    orientation[1] = _("Landscape");
    orientation[2] = _("Portrait");
    new wxConfigDialog_EntryRadio(*m_pConfigDialog, "Defaults", "Orientation", _("Defaults"), _("Orientation"), 3, orientation, 0);

    // * Appearance
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, "View", "FitWindow", _("Appearance"), _("Fit the image inside the window"), TRUE);
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, "View", "Explorer", _("Appearance"), _("View explorer panel"), TRUE);

    m_pConfigDialog->doLayout();
}

void RatioFrame::DoConfig()
{
    wxCommandEvent evt;
    int i;
    bool opt;
    // Toggle Explorer
    opt = TRUE; 
    m_pConfig->Read("View/Explorer", &opt);
    if ( (!opt) ^ (!m_pDirCtrl->IsShown()) )  OnToggleExplorer(evt);
    // Fit On Page
    opt = TRUE; 
    m_pConfig->Read("View/FitWindow", &opt);
    if ( (!opt) ^ ((m_pImageBox->GetScale()==1)) )  OnToggleFitOnPage(evt);
    // Orientation
    i = 0;
    m_pConfig->Read("Defaults/Orientation", &i);
    switch(i)
    {
    case 0: OnRatioPOrL(evt);      break;
    case 1: OnRatioLandscape(evt); break;
    case 2: OnRatioPortrait(evt);  break;
    }
    // Ratio
    wxString str;
    str = "";
    m_pConfig->Read("Defaults/Ratio",&str);
    if (m_pRatioCombo->FindString(str) >= 0) { m_pRatioCombo->SetSelection(m_pRatioCombo->FindString(str)); }
    OnRatioChange(evt);
}

void RatioFrame::InitMenu()
{
    // Menu
	m_pMenuBar = new wxMenuBar();
	wxMenu * curMenu, * subMenu;
	// - File Menu
	curMenu = new wxMenu();
	curMenu->Append(MENU_FILE_OPEN, _("&Open..."), _("Open a file."));
	curMenu->Append(MENU_FILE_SAVE, _("&Save") + wxString("\ts"), _("Save the current file."));
	curMenu->Append(MENU_FILE_SAVEAS, _("&Save as..."), _("Save the current file under another name."));
	curMenu->Append(MENU_FILE_RELOAD, _("&Reload"), _("Reload original file."));
	curMenu->Append(MENU_FILE_DELETE, _("&Delete") + wxString("\tDel"), _("Suppress the file."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_PREV, _("&Previous File")+wxString("\tLeft"), _("Open the previous file in the explorer."));
	curMenu->Append(MENU_FILE_NEXT, _("&Next File")+wxString("\tRight"), _("Open the next file in the explorer."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_PREFS, _("&Preferences"), _("Define application's settings."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_QUIT, _("&Quit")+wxString("\tQ"), _("Quit the application."));
	m_pMenuBar->Append(curMenu, _("&File"));
	// - Image Menu
	curMenu = new wxMenu();
	curMenu->Append(MENU_IMAGE_CROP, _("&Crop")+wxString("\tc"), _("Crop image to selection."));
	curMenu->Append(MENU_IMAGE_ROTATE_LEFT, _("Rotate &Left")+wxString("\tl"), _("Rotate the image 90 degrees to the left."));
	curMenu->Append(MENU_IMAGE_ROTATE_RIGHT, _("Rotate &Right")+wxString("\tr"), _("Rotate the image 90 degrees to the right."));
	curMenu->Append(MENU_IMAGE_FLIP_HORIZONTAL, _("Flip &Horizontal"), _("Flip the image horizontally."));
	curMenu->Append(MENU_IMAGE_FLIP_VERTICAL, _("Flip &Vertical"), _("Flip the image vertically."));
	curMenu->AppendSeparator();
    subMenu = new wxMenu();
	subMenu->AppendCheckItem(MENU_IMAGE_JPEG_LOSSLESS, _("Lossless Operations"), _("Activate lossless operations whenever it is possible."));
	subMenu->Check(MENU_IMAGE_JPEG_LOSSLESS,TRUE);
	subMenu->Append(MENU_IMAGE_JPEG_OPTIMIZE, _("JPEG &Optimize"), _("Optimize Huffman JPEG table."));
	subMenu->Append(MENU_IMAGE_JPEG_PROGRESSIVE, _("JPEG &Progressive"), _("Save the file as progressive."));
	subMenu->Append(MENU_IMAGE_JPEG_DETECTQUAL, _("&Detect Quality"), _("Detect the quality of the current file, and store it as default in the preferences."));
    curMenu->Append(MENU_IMAGE_JPEG, _("&JPEG"), subMenu, _("JPEG Specific Options"));
	m_pMenuBar->Append(curMenu, _("&Image"));
    m_pMenuBar->Enable(MENU_IMAGE_JPEG, FALSE);
	// - Ratio Menu
	curMenu = new wxMenu();
	curMenu->AppendRadioItem(MENU_RATIO_PORL, _("&Automatic Orientation"), _("Automatic Orientation choice between Portrait or Landscape."));
	curMenu->AppendRadioItem(MENU_RATIO_LANDSCAPE, _("&Landscape"), _("Force Landscape Orientation"));
	curMenu->AppendRadioItem(MENU_RATIO_PORTRAIT, _("&Portrait"), _("Force Portrait Orientation"));
	curMenu->AppendSeparator();
	curMenu->AppendRadioItem(MENU_RATIO_R_NONE, _("&No Ratio"), _("Ratio checking is not used."));
	curMenu->AppendRadioItem(MENU_RATIO_R_NUMERIC, _("&Ratio 4:3 - Numeric"), _("Common ratio used in numeric photo."));
	curMenu->AppendRadioItem(MENU_RATIO_R_CLASSIC, _("&Ratio 3:2 - Classic"), _("Common ratio used in classical photo."));
	curMenu->AppendRadioItem(MENU_RATIO_R_SQUARE, _("&Ratio 1:1 - Square"), _("Square ratio."));
	curMenu->AppendRadioItem(MENU_RATIO_R_CUSTOM, _("&Custom Ratio"), _("To enter a custom ratio, please go in the combo box."));
	curMenu->Enable(MENU_RATIO_R_CUSTOM,FALSE);
	m_pMenuBar->Append(curMenu, _("&Ratio"));
	// - View Menu
	curMenu = new wxMenu();
	curMenu->AppendCheckItem(MENU_VIEW_EXPLORER, _("Show &Explorer"), _("Show left explorer tab."));
	curMenu->Check(MENU_VIEW_EXPLORER,TRUE);
	curMenu->AppendSeparator();
	curMenu->AppendCheckItem(MENU_VIEW_FITONPAGE, _("&Fit on Page"), _("View the whole image in the window."));
	curMenu->Check(MENU_VIEW_FITONPAGE,TRUE);
	m_pMenuBar->Append(curMenu, _("&View"));
	// - Help Menu
	curMenu = new wxMenu();
	curMenu->Append(MENU_HELP_ABOUT, _("&About..."), _("About this application."));
	m_pMenuBar->Append(curMenu, _("&Help"));
	SetMenuBar(m_pMenuBar);
}

void RatioFrame::InitToolBar()
{
	// ToolBar
	CreateToolBar();
	GetToolBar()->AddTool(TOOL_OPEN, _("Open"), wxBitmap(xpm_open), wxNullBitmap, wxITEM_NORMAL , _("Open a file"), _("Open a file"));
	GetToolBar()->AddTool(TOOL_SAVE, _("Save"), wxBitmap(xpm_save), wxNullBitmap, wxITEM_NORMAL ,_("Save the current file"),_("Save the current file"));
	GetToolBar()->AddTool(TOOL_RELOAD, _("Reload"), wxBitmap(xpm_undo), wxNullBitmap, wxITEM_NORMAL , _("Reload original file"), _("Reload original file"));
	GetToolBar()->AddTool(TOOL_PREV, _("Previous"), wxBitmap(xpm_arrow_left), wxNullBitmap, wxITEM_NORMAL ,_("Previous file"),_("Previous file"));
	GetToolBar()->AddTool(TOOL_NEXT, _("Next"), wxBitmap(xpm_arrow_right),wxNullBitmap, wxITEM_NORMAL , _("Next file"), _("Next file"));
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_EXPLORER, _("Explore"), wxBitmap(xpm_explorer), wxNullBitmap, wxITEM_CHECK , _("Show the explorer"), _("Show the explorer"));
	GetToolBar()->ToggleTool(TOOL_EXPLORER, TRUE);
	GetToolBar()->AddTool(TOOL_FITONPAGE, _("Explore"), wxBitmap(xpm_fullscrn), wxNullBitmap, wxITEM_CHECK , _("Fit On Page."), _("Fit On Page."));
	GetToolBar()->ToggleTool(TOOL_FITONPAGE, TRUE);
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_CROP, _("Crop"), wxBitmap(xpm_crop), wxNullBitmap, wxITEM_NORMAL ,  _("Crop the image"),  _("Crop the image"));
	GetToolBar()->AddTool(TOOL_ROTATE_LEFT, _("Rotate Left"), wxBitmap(xpm_rotate_left),wxNullBitmap, wxITEM_NORMAL , _("Rotate the image to the left"), _("Rotate the image to the left"));
	GetToolBar()->AddTool(TOOL_ROTATE_RIGHT, _("Rotate Right"), wxBitmap(xpm_rotate_right),wxNullBitmap, wxITEM_NORMAL , _("Rotate the image to the right"), _("Rotate the image to the right"));
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_OR_P_OR_L, _("Automatic"), wxBitmap(xpm_landorpo), wxNullBitmap, wxITEM_RADIO ,_("Automatic Selection of Orientation"), _("Automatic Selection of Orientation"));
	GetToolBar()->AddTool(TOOL_OR_LANDSCAPE, _("Landscape"), wxBitmap(xpm_landscap), wxNullBitmap, wxITEM_RADIO ,_("Landscape Orientation"), _("Landscape Orientation"));
	GetToolBar()->AddTool(TOOL_OR_PORTRAIT, _("Portrait"), wxBitmap(xpm_portrait), wxNullBitmap, wxITEM_RADIO ,_("Portrait Orientation"), _("Portrait Orientation"));
	m_pRatioCombo = new wxComboBox(GetToolBar(), WIDGET_RATIOCOMBO, _("test"));
	m_pRatioCombo->Append(_("0:0 (None)"));
	m_pRatioCombo->Append(_("4:3 (Numeric Photo)"));
	m_pRatioCombo->Append(_("3:2 (Classical Photo)"));
	m_pRatioCombo->Append(_("1:1 (Square)"));
	m_pRatioCombo->SetSelection(0);
	m_pRatioCombo->SetWindowStyle(wxCB_DROPDOWN);
	GetToolBar()->AddControl(m_pRatioCombo);
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_JPEGLOSSLESS, _("JPEG Lossless"), wxBitmap(xpm_light_green), wxBitmap(xpm_light_red), wxITEM_CHECK, _("JPEG Lossless"), _("Performs JPEG operations lossless whenever possible."));
	GetToolBar()->ToggleTool(TOOL_JPEGLOSSLESS, TRUE);
    GetToolBar()->Realize();
}
 
void RatioFrame::InitAccelerator()
{
    // Accelerator
    const int numEntries = 8;
    wxAcceleratorEntry entries[numEntries];
    int i = 0;
    entries[i++].Set(wxACCEL_CTRL, (int) 'Q', MENU_FILE_QUIT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 's', MENU_FILE_SAVE);
    entries[i++].Set(wxACCEL_NORMAL, WXK_NEXT, MENU_FILE_NEXT);
    entries[i++].Set(wxACCEL_NORMAL, WXK_PRIOR, MENU_FILE_PREV);
    entries[i++].Set(wxACCEL_NORMAL, WXK_DELETE, MENU_FILE_DELETE);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'l', MENU_IMAGE_ROTATE_LEFT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'r', MENU_IMAGE_ROTATE_RIGHT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'c', MENU_IMAGE_CROP);
    wxASSERT(i == numEntries);
    wxAcceleratorTable accel(numEntries, entries);
    SetAcceleratorTable(accel);
}

void RatioFrame::InitStatusBar()
{
	// StatusBar
	CreateStatusBar(4);
	SetStatusText("Ready", 0);
	int width[4]; width[0] = -1; width[1] = 60; width[2] = 100; width[3] = 100;
	GetStatusBar()->SetStatusWidths(4, width);
}

void RatioFrame::InitControls()
{
	// Other Controls

	wxBoxSizer * horSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(horSizer);

	/*
	// Text Ctrl
	m_pTextCtrl = new wxTextCtrl(this, -1, wxString("Some text"), 
								  wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	horSizer->Add (m_pTextCtrl, 1, wxEXPAND);
	*/

	// Dir Browser Ctrl
	m_pDirCtrl = new wxDirCtrl(this, -1, 
		wxDirDialogDefaultFolderStr, 
		wxDefaultPosition, wxDefaultSize,
		wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_SHOW_FILTERS,
		RPHOTO_FILTERIMGLIST_WORLD,
		1 /* 1 For All Images, 4 For JPEG only */);
	m_pDirCtrl->GetTreeCtrl()->SetWindowStyle(m_pDirCtrl->GetTreeCtrl()->GetWindowStyle() | wxTR_HAS_VARIABLE_ROW_HEIGHT);

	horSizer->Add (m_pDirCtrl, 1, wxEXPAND);
	Connect(m_pDirCtrl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED,
		(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
		& RatioFrame::OnDirCtrlChange); 

    // Image Box
	m_pImageBox = new wxRatioImageBox (this, -1);
    if (wxFileName::FileExists(RT_DEFAULT_IMAGE))
    {
        ImageLoad(RT_DEFAULT_IMAGE);
    }
    else
    {
        m_pImageBox->GetImage().Create(1,1);
    }
	m_pImageBox->SetScale(scFIT_TO_PAGE_IF_LARGER);
	m_pImageBox->SetWindowStyle(0);
	horSizer->Add (m_pImageBox, 3, wxEXPAND);
}


RatioFrame::~RatioFrame()
{
	ImageCleanup();
    if (m_pConfig) delete m_pConfig;
    if (m_pConfigDialog) delete m_pConfigDialog;
}

// Event Map Table
BEGIN_EVENT_TABLE(RatioFrame, wxFrame)
   EVT_MENU(MENU_FILE_OPEN, RatioFrame::OnFileOpen)
   EVT_MENU(MENU_FILE_SAVE, RatioFrame::OnFileSave)
   EVT_MENU(MENU_FILE_SAVEAS, RatioFrame::OnFileSaveAs)
   EVT_MENU(MENU_FILE_DELETE, RatioFrame::OnFileDelete)
   EVT_MENU(MENU_FILE_RELOAD, RatioFrame::OnFileReload)
   EVT_MENU(MENU_FILE_PREV, RatioFrame::OnFilePrev)
   EVT_MENU(MENU_FILE_NEXT, RatioFrame::OnFileNext)
   EVT_MENU(MENU_FILE_PREFS, RatioFrame::OnPreferences)
   EVT_MENU(MENU_FILE_QUIT, RatioFrame::OnMenuFileQuit)
   EVT_MENU(MENU_VIEW_EXPLORER, RatioFrame::OnToggleExplorer)
   EVT_MENU(MENU_VIEW_FITONPAGE, RatioFrame::OnToggleFitOnPage)
   EVT_MENU(MENU_RATIO_R_NONE, RatioFrame::OnRatioChange)
   EVT_MENU(MENU_RATIO_R_NUMERIC, RatioFrame::OnRatioChange)
   EVT_MENU(MENU_RATIO_R_CLASSIC, RatioFrame::OnRatioChange)
   EVT_MENU(MENU_RATIO_R_SQUARE, RatioFrame::OnRatioChange)
   EVT_MENU(MENU_IMAGE_CROP, RatioFrame::OnImageCrop)
   EVT_MENU(MENU_IMAGE_ROTATE_LEFT, RatioFrame::OnImageRotateLeft)
   EVT_MENU(MENU_IMAGE_ROTATE_RIGHT, RatioFrame::OnImageRotateRight)
   EVT_MENU(MENU_IMAGE_FLIP_HORIZONTAL, RatioFrame::OnImageFlipHorizontal)
   EVT_MENU(MENU_IMAGE_FLIP_VERTICAL, RatioFrame::OnImageFlipVertical)
   EVT_MENU(MENU_IMAGE_JPEG_LOSSLESS, RatioFrame::OnToggleJPEGLossless)
   EVT_MENU(MENU_IMAGE_JPEG_OPTIMIZE, RatioFrame::OnImageJPEGOptimize)
   EVT_MENU(MENU_IMAGE_JPEG_PROGRESSIVE, RatioFrame::OnImageJPEGProgressive)
   EVT_MENU(MENU_IMAGE_JPEG_DETECTQUAL, RatioFrame::OnImageJPEGDetectQuality)
   EVT_MENU(MENU_HELP_ABOUT, RatioFrame::OnHelpAbout)
   EVT_TOOL(MENU_RATIO_PORL, RatioFrame::OnRatioPOrL)
   EVT_TOOL(MENU_RATIO_PORTRAIT, RatioFrame::OnRatioPortrait)
   EVT_TOOL(MENU_RATIO_LANDSCAPE, RatioFrame::OnRatioLandscape)
   EVT_TOOL(TOOL_EXPLORER, RatioFrame::OnToggleExplorer)
   EVT_TOOL(TOOL_FITONPAGE, RatioFrame::OnToggleFitOnPage)
   EVT_TOOL(TOOL_OPEN, RatioFrame::OnFileOpen)
   EVT_TOOL(TOOL_SAVE, RatioFrame::OnFileSave)
   EVT_TOOL(TOOL_RELOAD, RatioFrame::OnFileReload)
   EVT_TOOL(TOOL_PREV, RatioFrame::OnFilePrev)
   EVT_TOOL(TOOL_NEXT, RatioFrame::OnFileNext)
   EVT_TOOL(TOOL_CROP, RatioFrame::OnImageCrop)
   EVT_TOOL(TOOL_ROTATE_LEFT, RatioFrame::OnImageRotateLeft)
   EVT_TOOL(TOOL_ROTATE_RIGHT, RatioFrame::OnImageRotateRight)
   EVT_TOOL(TOOL_OR_P_OR_L, RatioFrame::OnRatioPOrL)
   EVT_TOOL(TOOL_OR_PORTRAIT, RatioFrame::OnRatioPortrait)
   EVT_TOOL(TOOL_OR_LANDSCAPE, RatioFrame::OnRatioLandscape)
   EVT_COMBOBOX(WIDGET_RATIOCOMBO, RatioFrame::OnRatioChange)
   EVT_TEXT_ENTER(WIDGET_RATIOCOMBO, RatioFrame::OnRatioEnter)
   EVT_TOOL(TOOL_JPEGLOSSLESS, RatioFrame::OnToggleJPEGLossless)
END_EVENT_TABLE()



bool RatioFrame::CallJPEGTran(const wxString & command, bool noerror)
{
	long rc;
	wxString pathJpegtran = "jpegtran";
    const wxString extraJpegtranOptions = "-copy all ";

    m_pConfig->Read("JPEG/JPEGTran", &pathJpegtran);
	wxString cmd;
	if (!m_bTemp)
	{
		m_bTemp = TRUE;
		m_sFilename = wxFileName::CreateTempFileName(RPHOTO_TEMP_PREFIX);
		wxCopyFile(m_sOriginalFilename,m_sFilename,TRUE);
	}
	cmd = pathJpegtran + " " + extraJpegtranOptions + command + " -outfile \"" + m_sFilename + "\" \"" + m_sFilename + "\"";
	GetStatusBar()->PushStatusText(_("Executing : ") + cmd + " ...");
	//wxMessageBox(cmd);

	wxProcess process(this);
	process.Redirect();
	wxBeginBusyCursor();

    {
        wxLogNull noLog;
        rc = wxExecute(cmd, wxEXEC_SYNC, &process);
    }

	wxEndBusyCursor();
	GetStatusBar()->PopStatusText();
	if ((!noerror) && (process.GetErrorStream() != NULL))
	{
		process.GetErrorStream()->Peek();
		if (!(process.GetErrorStream()->Eof()))
		{
			char buffer[10240];
			process.GetErrorStream()->Read(buffer, sizeof(buffer));
			buffer[process.GetErrorStream()->LastRead()] = 0;
			wxMessageBox(_("JPEGTran has returned an error : \n\n") +wxString(buffer) +"\n\n" + cmd, _("JPEGTran Error"), wxOK | wxICON_ERROR, this);
		}
	}
	if (rc)
	{
		// Error
        // -1 : the process cannot be launched, probably because of jpegtran path
        if ((!noerror) && (rc == -1)) 
            wxMessageBox(_("Unable to launch JPEGTran : \n\nPlease check that jpegtran executable is in the path.")  + wxString("\n\n") + wxString(cmd), 
            _("JPEGTran Error"), wxOK | wxICON_ERROR, this);
        // Other error
	}
	else
	{
		// OK
    	wxBeginBusyCursor();
		m_pImageBox->LoadFile(m_sFilename);
		m_pImageBox->TrackerReset();
    	wxEndBusyCursor();
	}

	return (rc)?FALSE:TRUE;
}

bool RatioFrame::CallJPEGTranTryPerfect(const wxString & command)
{
	// Try once, with perfect option, and if it fails, trim and warn.
	bool rc, optWarnings;
	if (!(rc = CallJPEGTran("-perfect " + command, true)))
	{
		rc = CallJPEGTran("-trim " + command);
		wxBell();
        wxString msg = _("WARNING : This operation is not possible lossless. Non-transformable edge blocks have been dropped.");
        optWarnings = FALSE;
        m_pConfig->Read("JPEG/DisplayLosslessWarnings", &optWarnings);
        if (!optWarnings)
        {
		    GetStatusBar()->SetStatusText(msg);
        }
        else
        {
		    wxMessageBox(msg,_("JPEGTran warning"),wxICON_INFORMATION);
        }
	}
	return rc;
}

void RatioFrame::ImageLoad(wxString name, bool temp)
{
    wxBeginBusyCursor();
	ImageCleanup();
	m_sFilename = name;
	if (!m_bTemp) m_sOriginalFilename = m_sFilename;
	m_pImageBox->LoadFile(m_sFilename);
	m_pImageBox->TrackerReset();
    m_bJPEGlossless = (isJPEGFile(name))?TRUE:FALSE;
    GetMenuBar()->Enable(MENU_IMAGE_JPEG, m_bJPEGlossless);
    GetMenuBar()->Check(MENU_IMAGE_JPEG_LOSSLESS, m_bJPEGlossless);
    GetToolBar()->ToggleTool(TOOL_JPEGLOSSLESS, m_bJPEGlossless);
    GetToolBar()->EnableTool(TOOL_JPEGLOSSLESS, m_bJPEGlossless);
    UpdateDirCtrl();
    wxEndBusyCursor();
}

void RatioFrame::ImageCleanup()
{
	if (m_bTemp)
	{
		wxRemoveFile(m_sFilename);
		m_bTemp = FALSE;
	}
}

void RatioFrame::UpdateDirCtrl()
{
    wxFileName fn(m_sFilename);
    fn.MakeAbsolute();
    if (m_pDirCtrl->GetPath() != fn.GetFullPath())
    {
        m_pDirCtrl->SetPath(fn.GetFullPath());
    }
}

void RatioFrame::OnMenuFileQuit(wxCommandEvent &event)
{
  Close(FALSE);
}

void RatioFrame::OnHelpAbout(wxCommandEvent &event)
{
    wxMessageBox(wxString::Format(_("%s %s\n\n(c) 2004 - Rémi Peyronnet\nhttp://www.via.ecp.fr/~remi"), RT_NAME, RT_VERSION), RT_NAME, wxOK |wxICON_INFORMATION);
}

void RatioFrame::OnDirCtrlChange(wxCommandEvent &event)
{
	if (m_pDirCtrl->GetFilePath() != "")
	{
		ImageLoad(m_pDirCtrl->GetFilePath());
	}
	else
	{
		event.Skip();
	}	
}

void RatioFrame::OnToggleExplorer(wxCommandEvent &event)
{
	bool newState = (m_pDirCtrl->IsShown())?FALSE:TRUE;
	GetMenuBar()->Check(MENU_VIEW_EXPLORER, newState);
	GetToolBar()->ToggleTool(TOOL_EXPLORER, newState);
	GetSizer()->Show(m_pDirCtrl,newState);
	Layout();
}

void RatioFrame::OnToggleFitOnPage(wxCommandEvent &event)
{
	bool newState = (m_pImageBox->GetScale()!=1)?FALSE:TRUE;
	GetMenuBar()->Check(MENU_VIEW_FITONPAGE, newState);
	GetToolBar()->ToggleTool(TOOL_FITONPAGE, newState);
	m_pImageBox->SetScale((newState)?scFIT_TO_PAGE_IF_LARGER:1);
	Layout();
}


void RatioFrame::OnToggleJPEGLossless(wxCommandEvent &event)
{
	m_bJPEGlossless = (m_bJPEGlossless)?FALSE:TRUE;
	GetMenuBar()->Check(MENU_IMAGE_JPEG_LOSSLESS, m_bJPEGlossless);
	GetToolBar()->ToggleTool(TOOL_JPEGLOSSLESS, m_bJPEGlossless);
}

void RatioFrame::OnFileOpen(wxCommandEvent &event)
{
	wxString name;
	name = wxFileSelector(_("Open a file"),"","","", RPHOTO_FILTERIMGLIST_WORLD,wxOPEN | wxFILE_MUST_EXIST);
    if (name != "")
    {
        ImageCleanup();
        m_sFilename = m_sOriginalFilename = name;
        ImageLoad(m_sFilename);
    }
}

void RatioFrame::OnFileSave(wxCommandEvent &event)
{
    if (m_bJPEGlossless)
    {
		wxCopyFile(m_sFilename,m_sOriginalFilename,TRUE);
    }
    else if (m_pImageBox->GetImage() != NULL)
    {
        if (isJPEGFile(m_sFilename))
        {
            wxString squality = "";   m_pConfig->Read("JPEG/Quality",&squality);
            long quality = 0;          squality.ToLong(&quality);
            m_pImageBox->GetImage().SetOption("quality", (int)quality);
            m_pImageBox->GetImage().SaveFile(m_sFilename, wxBITMAP_TYPE_JPEG);
        }
        else  m_pImageBox->GetImage().SaveFile(m_sFilename);
        ImageLoad(m_sFilename);
    }
}

void RatioFrame::OnFileSaveAs(wxCommandEvent &event)
{
    wxString name;
	name = wxFileSelector(_("Save a file"),"","","", RPHOTO_FILTERIMGLIST_WORLD,wxSAVE);
    if (name != "")
    {
        if ((!m_bDirty) && (m_bJPEGlossless) && isJPEGFile(m_sOriginalFilename) && isJPEGFile(name))
        {
       	    wxCopyFile(m_sFilename, name, TRUE);
        }
        else if (m_pImageBox->GetImage() != NULL)
        {
            if (isJPEGFile(m_sFilename))
            {
                wxString squality = "";   m_pConfig->Read("JPEG/Quality",&squality);
                long quality = 0;          squality.ToLong(&quality);
                m_pImageBox->GetImage().SetOption("quality", (int)quality);
                m_pImageBox->GetImage().SaveFile(name, wxBITMAP_TYPE_JPEG);
            }
            else  m_pImageBox->GetImage().SaveFile(name);
        }

        ImageCleanup();
        m_sFilename = m_sOriginalFilename = name;
        m_pDirCtrl->ReCreateTree();
        ImageLoad(m_sFilename);
    }
}


void RatioFrame::OnFileDelete(wxCommandEvent &event)
{
    if (wxMessageBox(_("Are you sure to delete the file ") + m_sFilename + " ?", _("Delete confirmation"), wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        wxString path, name;
        path = "";
        name = m_pDirCtrl->GetFilePath();
        // Find another file
        OnFileNext(event);
        if (m_pDirCtrl->GetFilePath() == name) OnFilePrev(event);
        if (m_pDirCtrl->GetFilePath() == name) path = m_pDirCtrl->GetPath();
        wxRemoveFile(name);
        m_pDirCtrl->ReCreateTree();
        if (path != "")  m_pDirCtrl->SetPath(path); else UpdateDirCtrl();
    }
}

void RatioFrame::OnFileReload(wxCommandEvent &event)
{
	ImageLoad(m_sOriginalFilename);
}

void RatioFrame::OnFilePrev(wxCommandEvent &event)
{
    wxTreeCtrl * tree;
    tree = m_pDirCtrl->GetTreeCtrl();
    if (tree->GetPrevSibling(tree->GetSelection()).IsOk())
	{
		tree->SelectItem(tree->GetPrevSibling(tree->GetSelection()));
	}
    if ((m_pDirCtrl->GetFilePath() == "") && (tree->GetNextSibling(tree->GetSelection()).IsOk()))
    {
        tree->SelectItem(tree->GetNextSibling(tree->GetSelection()));
    }
}

void RatioFrame::OnFileNext(wxCommandEvent &event)
{
    wxTreeCtrl * tree;
    tree = m_pDirCtrl->GetTreeCtrl();
	if (tree->GetNextSibling(tree->GetSelection()).IsOk())
	{
    	tree->SelectItem(tree->GetNextSibling(tree->GetSelection()));
	}
    if ((m_pDirCtrl->GetFilePath() == "") && (tree->GetPrevSibling(tree->GetSelection()).IsOk()))
    {
        tree->SelectItem(tree->GetPrevSibling(tree->GetSelection()));
    }
}

void RatioFrame::OnImageCrop(wxCommandEvent &event)
{
	wxRect cropZone;
	cropZone = m_pImageBox->GetSelectedZone();
	if ((cropZone.width > 0) && (cropZone.height > 0))
	{
        Dirty();
        if (m_bJPEGlossless)
        {
	        wxString cropCmd = wxString::Format("-crop %dx%d+%d+%d", 
		        cropZone.width, cropZone.height, cropZone.x, cropZone.y);
	        CallJPEGTranTryPerfect(cropCmd);
        }
        else
        {
            m_pImageBox->SetImage(m_pImageBox->GetImage().GetSubImage(cropZone));
            m_pImageBox->TrackerReset();
        }
	}
	else
	{
		GetStatusBar()->SetStatusText(_("No cropping zone defined."));
	}
}

void RatioFrame::OnImageRotateLeft(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
	    CallJPEGTranTryPerfect(wxString("-rotate 270"));
    }
    else
    {
        m_pImageBox->SetImage(m_pImageBox->GetImage().Rotate90(FALSE));
        m_pImageBox->TrackerReset();
    }
}

void RatioFrame::OnImageRotateRight(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString("-rotate 90"));
    }
    else
    {
        m_pImageBox->SetImage(m_pImageBox->GetImage().Rotate90(TRUE));
        m_pImageBox->TrackerReset();
    }
}

void RatioFrame::OnImageFlipHorizontal(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString("-flip horizontal"));
    }
    else
    {
        m_pImageBox->SetImage(m_pImageBox->GetImage().Mirror(TRUE));
    }
}

void RatioFrame::OnImageFlipVertical(wxCommandEvent &event)
{
	if (m_bJPEGlossless)
    {
        CallJPEGTranTryPerfect(wxString("-flip vertical"));
    }
    else
    {
        m_pImageBox->SetImage(m_pImageBox->GetImage().Mirror(FALSE));
    }
}

void RatioFrame::OnImageJPEGOptimize(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString("-optimize"));
    }
}

void RatioFrame::OnImageJPEGProgressive(wxCommandEvent &event)
{
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString("-progressive"));
    }
}

void RatioFrame::OnImageJPEGDetectQuality(wxCommandEvent &event)
{
    wxString fname;
    wxImage & image = m_pImageBox->GetImage();
    fname = wxFileName::CreateTempFileName("rphoto_test");
    int quality = 80, prevQual = 0, minQual = 0, maxQual = 100;
    long sizeTarget, sizeCur;

    wxBeginBusyCursor();
    sizeTarget = wxFile(m_sFilename).Length();
    while (abs(quality - prevQual) > 1)
    {
        prevQual = quality;
        image.SetOption("quality", quality);
        image.SaveFile(fname, wxBITMAP_TYPE_JPEG);
        sizeCur = wxFile(fname).Length();
        if (sizeCur > sizeTarget)
        {
            maxQual = quality;
            quality = (int)((quality + minQual) / 2);
        }
        if (sizeCur < sizeTarget)
        {
            minQual = quality;
            quality = (int)((quality + maxQual) / 2);
        }
    }
    wxRemoveFile(fname);
    wxEndBusyCursor();
    m_pConfig->Write("JPEG/Quality", wxString::Format("%d",quality));
    GetStatusBar()->SetStatusText(wxString::Format(_("Detected the quality %d."), quality));

}

void RatioFrame::OnRatioPOrL(wxCommandEvent &event)
{
	GetMenuBar()->Check(MENU_RATIO_PORL, TRUE);
	GetToolBar()->ToggleTool(TOOL_OR_P_OR_L, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_PORTRAIT, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_LANDSCAPE, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_P_OR_L, TRUE);
	m_pImageBox->SetOrientation(0);
}

void RatioFrame::OnRatioPortrait(wxCommandEvent &event)
{
	GetMenuBar()->Check(MENU_RATIO_PORTRAIT, TRUE);
	GetToolBar()->ToggleTool(TOOL_OR_P_OR_L, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_PORTRAIT, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_LANDSCAPE, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_PORTRAIT, TRUE);
	m_pImageBox->SetOrientation(-1);
}

void RatioFrame::OnRatioLandscape(wxCommandEvent &event)
{
	GetMenuBar()->Check(MENU_RATIO_LANDSCAPE, TRUE);
	GetToolBar()->ToggleTool(TOOL_OR_P_OR_L, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_PORTRAIT, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_LANDSCAPE, FALSE);
	GetToolBar()->ToggleTool(TOOL_OR_LANDSCAPE, TRUE);
	m_pImageBox->SetOrientation(1);
}

bool RatioFrame::CalcRatio(const wxString &str)
{
	long lNum, lDenom;
	double ratio = -1;

	if (str.BeforeFirst(':').ToLong(&lNum) && 	
		str.AfterFirst(':').BeforeFirst(' ').ToLong(&lDenom))
	{
		// Parsing OK
		if ((lNum > 0) && (lDenom > 0)) ratio = (double)lNum / lDenom;
        if (lNum == 0) ratio = 0;
	}

	if (ratio == -1)
	{
		// Erreur
		wxMessageBox(wxString::Format(_("Invalid fraction : %ld / %ld\nThis should have the following format : 'x:y description'"), lNum, lDenom));
		return FALSE;
	}
	else
	{
		if (ratio != m_pImageBox->GetRatio())
		{	
			m_pImageBox->SetRatio(ratio);
			// wxMessageBox(wxString::Format("Fraction : %f", ratio));
		}
		return TRUE;
	}

}


void RatioFrame::OnRatioChange(wxCommandEvent &event)
{
	int sel;
	if (event.GetId() == MENU_RATIO_R_NONE)	m_pRatioCombo->SetSelection(0);
	if (event.GetId() == MENU_RATIO_R_NUMERIC)	m_pRatioCombo->SetSelection(1);
	if (event.GetId() == MENU_RATIO_R_CLASSIC)	m_pRatioCombo->SetSelection(2);
	if (event.GetId() == MENU_RATIO_R_SQUARE)	m_pRatioCombo->SetSelection(3);
	if (event.GetId() == WIDGET_RATIOCOMBO)
			 sel = event.GetSelection();
		else sel = m_pRatioCombo->GetSelection();
	if (sel <= 3) { GetMenuBar()->Check(MENU_RATIO_R_NONE + m_pRatioCombo->GetSelection(), TRUE); GetMenuBar()->Enable(MENU_RATIO_R_CUSTOM, FALSE);}
			else  { GetMenuBar()->Enable(MENU_RATIO_R_CUSTOM, TRUE); GetMenuBar()->Check(MENU_RATIO_R_CUSTOM, TRUE);}

	CalcRatio(m_pRatioCombo->GetStringSelection());
}

void RatioFrame::OnRatioEnter(wxCommandEvent &event)
{
	if (CalcRatio(event.GetString()))
	{
		m_pRatioCombo->Append(event.GetString());
		m_pRatioCombo->SetSelection(m_pRatioCombo->GetCount()-1);
		wxCommandEvent evt;
		OnRatioChange(evt); 
	}
}

void RatioFrame::OnPreferences(wxCommandEvent &event)
{
    // Run the dialog
    m_pConfigDialog->loadFromConfig();
    m_pConfigDialog->ShowModal();
    DoConfig();
}

bool RatioFrame::isJPEGFile(const wxString & name)
{
    wxFileName fn(name);
    wxString ext = fn.GetExt();
    ext.MakeLower();
    if (ext == "jpeg") return TRUE;
    if (ext == "jpg") return TRUE;
    return FALSE;
}

void RatioFrame::Dirty(bool isJPEGLossless)
{
    if ((!isJPEGLossless) || (!m_bJPEGlossless) )   m_bDirty = TRUE;
}

void RatioFrame::Clean()
{
    m_bDirty = FALSE;
}

