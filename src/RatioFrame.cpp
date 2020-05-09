/* ****************************************************************************
 * RatioFrame.cpp                                                             *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * RPhoto : image tool for a quick and efficient use with digital photos      *
 * Copyright (C) 2004 - Rmi Peyronnet <remi+rphoto@via.ecp.fr>               *
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
#include <libexif/exif-entry.h>
#include "../resources/resource.h"
#include "../resources/resource.inc" // Include XPM Data
#include "RatioFrame.h"
#include <wx/process.h>
#include <wx/file.h>
#include <wx/txtstrm.h>
#include <wx/process.h>
#include "../lib/wxmisc/str64.h"
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <windows.h>
#endif

#include "wxJPEGExifHandler/imagejpg.h" // for wxImageOpt

#include <math.h>

// Widget ListCtrlResize
#define USE_LISTCTRLRESIZE
#ifdef USE_LISTCTRLRESIZE
#include <wxmisc/ListCtrlResize.h>
#else
#include <wx/listctrl.h>
#define wxListCtrlResize wxListCtrl
#endif

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ListOfUndo);


IMPLEMENT_CLASS(RatioFrame, wxFrame)


static wxEJPGHandler::wxImageOpt imageOpts;


#define BEGIN_IMAGE()		imageUndoPoint();
#define END_IMAGE()			UpdateControlsState();

#define BEGIN_IMAGE_MANIP(img)	BEGIN_IMAGE(); imageOpts.getImageOptions(img);
#define END_IMAGE_MANIP(img)	imageOpts.setImageOptions(img); END_IMAGE();

#define BEGIN_IMAGE_JPEG()	BEGIN_IMAGE();
#define END_IMAGE_JPEG()	END_IMAGE();



static int dummyDeclForIntelliSense;

static bool isInInit = false;
static bool isInUpdate = false;

RatioFrame::RatioFrame(wxWindow* parent, 
					   wxWindowID id, 
					   const wxString& title, 
					   const wxPoint& pos, 
					   const wxSize& size, 
					   long style, 
					   const wxString& name)
	: wxFrame(parent, id, title, pos, size, style, name)
{
	wxFileName fn;
	wxString exeFolder = wxT("");

    // Warning remover
    dummyDeclForIntelliSense = 0;
    isInInit = true;
    
	// Local Working Directory Stuff
#ifdef __WXMSW__
	TCHAR  szthis[300];
	TCHAR * c = szthis + GetModuleFileName(0, szthis, 300);
	while(*c != '\\') c--; // c POINTE A LA FIN, RECULER TANT QUE NON '\\'
	*c = 0;
	exeFolder = wxString(szthis);
	// SetCurrentDirectory(szthis);
    m_locale.AddCatalogLookupPathPrefix(exeFolder+ "\\lang");
#else
#ifdef BINDIR
// Preprocessor Guru by http://www.decompile.com/cpp/faq/file_and_line_error_string.htm
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
    char exeChar[] = TOSTRING(BINDIR);
    exeFolder = wxString(exeChar,wxConvLocal);
    // Quick Debian Hack : remove /debian/rphoto from BINDIR
    if (exeFolder.Find(wxT("debian/rphoto")) != -1)
    {
      exeFolder = exeFolder.Mid(exeFolder.Find(wxT("debian/rphoto")) + ::wxStrlen(wxT("debian/rphoto")));
    }
    //wxLogDebug(wxT("exeFolder : ") + exeFolder); 
#endif    
#endif
    
	// Locale Stuff
    m_locale.Init();
    m_locale.AddCatalog(wxT("rphoto"));

    // Variables
	m_bTemp = FALSE;
    m_bDirty = FALSE;
    m_bJPEGlossless = FALSE;
	m_isFileLoading = FALSE;
	m_isAutorotating = FALSE;
	m_bSaveCurFolder = FALSE;
	m_iOrientation = 1;
	m_HistoryUndo.DeleteContents(true);
	m_HistoryRedo.DeleteContents(true);
	m_iToolbarWidth = 0;

	// Icon
	SetIcon(wxIcon(xpm_FilesPhoto));
	Maximize(true);

    InitConfig();
    InitMenu();
    InitStatusBar();
    InitToolBar();
    InitControls();
    InitAccelerator();

	fn = wxFileName(wxT("rphoto.htb"));
	if (exeFolder != wxT("")) fn.MakeAbsolute(exeFolder);
	m_help.AddBook(fn.GetFullPath());

    DoConfig();
	m_pDirCtrl->SetSize(500, 500); // Small hack to size correctly the dirCtrl
	m_pImageBox->SetFocus();

    SetSizeHints(600,500);
    Layout();

	SetDropTarget(new rpFrameDropTarget(this));

    isInInit = false;

	wxString defPath;
	m_pConfig->Read(wxT("Defaults/DefPath"), &defPath, wxT(""));
	if (defPath.Length() > 1)  m_pDirCtrl->ExpandPath(defPath);

	UpdateControlsState();
	imageUpdateExif();
	m_bDirty = FALSE;
}

void RatioFrame::InitConfig()
{
    // Config
    m_pConfig = new wxConfig(wxT("RPhoto"));
    m_pConfigDialog = new wxConfigDialog(*m_pConfig, this, -1, _("Preferences"), wxDefaultPosition, wxSize(500,450), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );

    // Setup options
    // * General stuff
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("Main"), wxT("AutoSave"), _("Main"), _("Auto Save"), FALSE);
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("Main"), wxT("AutoCrop"), _("Main"), _("Crop before saving"), FALSE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Main"), wxT("AutoSaveFolder"), _("Main"), _("Auto Save Folder"), wxT(""));
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Main"), wxT("AutoSaveSuffix"), _("Main"), _("Auto Save Suffix"), wxT(""));
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("Main"), wxT("AutoRotateCrop"), _("Main"), _("Auto Crop after Rotation"), FALSE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Main"), wxT("UndoHistory"), _("Main"), _("Undo History"), wxT("10"));
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("Main"), wxT("AutoSelectMax"), _("Main"), _("Auto Select Max"), FALSE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Main"), wxT("HandlesWidth"), _("Main"), _("Width of the handles (pixels)"), wxT("5"));

    // * JPEG stuff
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("JPEGTran"), _("JPEG"), _("JPEGTran's path"), wxT("jpegtran"));
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("JHead"), _("JPEG"), _("JHead's path"), wxT("jhead"));
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("JPEG"), wxT("AutoRot"), _("JPEG"), _("AutoRot on Load"), FALSE);
    // new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("WrJpgCom"), _("JPEG"), _("WrJpgCom's path"), wxT("wrjpgcom"));
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("JPEG"), wxT("DisplayLosslessWarnings"), _("JPEG"), _("Display Lossless warnings"), TRUE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("Quality"), _("JPEG"), _("JPEG Quality (0-100)"), wxT("80"));
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("FavExif"), _("JPEG"), _("Favorite Exif Tags"), RPHOTO_DEFAULT_EXIF_FAVORITES);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("JPEG"), wxT("ComEncoding"), _("JPEG"), _("Comments Encoding"), RPHOTO_DEFAULT_COMMENT_ENCODING);
    
    // * Default settings
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Defaults"), wxT("DefPath"), _("Defaults"), _("Default Path"), wxT(""));
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("Defaults"), wxT("SaveDefPath"), _("Defaults"), _("Remember Last Opened Path"), FALSE);
	wxArrayString ratios;
    wxStringTokenizer tkz(wxString(RPHOTO_DEFAULT_FORCED_RATIO) + wxT("|") + wxString(RPHOTO_DEFAULT_CUSTOM_RATIO), wxT("|"));
	while ( tkz.HasMoreTokens() )
	{
		ratios.Add(tkz.GetNextToken());
	}
	m_saRatios = ratios;
    new wxConfigDialog_EntryCombo(*m_pConfigDialog, wxT("Defaults"), wxT("Ratio"), _("Defaults"), _("Ratio"), ratios, ratios.Item(0));
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Defaults"), wxT("RatioList"), _("Defaults"), _("Ratio's List"), RPHOTO_DEFAULT_CUSTOM_RATIO);
	ratios.Clear();
	tkz.Reinit(wxString(RPHOTO_DEFAULT_FORCED_GUIDE_RATIO) + wxT("|") + wxString(RPHOTO_DEFAULT_CUSTOM_GUIDE_RATIO));
	while ( tkz.HasMoreTokens() )
	{
		ratios.Add(tkz.GetNextToken());
	}
	
    new wxConfigDialog_EntryCombo(*m_pConfigDialog, wxT("Defaults"), wxT("GuideRatio"), _("Defaults"), _("Guide Ratio"),ratios, ratios.Item(0));
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Defaults"), wxT("GuideRatioList"), _("Defaults"), _("Guide Lines Ratio's List"), RPHOTO_DEFAULT_CUSTOM_GUIDE_RATIO);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("Defaults"), wxT("ResizeList"), _("Defaults"), _("Resize's List"), RPHOTO_DEFAULT_CUSTOM_RESIZE);


	wxString orientation[3];
    orientation[0] = _("Automatic");
    orientation[1] = _("Landscape");
    orientation[2] = _("Portrait");
    new wxConfigDialog_EntryRadio(*m_pConfigDialog, wxT("Defaults"), wxT("Orientation"), _("Defaults"), _("Orientation"), 3, orientation, 0);

    // * Appearance
    new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("View"), wxT("FitWindow"), _("Appearance"), _("Fit the image inside the window"), TRUE);
    // new wxConfigDialog_EntryCheck(*m_pConfigDialog, wxT("View"), wxT("Explorer"), _("Appearance"), _("View explorer panel"), TRUE);
    new wxConfigDialog_EntryTextEdit(*m_pConfigDialog, wxT("View"), wxT("GreyOut"), _("Appearance"), _("Grey Out cutted off area (percent)"), wxT("50"));

    m_pConfigDialog->doLayout();
}

void RatioFrame::DoConfig()
{
    wxCommandEvent evt;
    int i;
	double d;
    bool opt;
    // Toggle Explorer
	/*
    opt = TRUE; 
    m_pConfig->Read(wxT("View/Explorer"), &opt);
    if ( (!opt) ^ (!m_pDirCtrl->IsShown()) )  OnToggleExplorer(evt);
	*/
    // Fit On Page
    opt = TRUE; 
    m_pConfig->Read(wxT("View/FitWindow"), &opt);
    if ( (!opt) ^ ((m_pImageBox->GetScale()==1)) )  OnToggleFitOnPage(evt);
    // Orientation
    i = 0;
    m_pConfig->Read(wxT("Defaults/Orientation"), &i);
    switch(i)
    {
    case 0: OnRatioPOrL(evt);      break;
    case 1: OnRatioLandscape(evt); break;
    case 2: OnRatioPortrait(evt);  break;
    }
    // Ratio
    wxString str, token;
    str = wxT("");
    m_pConfig->Read(wxT("Defaults/RatioList"),&str);
	if (str != wxT(""))
	{
		m_saRatios.Clear();
		m_pRatioCombo->Clear();
		wxStringTokenizer tkz(wxString(RPHOTO_DEFAULT_FORCED_RATIO) + wxT("|") + str, wxT("|"));
		while ( tkz.HasMoreTokens() )
		{
			token = tkz.GetNextToken();
			m_saRatios.Add(token);
			m_pRatioCombo->Append(token);
		}
	}
    str = wxT("");
    m_pConfig->Read(wxT("Defaults/Ratio"),&str);
    if (m_pRatioCombo->FindString(str) >= 0) { m_pRatioCombo->SetSelection(m_pRatioCombo->FindString(str)); OnRatioChange(evt); }    
	else { m_pRatioCombo->SetStringSelection(str) ;evt.SetString(str); OnRatioEnter(evt); }
    // Guide Ratio
    str = wxT(""); 
	m_pConfig->Read(wxT("Defaults/GuideRatioList"),&str);
	if (str != wxT(""))
	{
		//m_saRatios.Clear();
		m_pGuideCombo->Clear();
		wxStringTokenizer tkz(wxString(RPHOTO_DEFAULT_FORCED_GUIDE_RATIO) + wxT("|") + str, wxT("|"));
		while ( tkz.HasMoreTokens() )
		{
			token = tkz.GetNextToken();
			//m_saRatios.Add(token);
			m_pGuideCombo->Append(token);
		}
	}
    str = wxT("");
    m_pConfig->Read(wxT("Defaults/GuideRatio"),&str);
    if (m_pGuideCombo->FindString(str) >= 0) { m_pGuideCombo->SetSelection(m_pGuideCombo->FindString(str)); OnGuideChange(evt); }
	else { m_pGuideCombo->SetStringSelection(str) ; evt.SetString(str); OnGuideEnter(evt); }
	// Resize
    str = wxT("");
    m_pConfig->Read(wxT("Defaults/ResizeList"),&str);
	m_saResize.Clear();
	m_pResizeCombo->Clear();
	wxStringTokenizer tkz(wxString(RPHOTO_DEFAULT_FORCED_RESIZE) + wxT("|") + str, wxT("|"));
	while ( tkz.HasMoreTokens() )
	{
		token = tkz.GetNextToken();
		m_saResize.Add(token);
		m_pResizeCombo->Append(token);
	}
    str = wxT("");
	// Comments encoding
	str = RPHOTO_DEFAULT_COMMENT_ENCODING;
    m_pConfig->Read(wxT("JPEG/ComEncoding"), &str);
    m_sComEncoding = str;
    // Auto Save
    opt = FALSE;
    m_pConfig->Read(wxT("Main/AutoSave"), &opt);
    m_bAutoSave = opt;
    m_pConfig->Read(wxT("Main/AutoSaveFolder"), &str);
	m_sAutoSaveFolder = str;
    m_pConfig->Read(wxT("Main/AutoSaveSuffix"), &str);
	m_sAutoSaveSuffix = str;
	// AutoSelectMax
    opt = FALSE;
    m_pConfig->Read(wxT("Main/AutoSelectMax"), &opt);
    m_bAutoSelectMax = opt;
	// AutoRot
    opt = FALSE;
    m_pConfig->Read(wxT("JPEG/AutoRot"), &opt);
	m_bAutoRot = opt;
	// Default Path
    opt = FALSE;
    m_pConfig->Read(wxT("Defaults/SaveDefPath"), &opt);
	m_bSaveCurFolder = opt;
	// AutoCrop
    opt = FALSE;
    m_pConfig->Read(wxT("Main/AutoCrop"), &opt);
	m_bAutoCrop = opt;
	// Grey Out area
	m_pConfig->Read(wxT("View/GreyOut"), &str);
	str.ToDouble(&d); i = (int)d;
	if (i < 0) i = 0;
	if (i > 100) i = 100;
	m_pImageBox->GetRectTracker().SetGreyOutColour(i);
	// Handles Width
	m_pConfig->Read(wxT("Main/HandlesWidth"), &str);
	str.ToDouble(&d); i = (int)d;
	if (i < 1) i = 1;
	if (i > 100) i = 100;
	m_pImageBox->GetRectTracker().SetHandlesWidth(i);
	m_pImageBox->GetLineTracker().SetHandlesWidth(i);
}

