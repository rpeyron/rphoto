/* ****************************************************************************
 * RatioFrame.h                                                               *
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


#ifndef __RATIOFRAME_H__
#define __RATIOFRAME_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "RatioFrame.h"
#endif

#include "RPhoto.h"
#include "RatioImageBox.h"
#include <wx/frame.h>
#include <wxRectTracker/RectTracker.h>
#include <wxmisc/ConfigDialog.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/dnd.h>
#include <wx/html/helpctrl.h>

// Should be useful only with wx2.4
#if !wxCHECK_VERSION(2, 6, 0)
	#define USE_FIXED_DIRCTRL 1
#endif

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
#define MENU_FILE_MOVE      10005
#define MENU_FILE_MOVENEXT  10006
#define MENU_FILE_RELOAD	10013
#define MENU_FILE_PREV		10011
#define MENU_FILE_NEXT		10012
#define MENU_FILE_PREFS		10090
#define MENU_FILE_QUIT		10099
#define MENU_IMAGE_UNDO		10130
#define MENU_IMAGE_REDO		10131
#define MENU_IMAGE_CROP		10101
#define MENU_IMAGE_ROTATE_LEFT		10102
#define MENU_IMAGE_ROTATE_RIGHT		10103
#define MENU_IMAGE_FLIP_HORIZONTAL	10104
#define MENU_IMAGE_FLIP_VERTICAL	10105
#define MENU_IMAGE_INCLIN			10106
#define MENU_IMAGE_COMMENT			10107
#define MENU_IMAGE_RESIZE			10108
#define MENU_IMAGE_JPEG         	10110
#define MENU_IMAGE_JPEG_OPTIMIZE	10111
#define MENU_IMAGE_JPEG_PROGRESSIVE	10112
#define MENU_IMAGE_JPEG_LOSSLESS	10113
#define MENU_IMAGE_JPEG_DETECTQUAL	10114
#define MENU_IMAGE_MODE_CROP		10121
#define MENU_IMAGE_MODE_INCLIN		10122
#define MENU_IMAGE_MODE_REDEYE		10123
#define MENU_RATIO_PORL				10201
#define MENU_RATIO_LANDSCAPE		10202
#define MENU_RATIO_PORTRAIT			10203
#define MENU_RATIO_R_NONE			10211
#define MENU_RATIO_R_NUMERIC		10212
#define MENU_RATIO_R_CLASSIC		10213
#define MENU_RATIO_R_TV				10214
#define MENU_RATIO_R_SQUARE			10215
#define MENU_RATIO_R_CUSTOM			10299
#define MENU_VIEW_EXPLORER			10301
#define MENU_VIEW_FITONPAGE			10311
#define MENU_HELP_ABOUT				10901
#define MENU_HELP_HELP				10902
#define MENU_ACCEL_APPLY_COMMENT	11001
// Tools Ids
#define TOOL_ACTION			20000
#define TOOL_OPEN			20001
#define TOOL_SAVE			20002
#define TOOL_PREV			20003
#define TOOL_NEXT			20004
#define TOOL_RELOAD			20005
#define TOOL_EXPLORER		20011
#define TOOL_FITONPAGE		20012
#define TOOL_COLORS			20013
#define TOOL_TEXTS			20014
#define TOOL_CROP			20021
#define TOOL_ROTATE_LEFT	20022
#define TOOL_ROTATE_RIGHT	20023
#define TOOL_UNDO			20024
#define TOOL_INCLIN			20025
#define TOOL_OR_P_OR_L		20031
#define TOOL_OR_PORTRAIT	20032
#define TOOL_OR_LANDSCAPE	20033
#define TOOL_JPEGLOSSLESS	20041
#define TOOL_MODE_CROP		20051
#define TOOL_MODE_INCLIN	20052
#define TOOL_MODE_REDEYE	20053
// WIDGETS Ids
#define WIDGET_RATIOCOMBO	30001
#define WIDGET_RESIZECOMBO	30002
#define WIDGET_APPLY_COM	30011
#define WIDGET_EDIT_COM		30012


class rpUndo;

WX_DECLARE_LIST(rpUndo, ListOfUndo);

class RatioFrame : public wxFrame
{
public:
	RatioFrame(wxWindow * parent, 
			wxWindowID id, 
			const wxString& title, 
			const wxPoint& pos = wxDefaultPosition, 
			const wxSize& size = wxDefaultSize, 
			long style = wxDEFAULT_FRAME_STYLE, 
			const wxString& name = wxT("frame"));
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
	void OnDirMoveChange(wxCommandEvent &event);
    void OnHelpAbout(wxCommandEvent &event);
    void OnHelpHelp(wxCommandEvent &event);
	// - Tools
	void OnToggleExplorer(wxCommandEvent &event);
	void OnToggleFitOnPage(wxCommandEvent &event);
	void OnToggleJPEGLossless(wxCommandEvent &event);
	void OnFileOpen(wxCommandEvent &event);
	void OnFileSave(wxCommandEvent &event);
	void OnFileSaveAs(wxCommandEvent &event);
	void OnFileDelete(wxCommandEvent &event);
	void OnFileMove(wxCommandEvent &event);
	void OnFileMoveAndNext(wxCommandEvent &event);
	void OnFileReload(wxCommandEvent &event);
	void OnFilePrev(wxCommandEvent &event);
	void OnFileNext(wxCommandEvent &event);
	void OnFileNameBeginEdit(wxTreeEvent & event);
	void OnFileNameEndEdit(wxTreeEvent & event);
	void OnToolAction(wxCommandEvent &event);
	void OnModeCrop(wxCommandEvent &event);
	void OnModeInclin(wxCommandEvent &event);
	void OnModeRedeye(wxCommandEvent &event);
	void OnImageUndo(wxCommandEvent &event);
	void OnImageRedo(wxCommandEvent &event);
	void OnImageCrop(wxCommandEvent &event);
	void OnImageRotateLeft(wxCommandEvent &event);
	void OnImageRotateRight(wxCommandEvent &event);
	void OnImageInclin(wxCommandEvent &event);
	void OnImageFlipHorizontal(wxCommandEvent &event);
	void OnImageFlipVertical(wxCommandEvent &event);
	void OnImageJPEGOptimize(wxCommandEvent &event);
	void OnImageJPEGProgressive(wxCommandEvent &event);
	void OnImageJPEGDetectQuality(wxCommandEvent &event);
	void OnImageWriteComment(wxCommandEvent & event);
	void OnImageComment(wxCommandEvent & event);
	void OnImageResize(wxCommandEvent & event);
	void OnRatioPOrL(wxCommandEvent &event);
	void OnRatioPortrait(wxCommandEvent &event);
	void OnRatioLandscape(wxCommandEvent &event);
	void OnRatioChange(wxCommandEvent &event);
	void OnRatioEnter(wxCommandEvent &event);
	void OnResizeChange(wxCommandEvent &event);
	void OnResizeEnter(wxCommandEvent &event);
	// - Misc
	void UpdateControlsState();
	void UpdateControlsStateEvt(wxCommandEvent & event);
	// - Dclaration de la table
    DECLARE_EVENT_TABLE()

	// Gestion des images
	void ImageLoad(wxString name, bool original = false);
	void ImageCleanup();
	void imageUndoPoint();
	void imageUpdateExif();
	bool CallJPEGTranTryPerfect(const wxString &command);

	// Fonctions Diverses
	bool CalcRatio(const wxString &str);
	bool Resize(const wxString &str);
    void UpdateDirCtrl(const wxString &from = wxT(""));

public:
	void LoadFile(const wxString & filename) { ImageCleanup(); ImageLoad(filename, true); }

public: // Hack : TODO : Move Dirty/Clean in a document class
    bool isJPEGFile(const wxString & name);
    void Dirty(bool isJPEGLossless = TRUE);
    void Clean();

protected:
	wxHtmlHelpController m_help;
	ListOfUndo m_HistoryUndo;
	ListOfUndo m_HistoryRedo;
	wxSplitterWindow * m_pSplitHoriz;
	wxSplitterWindow * m_pSplitVert;
	wxNotebook * m_pNbFiles;
	wxNotebook * m_pNbTools;
	wxRatioImageBox * m_pImageBox;
	wxTextCtrl * m_pTextCtrl;
	wxMenuBar * m_pMenuBar;
	wxDirCtrl * m_pDirCtrl;
	wxDirCtrl * m_pMoveCtrl;
	wxListCtrl * m_pAttrCtrl;
	wxListCtrl * m_pAttrFavCtrl;
	wxComboBox * m_pRatioCombo;
	wxComboBox * m_pResizeCombo;
	wxTextCtrl * m_pTextComment;
	wxString m_sFilename;
	wxString m_sOriginalFilename;
	bool m_bTemp;
    bool m_bJPEGlossless;
	wxString m_sComEncoding;
    bool m_bAutoSave;
    bool m_bDirty;
	wxArrayString m_saRatios;
	wxArrayString m_saResize;
    wxLocale m_locale;
    wxConfig * m_pConfig;
    wxConfigDialog * m_pConfigDialog;
	bool m_isFileLoading;

   	DECLARE_CLASS(RatioFrame)

	friend class rpUndo;
	friend class rpImage;
	friend class rpMove;
};

class rpUndo
{
public:
	rpUndo() {};
	virtual ~rpUndo() {};
	virtual bool isAction() { return false; };
	virtual void save(RatioFrame & frame) = 0;
	virtual void restore(RatioFrame & frame) = 0;
};

class rpImage : public rpUndo
{
protected:
	bool bKeepFiles;
	// Direct attributes
	wxString sOriginalFilename;
	wxString sFilename;
	bool bTemp;
	bool bDirty;
	bool bJPEGlossless;
	// Indirect attributes
	wxImage image;
	wxRect rectTracker;
	wxRect lineTracker;
	bool isInclinaison;
	double scale;

public:
	rpImage() {};

	virtual ~rpImage()
	{
		if (!this->bKeepFiles && this->bTemp) wxRemoveFile(this->sFilename);
	};

	virtual bool isAction() { return false; };

	void save(RatioFrame & frame)
	{
		bKeepFiles = false;
		// Direct
		sOriginalFilename = frame.m_sOriginalFilename;
		sFilename = frame.m_sFilename;
		bTemp = frame.m_bTemp;
		bDirty = frame.m_bDirty;
		bJPEGlossless = frame.m_bJPEGlossless;
		// Indirect
		if (!bJPEGlossless) image = frame.m_pImageBox->GetImage();
		rectTracker = frame.m_pImageBox->GetRectTracker().GetUnscrolledRect();
		lineTracker = frame.m_pImageBox->GetLineTracker().GetUnscrolledRect();
		isInclinaison = frame.m_pImageBox->IsModeInclinaison();
		scale = frame.m_pImageBox->GetScale();
	};

	void restore(RatioFrame & frame)
	{
		wxString oldOriginalFilename;

		oldOriginalFilename = frame.m_sOriginalFilename;
		bKeepFiles = true;
		// Direct
		frame.m_sOriginalFilename = sOriginalFilename;
		frame.m_sFilename = sFilename;
		frame.m_bTemp = bTemp;
		frame.m_bDirty = bDirty;
		frame.m_bJPEGlossless = bJPEGlossless;
		// Indirect
		if (bJPEGlossless) image = wxImage(sFilename);
		frame.m_pImageBox->SetImage(image);
		frame.m_pImageBox->GetRectTracker().SetUnscrolledRect(rectTracker);
		frame.m_pImageBox->GetLineTracker().SetUnscrolledRect(lineTracker);
		frame.m_pImageBox->SetModeInclinaison(isInclinaison);
		frame.m_pImageBox->SetScale(scale);
		// Update
		frame.UpdateControlsState();
		frame.imageUpdateExif();
		if (oldOriginalFilename != frame.m_sOriginalFilename) frame.UpdateDirCtrl();
	};

};


class rpMove : public rpUndo
{
protected:
	wxString m_sSourceFilename;
	wxString m_sDestFilename;
public:
	rpMove(wxString src, wxString dest) : m_sSourceFilename(src), m_sDestFilename(dest) {};
	virtual ~rpMove() {};
	virtual bool isAction() { return true; };
	virtual void save(RatioFrame & frame)	
	{ 
		frame.m_isFileLoading = true; // Avoid harmfull reload of image
		wxRenameFile(m_sSourceFilename, m_sDestFilename); 
		frame.m_sOriginalFilename = m_sDestFilename;
		if (frame.m_sFilename == m_sSourceFilename) frame.m_sFilename = m_sDestFilename;
		frame.UpdateDirCtrl(m_sSourceFilename);
		frame.m_isFileLoading = false;
	};
	virtual void restore(RatioFrame & frame) 
	{ 
		frame.m_isFileLoading = true; // Avoid harmfull reload of image
		wxRenameFile(m_sDestFilename, m_sSourceFilename); 
		frame.m_sOriginalFilename = m_sSourceFilename;
		if (frame.m_sFilename == m_sDestFilename) frame.m_sFilename = m_sSourceFilename;
		frame.UpdateDirCtrl(m_sDestFilename);
		frame.m_isFileLoading = false;
	};
};

class rpFrameDropTarget : public wxFileDropTarget
{
public:
	rpFrameDropTarget(RatioFrame * owner) : wxFileDropTarget() {m_pOwner = owner;}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
	{
		unsigned int i;
		for(i = 0; i < filenames.Count(); i++)
		{
			m_pOwner->LoadFile(filenames[i]);
		}
		return true;
	}

protected:
	RatioFrame * m_pOwner;
};

#endif // __RATIOFRAME_H__
