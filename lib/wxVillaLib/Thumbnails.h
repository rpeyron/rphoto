/////////////////////////////////////////////////////////////////////////////
// Name:        Thumbnails.h
// Purpose:     wxThumbnails class
// Author:      Alex Thuering
// Created:		2.02.2003
// RCS-ID:      $Id: Thumbnails.h 366 2008-09-06 18:22:16Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_THUMBNAILS_H
#define WXVILLALIB_THUMBNAILS_H

#include <wx/wx.h>
#include <wx/image.h>

const int POINTED_BRIGHTNESS = 24;

class wxThumbnails;

class wxThumb
{
  public:
	wxThumb(wxImage image, wxString caption = wxEmptyString)
	{ m_image = image; SetCaption(caption); m_id = 0; }
	wxThumb(wxString filename, wxString caption = wxEmptyString)
	{ m_filename = filename; SetCaption(caption); m_id = 0; }
	virtual ~wxThumb() {}
	
	wxString GetCaption() { return m_caption; }
	void SetCaption(wxString caption);
	
	wxImage GetImage() { return m_image; }
	void SetImage(wxImage image) { m_image = image; }
	
	wxString GetFilename() { return m_filename; }
	void SetFilename(wxString fname);
	
	int GetId() { return m_id; }
	void SetId(int id) { m_id = id; }
	
	wxBitmap GetBitmap() { return m_bitmap; }
	void SetBitmap(wxBitmap bitmap) { m_bitmap = bitmap; }
	void Update() { m_bitmap = wxBitmap(); } // clear buffer
	
	virtual wxBitmap GetBitmap(wxThumbnails* parent, int width, int height);
	wxString GetCaption(unsigned int line);
	int GetCaptionLinesCount(int width);
	
  protected:
	wxImage m_image; // create thumb image from this image
	wxString m_filename; // load thumb image from this file
	int m_id;
	wxBitmap m_bitmap; // buffer for thumbnail
	wxString m_caption;
	wxArrayInt m_captionBreaks;
	void BreakCaption(int width);
};

WX_DEFINE_ARRAY(wxThumb*, wxThumbArray);

inline int cmpthumb(wxThumb** first, wxThumb** second)
{
  if ((*first)->GetFilename() < (*second)->GetFilename())
	return -1;
  else if ((*first)->GetFilename() == (*second)->GetFilename())
	return (*first)->GetId() - (*second)->GetId();
  return 1;
}

enum wxThumbOutline
{
  wxTHUMB_OUTLINE_NONE,
  wxTHUMB_OUTLINE_FULL,
  wxTHUMB_OUTLINE_RECT,
  wxTHUMB_OUTLINE_IMAGE
};

enum wxThumbFilter
{
  wxTHUMB_FILTER_IMAGES = 1,
  wxTHUMB_FILTER_VIDEOS = 2,
  wxTHUMB_FILTER_AUDIOS = 4,
  wxTHUMB_FILTER_CUSTOM = 8,
};

/** wxThumbnails style flags */
enum
{
  wxTHUMB_HORIZONTAL = wxHORIZONTAL,
  wxTHUMB_VERTICAL = wxVERTICAL
};

/** The widget to display a series of images (thumbnails).
  * This class use pathched jpeg handler (imagjog.h).
  * To activate this handler use following the code
  * (after wxInitAllImageHandlers):
  * @code
  *   if (!wxImage::RemoveHandler(_T("JPEG file")))
  *     wxMessageBox(_T("Can't remove old JPEG handler"));
  *   wxImage::InsertHandler(new wxJPGHandler);
  * @endcode
  */
class wxThumbnails: public wxScrolledWindow
{
  public:
    wxThumbnails(wxWindow* parent, int id);
    virtual ~wxThumbnails();
	
	/** Removes all thumbnails (items). */
    void Clear();
	/** Shows a thumbnils of images from the given directory. */
    void ShowDir(wxString dir, int filter = -1);
	static bool IsAudioVideo(const wxString& fname);
	static bool IsVideo(const wxString& fname);
	static bool IsAudio(const wxString& fname);
    bool IsCustomFile(const wxString& fname) { return m_customExtentions.Index(fname.AfterLast(wxT('.')).Lower()) >= 0; }
    void AddCustomExtention(const wxString& ext) { m_customExtentions.Add(ext.Lower()); }
    void ClearCustomExtentions() { m_customExtentions.Clear(); }
    /** sets filter */
    void SetFilter(int filter) { m_filter = filter; }
    
	/** Return the index of the selected thumbnail. */
	inline int GetSelected(int selIndex = -1) { return selIndex == -1 ? m_selected : m_selectedArray[selIndex]; }
	inline wxThumb* GetSelectedItem(int selIndex = -1) { return GetItem(GetSelected(selIndex)); }
	inline int GetSelectedCount() { return m_selectedArray.Count(); }
	inline bool IsSelected(int index) { return m_selectedArray.Index(index) != -1; }
	void SetSelected(int value);
	inline void AddToSelection(int value) { m_selectedArray.Add(value); }
	/** Return the index of the pointed thumbnail. */
	inline int GetPointed() { return m_pointed; }
	inline wxThumb* GetPointedItem() { return GetItem(m_pointed); }
	
