/////////////////////////////////////////////////////////////////////////////
// Name:        Thumbnails.cpp
// Purpose:     wxThumbnails class
// Author:      Alex Thuering
// Created:		2.02.2003
// RCS-ID:      $Id: Thumbnails.cpp,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/filename.h>
#include "Thumbnails.h"
#include "ImageProc.h"
#include "imagjpg.h"
#include <wx/dnd.h>

const int POINTED_BRIGHTNESS = 24;

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxThumbArray);

BEGIN_EVENT_TABLE(wxThumbnails, wxScrolledWindow)
  EVT_PAINT(wxThumbnails::OnPaint)
  EVT_ERASE_BACKGROUND(wxThumbnails::OnEraseBackground)
  EVT_SIZE(wxThumbnails::OnResize)
  EVT_LEFT_DOWN(wxThumbnails::OnMouseDown)
  EVT_RIGHT_DOWN(wxThumbnails::OnMouseDown)
  EVT_LEFT_UP(wxThumbnails::OnMouseUp)
  EVT_LEFT_DCLICK(wxThumbnails::OnMouseDClick)
  EVT_MOTION(wxThumbnails::OnMouseMove)
  EVT_LEAVE_WINDOW(wxThumbnails::OnMouseLeave)
END_EVENT_TABLE()

DEFINE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_SEL_CHANGED)
DEFINE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_POINTED)
DEFINE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_DCLICK)
DEFINE_EVENT_TYPE(EVT_COMMAND_THUMBNAILS_CAPTION_CHANGED)

wxThumbnails::wxThumbnails(wxWindow* parent, int id):
  wxScrolledWindow(parent, id)
{
  SetThumbSize(96, 96);
  m_selected = -1;
  m_pointed = -1;
  m_labelControl = NULL;
  m_pmenu = NULL;
  m_dragging = false;
  m_leftDown = false;
}

void wxThumbnails::SetThumbSize(int width, int height,
 int border, int infoBorder)
{
  m_tWidth = width; 
  m_tHeight = height;
  m_tBorder = border;
  m_tInfoBorder = infoBorder;
  SetScrollRate((m_tWidth+m_tBorder)/4, (m_tHeight+m_tBorder)/4);
  SetSizeHints(m_tWidth+m_tBorder*2+16, m_tHeight+m_tBorder*2+16);
}

void wxThumbnails::GetThumbSize(int& width, int& height,
 int& border, int& infoBorder)
{
  width = m_tWidth; 
  height = m_tHeight;
  border = m_tBorder;
  infoBorder = m_tInfoBorder;
}

void wxThumbnails::Clear()
{
  m_items.Clear();
  m_selected = -1;
  UpdateProp();
  Refresh();
}

void wxThumbnails::ShowDir(wxString dir)
{
  m_dir = dir;
  SetCaption(m_dir);
  // update items
  m_items.Clear();
  wxString fname = wxFindFirstFile(m_dir + _T("/*"));
  while (!fname.IsEmpty())
  {
	if (wxImage::FindHandler(fname.AfterLast('.').Lower(), -1))
	  m_items.Add(wxThumb(fname));
    fname = wxFindNextFile();
  }
  m_items.Sort(cmpthumb);
  UpdateProp();
  Refresh();
}

void wxThumbnails::UpdateItems()
{
  wxThumb thumb(_T(""));
  if (m_selected>=0)
  {
    thumb.filename = m_items[m_selected].filename;
    thumb.id = m_items[m_selected].id;
  }
  m_selected = -1;
  UpdateShow();
  if (thumb.filename.Length())
  {
    for (int i=0; i<(int)m_items.GetCount(); i++)
      if (m_items[i].filename == thumb.filename &&
          m_items[i].id == thumb.id)
      {
        m_selected = i;
        ScrollToSelected();
        break;
      }
    Refresh();
    wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_SEL_CHANGED, this->GetId());
    GetEventHandler()->ProcessEvent(evt);
  }
}

void wxThumbnails::SetCaption(wxString caption)
{
  m_caption = caption;
  if (m_labelControl)
  {
	int maxWidth = m_labelControl->GetSize().GetWidth()/8;
	if (caption.length() > maxWidth)
	  caption = _T("...") + caption.Mid(caption.length()+3-maxWidth);
    m_labelControl->SetLabel(caption);
  }
  wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_CAPTION_CHANGED, this->GetId());
  GetEventHandler()->ProcessEvent(evt);
}

void wxThumbnails::UpdateShow()
{
  ShowDir(m_dir);
}

