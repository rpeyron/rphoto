/////////////////////////////////////////////////////////////////////////////
// Name:        Thumbnails.h
// Purpose:     wxThumbnails class
// Author:      Alex Thuering
// Created:		2.02.2003
// RCS-ID:      $Id: Thumbnails.h,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_THUMBNAILS_H
#define WXVILLALIB_THUMBNAILS_H

#include <wx/wx.h>
#include <wx/image.h>

class wxThumb
{
  public:
   wxThumb(wxString filename, int id = 0)
   { this->filename = filename; this->id = id; vImage = NULL;}
   
   wxString filename;
   int id;
   wxBitmap image;
   wxImage vImage; // use this image instead of from the file if it's not empty
};

WX_DECLARE_OBJARRAY(wxThumb, wxThumbArray);

inline int cmpthumb(wxThumb** first, wxThumb** second)
{
  if ((*first)->filename < (*second)->filename)
	return -1;
  else if ((*first)->filename == (*second)->filename)
	return (*first)->id - (*second)->id;
  return 1;
}

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
    virtual ~wxThumbnails() {}
	
	/** Removes all thumbnails (items). */
    void Clear();
	/** Shows a thumbnils of images from the given directory. */
    void ShowDir(wxString dir);
    
	/** Return the index of the selected thumbnail. */
	inline int GetSelected() { return m_selected; }
	inline void SetSelected(int value) { m_selected = value; Refresh(); }
	inline wxThumb* GetSelectedItem() { return GetItem(m_selected); }
	/** Return the index of the pointed thumbnail. */
	inline int GetPointed() { return m_pointed; }
	inline wxThumb* GetPointedItem() { return GetItem(m_pointed); }
	
	inline wxThumb* GetItem(int index)
    { return index>=0 && index<(int)m_items.GetCount()? &m_items[index]:NULL; }
	inline int GetItemCount() { return m_items.GetCount(); }
	void InsertItem(wxThumb& thumb, int pos = -1);
	inline void SortItems() { m_items.Sort(cmpthumb); }
	/** Rereads directory. */
	void UpdateItems();
    
	void SetThumbSize(int width, int height, int border=4, int infoBorder=0);
	void GetThumbSize(int& width, int& height, int& border, int& infoBorder);
	inline int GetThumbWidth() { return m_tWidth; }
	inline int GetThumbHeight() { return m_tHeight; }
	inline int GetThumbBorder() { return m_tBorder; }
	inline int GetThumbInfoBorder() { return m_tInfoBorder; }
	
	virtual void SetCaption(wxString caption);
	wxString GetCaption() { return m_caption; }
	void SetLabelControl(wxStaticText* value) { m_labelControl = value; }
	
	/** Sets the pop up menu. */
	void SetPopupMenu(wxMenu* menu) { m_pmenu = menu; }
	/** Returns the pop up menu if it was set, or null otherwise. */
	wxMenu* GetPopupMenu() { return m_pmenu; }
	
	inline void EnableDragging(bool value = true) { m_dragging = value; }
    
  protected:
    int m_tWidth;
    int m_tHeight;
    int m_tBorder;
    int m_tInfoBorder;
    int m_selected;
    int m_pointed;
    wxString m_dir;
	wxString m_caption;		  
    wxThumbArray m_items;
    int m_cols;
    int m_rows;
    wxStaticText* m_labelControl;
	wxMenu* m_pmenu;
	bool m_dragging;
	bool m_leftDown;
    void UpdateProp();
    void ScrollToSelected();
    virtual void DrawThumbnail(wxBitmap& bmp, wxThumb& thumb, bool selected,
      bool pointed);
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    void OnResize(wxSizeEvent &event);
    void OnMouseDown(wxMouseEvent &event);
	void OnMouseUp(wxMouseEvent &event);
	void OnMouseDClick(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);
    void OnEraseBackground(wxEraseEvent &event) {}
    wxRect GetPaintRect();
	virtual void UpdateShow();
    
  private:
    DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_SEL_CHANGED, 3200)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_POINTED, 3201)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_DCLICK, 3202)
  DECLARE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_CAPTION_CHANGED, 3203)
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

#endif // WXVILLALIB_THUMBNAILS_H