void RatioFrame::InitMenu()
{
	// The space in the menu is intended, to avoid that shortcuts interfer with controls
    // Menu
	m_pMenuBar = new wxMenuBar();
	wxMenu * curMenu, * subMenu;
	// - File Menu
	curMenu = new wxMenu();
	curMenu->Append(MENU_FILE_OPEN, _("&Open..."), _("Open a file."));
	curMenu->Append(MENU_FILE_SAVE, _("&Save") + wxString(wxT("\ts")), _("Save the current file."));
	curMenu->Append(MENU_FILE_SAVEAS, _("&Save as..."), _("Save the current file under another name."));
	curMenu->Append(MENU_FILE_RELOAD, _("&Reload"), _("Reload original file."));
	curMenu->Append(MENU_FILE_DELETE, _("&Delete") + wxString(wxT("\tDel")), _("Suppress the file."));
	curMenu->Append(MENU_FILE_MOVE, _("&Move") + wxString(wxT("\tm")), _("Move the file to the selected folder."));
	curMenu->Append(MENU_FILE_MOVENEXT, _("Mo&ve Next") + wxString(wxT("\t*")), _("Move the file to the selected folder and go to the next file."));
	curMenu->Append(MENU_FILE_SAVEPATH, _("Save to &Path") + wxString(wxT("\tp")), _("Save the file to the selected folder."));
	curMenu->Append(MENU_FILE_SAVEPATHN, _("Save to Path &and Next") + wxString(wxT("\ta")), _("Save the file to the selected folder and go to the next file."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_PREV, _("&Previous File")+wxString(wxT("\tPageUp")), _("Open the previous file in the explorer."));
	curMenu->Append(MENU_FILE_NEXT, _("&Next File")+wxString(wxT("\tPageDown")), _("Open the next file in the explorer."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_PREFS, _("&Preferences"), _("Define application's settings."));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_FILE_QUIT, _("&Quit")+wxString(wxT("\tQ")), _("Quit the application."));
	m_pMenuBar->Append(curMenu, _("&File"));
	// - Image Menu
	curMenu = new wxMenu();
	curMenu->Append(MENU_IMAGE_UNDO, _("&Undo")+wxString(wxT("\tCtrl-Z")), _("Undo previous image operation."));
	curMenu->Append(MENU_IMAGE_REDO, _("&Redo")+wxString(wxT("\tCtrl-R")), _("Redo last undoed operation."));
	curMenu->AppendSeparator();
	curMenu->AppendRadioItem(MENU_IMAGE_MODE_CROP, _("Crop Mode")+wxString(wxT("\tc")), _("Use this mode to crop images."));
	curMenu->AppendRadioItem(MENU_IMAGE_MODE_INCLIN, _("Inclinaison Mode")+wxString(wxT("\ti")), _("Use this mode to correct inclinaison of images."));
	//curMenu->AppendRadioItem(MENU_IMAGE_MODE_REDEYE, _("Red Eye Mode"), _("Use this mode to correct redeye"));
	curMenu->AppendSeparator();
	curMenu->Append(MENU_IMAGE_CROP, _("&Crop")+wxString(wxT("\tc")), _("Crop image to selection."));
	curMenu->Append(MENU_IMAGE_ROTATE_LEFT, _("Rotate &Left")+wxString(wxT("\tl")), _("Rotate the image 90 degrees to the left."));
	curMenu->Append(MENU_IMAGE_ROTATE_RIGHT, _("Rotate &Right")+wxString(wxT("\tr")), _("Rotate the image 90 degrees to the right."));
	curMenu->Append(MENU_IMAGE_INCLIN, _("Correct &Inclinaison")+wxString(wxT("\ti")), _("Mode Rotation : use the mouse to set horizontal / vertical lines."));
	curMenu->Append(MENU_IMAGE_FLIP_HORIZONTAL, _("Flip &Horizontal"), _("Flip the image horizontally."));
	curMenu->Append(MENU_IMAGE_FLIP_VERTICAL, _("Flip &Vertical"), _("Flip the image vertically."));
	curMenu->Append(MENU_IMAGE_COMMENT, _("Commen&t"), _("Modify comment."));
	curMenu->Append(MENU_IMAGE_RESIZE, _("Resi&ze"), _("Resize image.")+wxString(wxT("\t z")));
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
	curMenu->AppendRadioItem(MENU_RATIO_R_TV, _("&Ratio 16:9 - TV"), _("Common ratio used in numeric TV photo."));
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
	curMenu->Append(MENU_HELP_HELP, _("User &manual")+wxString(wxT("\tF1")), _("Help file."));
	curMenu->AppendSeparator();
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
	GetToolBar()->AddTool(TOOL_RELOAD, _("Reload"), wxBitmap(xpm_reopen), wxNullBitmap, wxITEM_NORMAL , _("Reload original file"), _("Reload original file"));
	GetToolBar()->AddTool(TOOL_PREV, _("Previous"), wxBitmap(xpm_arrow_left), wxNullBitmap, wxITEM_NORMAL ,_("Previous file"),_("Previous file"));
	GetToolBar()->AddTool(TOOL_NEXT, _("Next"), wxBitmap(xpm_arrow_right),wxNullBitmap, wxITEM_NORMAL , _("Next file"), _("Next file"));
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_FITONPAGE, _("Fit On Page"), wxBitmap(xpm_fullscrn), wxNullBitmap, wxITEM_CHECK , _("Fit On Page."), _("Fit On Page."));
	GetToolBar()->ToggleTool(TOOL_FITONPAGE, TRUE);
	//GetToolBar()->AddTool(TOOL_EXPLORER, _("Explore"), wxBitmap(xpm_explorer), wxNullBitmap, wxITEM_CHECK , _("Show the explorer"), _("Show the explorer"));
	//GetToolBar()->ToggleTool(TOOL_EXPLORER, TRUE);
	//GetToolBar()->AddTool(TOOL_COLORS, _("Colors"), wxBitmap(xpm_recycle), wxNullBitmap, wxITEM_CHECK , _("Adjust Colors."), _("Adjust Colors."));
	// GetToolBar()->ToggleTool(TOOL_COLORS, TRUE);
	//GetToolBar()->AddTool(TOOL_TEXTS, _("EXIF"), wxBitmap(xpm_text), wxNullBitmap, wxITEM_CHECK , _("Exif and comments."), _("Exif and comments."));
	//GetToolBar()->ToggleTool(TOOL_TEXTS, TRUE);
	GetToolBar()->AddTool(TOOL_JPEGLOSSLESS, _("JPEG Lossless"), wxBitmap(xpm_light_green), wxBitmap(xpm_light_red), wxITEM_CHECK, _("JPEG Lossless"), _("Performs JPEG operations lossless whenever possible."));
	GetToolBar()->ToggleTool(TOOL_JPEGLOSSLESS, TRUE);
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_MODE_CROP, _("Crop"), wxBitmap(xpm_mode_crop), wxNullBitmap, wxITEM_CHECK ,  _("Crop the image"),  _("Crop the image"));
	GetToolBar()->AddTool(TOOL_MODE_INCLIN, _("Inclinaison"), wxBitmap(xpm_mode_inclinaison),wxNullBitmap, wxITEM_CHECK , _("Correct inclinaison"), _("Correct inclinaison"));
	// GetToolBar()->AddTool(TOOL_MODE_REDEYE, _("Red Eye"), wxBitmap(xpm_mode_redeye), wxNullBitmap, wxITEM_CHECK ,  _("Suppress RedEye"),  _("Suppress RedEye"));
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_UNDO, _("Undo"), wxBitmap(xpm_undo), wxNullBitmap, wxITEM_NORMAL , _("Undo previous operation"), _("Undo previous operation"));
	GetToolBar()->AddTool(TOOL_CROP, _("Crop"), wxBitmap(xpm_crop), wxNullBitmap, wxITEM_NORMAL ,  _("Crop the image"),  _("Crop the image"));
	GetToolBar()->AddTool(TOOL_ROTATE_LEFT, _("Rotate Left"), wxBitmap(xpm_rotate_left),wxNullBitmap, wxITEM_NORMAL , _("Rotate the image to the left"), _("Rotate the image to the left"));
	GetToolBar()->AddTool(TOOL_ROTATE_RIGHT, _("Rotate Right"), wxBitmap(xpm_rotate_right),wxNullBitmap, wxITEM_NORMAL , _("Rotate the image to the right"), _("Rotate the image to the right"));
	GetToolBar()->AddTool(TOOL_INCLIN, _("Inclinaison"), wxBitmap(xpm_inclinaison),wxNullBitmap, wxITEM_NORMAL , _("Correct inclinaison"), _("Correct inclinaison"));
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(TOOL_OR_P_OR_L, _("Free Rotation"), wxBitmap(xpm_landorpo), wxNullBitmap, wxITEM_RADIO ,_("Automatic Selection of Orientation"), _("Automatic Selection of Orientation"));
	GetToolBar()->AddTool(TOOL_OR_LANDSCAPE, _("Landscape"), wxBitmap(xpm_landscap), wxNullBitmap, wxITEM_RADIO ,_("Landscape Orientation"), _("Landscape Orientation"));
	GetToolBar()->AddTool(TOOL_OR_PORTRAIT, _("Portrait"), wxBitmap(xpm_portrait), wxNullBitmap, wxITEM_RADIO ,_("Portrait Orientation"), _("Portrait Orientation"));
	// Note : static texts are not added here but in OnWindowResize if there is enough space
	m_pRatioCombo = new wxComboBox(GetToolBar(), WIDGET_RATIOCOMBO, _("test"));
	m_pRatioCombo->Append(_("0:0 (Free)                     "));
	m_pRatioCombo->SetSelection(0);
	m_pRatioCombo->SetWindowStyle(wxCB_DROPDOWN);
	m_pRatioCombo->SetToolTip(_("Ratio"));
	//m_pTextRatioCombo = new wxStaticText(GetToolBar(), TEXT_RATIOCOMBO, _("Ratio") + wxT(" : "));
	//GetToolBar()->AddControl(m_pTextRatioCombo);
	GetToolBar()->AddControl(m_pRatioCombo);
	m_pResizeCombo = new wxComboBox(GetToolBar(), WIDGET_RESIZECOMBO, _("test"));
	m_pResizeCombo->Append(_("Resize") + wxT("                 "));
	m_pResizeCombo->SetSelection(0);
	m_pResizeCombo->SetWindowStyle(wxCB_DROPDOWN);
	m_pResizeCombo->SetToolTip(_("Resize"));
	//m_pTextResizeCombo = new wxStaticText(GetToolBar(), TEXT_RESIZECOMBO, wxT("   ") + _("Resize") + wxT(" : "));
	//GetToolBar()->AddControl(m_pTextResizeCombo);
	GetToolBar()->AddControl(m_pResizeCombo);
	m_pGuideCombo = new wxComboBox(GetToolBar(), WIDGET_GUIDECOMBO, _("test"));
	m_pGuideCombo->Append(_("Guide Lines")+wxT("             "));
	m_pGuideCombo->SetSelection(0);
	m_pGuideCombo->SetWindowStyle(wxCB_DROPDOWN);
	m_pGuideCombo->SetToolTip(_("Guide Lines"));
	//m_pTextGuideCombo = new wxStaticText(GetToolBar(), TEXT_GUIDECOMBO, ("   ") + _("Guide Lines") + wxT(" : "));
	//GetToolBar()->AddControl(m_pTextGuideCombo);
	GetToolBar()->AddControl(m_pGuideCombo);
    GetToolBar()->Realize();
}
 