void wxThumbnails::UpdateProp()
{
  int width = GetClientSize().GetWidth();
  m_cols = (width-m_tBorder) / (m_tWidth+m_tBorder);
  if (m_cols == 0)
    m_cols = 1;
  m_rows = m_items.GetCount()/m_cols + (m_items.GetCount()%m_cols ? 1 : 0);
  SetVirtualSize(
    m_cols*(m_tWidth+m_tBorder)+m_tBorder,
    m_rows*(m_tHeight+m_tBorder)+m_tBorder);
  if (width != GetClientSize().GetWidth())
    UpdateProp();
}

void wxThumbnails::InsertItem(wxThumb& thumb, int pos)
{
  if (pos < 0 || pos > m_items.GetCount())
	m_items.Add(thumb);
  else
	m_items.Insert(thumb, pos);
  UpdateProp();
}

wxRect wxThumbnails::GetPaintRect()
{
  wxSize size = GetClientSize();
  wxRect paintRect(0, 0, size.GetWidth(), size.GetHeight());
  GetViewStart(&paintRect.x, &paintRect.y);
  int xu,yu;
  GetScrollPixelsPerUnit(&xu, &yu);
  paintRect.x *= xu; paintRect.y *= yu;
  return paintRect;
}

void wxThumbnails::ScrollToSelected()
{
  if (m_selected == -1)
    return;
  // get row
  int row = m_selected/m_cols;
  // calc position to scroll view
  int sy; // scroll to y
  wxRect paintRect = GetPaintRect();
  int y1 = row*(m_tHeight+m_tBorder);
  int y2 = y1 + m_tBorder + m_tHeight + m_tBorder;
  if (y1 < paintRect.GetTop())
    sy = y1; // scroll top
  else if (y2 > paintRect.GetBottom())
    sy = y2 - paintRect.height; // scroll bottom
  else return;
  // scroll view
  int xu, yu;
  GetScrollPixelsPerUnit(&xu, &yu);
  sy = sy/yu + (sy%yu ? 1 : 0); // convert sy to scroll units
  int x,y;
  GetViewStart(&x, &y);
  Scroll(x,sy);
}

void wxThumbnails::DrawThumbnail(wxBitmap& bmp, wxThumb& thumb, bool selected,
  bool pointed)
{
  wxMemoryDC dc;
  dc.SelectObject(bmp);
  dc.BeginDrawing();  
  int x = m_tBorder/2;
  int y = m_tBorder/2;
  // background
  dc.SetPen(wxPen(*wxBLACK,0,wxTRANSPARENT));
  dc.SetBrush(wxBrush(m_backgroundColour, wxSOLID));
  dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
  // contour
  dc.SetPen(wxPen(selected ? *wxBLUE : *wxLIGHT_GREY,0,wxSOLID));
  dc.SetBrush(wxBrush(*wxBLACK, wxTRANSPARENT));
  dc.DrawRectangle(x-1, y-1, m_tWidth+2, m_tHeight+2);
  // image
  if (!thumb.image.Ok())
  {
	int tw = m_tWidth;
    int th = m_tHeight - m_tInfoBorder;
    wxImage img;
	if (!thumb.vImage.Ok())
	{
	  //img.SetOption(_T("max_width"), tw);
	  //img.SetOption(_T("max_height"), th);
	  ((wxJPGHandler*) img.FindHandler(_T("JPEG file")))->m_maxWidth = tw;
	  ((wxJPGHandler*) img.FindHandler(_T("JPEG file")))->m_maxHeight = th;
	  img.LoadFile(thumb.filename);
	  ((wxJPGHandler*) img.FindHandler(_T("JPEG file")))->m_maxWidth = -1;
	  ((wxJPGHandler*) img.FindHandler(_T("JPEG file")))->m_maxHeight = -1;
	}
	else
	  img = thumb.vImage;
    float scale = (float) tw / img.GetWidth();
    if (scale > (float) th / img.GetHeight())
      scale = (float) th / img.GetHeight();
    thumb.image = wxBitmap(
	  img.Scale((int) (img.GetWidth()*scale), (int) (img.GetHeight()*scale)));
  }
  wxBitmap img = thumb.image;
  if (pointed)
  {
    wxImage imgTmp(img);
    wxAdjustBrightness(imgTmp, wxRect(0,0,img.GetWidth(),img.GetHeight()),
      POINTED_BRIGHTNESS);
    img = wxBitmap(imgTmp);
  }
  wxRect imgRect(
    x + (m_tWidth-img.GetWidth())/2,
	y + (m_tHeight-img.GetHeight()-m_tInfoBorder)/2,
    img.GetWidth(), img.GetHeight());      
  dc.DrawBitmap(img, imgRect.x, imgRect.y);    
  dc.EndDrawing();
  dc.SelectObject(wxNullBitmap);
}