	inline wxThumb* GetItem(int index)
    { return index>=0 && index<(int)m_items.GetCount()? m_items[index]:NULL; }
	inline int GetItemCount() { return m_items.GetCount(); }
	void InsertItem(wxThumb* thumb, int pos = -1);
	void RemoveItemAt(int pos, int count = 1);
	inline void SortItems() { m_items.Sort(cmpthumb); }
    
	/** Rereads directory. */
	virtual void UpdateItems();
    
	void SetThumbSize(int width, int height, int border=8);
	void GetThumbSize(int& width, int& height, int& border);
	inline int GetThumbWidth() { return m_tWidth; }
	inline int GetThumbHeight() { return m_tHeight; }
	inline int GetThumbBorder() { return m_tBorder; }
	
	virtual void SetCaption(wxString caption);
	wxString GetCaption() { return m_caption; }
	void SetLabelControl(wxStaticText* value) { m_labelControl = value; }
	
	void ShowFileNames(bool value = true) { m_showFileNames = value; }
	
	void SetOrientaion(int value = wxTHUMB_VERTICAL) { m_orient = value; }
	
	/** Sets the item pop up menu. */
	void SetPopupMenu(wxMenu* menu) { m_pmenu = menu; }
	/** Returns the pop up menu if it was set, or null otherwise. */
	wxMenu* GetPopupMenu() { return m_pmenu; }
	
	/** Sets the global pop up menu (if no item is selected). */
	void SetGlobalPopupMenu(wxMenu* menu) { m_gpmenu = menu; }
	/** Returns the pop up menu if it was set, or null otherwise. */
	wxMenu* GetGloalPopupMenu() { return m_gpmenu; }
	
	inline void EnableDragging(bool value = true) { m_allowDragging = value; }
	
  protected:
    /** Parameters of thumbnail */
    int m_tWidth;
    int m_tHeight;
    int m_tBorder;
	int m_tCaptionBorder;
    wxArrayInt m_tCaptionHeight;
	int m_tTextHeight;
	wxThumbOutline m_tOutline;
	bool m_tOutlineNotSelected;
	
	int m_selected;
    wxArrayInt m_selectedArray;
    int m_pointed;
	
    wxString m_dir;
	int m_filter;
	wxString m_caption;
		  
    wxThumbArray m_items;
    int m_cols;
    int m_rows;
	
    wxStaticText* m_labelControl;
	wxMenu* m_pmenu;
	wxMenu* m_gpmenu;
	bool m_allowDragging;
	bool m_mouseEventHandled;
	bool m_showFileNames;
	int m_orient;
    
    /** list of custom extentions */
    wxArrayString m_customExtentions;
    
	virtual int GetCaptionHeight(int begRow, int count = 1);
	virtual int GetItemIndex(int x, int y);
    virtual void UpdateProp(bool checkSize = true);
    virtual void ScrollToSelected();
    virtual void DrawThumbnail(wxBitmap& bmp, wxThumb& thumb, int index);
    virtual void OnPaint(wxPaintEvent& WXUNUSED(event));
    virtual void OnResize(wxSizeEvent &event);
    virtual void OnMouseDown(wxMouseEvent &event);
	virtual void OnMouseUp(wxMouseEvent &event);
	virtual void OnMouseDClick(wxMouseEvent &event);
    virtual void OnMouseMove(wxMouseEvent &event);
    virtual void OnMouseLeave(wxMouseEvent &event);
	virtual void OnThumbChanged(wxCommandEvent &event);
	virtual void OnKeyUp(wxKeyEvent& event);
    virtual void OnEraseBackground(wxEraseEvent &event) {}
    wxRect GetPaintRect();
    /* Rereads directory but don't update selection (use UpdateItems()) */
	virtual void UpdateItemsInt();
    
  private:
    DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_SEL_CHANGED, 3200)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_POINTED, 3201)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_DCLICK, 3202)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_CAPTION_CHANGED, 3203)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_THUMB_CHANGED, 3204)
END_DECLARE_EVENT_TYPES()

#define EVT_THUMBNAILS_SEL_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_THUMBNAILS_SEL_CHANGED, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#define EVT_THUMBNAILS_POINTED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_THUMBNAILS_POINTED, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#define EVT_THUMBNAILS_DCLICK(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_THUMBNAILS_DCLICK, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#define EVT_THUMBNAILS_CAPTION_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_THUMBNAILS_CAPTION_CHANGED,id,wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#define EVT_THUMBNAILS_THUMB_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_THUMBNAILS_THUMB_CHANGED,id,wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#endif // WXVILLALIB_THUMBNAILS_H