void RatioFrame::InitAccelerator()
{
    // Accelerator
    const int numEntries = 16;
    wxAcceleratorEntry entries[numEntries];
    int i = 0;
    entries[i++].Set(wxACCEL_CTRL, (int) 'Q', MENU_FILE_QUIT);
    entries[i++].Set(wxACCEL_CTRL, (int) 'Z', MENU_IMAGE_UNDO);
    entries[i++].Set(wxACCEL_CTRL, (int) 'R', MENU_IMAGE_REDO);
    entries[i++].Set(wxACCEL_NORMAL, (int) 's', MENU_FILE_SAVE);
    entries[i++].Set(wxACCEL_NORMAL, WXK_PAGEDOWN /* WXK_NEXT */ , MENU_FILE_NEXT);
    entries[i++].Set(wxACCEL_NORMAL, WXK_PAGEUP /* WXK_PRIOR */, MENU_FILE_PREV);
    entries[i++].Set(wxACCEL_NORMAL, WXK_DELETE, MENU_FILE_DELETE);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'm', MENU_FILE_MOVE);
    entries[i++].Set(wxACCEL_NORMAL, (int) '*', MENU_FILE_MOVENEXT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'l', MENU_IMAGE_ROTATE_LEFT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'r', MENU_IMAGE_ROTATE_RIGHT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 't', MENU_IMAGE_COMMENT);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'i', TOOL_MODE_INCLIN);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'c', TOOL_MODE_CROP);
    entries[i++].Set(wxACCEL_NORMAL, (int) 'z', MENU_IMAGE_RESIZE);
    entries[i++].Set(wxACCEL_NORMAL, WXK_RETURN, TOOL_ACTION);
    wxASSERT(i == numEntries);
    wxAcceleratorTable accel(numEntries, entries);
    m_pImageBox->SetAcceleratorTable(accel);
	// Accelerator for special controls
	wxASSERT(numEntries > 0);
	entries[0].Set(wxACCEL_CTRL, WXK_RETURN, MENU_ACCEL_APPLY_COMMENT);
	accel = wxAcceleratorTable(1, entries);
	FindWindowById(WIDGET_EDIT_COM)->SetAcceleratorTable(accel);
}

void RatioFrame::InitStatusBar()
{
	// StatusBar
	CreateStatusBar(4);
	SetStatusText(wxT("Ready"), 0);
	int width[4]; width[0] = -1; width[1] = 60; width[2] = 130; width[3] = 100;
	GetStatusBar()->SetStatusWidths(4, width);
}