void wxThumbnails::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxRect paintRect = GetPaintRect();
  
  wxPaintDC dc(this);  
  PrepareDC(dc);
  dc.BeginDrawing();
  // items
  for (int i=0; i<(int)m_items.GetCount(); i++)
  {
    int col = i%m_cols;
    int row = i/m_cols;
    int tx = m_tBorder/2 + col*(m_tWidth+m_tBorder);
    int ty = m_tBorder/2 + row*(m_tHeight+m_tBorder);
    // visible?
    if (!paintRect.Intersects(
		wxRect(tx, ty, m_tWidth+m_tBorder, m_tHeight+m_tBorder)))
      continue;
    wxBitmap thmb(m_tWidth+m_tBorder, m_tHeight+m_tBorder);
    DrawThumbnail(thmb, m_items[i], i == m_selected, i == m_pointed);
    dc.DrawBitmap(thmb, tx, ty);
  }
  
  // background
  dc.SetPen(wxPen(*wxBLACK,0,wxTRANSPARENT));
  dc.SetBrush(wxBrush(m_backgroundColour, wxSOLID));
  wxRect buffer(m_tBorder/2,m_tBorder/2,
    m_cols*(m_tWidth+m_tBorder), m_rows*(m_tHeight+m_tBorder));
  int w = wxMax(GetClientSize().GetWidth(), buffer.width);
  int h = wxMax(GetClientSize().GetHeight(), buffer.height);
  dc.DrawRectangle(0, 0, w, buffer.y);
  dc.DrawRectangle(0, 0, buffer.x, h);
  dc.DrawRectangle(buffer.GetRight(), 0, w-buffer.GetRight(), h+50);
  dc.DrawRectangle(0, buffer.GetBottom(), w, h-buffer.GetBottom()+50);
  int col = m_items.GetCount()%m_cols;
  if (col > 0)
  {
    buffer.x += col*(m_tWidth+m_tBorder);
    buffer.y += (m_rows-1)*(m_tHeight+m_tBorder);
    dc.DrawRectangle(buffer);
  }
  dc.EndDrawing();
}

void wxThumbnails::OnResize(wxSizeEvent &event)
{
  UpdateProp();
  ScrollToSelected();
}

void wxThumbnails::OnMouseDown(wxMouseEvent &event)
{
  if (event.LeftDown())
	m_leftDown = true;
  int x = event.GetX();
  int y = event.GetY();
  CalcUnscrolledPosition(x, y, &x, &y);
  // get item number to select
  int col = (x-m_tBorder)/(m_tWidth+m_tBorder);
  if (col >= m_cols)
    col = m_cols - 1;
  int row = (y-m_tBorder)/(m_tHeight+m_tBorder);
  int sel = row*m_cols + col;
  if (sel >= (int)m_items.GetCount())
    sel = -1;
  if (sel != m_selected)
  {
    // set new selection
    m_selected = sel;
    ScrollToSelected();
    Refresh();
    wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_SEL_CHANGED, this->GetId());
    GetEventHandler()->ProcessEvent(evt);
  }
  
  // Popup menu
  if (m_pmenu && event.RightDown() && sel>=0)
  {
    m_pmenu->SetTitle(wxFileName(m_items[sel].filename).GetFullName());
    PopupMenu(m_pmenu, event.GetPosition());
  }
}

void wxThumbnails::OnMouseUp(wxMouseEvent &event)
{
  if (event.LeftUp())
	m_leftDown = false;
}

void wxThumbnails::OnMouseDClick(wxMouseEvent &event)
{
  wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_DCLICK, this->GetId());
  GetEventHandler()->ProcessEvent(evt);
}

void wxThumbnails::OnMouseMove(wxMouseEvent &event)
{
  // -- drag & drop --
  if (m_dragging && m_leftDown && event.Dragging() && m_selected>=0)
  {
	wxFileDataObject files;
	files.AddFile(m_items[m_selected].filename);
	wxDropSource source(files, this);
	source.DoDragDrop(0);
	m_leftDown = false;
  }
  
  // -- light-effect --
  int x = event.GetX();
  int y = event.GetY();
  CalcUnscrolledPosition(x, y, &x, &y);
  // get item number
  int col = (x-m_tBorder)/(m_tWidth+m_tBorder);
  if (col >= m_cols)
    col = m_cols - 1;
  int row = (y-m_tBorder)/(m_tHeight+m_tBorder);
  int sel = row*m_cols + col;
  if (sel >= (int)m_items.GetCount())
    sel = -1;
  if (sel == m_pointed)
    return;
  // update thumbnail
  m_pointed = sel;
  Refresh();
  wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_POINTED, this->GetId());
  GetEventHandler()->ProcessEvent(evt);
}

void wxThumbnails::OnMouseLeave(wxMouseEvent &event)
{
  if (m_pointed != -1)
  {
	m_pointed = -1;
	Refresh();
	wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_POINTED, this->GetId());
	GetEventHandler()->ProcessEvent(evt);
  }
}
