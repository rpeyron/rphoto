/* ****************************************************************************
 * RatioFrame.h                                                               *
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


#ifndef __RATIOFRAME_H__
#define __RATIOFRAME_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "RatioFrame.h"
#endif

#include "RPhoto.h"
#include <wx/frame.h>
#include "RectTracker.h"
#include "RatioImageBox.h"
#include "ConfigDialog.h"

#define USE_FIXED_DIRCTRL 1
#ifdef USE_FIXED_DIRCTRL
#include "../lib/wxFix/dirctrlg.h"
#define wxDirCtrl wxFixedDirCtrl
#else
#include "wx/dirctrl.h"
#define wxDirCtrl wxGenericDirCtrl
#endif

// Menu Ids
#define MENU_FILE_OPEN		10001
#define MENU_FILE_SAVE		10002
#define MENU_FILE_SAVEAS    10003
#define MENU_FILE_DELETE    10004
#define MENU_FILE_RELOAD	10013
#define MENU_FILE_PREV		10011
#define MENU_FILE_NEXT		10012
#define MENU_FILE_PREFS		10090
#define MENU_FILE_QUIT		10099
#define MENU_IMAGE_CROP		10101
#define MENU_IMAGE_ROTATE_LEFT		10102
#define MENU_IMAGE_ROTATE_RIGHT		10103
#define MENU_IMAGE_FLIP_HORIZONTAL	10104
#define MENU_IMAGE_FLIP_VERTICAL	10105
#define MENU_IMAGE_JPEG         	10110
#define MENU_IMAGE_JPEG_OPTIMIZE	10111
#define MENU_IMAGE_JPEG_PROGRESSIVE	10112
#define MENU_IMAGE_JPEG_LOSSLESS	10113
#define MENU_IMAGE_JPEG_DETECTQUAL	10114
#define MENU_RATIO_PORL				10201
#define MENU_RATIO_LANDSCAPE		10202
#define MENU_RATIO_PORTRAIT			10203
#define MENU_RATIO_R_NONE			10211
#define MENU_RATIO_R_NUMERIC		10212
#define MENU_RATIO_R_CLASSIC		10213
#define MENU_RATIO_R_SQUARE			10214
#define MENU_RATIO_R_CUSTOM			10219
#define MENU_VIEW_EXPLORER	10301
#define MENU_VIEW_FITONPAGE 10311
#define MENU_HELP_ABOUT		10901
// Tools Ids
#define TOOL_OPEN			20001
#define TOOL_SAVE			20002
#define TOOL_RELOAD			20005
#define TOOL_PREV			20003
#define TOOL_NEXT			20004
#define TOOL_EXPLORER		20011
#define TOOL_FITONPAGE		20012
#define TOOL_CROP			20021
#define TOOL_ROTATE_LEFT	20022
#define TOOL_ROTATE_RIGHT	20023
#define TOOL_OR_P_OR_L		20031
#define TOOL_OR_PORTRAIT	20032
#define TOOL_OR_LANDSCAPE	20033
#define TOOL_JPEGLOSSLESS	20041
// WIDGETS Ids
#define WIDGET_RATIOCOMBO	30001


class RatioFrame : public wxFrame
{
public:
	RatioFrame(wxWindow * parent, 
			wxWindowID id, 
			const wxString& title, 
			const wxPoint& pos = wxDefaultPosition, 
			const wxSize& size = wxDefaultSize, 
			long style = wxDEFAULT_FRAME_STYLE, 
			const wxString& name = "frame");
	~RatioFrame();

protected:
    // Initialisation stuff
    void InitConfig();
    void InitMenu();
    void InitToolBar();
    void InitAccelerator();
    void InitStatusBar();
    void InitControls();
    void DoConfig();
    // Events
    // - Menu
    void OnMenuFileQuit(wxCommandEvent &event);
    void OnPreferences(wxCommandEvent &event);
	void OnDirCtrlChange(wxCommandEvent &event);
    void OnHelpAbout(wxCommandEvent &event);
	// - Tools
	void OnToggleExplorer(wxCommandEvent &event);
	void OnToggleFitOnPage(wxCommandEvent &event);
	void OnToggleJPEGLossless(wxCommandEvent &event);
	void OnFileOpen(wxCommandEvent &event);
	void OnFileSave(wxCommandEvent &event);
	void OnFileSaveAs(wxCommandEvent &event);
	void OnFileDelete(wxCommandEvent &event);
	void OnFileReload(wxCommandEvent &event);
	void OnFilePrev(wxCommandEvent &event);
	void OnFileNext(wxCommandEvent &event);
	void OnImageCrop(wxCommandEvent &event);
	void OnImageRotateLeft(wxCommandEvent &event);
	void OnImageRotateRight(wxCommandEvent &event);
	void OnImageFlipHorizontal(wxCommandEvent &event);
	void OnImageFlipVertical(wxCommandEvent &event);
	void OnImageJPEGOptimize(wxCommandEvent &event);
	void OnImageJPEGProgressive(wxCommandEvent &event);
	void OnImageJPEGDetectQuality(wxCommandEvent &event);
	void OnRatioPOrL(wxCommandEvent &event);
	void OnRatioPortrait(wxCommandEvent &event);
	void OnRatioLandscape(wxCommandEvent &event);
	void OnRatioChange(wxCommandEvent &event);
	void OnRatioEnter(wxCommandEvent &event);
	// - Misc
	// - Déclaration de la table
    DECLARE_EVENT_TABLE()

	// Gestion des images
	void ImageLoad(wxString name, bool temp = false);
	void ImageCleanup();
	bool CallJPEGTran(const wxString &command, bool noerror = false);
	bool CallJPEGTranTryPerfect(const wxString &command);

	// Fonctions Diverses
	bool CalcRatio(const wxString &str);
    void UpdateDirCtrl();

protected:
    bool isJPEGFile(const wxString & name);
    void Dirty(bool isJPEGLossless = TRUE);
    void Clean();

protected:
	wxRatioImageBox * m_pImageBox;
	wxTextCtrl * m_pTextCtrl;
	wxMenuBar * m_pMenuBar;
	wxDirCtrl * m_pDirCtrl;
	wxComboBox * m_pRatioCombo;
	wxString m_sFilename;
	wxString m_sOriginalFilename;
	bool m_bTemp;
    bool m_bJPEGlossless;
    bool m_bDirty;
    wxLocale m_locale;
    wxConfig * m_pConfig;
    wxConfigDialog * m_pConfigDialog;

   	DECLARE_CLASS(RatioFrame)
};


#endif // __RATIOFRAME_H__