void RatioFrame::InitControls()
{
	wxWindow * panel;
	// Other Controls
	m_pSplitHoriz = new wxSplitterWindow(this, -1);
	m_pSplitVert  = new wxSplitterWindow(m_pSplitHoriz, -1);
	// Clean style
	m_pSplitHoriz->SetWindowStyle(wxSP_3DSASH | wxSP_NOBORDER);
	m_pSplitVert->SetWindowStyle(wxSP_3DSASH | wxSP_NOBORDER);
	// Do not allow the user to unsplit by double clicking
	m_pSplitHoriz->SetMinimumPaneSize(20);	
	m_pSplitVert->SetMinimumPaneSize(20);	

	// Explorer Notebook
	m_pNbFiles = new wxNotebook(m_pSplitVert, -1);
	//m_pNbFiles->SetWindowStyle(wxNO_3D);

	// Dir Browser Ctrl
	m_pDirCtrl = new wxDirCtrl(m_pNbFiles, -1, 
		wxDirDialogDefaultFolderStr, 
		wxDefaultPosition, wxDefaultSize,
		wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_SHOW_FILTERS | wxDIRCTRL_EDIT_LABELS,
		RPHOTO_FILTERIMGLIST_WORLD,
		1 /* 1 For All Images, 4 For JPEG only */);
	m_pDirCtrl->GetTreeCtrl()->SetWindowStyle(m_pDirCtrl->GetTreeCtrl()->GetWindowStyle() | wxTR_HAS_VARIABLE_ROW_HEIGHT);
	m_pNbFiles->AddPage(m_pDirCtrl, _("Browse"), true);

	Connect(m_pDirCtrl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED,
		(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
		& RatioFrame::OnDirCtrlChange); 
	
	m_pDirCtrl->GetTreeCtrl()->Connect(m_pDirCtrl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,
		(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
		& RatioFrame::OnFileNameBeginEdit); 
	
	m_pDirCtrl->GetTreeCtrl()->Connect(m_pDirCtrl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_END_LABEL_EDIT,
		(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxTreeEventFunction)
		& RatioFrame::OnFileNameEndEdit); 

	panel = new wxPanel(m_pNbFiles, -1);
	m_pMoveCtrl = new wxDirCtrl(panel, -1, 
		wxDirDialogDefaultFolderStr, 
		wxDefaultPosition, wxDefaultSize,
		wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_DIR_ONLY);
	m_pMoveCtrl->GetTreeCtrl()->SetWindowStyle(m_pDirCtrl->GetTreeCtrl()->GetWindowStyle() | wxTR_HAS_VARIABLE_ROW_HEIGHT);
	//m_pMoveCtrl->GetTreeCtrl()->SetId(wxID_TREECTRL+2);
	panel->SetSizer(new wxBoxSizer(wxVERTICAL));
	panel->GetSizer()->Add(m_pMoveCtrl, 1, wxEXPAND);
	panel->GetSizer()->Add(new wxButton(panel, MENU_FILE_MOVE, _("Move Here")), 0, wxEXPAND);
	panel->GetSizer()->Add(new wxButton(panel, MENU_FILE_SAVEPATH, _("Save Here")), 0, wxEXPAND);
	m_pNbFiles->AddPage(panel, _("Move To"), false);

	Connect(m_pMoveCtrl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED,
		(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
		& RatioFrame::OnDirMoveChange); 

	// Property List
	m_pNbTools = new wxNotebook(m_pSplitVert, -1);

	m_pAttrFavCtrl = new wxListCtrlResize(m_pNbTools, -1, wxDefaultPosition, wxDefaultSize, 
		wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	m_pAttrFavCtrl->DeleteAllColumns();
	m_pAttrFavCtrl->InsertColumn(0, _("Tag"), wxLIST_FORMAT_LEFT /*, 20*/);
	m_pAttrFavCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_RIGHT /*, 20*/);
	m_pNbTools->AddPage(m_pAttrFavCtrl, _("Fav. Exif"), true);

	m_pAttrCtrl = new wxListCtrlResize(m_pNbTools, -1, wxDefaultPosition, wxDefaultSize, 
		wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	m_pAttrCtrl->InsertColumn(0, _("Tag"), wxLIST_FORMAT_LEFT, 20);
	m_pAttrCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_RIGHT, 20);
	m_pNbTools->AddPage(m_pAttrCtrl, _("All Exif"), false);

	panel = new wxPanel(m_pNbTools, -1);
	panel->SetSizer(new wxBoxSizer(wxVERTICAL));
	m_pTextComment = new wxTextCtrl(panel, WIDGET_EDIT_COM, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2 );
	panel->GetSizer()->Add(m_pTextComment, 1, wxEXPAND);
	panel->GetSizer()->Add(new wxButton(panel, WIDGET_APPLY_COM, _("Apply")), 0, wxEXPAND);
	FindWindowById(WIDGET_APPLY_COM)->Enable(false);
	m_pNbTools->AddPage(panel, _("Comment"), false);


	m_pAttrCtrl->SetItem(m_pAttrCtrl->InsertItem(1, wxT("Valeur")), 1, wxT("Test"));

    // Image Box
	m_pImageBox = new wxRatioImageBox (m_pSplitHoriz, -1);
    if (wxFileName::FileExists(RT_DEFAULT_IMAGE))
    {
        ImageLoad(RT_DEFAULT_IMAGE);
    }
    else
    {
        m_pImageBox->GetImage().Create(1,1);
    }
	m_pImageBox->SetWindowStyle(0);

	m_pSplitVert->SplitHorizontally(m_pNbFiles, m_pNbTools, 0);

	m_pSplitHoriz->SplitVertically(m_pSplitVert, m_pImageBox, 250);

	m_pSplitVert->SetSashGravity(0.5);

	// Must be done after sizing : wx2.6 compat.
	m_pImageBox->SetScale(scFIT_TO_PAGE_IF_LARGER);
}

void RatioFrame::UpdateControlsStateEvt(wxCommandEvent & event)
{
   UpdateControlsState();
}

void RatioFrame::UpdateControlsState()
{
	bool curState;

	if (isInInit) return;
    if (isInUpdate) return;
	isInUpdate = true;

	// Disable not yet Implemented
	/*
	GetToolBar()->EnableTool(TOOL_EXPLORER, false);
	GetToolBar()->EnableTool(TOOL_COLORS, false);
	GetToolBar()->EnableTool(TOOL_TEXTS, false); 
	GetToolBar()->EnableTool(TOOL_MODE_REDEYE, false); 
	GetMenuBar()->Enable(MENU_IMAGE_MODE_REDEYE, false);
	GetMenuBar()->Enable(MENU_VIEW_EXPLORER, false);
	*/
	// JPEG Lossless
    GetMenuBar()->Enable(MENU_IMAGE_JPEG, m_bJPEGlossless);
    GetMenuBar()->Check(MENU_IMAGE_JPEG_LOSSLESS, m_bJPEGlossless);
    GetToolBar()->ToggleTool(TOOL_JPEGLOSSLESS, m_bJPEGlossless);
    GetToolBar()->EnableTool(TOOL_JPEGLOSSLESS, m_bJPEGlossless);

	GetToolBar()->EnableTool(TOOL_UNDO, m_bDirty);	// Dirty

	// Undo
	if (m_HistoryUndo.GetCount() > 0)
	{
		GetMenuBar()->Enable(MENU_IMAGE_UNDO, true);
		GetToolBar()->EnableTool(TOOL_UNDO, true);
	}
	else
	{
		GetMenuBar()->Enable(MENU_IMAGE_UNDO, false);
		GetToolBar()->EnableTool(TOOL_UNDO, false);
	}
	GetMenuBar()->Enable(MENU_IMAGE_REDO, (m_HistoryRedo.GetCount() > 0));
	// Mode
	if (m_pImageBox->IsModeInclinaison())
	{
		// Toolbar
		GetToolBar()->ToggleTool(TOOL_MODE_CROP, false);
		GetToolBar()->ToggleTool(TOOL_MODE_REDEYE, false);
		GetToolBar()->ToggleTool(TOOL_MODE_INCLIN, true);
		GetToolBar()->EnableTool(TOOL_CROP, false);
		GetToolBar()->EnableTool(TOOL_INCLIN, true);
		GetToolBar()->EnableTool(TOOL_OR_P_OR_L, true);
		GetToolBar()->EnableTool(TOOL_OR_LANDSCAPE, true);
		GetToolBar()->EnableTool(TOOL_OR_PORTRAIT, true);
		m_pRatioCombo->Enable(true);
		// Menubar
		//GetMenuBar()->Check(MENU_IMAGE_MODE_REDEYE, false);
		GetMenuBar()->Check(MENU_IMAGE_MODE_CROP, false);
		GetMenuBar()->Check(MENU_IMAGE_MODE_INCLIN, true);
		GetMenuBar()->Enable(MENU_IMAGE_CROP, false);
		GetMenuBar()->Enable(MENU_IMAGE_INCLIN, true);
	}
	else
	{
		// Toolbar
		GetToolBar()->ToggleTool(TOOL_MODE_INCLIN, false);
		GetToolBar()->ToggleTool(TOOL_MODE_REDEYE, false);
		GetToolBar()->ToggleTool(TOOL_MODE_CROP, true);
		GetToolBar()->EnableTool(TOOL_CROP, true);
		GetToolBar()->EnableTool(TOOL_INCLIN, false);
		GetToolBar()->EnableTool(TOOL_OR_P_OR_L, true);
		GetToolBar()->EnableTool(TOOL_OR_LANDSCAPE, true);
		GetToolBar()->EnableTool(TOOL_OR_PORTRAIT, true);
		m_pRatioCombo->Enable(true);
		// Menubar
		GetMenuBar()->Check(MENU_IMAGE_MODE_INCLIN, false);
		//GetMenuBar()->Check(MENU_IMAGE_MODE_REDEYE, false);
		GetMenuBar()->Check(MENU_IMAGE_MODE_CROP, true);
		GetMenuBar()->Enable(MENU_IMAGE_CROP, true);
		GetMenuBar()->Enable(MENU_IMAGE_INCLIN, false);
	}

	if (m_pMoveCtrl->GetPath() != wxT(""))
	{
		GetMenuBar()->Enable(MENU_FILE_MOVE, true);
		m_pMoveCtrl->FindWindowById(MENU_FILE_MOVE, m_pMoveCtrl->GetParent())->Enable(true);
		m_pMoveCtrl->FindWindowById(MENU_FILE_SAVEPATH, m_pMoveCtrl->GetParent())->Enable(true);
	}
	else
	{
		GetMenuBar()->Enable(MENU_FILE_MOVE, false);
		m_pMoveCtrl->FindWindowById(MENU_FILE_MOVE, m_pMoveCtrl->GetParent())->Enable(false);
		m_pMoveCtrl->FindWindowById(MENU_FILE_SAVEPATH, m_pMoveCtrl->GetParent())->Enable(false);
	}

	// Comment
	if (m_pTextComment->IsModified())
	{
		m_pTextComment->SetStyle(0, m_pTextComment->GetLastPosition(), wxTextAttr(*wxRED));
		FindWindowById(WIDGET_APPLY_COM)->Enable(true);
		Dirty();
	}
	else
	{
		m_pTextComment->SetStyle(0, m_pTextComment->GetLastPosition(), wxTextAttr(*wxBLACK));
		FindWindowById(WIDGET_APPLY_COM)->Enable(false);
		m_pTextComment->DiscardEdits();
	}

	// Scale
	curState = m_pImageBox->GetScale()!=1;
	GetMenuBar()->Check(MENU_VIEW_FITONPAGE, curState);
	GetToolBar()->ToggleTool(TOOL_FITONPAGE, curState);

	SetTitle(wxString::Format(_("%s - RPhoto"), wxFileName(m_sOriginalFilename).GetFullName().c_str()));

	isInUpdate = false;
}

RatioFrame::~RatioFrame()
{
	ImageCleanup();
	if (m_bTemp && wxFileExists(m_sFilename)) wxRemoveFile(m_sFilename);
    if (m_pConfig) delete m_pConfig;
    if (m_pConfigDialog) delete m_pConfigDialog;
}

// Event Map Table
BEGIN_EVENT_TABLE(RatioFrame, wxFrame)
   EVT_MENU(MENU_FILE_OPEN, RatioFrame::OnFileOpen)
   EVT_MENU(MENU_FILE_SAVE, RatioFrame::OnFileSave)
   EVT_MENU(MENU_FILE_SAVEAS, RatioFrame::OnFileSaveAs)
   EVT_MENU(MENU_FILE_DELETE, RatioFrame::OnFileDelete)
   EVT_MENU(MENU_FILE_MOVE, RatioFrame::OnFileMove)
   EVT_MENU(MENU_FILE_MOVENEXT, RatioFrame::OnFileMoveAndNext)
   EVT_MENU(MENU_FILE_SAVEPATH, RatioFrame::OnFileSavePath)
   EVT_MENU(MENU_FILE_SAVEPATHN, RatioFrame::OnFileSavePathAndNext)
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
   EVT_MENU(MENU_IMAGE_UNDO, RatioFrame::OnImageUndo)
   EVT_MENU(MENU_IMAGE_REDO, RatioFrame::OnImageRedo)
   EVT_MENU(MENU_IMAGE_MODE_CROP, RatioFrame::OnModeCrop)
   EVT_MENU(MENU_IMAGE_MODE_INCLIN, RatioFrame::OnModeInclin)
   EVT_MENU(MENU_IMAGE_MODE_REDEYE, RatioFrame::OnModeRedeye)
   EVT_MENU(MENU_IMAGE_CROP, RatioFrame::OnImageCrop)
   EVT_MENU(MENU_IMAGE_ROTATE_LEFT, RatioFrame::OnImageRotateLeft)
   EVT_MENU(MENU_IMAGE_ROTATE_RIGHT, RatioFrame::OnImageRotateRight)
   EVT_MENU(MENU_IMAGE_INCLIN, RatioFrame::OnImageInclin)
   EVT_MENU(MENU_IMAGE_FLIP_HORIZONTAL, RatioFrame::OnImageFlipHorizontal)
   EVT_MENU(MENU_IMAGE_FLIP_VERTICAL, RatioFrame::OnImageFlipVertical)
   EVT_MENU(MENU_IMAGE_COMMENT, RatioFrame::OnImageComment)
   EVT_MENU(MENU_IMAGE_RESIZE, RatioFrame::OnImageResize)
   EVT_MENU(MENU_IMAGE_JPEG_LOSSLESS, RatioFrame::OnToggleJPEGLossless)
   EVT_MENU(MENU_IMAGE_JPEG_OPTIMIZE, RatioFrame::OnImageJPEGOptimize)
   EVT_MENU(MENU_IMAGE_JPEG_PROGRESSIVE, RatioFrame::OnImageJPEGProgressive)
   EVT_MENU(MENU_IMAGE_JPEG_DETECTQUAL, RatioFrame::OnImageJPEGDetectQuality)
   EVT_MENU(MENU_HELP_ABOUT, RatioFrame::OnHelpAbout)
   EVT_MENU(MENU_HELP_HELP, RatioFrame::OnHelpHelp)
   EVT_TOOL(MENU_RATIO_PORL, RatioFrame::OnRatioPOrL)
   EVT_TOOL(MENU_RATIO_PORTRAIT, RatioFrame::OnRatioPortrait)
   EVT_TOOL(MENU_RATIO_LANDSCAPE, RatioFrame::OnRatioLandscape)
   EVT_TOOL(TOOL_EXPLORER, RatioFrame::OnToggleExplorer)
   EVT_TOOL(TOOL_FITONPAGE, RatioFrame::OnToggleFitOnPage)
   EVT_TOOL(TOOL_ACTION, RatioFrame::OnToolAction)
   EVT_TOOL(TOOL_OPEN, RatioFrame::OnFileOpen)
   EVT_TOOL(TOOL_SAVE, RatioFrame::OnFileSave)
   EVT_TOOL(TOOL_RELOAD, RatioFrame::OnFileReload)
   EVT_TOOL(TOOL_PREV, RatioFrame::OnFilePrev)
   EVT_TOOL(TOOL_NEXT, RatioFrame::OnFileNext)
   EVT_TOOL(TOOL_UNDO, RatioFrame::OnImageUndo)
   EVT_TOOL(TOOL_CROP, RatioFrame::OnImageCrop)
   EVT_TOOL(TOOL_INCLIN, RatioFrame::OnImageInclin)
   EVT_TOOL(TOOL_ROTATE_LEFT, RatioFrame::OnImageRotateLeft)
   EVT_TOOL(TOOL_ROTATE_RIGHT, RatioFrame::OnImageRotateRight)
   EVT_TOOL(TOOL_OR_P_OR_L, RatioFrame::OnRatioPOrL)
   EVT_TOOL(TOOL_OR_PORTRAIT, RatioFrame::OnRatioPortrait)
   EVT_TOOL(TOOL_OR_LANDSCAPE, RatioFrame::OnRatioLandscape)
   EVT_COMBOBOX(WIDGET_RATIOCOMBO, RatioFrame::OnRatioChange)
   EVT_TEXT_ENTER(WIDGET_RATIOCOMBO, RatioFrame::OnRatioEnter)
   EVT_COMBOBOX(WIDGET_RESIZECOMBO, RatioFrame::OnResizeChange)
   EVT_TEXT_ENTER(WIDGET_RESIZECOMBO, RatioFrame::OnResizeEnter)
   EVT_COMBOBOX(WIDGET_GUIDECOMBO, RatioFrame::OnGuideChange)
   EVT_TEXT_ENTER(WIDGET_GUIDECOMBO, RatioFrame::OnGuideEnter)
   EVT_TOOL(TOOL_JPEGLOSSLESS, RatioFrame::OnToggleJPEGLossless)
   EVT_TOOL(TOOL_MODE_INCLIN, RatioFrame::OnModeInclin)
   EVT_TOOL(TOOL_MODE_CROP, RatioFrame::OnModeCrop)
   EVT_TOOL(TOOL_MODE_REDEYE, RatioFrame::OnModeRedeye)
   EVT_BUTTON(MENU_FILE_MOVE, RatioFrame::OnFileMove)
   EVT_BUTTON(WIDGET_APPLY_COM, RatioFrame::OnImageWriteComment)
   EVT_MENU(MENU_ACCEL_APPLY_COMMENT, RatioFrame::OnImageWriteComment)
   EVT_TEXT(WIDGET_EDIT_COM, RatioFrame::UpdateControlsStateEvt)
   EVT_CLOSE(RatioFrame::OnClose)
   EVT_SIZE(RatioFrame::OnWindowResize)
END_EVENT_TABLE()


static bool subCallJPEGTran(const wxString & command, wxString sFilename, wxString pathJpegtran, wxFrame * parent, bool noerror = false)
{
	long rc;
    const wxString extraJpegtranOptions = wxT("-copy all ");
	wxString cmd;

	cmd = pathJpegtran + wxT(" ") + extraJpegtranOptions + command + wxT(" -outfile \"") + sFilename + wxT("\" \"") + sFilename + wxT("\"");
	parent->GetStatusBar()->PushStatusText(_("Executing : ") + cmd + wxT(" ..."));
	//wxMessageBox(cmd);

	wxProcess process(parent);
	process.Redirect();
	wxBeginBusyCursor();

    {
        wxLogNull noLog;
        rc = wxExecute(cmd, wxEXEC_SYNC, &process);
    }

	wxEndBusyCursor();
	parent->GetStatusBar()->PopStatusText();
	if ((!noerror) && (process.GetErrorStream() != NULL))
	{
		process.GetErrorStream()->Peek();
		if (!(process.GetErrorStream()->Eof()))
		{
			char buffer[10240];
			process.GetErrorStream()->Read(buffer, sizeof(buffer));
			buffer[process.GetErrorStream()->LastRead()] = 0;
			wxMessageBox(_("JPEGTran has returned an error : \n\n") +wxString(buffer,wxConvLocal) + wxT("\n\n") + cmd, _("JPEGTran Error"), wxOK | wxICON_ERROR, parent);
		}
	}
	if (rc)
	{
		// Error
        // -1 : the process cannot be launched, probably because of jpegtran path
        if ((!noerror) && (rc == -1)) 
            wxMessageBox(_("Unable to launch JPEGTran : \n\nPlease check that jpegtran executable is in the path.")  + wxString(wxT("\n\n")) + wxString(cmd), 
            _("JPEGTran Error"), wxOK | wxICON_ERROR, parent);
        // Other error
	}
	return (rc)?FALSE:TRUE;
}

bool RatioFrame::CallJPEGTranTryPerfect(const wxString & command)
{
	// Try once, with perfect option, and if it fails, trim and warn.
	wxString oldFilename;
	wxString pathJpegtran = wxT("jpegtran");
	bool rc, optWarnings;

	BEGIN_IMAGE_JPEG()

	if (m_bTemp != TRUE) m_sFilename = m_sOriginalFilename;
	oldFilename = m_sFilename;

	m_bTemp = TRUE;
	m_sFilename = wxFileName::CreateTempFileName(RPHOTO_TEMP_PREFIX); // +wxT(".")+wxFileName(m_sOriginalFilename).GetExt();

	// Readonly support : wxCopyFile copie également le caractère readonly et il n'y a pas de fonctions portable pour changer les attributs, donc une copie manuelle
	if (wxFileName::IsFileWritable(oldFilename))
	{
		wxCopyFile(oldFilename,m_sFilename,TRUE);
	}
	else
	{
		// Depuis wxDoCopyFile de filefn.cpp
		wxFile fileIn;
		wxFile fileOut;
		if (!fileIn.Open(oldFilename)) return false;
		if (!fileOut.Create(m_sFilename, TRUE) )	return false;
		char buf[4096];
		for ( ;; )
		{
			ssize_t count = fileIn.Read(buf, WXSIZEOF(buf));
			if ( count == wxInvalidOffset )	return false;
			if ( !count )	break;
			if ( fileOut.Write(buf, count) < (size_t)count )	return false;
		}
		fileIn.Close();
		fileOut.Close();
	}

    m_pConfig->Read(wxT("JPEG/JPEGTran"), &pathJpegtran);

	if (!(rc = subCallJPEGTran(wxT("-perfect ") + command, m_sFilename, pathJpegtran, this, true)))
	{
		rc = subCallJPEGTran(wxT("-trim ") + command, m_sFilename, pathJpegtran, this);
		wxBell();
        wxString msg = _("WARNING : This operation is not possible lossless. Non-transformable edge blocks have been dropped.");
        optWarnings = FALSE;
        m_pConfig->Read(wxT("JPEG/DisplayLosslessWarnings"), &optWarnings);
        if (!optWarnings)
        {
		    GetStatusBar()->SetStatusText(msg);
        }
        else
        {
		    wxMessageBox(msg,_("JPEGTran warning"),wxICON_INFORMATION);
        }
	}
	if (rc)
	{
    	wxBeginBusyCursor();
		m_pImageBox->LoadFile(m_sFilename);
		m_pImageBox->TrackerReset();
    	wxEndBusyCursor();
	}
	END_IMAGE_JPEG()

	return rc;
}

bool RatioFrame::CallJHead(const wxString & command)
{
	wxString oldFilename;
	wxString pathJhead = wxT("jhead");
	wxString pathJpegtran = wxT("jpegtran");
	wxFileName pathFilename;
    const wxString extraJheadOptions = wxT("-q ");
	long rc;
	wxString cmd;


	BEGIN_IMAGE_JPEG()

	if (m_bTemp != TRUE) m_sFilename = m_sOriginalFilename;
	oldFilename = m_sFilename;

	m_bTemp = TRUE;
	m_sFilename = wxFileName::CreateTempFileName(RPHOTO_TEMP_PREFIX); // +wxT(".")+wxFileName(m_sOriginalFilename).GetExt();

	// Readonly support : wxCopyFile copie également le caractère readonly et il n'y a pas de fonctions portable pour changer les attributs, donc une copie manuelle
	if (wxFileName::IsFileWritable(oldFilename))
	{
		wxCopyFile(oldFilename,m_sFilename,TRUE);
	}
	else
	{
		// Depuis wxDoCopyFile de filefn.cpp
		wxFile fileIn;
		wxFile fileOut;
		if (!fileIn.Open(oldFilename)) return false;
		if (!fileOut.Create(m_sFilename, TRUE) )	return false;
		char buf[4096];
		for ( ;; )
		{
			ssize_t count = fileIn.Read(buf, WXSIZEOF(buf));
			if ( count == wxInvalidOffset )	return false;
			if ( !count )	break;
			if ( fileOut.Write(buf, count) < (size_t)count )	return false;
		}
		fileIn.Close();
		fileOut.Close();
	}

    m_pConfig->Read(wxT("JPEG/JHead"), &pathJhead);
    m_pConfig->Read(wxT("JPEG/JPEGTran"), &pathJpegtran);
	pathFilename.Assign(pathJpegtran);
	pathFilename.Normalize();

#ifdef _WINDOWS
	wxSetEnv(wxT("PATH"), wxString(wxGetenv(wxT("PATH"))) + wxT(";") + wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());
#endif
	//cmd = wxT("PATH=") + pathFilename.GetPath() + wxT(";%PATH%& ") + pathJhead + wxT(" ") + extraJheadOptions + command + wxT(" \"") + m_sFilename + wxT("\"");
	cmd = pathJhead + wxT(" ") + extraJheadOptions + command + wxT(" \"") + m_sFilename + wxT("\"");
	this->GetStatusBar()->PushStatusText(_("Executing : ") + cmd + wxT(" ..."));

	wxProcess process(this);
	process.Redirect();
	wxBeginBusyCursor();

    {
        wxLogNull noLog;
        rc = wxExecute(cmd, wxEXEC_SYNC, &process);
    }

	wxEndBusyCursor();
	this->GetStatusBar()->PopStatusText();
	if ((process.GetErrorStream() != NULL))
	{
		process.GetErrorStream()->Peek();
		if (!(process.GetErrorStream()->Eof()))
		{
			char buffer[10240];
			process.GetErrorStream()->Read(buffer, sizeof(buffer));
			buffer[process.GetErrorStream()->LastRead()] = 0;
			wxMessageBox(_("JHead has returned an error : \n\n") +wxString(buffer,wxConvLocal) + wxT("\n\n") + cmd, _("JHead Error"), wxOK | wxICON_ERROR, this);
		}
	}
	if (rc)
	{
		// Error
        // -1 : the process cannot be launched, probably because of jpegtran path
        if ((rc == -1)) 
            wxMessageBox(_("Unable to launch JHead : \n\nPlease check that jhead executable is in the path.")  + wxString(wxT("\n\n")) + wxString(cmd), 
            _("JHead Error"), wxOK | wxICON_ERROR, this);
        // Other error
	}
	else
	{
    	wxBeginBusyCursor();
		m_pImageBox->LoadFile(m_sFilename);
		m_pImageBox->TrackerReset();
    	wxEndBusyCursor();
	}
	END_IMAGE_JPEG()

	return (rc)?FALSE:TRUE;
}

void RatioFrame::ImageLoad(wxString name, bool original)
{
	if (m_isFileLoading) return;
	m_isFileLoading = true;
	BEGIN_IMAGE()
    wxBeginBusyCursor();
	if (ImageCleanup())
	{
		if (original) 
		{
				SetStatusText(wxString::Format(_("Loading %s..."), name.c_str()));
				if (m_bSaveCurFolder) m_pConfig->Write(wxT("Defaults/DefPath"), wxFileName(name).GetPath());
		}
		m_sFilename = name;
		if ((!m_bTemp) || (original)) { m_sOriginalFilename = m_sFilename; m_bTemp = false; }
		m_pImageBox->LoadFile(m_sFilename);
		if ((m_bAutoSelectMax) && (! m_pImageBox->IsModeInclinaison()))
		{
			wxRect tracker, tracker_max;
			// Set Tracker to max according ratio / fixed size
			m_pImageBox->GetRectTracker().SetTrackerRect(wxRect(0,0,1000000,1000000));
			m_pImageBox->GetRectTracker().Update();
			// Center Tracker
			tracker = m_pImageBox->GetRectTracker().GetTrackerRect();
			tracker_max = m_pImageBox->GetRectTracker().GetMaxRect();
			tracker.x = (tracker_max.width - tracker.width) / 2;
			tracker.y = (tracker_max.height - tracker.height) / 2;
			m_pImageBox->GetRectTracker().SetTrackerRect(tracker);
			m_pImageBox->GetRectTracker().Update();
			// Check if Fixed Size exceeds image
			if ((m_pImageBox->GetRectTracker().GetFixedHeight() > 0) && (m_pImageBox->GetRectTracker().GetFixedWidth() > 0))
			{
				if ( ( tracker.x < 0 ) || ( tracker.y < 0)  )
				{
					m_pImageBox->TrackerReset();
				}
			}
		}
		else
		{
			m_pImageBox->TrackerReset();
		}
		m_iOrientation = 1;
		imageUpdateExif();
		m_pResizeCombo->Select(0);
		m_bJPEGlossless = (isJPEGFile(name))?TRUE:FALSE;
	}
	UpdateControlsState();
	UpdateDirCtrl();
    wxEndBusyCursor();
	END_IMAGE();
	m_isFileLoading = false;

	// AutoRot
	if ((m_bAutoRot) && (m_iOrientation != 1) && (!m_isAutorotating))
	{
		m_isAutorotating = true;
		// Dirty(); // Uncomment to ask for save (else it will always been autorotating)
		CallJHead(wxT("-autorot"));
		m_isAutorotating = false;
	}
}

bool RatioFrame::ImageCleanup()
{
    wxCommandEvent evt;
	wxFileName destFn;
	if (m_bAutoCrop)
	{
		OnToolAction(evt);
	}
    if ((m_bDirty) && (m_bAutoSave))
    {
		if (m_sAutoSaveFolder.IsEmpty() && m_sAutoSaveSuffix.IsEmpty())
		{
			OnFileSave(evt);
		}
		else
		{
			destFn = m_sOriginalFilename;
			destFn.SetName(destFn.GetName() + m_sAutoSaveSuffix);
			if (!m_sAutoSaveFolder.IsEmpty())
			{
				if (wxFileName(m_sAutoSaveFolder).IsAbsolute())
				{
					destFn.SetPath(m_sAutoSaveFolder);
				}
				else
				{
					destFn.SetPath(destFn.GetPath() + destFn.GetPathSeparator() + m_sAutoSaveFolder);
					destFn.Normalize();
				}
			}
			ImageSaveAs(destFn.GetFullPath());
		}
	}
	else if (m_bDirty)
	{
		switch (wxMessageBox(_("The current image has unsaved changes. \nDo you want to save it ?"),_("Save file?"), wxYES_NO | wxCANCEL, this))
		{
		case wxYES:
			OnFileSave(evt);
			break;
		case wxNO:
			m_bDirty = false;
			break;
		case wxCANCEL:
		default:
			return false;
		}
	}
	return true;
}

void RatioFrame::imageUpdateExif()
{
	wxString favExif, favCur;
	wxStringBase64 strExif;
	wxCSConv conv(m_sComEncoding);
	unsigned char * bufExif = NULL;
	unsigned char value[1024];
	ExifData * edata = NULL;
	unsigned int ifd, ientry;
	int fav, exifLen;

	// Comment
	m_pTextComment->ChangeValue(wxT(""));  // Clear
	if (m_pImageBox->GetImage().HasOption(wxT("COM")))
	{
		strExif = m_pImageBox->GetImage().GetOption(wxT("COM"));
		favExif = strExif.DecodeBase64(conv);
		//wxLogDebug(wxT("COM='") + strExif + wxT("' : '") + favExif + wxT("'\n"));
		m_pTextComment->ChangeValue(favExif);
		m_pTextComment->DiscardEdits(); // Should not be necessary with ChangeValue instead of SetValue (2016/06/05)
	}

	m_pAttrCtrl->DeleteAllItems();
	m_pAttrFavCtrl->DeleteAllItems();
	favExif = RPHOTO_DEFAULT_EXIF_FAVORITES;
	m_pConfig->Read(wxT("JPEG/FavExif"), &favExif);
	while (favExif.Length() > 0)
	{
		favCur = favExif.BeforeFirst(',');
		if (favCur == wxT("")) favCur = favExif;
		m_pAttrFavCtrl->InsertItem(m_pAttrFavCtrl->GetItemCount(), favCur);
		favExif = favExif.Mid(favCur.Length() + 1);
	}

	if (!m_pImageBox->GetImage().HasOption(wxT("APP1"))) return;

	// Loop between all APP1 tags, and process multiples Exif if is has
	wxStringTokenizer tkz;
	tkz.SetString(m_pImageBox->GetImage().GetOption(wxT("APP1")), wxT("#"));
	while (tkz.HasMoreTokens())
	{
		strExif = tkz.GetNextToken();
		if (bufExif != NULL) free(bufExif);
		exifLen = strExif.DecodeBase64(bufExif);
		edata = exif_data_new();
		if ((bufExif) && (edata))
		{
			exif_data_load_data(edata, bufExif, exifLen);

			for (ifd = 0; ifd < EXIF_IFD_COUNT; ifd++)
			{
				if (edata->ifd[ifd] && edata->ifd[ifd]->count)
				{
					for (ientry = 0; ientry < edata->ifd[ifd]->count; ientry++)
					{
						m_pAttrCtrl->InsertItem(m_pAttrCtrl->GetItemCount(),
							wxString((const char *)exif_tag_get_name(edata->ifd[ifd]->entries[ientry]->tag), wxConvLocal));
						exif_entry_get_value(edata->ifd[ifd]->entries[ientry], (char *)value, sizeof(value));
						m_pAttrCtrl->SetItem(m_pAttrCtrl->GetItemCount() - 1, 1, wxString((const char *)value, wxConvLocal));
						// Favorites
						for (fav = 0; fav < m_pAttrFavCtrl->GetItemCount(); fav++)
							if (m_pAttrFavCtrl->GetItemText(fav) == wxString((const char *)exif_tag_get_name(edata->ifd[ifd]->entries[ientry]->tag), wxConvLocal))
								m_pAttrFavCtrl->SetItem(fav, 1, wxString((const char *)value, wxConvLocal));

						// OrientationTag (for AutoRot)
						if (edata->ifd[ifd]->entries[ientry]->tag == EXIF_TAG_ORIENTATION)
						{
							m_iOrientation = exif_get_short(edata->ifd[ifd]->entries[ientry]->data, exif_data_get_byte_order(edata));
						}
					}
				}
			}
		}
	}
	if (bufExif != NULL) { free(bufExif); bufExif = NULL; }
	if (edata != NULL) {	exif_data_free(edata); edata = NULL; }
}

void RatioFrame::UpdateDirCtrl(const wxString & from)
{
	if (from != wxT("")) m_pDirCtrl->ReCreateTree();
    wxFileName fn(m_sOriginalFilename);
    fn.MakeAbsolute();
    if (m_pDirCtrl->GetPath() != fn.GetFullPath())
    {
        m_pDirCtrl->SetPath(fn.GetFullPath());
    }
	m_pImageBox->SetFocus();
}

void RatioFrame::OnWindowResize(wxSizeEvent& event)
{
	int x, y;
	wxControl * last;
	if (isInInit) {
		event.Skip(); 
		return;
	}
	// Handle Toolbar size
	if (GetToolBar()) {
		// Get last item position
		last = GetToolBar()->GetToolByPos(GetToolBar()->GetToolsCount() - 1)->GetControl();
		last->GetPosition(&x, &y);
		x += last->GetSize().GetWidth();
		if (x > m_iToolbarWidth) m_iToolbarWidth = x;
		if (m_iToolbarWidth > event.GetSize().GetWidth())
		{
			if (GetToolBar()->FindById(TEXT_RATIOCOMBO))  GetToolBar()->DeleteTool(TEXT_RATIOCOMBO);
			if (GetToolBar()->FindById(TEXT_RESIZECOMBO))  GetToolBar()->DeleteTool(TEXT_RESIZECOMBO);
			if (GetToolBar()->FindById(TEXT_GUIDECOMBO))  GetToolBar()->DeleteTool(TEXT_GUIDECOMBO);
		}
		else 
		{
			if (!GetToolBar()->FindById(TEXT_RATIOCOMBO)) {
				m_pTextRatioCombo = new wxStaticText(GetToolBar(), TEXT_RATIOCOMBO, _("Ratio") + wxT(" : "));
				GetToolBar()->InsertControl(GetToolBar()->GetToolPos(WIDGET_RATIOCOMBO), m_pTextRatioCombo);
				GetToolBar()->Realize();
			}
			if (!GetToolBar()->FindById(TEXT_RESIZECOMBO)) {
				m_pTextResizeCombo = new wxStaticText(GetToolBar(), TEXT_RESIZECOMBO, wxT("   ") + _("Resize") + wxT(" : "));
				GetToolBar()->InsertControl(GetToolBar()->GetToolPos(WIDGET_RESIZECOMBO), m_pTextResizeCombo);
				GetToolBar()->Realize();
			}
			if (!GetToolBar()->FindById(TEXT_GUIDECOMBO)) {
				m_pTextGuideCombo = new wxStaticText(GetToolBar(), TEXT_GUIDECOMBO, ("   ") + _("Guide Lines") + wxT(" : "));
				GetToolBar()->InsertControl(GetToolBar()->GetToolPos(WIDGET_GUIDECOMBO), m_pTextGuideCombo);
				GetToolBar()->Realize();
			}
		}
	}
	event.Skip();
}


void RatioFrame::OnClose(wxCloseEvent& event)
{
	if ( event.CanVeto() )
	{
		if (!ImageCleanup())
		{
			event.Veto();
			return;
		}
	}
	event.Skip();  //  the default event handler does call Destroy()
}

void RatioFrame::OnMenuFileQuit(wxCommandEvent &event)
{
  Close(FALSE);
}

void RatioFrame::OnHelpAbout(wxCommandEvent &event)
{
    wxMessageBox(wxString::Format(_("%s %s\nBuild: %s\n\n(c) %s\n%s"), RPHOTO_NAME, RPHOTO_VERSION, RPHOTO_BUILD, RPHOTO_COPYRIGHT, RPHOTO_URL), RPHOTO_NAME, wxOK |wxICON_INFORMATION);
}

void RatioFrame::OnHelpHelp(wxCommandEvent &event)
{
	m_help.Display(_("help_en.html"));
}

void RatioFrame::OnDirCtrlChange(wxCommandEvent &event)
{
	if (isInUpdate) return;
	wxString curFilename;
	if (m_pDirCtrl->GetFilePath() != wxT(""))
	{
		curFilename = m_pDirCtrl->GetFilePath();
		if ((!m_isFileLoading) && (curFilename != m_sOriginalFilename))
		{
			if (!ImageCleanup())
			{
				event.Skip();
				UpdateDirCtrl();
			}
			else
			{
				ImageLoad(curFilename, true);
			}
		}
	}
	else
	{
		event.Skip();
	}	
	UpdateControlsState();
}

void RatioFrame::OnDirMoveChange(wxCommandEvent &event)
{
	if (isInUpdate) return;
	UpdateControlsState();
	event.Skip();
}

void RatioFrame::OnToggleExplorer(wxCommandEvent &event)
{
	/* TODO
	bool newState = (m_pDirCtrl->IsShown())?FALSE:TRUE;
	GetMenuBar()->Check(MENU_VIEW_EXPLORER, newState);
	GetToolBar()->ToggleTool(TOOL_EXPLORER, newState);
	GetSizer()->Show(m_pDirCtrl,newState);
	Layout();
	*/
}

void RatioFrame::OnToggleFitOnPage(wxCommandEvent &event)
{
	if (isInUpdate) return;
	bool newState = (m_pImageBox->GetScale()!=1)?FALSE:TRUE;
	GetMenuBar()->Check(MENU_VIEW_FITONPAGE, newState);
	GetToolBar()->ToggleTool(TOOL_FITONPAGE, newState);
	m_pImageBox->SetScale((newState)?scFIT_TO_PAGE_IF_LARGER:1);
	// if FitOnPage, helps to suppress scrollbars...
	if (newState)
	{
		m_pImageBox->SetScrollRate(0,0);
		m_pImageBox->SetScrollRate(1,1);
	}
}


void RatioFrame::OnToggleJPEGLossless(wxCommandEvent &event)
{
	if (isInUpdate) return;
	m_bJPEGlossless = (m_bJPEGlossless)?FALSE:TRUE;
	UpdateControlsState();
}

void RatioFrame::OnFileOpen(wxCommandEvent &event)
{
	wxString name;
	name = wxFileSelector(_("Open a file"),wxT(""),wxT(""),wxT(""), RPHOTO_FILTERIMGLIST_WORLD, wxFD_OPEN | wxFD_FILE_MUST_EXIST /* wxOPEN | wxFILE_MUST_EXIST */);
    if (name != wxT(""))
    {
        if (ImageCleanup())
		{
			ImageLoad(name);
			m_sFilename = m_sOriginalFilename = name;
			m_bTemp = false;

		}
	}
}

void RatioFrame::OnFileSave(wxCommandEvent &event)
{
	if (m_sFilename == wxT("")) return; 
	if (m_pTextComment->IsModified())
	{
		if (m_bAutoSave)
		{
			SetStatusText(_("Applying modification of the comment."));
			OnImageWriteComment(event);
		}
		else
		{
			if (wxMessageBox(_("You have modified the comment but not saved it. Would you like to save this comment ?"), _("Comment modification"), wxYES_NO | wxICON_QUESTION, this) == wxYES) 
				OnImageWriteComment(event);
		}
	}
	if (!wxFileName::IsFileWritable(m_sOriginalFilename)) { OnFileSaveAs(event); return; }
    if (m_bJPEGlossless)
    {
		if (m_sFilename != m_sOriginalFilename) wxCopyFile(m_sFilename,m_sOriginalFilename,TRUE);
        Clean();
    }
    else if ((m_pImageBox != NULL))
    {
		if (m_bTemp) { m_sFilename = m_sOriginalFilename; m_bTemp = false; }
        if (isJPEGFile(m_sFilename))
        {
            wxString squality = wxT("");   m_pConfig->Read(wxT("JPEG/Quality"),&squality);
            long quality = 0;          squality.ToLong(&quality);
            m_pImageBox->GetImage().SetOption(wxT("quality"), (int)quality);
            m_pImageBox->GetImage().SaveFile(m_sFilename, wxBITMAP_TYPE_JPEG);
        }
        else  m_pImageBox->GetImage().SaveFile(m_sFilename);
        Clean();
        if (!m_isFileLoading) ImageLoad(m_sFilename);
    }
} 

void RatioFrame::ImageSaveAs(wxString name)
{
	if (name == wxT("")) return;
	if (m_bTemp) { m_sFilename = m_sOriginalFilename; m_bTemp = false; }
    if ((!m_bDirty) && (m_bJPEGlossless) && isJPEGFile(m_sOriginalFilename) && isJPEGFile(name))
    {
       	wxCopyFile(m_sFilename, name, TRUE);
        Clean();
    }
    else if (m_pImageBox != NULL)
    {
        if (isJPEGFile(m_sFilename))
        {
            wxString squality = wxT("");   m_pConfig->Read(wxT("JPEG/Quality"),&squality);
            long quality = 0;          squality.ToLong(&quality);
            m_pImageBox->GetImage().SetOption(wxT("quality"), (int)quality);
            m_pImageBox->GetImage().SaveFile(name, wxBITMAP_TYPE_JPEG);
        }
        else  m_pImageBox->GetImage().SaveFile(name);
        Clean();
    }

    ImageCleanup();
    m_sFilename = m_sOriginalFilename = name;
}

void RatioFrame::OnFileSaveAs(wxCommandEvent &event)
{
    wxString name;
	name = wxFileSelector(_("Save a file"),wxT(""),m_sOriginalFilename,wxT(""), RPHOTO_FILTERIMGLIST_WORLD,wxFD_SAVE /* wxSAVE*/);
    if (name != wxT(""))
    {
		ImageSaveAs(name);
        m_pDirCtrl->ReCreateTree();
        ImageLoad(m_sFilename);
    }
}


void RatioFrame::OnFileDelete(wxCommandEvent &event)
{
    wxString path, name;
    path = wxT("");
    name = m_pDirCtrl->GetFilePath();
    if (wxMessageBox(_("Are you sure to delete the file ") + name + wxT(" ?"), _("Delete confirmation"), wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        // Find another file
        OnFileNext(event);
        if (m_pDirCtrl->GetFilePath() == name) OnFilePrev(event);
		isInUpdate = true;
        if (m_pDirCtrl->GetFilePath() == name) path = m_pDirCtrl->GetPath();
        if (wxFileExists(name)) wxRemoveFile(name);
		Clean();
        m_pDirCtrl->ReCreateTree();
        if (path != wxT(""))  m_pDirCtrl->SetPath(path); else UpdateDirCtrl();
		isInUpdate = false;
    }
}

void RatioFrame::OnFileMove(wxCommandEvent &event)
{
	rpMove * move;

	if (m_pMoveCtrl->GetPath() == wxT(""))
	{
		SetStatusText(_("No destination path selected."));
		return;
	}
	else
	{
		SetStatusText(wxString::Format(_("Move to %s"),m_pMoveCtrl->GetPath().c_str()));
	}

	move = new rpMove(
			m_sOriginalFilename,
			wxFileName(m_pMoveCtrl->GetPath(), wxFileName(m_sOriginalFilename).GetFullName()).GetFullPath()
		);
	// Do the actual move
	move->save(*this);
	// Undo
	m_HistoryUndo.Append(move);
	// Do not limit the number of undo items, as it does not take much space...
	UpdateControlsState();
}

void RatioFrame::OnFileMoveAndNext(wxCommandEvent &event)
{
	wxFileName fn;
	fn = wxFileName(m_pDirCtrl->GetPath());
	fn.SetFullName(m_pDirCtrl->GetTreeCtrl()->GetItemText(m_pDirCtrl->GetTreeCtrl()->GetNextSibling(m_pDirCtrl->GetTreeCtrl()->GetSelection())));
	OnFileMove(event);
	m_pDirCtrl->SetPath(fn.GetFullPath());
}


void RatioFrame::OnFileSavePath(wxCommandEvent &event)
{
	wxString fn;
	if (m_pMoveCtrl->GetPath() == wxT(""))
	{
		SetStatusText(_("No destination path selected."));
		return;
	}
	else
	{
		fn = wxFileName(m_pMoveCtrl->GetPath(), wxFileName(m_sOriginalFilename).GetFullName()).GetFullPath();
		SetStatusText(wxString::Format(_("Save to %s"),fn.c_str()));
		ImageSaveAs(fn);
	}

	UpdateControlsState();
}

void RatioFrame::OnFileSavePathAndNext(wxCommandEvent &event)
{
	// TODO
	wxFileName fn;
	fn = wxFileName(m_pDirCtrl->GetPath());
	fn.SetFullName(m_pDirCtrl->GetTreeCtrl()->GetItemText(m_pDirCtrl->GetTreeCtrl()->GetNextSibling(m_pDirCtrl->GetTreeCtrl()->GetSelection())));
	OnFileSavePath(event);
	m_pDirCtrl->SetPath(fn.GetFullPath());
}

void RatioFrame::OnFileReload(wxCommandEvent &event)
{
    // Prevent file to be auto-saved
    m_bDirty = FALSE;
	ImageLoad(m_sOriginalFilename);
    m_sFilename = m_sOriginalFilename;
	m_bTemp = false;
}

void RatioFrame::OnFilePrev(wxCommandEvent &event)
{
    wxTreeCtrl * tree;
    tree = m_pDirCtrl->GetTreeCtrl();
    if (tree->GetPrevSibling(tree->GetSelection()).IsOk())
	{
		tree->SelectItem(tree->GetPrevSibling(tree->GetSelection()));
	}
    if ((m_pDirCtrl->GetFilePath() == wxT("")) && (tree->GetNextSibling(tree->GetSelection()).IsOk()))
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
    if ((m_pDirCtrl->GetFilePath() == wxT("")) && (tree->GetPrevSibling(tree->GetSelection()).IsOk()))
    {
        tree->SelectItem(tree->GetPrevSibling(tree->GetSelection()));
    }
}


void RatioFrame::OnImageUndo(wxCommandEvent &event)
{
	rpUndo * undo;
	if (m_HistoryUndo.GetCount() <= 0) return;

	wxBeginBusyCursor();
	// Move current into a redo one
	undo = new rpImage();
	undo->save(*this);
	m_HistoryRedo.Append(undo);

	// Retrieve and restore the current undoed
	undo = m_HistoryUndo.GetLast()->GetData();
	undo->restore(*this);
	
	// If action, put action in redo list and restore next
	if (undo->isAction())
	{
		m_HistoryUndo.DeleteContents(false);
		m_HistoryRedo.Append(undo);
		m_HistoryUndo.DeleteObject(undo);
		m_HistoryUndo.DeleteContents(true);
		//undo = m_HistoryUndo.GetLast()->GetData();
		//undo->restore(*this);
	}
	else
	{
		// Delete previous history from the undo list
		m_HistoryUndo.DeleteObject(undo);
	}
	UpdateControlsState();
	wxEndBusyCursor();
}

void RatioFrame::OnImageRedo(wxCommandEvent &event)
{
	rpUndo * undo;
	if (m_HistoryRedo.GetCount() <= 0) return;

	wxBeginBusyCursor();
	// Move current into a undo one
	undo = new rpImage();
	undo->save(*this);
	m_HistoryUndo.Append(undo);

	// Retrieve the current undoed
	undo = m_HistoryRedo.GetLast()->GetData();
	// Restore data
	undo->restore(*this);

	// If action, put action in redo list and restore next
	if (undo->isAction())
	{
		m_HistoryRedo.DeleteContents(false);
		m_HistoryUndo.Append(undo);
		m_HistoryRedo.DeleteObject(undo);
		m_HistoryRedo.DeleteContents(true);
		//undo = m_HistoryRedo.GetLast()->GetData();
		//undo->restore(*this);
	}
	else
	{
		// Delete the object from the redo list
		m_HistoryRedo.DeleteObject(undo);
	}
	UpdateControlsState();
	wxEndBusyCursor();
}


void RatioFrame::OnToolAction(wxCommandEvent &event)
{
	if (m_pImageBox->IsModeInclinaison())
	{
		OnImageInclin(event);
	}
	else
	{
		OnImageCrop(event);
	}
}

void RatioFrame::OnModeInclin(wxCommandEvent &event)
{
	if (isInUpdate) return;
	if (m_pImageBox->IsModeInclinaison())
	{
		// Default action in this mode
		OnImageInclin(event);
	}
	else
	{
		m_pImageBox->SetModeInclinaison(true);
		if (m_bJPEGlossless) OnToggleJPEGLossless(event);
	}
	UpdateControlsState();
}

void RatioFrame::OnModeCrop(wxCommandEvent &event)
{
	if (isInUpdate) return;
	if (!m_pImageBox->IsModeInclinaison())
	{
		// Default action in this mode
		OnImageCrop(event);
	}
	else
	{
		m_pImageBox->SetModeInclinaison(false);
	}
	UpdateControlsState();
}

void RatioFrame::OnModeRedeye(wxCommandEvent &event)
{
	RPHOTO_NOT_IMPLEMENTED;
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
	        wxString cropCmd = wxString::Format(wxT("-crop %dx%d+%d+%d"), 
		        cropZone.width, cropZone.height, cropZone.x, cropZone.y);
	        CallJPEGTranTryPerfect(cropCmd);
        }
        else
        {
			BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
            m_pImageBox->SetImage(m_pImageBox->GetImage().GetSubImage(cropZone));
            m_pImageBox->TrackerReset();
			END_IMAGE_MANIP(m_pImageBox->GetImage());
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
	    CallJPEGTranTryPerfect(wxString(wxT("-rotate 270")));
    }
    else
    {
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
        m_pImageBox->SetImage(m_pImageBox->GetImage().Rotate90(FALSE));
        m_pImageBox->TrackerReset();
		END_IMAGE_MANIP(m_pImageBox->GetImage());
    }
}

void RatioFrame::OnImageRotateRight(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString(wxT("-rotate 90")));
    }
    else
    {
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
        m_pImageBox->SetImage(m_pImageBox->GetImage().Rotate90(TRUE));
        m_pImageBox->TrackerReset();
		END_IMAGE_MANIP(m_pImageBox->GetImage());
    }
}


void RatioFrame::OnImageInclin(wxCommandEvent &event)
{
	int dx, dy, sx, sy, nw, nh,  cw, ch;
	double cw1, ch1, cw2, ch2;
	double angle, degree, rat;
	bool bAutoCrop;
	#define PI 3.141592653589793

	if (m_pImageBox == NULL) return;

	dx = m_pImageBox->GetLineTracker().GetPosEnd().x - m_pImageBox->GetLineTracker().GetPosBegin().x;
	dy = m_pImageBox->GetLineTracker().GetPosEnd().y - m_pImageBox->GetLineTracker().GetPosBegin().y;
	sx = m_pImageBox->GetImage().GetWidth();
	sy = m_pImageBox->GetImage().GetHeight();

	if ((!m_pImageBox->GetLineTracker().IsEnabled()) || (dx == 0) || (dy == 0)|| (dx == -1) || (dy == -1))
	{
		SetStatusText(_("No angle defined."));
		return;
	}

	else angle = atan((double) dy / dx);
	if (angle > (PI / 4.0)) angle -= PI / 2.0;
	if (angle < (-PI / 4.0)) angle += PI / 2.0;
	degree = angle / PI * 180.0;
	if (angle != 0.0) 
	{
		wxBeginBusyCursor();
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
		// Rotate
		m_pImageBox->GetLineTracker().SetTrackerPosition(wxPoint(0,0),wxPoint(0,0));
       	SetStatusText(wxString::Format(_("Rotating of %f degree. Please wait..."),degree),0);            
		m_pImageBox->SetImage(m_pImageBox->GetImage().Rotate(angle, wxPoint(m_pImageBox->GetImage().GetWidth()/2,m_pImageBox->GetImage().GetHeight()/2)));
		m_pImageBox->Update();
		// Crop
		nw = m_pImageBox->GetImage().GetWidth();
		nh = m_pImageBox->GetImage().GetHeight();
		// TODO :
		rat = ((m_pImageBox->GetRatio() > 0)?(double)m_pImageBox->GetRatio():(double)sx/sy);
		if (rat == 0) rat = 1;		// To avoid problems
		if (rat < 1) rat = 1 / rat;	// Normalize before playing with ratio
		switch(m_pImageBox->GetOrientation())
		{
		case -1: // Force Portrait 
			rat = 1 / rat;
			break;
		case  1: // Force Landscape
			break;
		default: // Optimal
			if (sy > sx) rat = 1 / rat; // Optimal ratio is always the ratio from source
			break;
		}
		// By Height
		ch1 = ((double) sy / (rat * sin(fabs(angle)) + cos(fabs(angle))));
		cw1 = (ch1 * rat);
		// By Width
		ch2 = ((double) sx / (sin(fabs(angle)) + rat * cos(fabs(angle))));
		cw2 = (ch2 * rat);
		// Choose between : 
		// * Invalidate if wrong
		if ( ((cw1 * cos(fabs(angle))) + (ch1 * sin(fabs(angle)))) > sx) { cw1 = 0; ch1 = 0;  } 
		if ( ((cw2 * sin(fabs(angle))) + (ch2 * cos(fabs(angle)))) > sy) { cw2 = 0; ch2 = 0;  } 
		// * Take most important in surface
		if ((ch1 * cw1) >= (ch2 * cw2)) { ch = (int)(ch1 - 0.5); cw = (int)(cw1 - 0.5); } else { ch = (int)(ch2 - 0.5); cw = (int)(cw2 - 0.5); }
		m_pImageBox->SetModeInclinaison(false);
		m_pImageBox->GetRectTracker().SetUnscrolledRect(wxRect(
				(int)((double) (m_pImageBox->GetImage().GetWidth() - cw ) / 2.0 * m_pImageBox->GetScaleValue() + 0.5),
				(int)((double) (m_pImageBox->GetImage().GetHeight() - ch ) / 2.0 * m_pImageBox->GetScaleValue() + 0.5),
				(int)((double) cw * m_pImageBox->GetScaleValue() - 0.5),
				(int)((double) ch * m_pImageBox->GetScaleValue() - 0.5)
			));
		m_pImageBox->GetRectTracker().Update();
       	SetStatusText(_("Done."),0);            
		END_IMAGE_MANIP(m_pImageBox->GetImage());
		wxEndBusyCursor();
        Dirty();
		bAutoCrop = FALSE; m_pConfig->Read(wxT("Main/AutoRotateCrop"), &bAutoCrop);
		if (bAutoCrop)
		{
			OnImageCrop(event);
		}
	}
}

void RatioFrame::OnImageFlipHorizontal(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString(wxT("-flip horizontal")));
    }
    else
    {
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
        m_pImageBox->SetImage(m_pImageBox->GetImage().Mirror(TRUE));
		END_IMAGE_MANIP(m_pImageBox->GetImage());
    }
}

void RatioFrame::OnImageFlipVertical(wxCommandEvent &event)
{
	if (m_bJPEGlossless)
    {
        CallJPEGTranTryPerfect(wxString(wxT("-flip vertical")));
    }
    else
    {
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
        m_pImageBox->SetImage(m_pImageBox->GetImage().Mirror(FALSE));
		END_IMAGE_MANIP(m_pImageBox->GetImage());
    }
}

void RatioFrame::OnImageJPEGOptimize(wxCommandEvent &event)
{
    Dirty();
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString(wxT("-optimize")));
    }
}

void RatioFrame::OnImageJPEGProgressive(wxCommandEvent &event)
{
    if (m_bJPEGlossless)
    {
    	CallJPEGTranTryPerfect(wxString(wxT("-progressive")));
    }
}

void RatioFrame::OnImageJPEGDetectQuality(wxCommandEvent &event)
{
    wxString fname;
    wxImage & image = m_pImageBox->GetImage();
    fname = wxFileName::CreateTempFileName(wxT("rphoto_test"));
    int quality = 80, prevQual = 0, minQual = 0, maxQual = 100;
    long sizeTarget, sizeCur;

    wxBeginBusyCursor();
    sizeTarget = wxFile(m_sFilename).Length();
    while (abs(quality - prevQual) > 1)
    {
        prevQual = quality;
        image.SetOption(wxT("quality"), quality);
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
    if (wxFileExists(fname)) wxRemoveFile(fname);
    wxEndBusyCursor();
    m_pConfig->Write(wxT("JPEG/Quality"), wxString::Format(wxT("%d"),quality));
    GetStatusBar()->SetStatusText(wxString::Format(_("Detected the quality %d."), quality));

}

void RatioFrame::OnImageResize(wxCommandEvent & event)
{
	wxString resizestr;
	// resizestr = ::wxGetSingleChoice(_("Please input the size you want.\n\nExample of valid formats are below : \n'50%' to resize at 50 percent\n'640x480' to resize at width = 640 and height = 480\n'42kb' to resize at a size that should be near 42kb when saving the file\n\n"), _("Resize Image"), m_saResize);
	resizestr = ::wxGetTextFromUser(
			wxString::Format(_("Please input the size you want.\n\nExample of valid formats are below : \n'50%s' to resize at 50 percent\n'640x480' to resize at width = 640 and height = 480 (0 = apply ratio)\n'42kb' to resize at a size that should be near 42kb when saving the file\n\n"),wxT("%")), 
			_("Resize Image"), 
			wxString::Format(wxT("%ldx%ld"),m_pImageBox->GetImage().GetHeight(),m_pImageBox->GetImage().GetWidth()) 
	);
	if (resizestr != wxT(""))
	{
		event.SetString(resizestr);
		OnResizeEnter(event);
	}
}

void RatioFrame::OnImageComment(wxCommandEvent & event)
{
	m_pNbTools->SetSelection(2);
	m_pTextComment->SetFocus();
}

// Include modified wxjpgcom source code to avoid binary wrjpgcom unstable dependance
#include "wrjpgcom.inc"

void RatioFrame::OnImageWriteComment(wxCommandEvent & event)
{
	wxStringBase64 str64;
	wxString str, oldFilename;
    wxCSConv conv(m_sComEncoding);
	int /*i,*/ rc;

	if (!m_pTextComment->IsModified()) return;

	Dirty();
	if (m_bJPEGlossless)
	{
		BEGIN_IMAGE_JPEG()
   		wxBeginBusyCursor();
		if (m_bTemp != TRUE) m_sFilename = m_sOriginalFilename;
		oldFilename = m_sFilename;
		m_bTemp = TRUE;
		m_sFilename = wxFileName::CreateTempFileName(RPHOTO_TEMP_PREFIX); //+wxT(".")+wxFileName(m_sOriginalFilename).GetExt();
		str = m_pTextComment->GetValue();
		rc = wrjpgcom((const char *) oldFilename.mb_str(wxConvFile), (const char *) m_sFilename.mb_str(wxConvFile), str.mb_str(conv), 0);
		if (rc == 0) { m_pImageBox->LoadFile(m_sFilename);  }
		else { GetStatusBar()->SetStatusText(wxString::Format(_("Error writing comment : error %d."), rc)); wxCopyFile(oldFilename,m_sFilename,true);  }
   		wxEndBusyCursor();
		END_IMAGE_JPEG()
	}
	else
	{
		BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
		str64 = wxStringBase64::EncodeBase64(m_pTextComment->GetValue(), conv);
		m_pImageBox->GetImage().SetOption(wxT("COM"), str64);
		END_IMAGE_MANIP(m_pImageBox->GetImage());
	}
	m_pTextComment->DiscardEdits();
	m_pImageBox->SetFocus();
	UpdateControlsState();
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
	long lNum = 0, lDenom = 0;
	double ratio = -1;

	// Fixed or Ratio ?
	if (str.Find(':') != -1)
	{
	    // Ratio
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
			if ((ratio != m_pImageBox->GetRatio()) || (m_pImageBox->GetFixedWidth() != -1))
    		{	
    			m_pImageBox->SetRatio(ratio);
    			// wxMessageBox(wxString::Format("Fraction : %f", ratio));
    		}
    		return TRUE;
    	}
    }
    else
    {
        // Fixed
	    str.BeforeFirst('x').ToLong(&lNum);
	    str.AfterFirst('x').BeforeFirst(' ').ToLong(&lDenom);
	    
    	if ((lNum > 0) && (lDenom > 0))
    	{
   			m_pImageBox->SetFixedSize(lNum, lDenom);
    		return TRUE;
    	}
    	else
    	{
    		// Erreur
    		wxMessageBox(wxString::Format(_("Invalid fixed size : %ld x %ld\nThis should have the following format : '111x222 description'"), lNum, lDenom));
    		return FALSE;
    	}
    }    

}

bool RatioFrame::Resize(const wxString &str)
{
	long lNum = 0, lDenom = 0;
	double ratio;
	long destWidth, destHeight, curWidth, curHeight, swap;
	int porl;

	destWidth  = curWidth = m_pImageBox->GetImage().GetWidth();
	destHeight = curHeight = m_pImageBox->GetImage().GetHeight();
	ratio = m_pImageBox->GetRatio();
	porl = m_pImageBox->GetOrientation();

	if (ratio == 0) ratio = destWidth / destHeight;

	if (str.Find('%') != -1)
	{
		// Percent
	    if (str.BeforeFirst('%').ToLong(&lNum))
    	{
			if (lNum > 0)
			{
				destWidth = (destWidth * lNum) / 100;
				destHeight = (destHeight * lNum) / 100;
			}
			else
			{
    			wxMessageBox(wxString::Format(_("Invalid percent : %ld\nThis should have the following format : 'x% description'"), lNum));
    			return FALSE;
			}
    	}
	}
	else if (str.Find('x') != -1)
	{
		// Fixed
	    str.BeforeFirst('x').ToLong(&lNum);
	    str.AfterFirst('x').BeforeFirst(' ').ToLong(&lDenom);
	    
    	if ((lNum > 0) && (lDenom > 0))
    	{
			destWidth = lNum;
			destHeight = lDenom;
    	}
		else if ((lNum > 0) && (ratio > 0))
		{
			destWidth = lNum;
			destHeight = lNum / ratio;
		}
		else if ((lDenom > 0) && (ratio > 0))
		{
			destWidth = lDenom * ratio;
			destHeight = lDenom;
		}
		else
    	{
    		// Erreur
    		wxMessageBox(wxString::Format(_("Invalid fixed size : %ld x %ld\nThis should have the following format : '111x222 description'"), lNum, lDenom));
    		return FALSE;
    	}

		// Adjust Portait Or Landscape
		if (
			// Automatic : swap if not in the same direction
			( (porl == 0) && ( ((curWidth > curHeight) && (destWidth < destHeight)) || ((curWidth < curHeight) && (destWidth > destHeight)) ) ) ||
			// Landscape : swap if portrait
			( (porl > 0) && (destWidth < destHeight) ) ||
			// Portrait : swap if landscape
			( (porl < 0) && (destWidth > destHeight) )
		   )
		{
			// Swap
			swap = destWidth;
			destWidth = destHeight;
			destHeight = swap;
		}
	}
	else if (str.Find(_("kb")) != -1)
	{
		// Size
		str.BeforeFirst('k').ToLong(&lNum);

		wxString fname;
		wxImage & image = m_pImageBox->GetImage(), work;
		fname = wxFileName::CreateTempFileName(wxT("rphoto_test"));
		long sizeTarget, sizeCur;
		double ratio = 1, minratio = 0, maxratio = 1, oldratio = 0;
		bool minok, maxok;

	    sizeCur = wxFile(m_sFilename).Length();
		sizeTarget = lNum * 1024;
		if (sizeCur > sizeTarget)
		{
			wxBeginBusyCursor();
			GetStatusBar()->SetStatusText(wxString::Format(_("Detecting optimal size to obtain a size below %d bytes."), sizeTarget));

			// Check boudary max
			maxok = true;
			work = image.Copy();
			work.SaveFile(fname, wxBITMAP_TYPE_JPEG);
			sizeCur = wxFile(fname).Length();
			if (sizeCur < sizeTarget) { maxok = false; GetStatusBar()->SetStatusText(wxString::Format(_("Current size is already below %d bytes !"), sizeTarget)); }
			// Check boudary min
			minok = true;
			work = image.Copy();
			work.Rescale(1, 1 /*, wxIMAGE_QUALITY_HIGH */);
			work.SaveFile(fname, wxBITMAP_TYPE_JPEG);
			sizeCur = wxFile(fname).Length();
			if (sizeCur > sizeTarget) { minok = false; GetStatusBar()->SetStatusText(wxString::Format(_("Empty image size is above %d bytes !"), sizeTarget)); }

			// If ok, continue
			if ((maxok) && (minok))
			{
				oldratio = 1; ratio = 0.5;
				// Target to 2%, but the error will be superior, as the real rescale will be done with high quality + avoid ratio loops
				while ( ((abs(sizeCur - sizeTarget) > (sizeTarget * 2 / 100)) ) && ( abs(oldratio-ratio) > 0.001) )
				{
					work = image.Copy();
//					work.Rescale(destWidth * ratio, destHeight * ratio, wxIMAGE_QUALITY_HIGH);
					work.Rescale(destWidth * ratio, destHeight * ratio /*, wxIMAGE_QUALITY_HIGH */);
					work.SaveFile(fname, wxBITMAP_TYPE_JPEG);
					sizeCur = wxFile(fname).Length();
					oldratio = ratio;
					if (sizeCur > sizeTarget) { maxratio = ratio; ratio = (ratio + minratio) / 2; }
					if (sizeCur < sizeTarget) { minratio = ratio; ratio = (ratio + maxratio) / 2; }
				}
				// Adjust to wxIMAGE_QUALITY_HIGH (experimental...)  (if rescale is done without)
				ratio = ratio * 0.90;
				GetStatusBar()->SetStatusText(wxString::Format(_("Detected the size ok !"), ratio));
			}
			if (wxFileExists(fname)) wxRemoveFile(fname);
			wxEndBusyCursor();
		}
		destWidth = destWidth * ratio;
		destHeight = destHeight * ratio;
	}
	else 
	{
   		wxMessageBox(wxString::Format(_("Invalid size : please see provided examples")));
   		return FALSE;
	}

	// Now we can resize
	wxBeginBusyCursor();
	BEGIN_IMAGE_MANIP(m_pImageBox->GetImage());
    Dirty();
	if (m_bJPEGlossless) { wxCommandEvent event; OnToggleJPEGLossless(event); }
	m_pImageBox->GetLineTracker().SetTrackerPosition(wxPoint(0,0),wxPoint(0,0));
	m_pImageBox->TrackerReset();
   	SetStatusText(wxString::Format(_("Resizing to %ld x %ld. Please wait..."), destWidth, destHeight),0);            
	m_pImageBox->SetImage(m_pImageBox->GetImage().Scale(destWidth, destHeight, wxIMAGE_QUALITY_HIGH));
	m_pImageBox->Update();
   	SetStatusText(_("Done."),0);            
	END_IMAGE_MANIP(m_pImageBox->GetImage());
	wxEndBusyCursor();

	return TRUE;
}

bool RatioFrame::CalcGuideRatio(const wxString &str)
{
	bool ok;
	double ratio = -1;
	wxString wstr;
	wstr = str.BeforeFirst(' ');
	ok = wstr.ToDouble(&ratio);
#if wxCHECK_VERSION(2, 9, 0)
	if ((!ok) || (ratio > 100))  ok = wstr.ToCDouble(&ratio);
#endif
	if (ok)
	{
		m_pImageBox->SetGuideRatio(ratio);
		SetStatusText(wxString::Format(_("Guide lines ratio set to %f (\"%s\")\n."),ratio,str.c_str()));
	}
	else
	{
		if (!str.IsEmpty())
		{
			wxMessageBox(wxString::Format(_("Invalid numeric format : %s.\nThis should have the following format : 'x.y description' (the numeric separator depends on your local parameters)"), str.c_str()));
		}
	}
	return (ratio > 0);
}  

void RatioFrame::OnRatioChange(wxCommandEvent &event)
{
	int sel;
	if (event.GetId() == MENU_RATIO_R_NONE)		m_pRatioCombo->SetSelection(0);
	if (event.GetId() == MENU_RATIO_R_NUMERIC)	m_pRatioCombo->SetSelection(1);
	if (event.GetId() == MENU_RATIO_R_CLASSIC)	m_pRatioCombo->SetSelection(2);
	if (event.GetId() == MENU_RATIO_R_TV)		m_pRatioCombo->SetSelection(3);
	if (event.GetId() == MENU_RATIO_R_SQUARE)	m_pRatioCombo->SetSelection(4);
	if (event.GetId() == WIDGET_RATIOCOMBO)
			 sel = event.GetSelection();
		else sel = m_pRatioCombo->GetSelection();
	if (sel <= 4) { GetMenuBar()->Check(MENU_RATIO_R_NONE + m_pRatioCombo->GetSelection(), TRUE); GetMenuBar()->Enable(MENU_RATIO_R_CUSTOM, FALSE);}
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

void RatioFrame::OnResizeChange(wxCommandEvent &event)
{
	int sel;
	if (event.GetId() == WIDGET_RATIOCOMBO)
			 sel = event.GetSelection();
		else sel = m_pResizeCombo->GetSelection();
	/*
	if (sel <= 4) { GetMenuBar()->Check(MENU_RATIO_R_NONE + m_pRatioCombo->GetSelection(), TRUE); GetMenuBar()->Enable(MENU_RATIO_R_CUSTOM, FALSE);}
			else  { GetMenuBar()->Enable(MENU_RATIO_R_CUSTOM, TRUE); GetMenuBar()->Check(MENU_RATIO_R_CUSTOM, TRUE);}
	*/
	if (sel > 0) 
	{
		Resize(m_pResizeCombo->GetStringSelection());
		m_pResizeCombo->SetSelection(sel);
	}
}

void RatioFrame::OnResizeEnter(wxCommandEvent &event)
{
	if (Resize(event.GetString()))
	{
		m_pResizeCombo->Append(event.GetString());
		wxCommandEvent evt;
		OnResizeChange(evt); 
	}
	//m_pResizeCombo->SetSelection(m_pResizeCombo->GetCount()-1);
	m_pResizeCombo->SetSelection(0);
}

void RatioFrame::OnGuideChange(wxCommandEvent &event)
{
	int sel;
	if (event.GetId() == WIDGET_RATIOCOMBO)
			 sel = event.GetSelection();
		else sel = m_pGuideCombo->GetSelection();
	if (sel >= 0) 
	{
		CalcGuideRatio(m_pGuideCombo->GetStringSelection());
		m_pGuideCombo->SetSelection(sel);
	}
}

void RatioFrame::OnGuideEnter(wxCommandEvent &event)
{
	if (CalcGuideRatio(event.GetString()))
	{
		m_pGuideCombo->Append(event.GetString());
		wxCommandEvent evt;
		OnGuideChange(evt); 
	}
	m_pGuideCombo->SetSelection(m_pResizeCombo->GetCount()-1);
}

void RatioFrame::OnPreferences(wxCommandEvent &event)
{
    // Run the dialog
    m_pConfigDialog->loadFromConfig();
	m_pConfigDialog->doLayout();
    m_pConfigDialog->ShowModal();
    DoConfig();
	imageUpdateExif();
	UpdateControlsState();
}

bool RatioFrame::isJPEGFile(const wxString & name)
{
    wxFileName fn(name);
    wxString ext = fn.GetExt();
    ext.MakeLower();
    if (ext == wxT("jpeg")) return TRUE;
    if (ext == wxT("jpg")) return TRUE;
    return FALSE;
}

void RatioFrame::Dirty(bool isJPEGLossless)
{
    // Why did I do that ???
    // if ((!isJPEGLossless) || (!m_bJPEGlossless) )   m_bDirty = TRUE;
    m_bDirty = TRUE;
	UpdateControlsState();
}

void RatioFrame::Clean()
{
    m_bDirty = FALSE;
	UpdateControlsState();
}

void RatioFrame::imageUndoPoint()
{
	wxString sMaxUndo;
	long iMaxUndo;
	rpImage * undo;

	// Quick Hack to avoid undo the empty file of the beginning..
	if (m_sOriginalFilename == wxT("")) return;
	// If we have undoed, free the undoed ones
	m_HistoryRedo.Clear();

	undo = new rpImage();
	undo->save(*this);
	m_HistoryUndo.Append(undo);

	// Undo History
    m_pConfig->Read(wxT("Main/UndoHistory"), &sMaxUndo, wxT("10"));
	sMaxUndo.ToLong(&iMaxUndo);

	// Delete old ones
	while ((int)m_HistoryUndo.GetCount() > iMaxUndo) m_HistoryUndo.DeleteNode(m_HistoryUndo.GetFirst());
}


void RatioFrame::OnFileNameBeginEdit(wxTreeEvent &event)
{
	wxTreeItemId curId;
	wxString curPath;
	RatioFrame * fixThis;
    // Don't rename a parent of the current selection
	fixThis = this;
#if !wxCHECK_VERSION(2, 6, 0)
	fixThis = ((RatioFrame *)wxGetApp().GetTopWindow());
#endif
	fixThis->m_isFileLoading = true;
	curPath = fixThis->m_pDirCtrl->GetPath();
	fixThis->m_pDirCtrl->SetPath(fixThis->m_sOriginalFilename);
	curId = fixThis->m_pDirCtrl->GetTreeCtrl()->GetItemParent(fixThis->m_pDirCtrl->GetTreeCtrl()->GetSelection());
	fixThis->m_pDirCtrl->SetPath(curPath);
	fixThis->m_isFileLoading = false;
	while ((curId.IsOk()) && (curId != event.GetItem())) curId = fixThis->m_pDirCtrl->GetTreeCtrl()->GetItemParent(curId);
    if (curId.IsOk()) { event.Veto();  return; }
}

void RatioFrame::OnFileNameEndEdit(wxTreeEvent &event)
{
	wxFileName newName;
	rpMove * move;
	wxTreeItemId curId;
	wxString curPath;
	RatioFrame * fixThis;
	// If not the current selection, let the basic behaviour of the component
	m_isFileLoading = true;
	fixThis = this;
#if !wxCHECK_VERSION(2, 6, 0)
	fixThis = ((RatioFrame *)wxGetApp().GetTopWindow());
#endif
	curPath = fixThis->m_pDirCtrl->GetPath();
	fixThis->m_pDirCtrl->SetPath(fixThis->m_sOriginalFilename);
	curId = fixThis->m_pDirCtrl->GetTreeCtrl()->GetSelection();
	fixThis->m_pDirCtrl->SetPath(curPath);
	fixThis->m_isFileLoading = false;
	if (event.GetItem() != curId) { event.Skip(); return; }

	// If not, create rpMove undo operation
    if ((event.GetLabel().IsEmpty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
        (event.GetLabel().First( wxT("/") ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

	newName.Assign(fixThis->m_pDirCtrl->GetPath());
    newName.SetFullName(event.GetLabel());

    if (wxFileExists(newName.GetFullPath()))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }

	move = new rpMove(fixThis->m_sOriginalFilename,	newName.GetFullPath());
	// Do the actual move
	move->save(*fixThis);
	// Undo
	fixThis->m_HistoryUndo.Append(move);
	// Do not limit the number of undo items, as it does not take much space...
	fixThis->UpdateControlsState();
}
