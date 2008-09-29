/////////////////////////////////////////////////////////////////////////////
// Name:        ThumbnailFactory.cpp
// Purpose:     wxThumbnailFactory class
// Author:      Alex Thuering
// Created:		15.02.2003
// RCS-ID:      $Id: ThumbnailFactory.cpp 366 2008-09-06 18:22:16Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "ThumbnailFactory.h"
#include "Thumbnails.h"
#include "imagjpg.h"
#include "ExifHandler.h"
#include <wx/dir.h>
#include <wx/app.h>
#include <wx/wx.h>
#include <sys/stat.h>

#ifdef GNOME2
#include <glib.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkdrawable.h>
#include <libgnomeui/gnome-ui-init.h>
#include <libgnomeui/gnome-thumbnail.h>
static GnomeThumbnailFactory* thumbnail_factory = NULL;
#elif defined(WX_SVG)
#include <wxSVG/mediadec_ffmpeg.h>
#elif defined(FFMPEG)
#include "mediadec_ffmpeg.h"
#endif
#define THUMBNAILS_DIR wxGetHomeDir() + wxFILE_SEP_PATH + _T(".thumb")

#ifdef __WXMSW__
#undef  wxBITMAP
#define wxBITMAP(icon) wxBitmap(icon##_xpm)
#endif
#include "rc/gnome-fs-loading-icon.xpm"
#include "rc/gnome-fs-regular.xpm"
#include "rc/gnome-mime-video-mpeg.xpm"
#include "rc/gnome-mime-audio.xpm"

class ThumbInfo
{
  public:
	ThumbInfo(wxString filename, wxWindow* parent, int width, int height)
	{
	  this->filename = filename;
	  this->parent = parent;
	  this->width = width;
	  this->height = height;
#ifdef GNOME2
	  pixbuf = NULL;
#endif
	}
	
	~ThumbInfo()
	{
#ifdef GNOME2
	  if (pixbuf)
		g_object_unref(pixbuf);
#endif
	}
	
	wxString filename;
	wxWindow* parent;
	time_t mtime;
	int width;
	int height;
	wxString mimeType;
#ifdef GNOME2
	char* uri;
	char* mime_type;
	GdkPixbuf* pixbuf;
#else
	wxString uri;
#endif
};

ThumbInfoArray wxThumbnailFactory::m_queue;
wxMutex wxThumbnailFactory::m_queueMutex;
wxThumbnailFactory* wxThumbnailFactory::thread = NULL;
int wxThumbnailFactory::maxFileSize = 102400; // immediately render files with size < 100K

void wxThumbnailFactory::Init()
{
#ifndef GNOME2
  wxLogNull log;
  wxMkdir(THUMBNAILS_DIR);
  wxMkdir(THUMBNAILS_DIR + wxFILE_SEP_PATH + _T("normal"));
  wxMkdir(THUMBNAILS_DIR + wxFILE_SEP_PATH + _T("fail"));
#endif
}

void wxThumbnailFactory::InitGnome(const char* appName, const char* appVersion,
  int argc, char** argv)
{
#ifdef GNOME2
  gnome_init(appName, appVersion, argc, argv);
  gnome_vfs_init();
  thumbnail_factory = gnome_thumbnail_factory_new(GNOME_THUMBNAIL_SIZE_NORMAL);
#endif
}

wxImage wxThumbnailFactory::GetThumbnail(wxWindow* parent,
 wxString filename, int width, int height)
{
  wxImage img;
  
  if (!filename.length())
    return img;
  
  ThumbInfo* info = GetThumbInfo(filename, parent, width, height);
  img = LoadThumbnail(*info); // load thumbnail, if it was already generated
  
  if (!img.Ok()) // add thumbnail to queue => generate
  {
    if (wxThumbnails::IsAudio(filename))
      img = wxBITMAP(gnome_mime_audio).ConvertToImage();
    else if (CanThumbnail(*info))
      img = AddToQueue(info);
    else
    {
      if (wxThumbnails::IsVideo(filename))
        img = wxBITMAP(gnome_mime_video_mpeg).ConvertToImage();
      else
        img = wxBITMAP(gnome_fs_regular).ConvertToImage();
    }
  }
  else
    delete info;
  return img;
}

wxThread::ExitCode wxThumbnailFactory::Entry()
{
  while (1)
  {
	m_queueMutex.Lock();
	if (!m_queue.Count())
	  break;
	ThumbInfo* info = m_queue.Item(0);
	m_queue.RemoveAt(0);
	m_queueMutex.Unlock();
	
	wxImage img = LoadThumbnail(*info);
	if (!img.Ok())
	  img = GenerateThumbnail(*info);
	wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_THUMB_CHANGED);
	evt.SetString(info->filename);
	evt.SetClientData(new wxImage(img));
	wxPostEvent(info->parent, evt);
	delete info;
  }
  thread = NULL;
  m_queueMutex.Unlock();
  return 0;
}

void wxThumbnailFactory::ClearQueue(wxWindow* parent)
{
  wxMutexLocker locker(m_queueMutex);
  for (int i=0; i<(int)m_queue.Count();)
  {
	if (m_queue[i]->parent == parent)
	{
	  delete m_queue[i];
	  m_queue.RemoveAt(i);
	}
	else
	  i++;
  }
}

//////////////////////////////////////////////////////////////////////////////

wxImage wxThumbnailFactory::AddToQueue(ThumbInfo* info)
{
  wxImage img = wxBITMAP(gnome_fs_loading_icon).ConvertToImage();
  wxMutexLocker locker(m_queueMutex);
  for (int i=0; i<(int)m_queue.Count(); i++)
	if (m_queue[i]->filename == info->filename &&
	    m_queue[i]->parent == info->parent)
	  return img;
  m_queue.Add(info);
  if (!thread) // start thread
  {
	thread = new wxThumbnailFactory;
	thread->Create();
	thread->Run();
  }
  return img;
}

#ifdef GNOME2
/*wxImage pixbuf2image(GdkPixbuf* pixbuf)
{
  wxBitmap bitmap(gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf));
  GdkBitmap* gbitmap = bitmap.GetPixmap();
  gdk_draw_pixbuf(gbitmap, NULL, pixbuf, 0,0,0,0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
  return bitmap.ConvertToImage();
}*/
#else
#include "thumb_md5.cpp"
#endif

ThumbInfo* wxThumbnailFactory::GetThumbInfo(wxString filename, wxWindow* parent,
  int width, int height)
{
  ThumbInfo* info = new ThumbInfo(filename, parent, width, height);
  if (wxImage::FindHandler(filename.AfterLast('.').Lower(), -1))
	info->mimeType = _T("image");
  else if (wxThumbnails::IsVideo(filename))
	info->mimeType = _T("video/mpeg");
  else
    info->mimeType = _T("unknown");
#ifdef GNOME2
  info->uri = gnome_vfs_get_uri_from_local_path(info->filename.mb_str());
  // get mtime & mime_type
  info->mtime = 0;
  GnomeVFSFileInfo *file_info = gnome_vfs_file_info_new();
  gnome_vfs_get_file_info(info->uri, file_info,
	(GnomeVFSFileInfoOptions) (GNOME_VFS_FILE_INFO_FOLLOW_LINKS));
  if (file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_MTIME)
	info->mtime = file_info->mtime;
  info->mime_type = "video/mpeg";
  gnome_vfs_file_info_unref(file_info);
#else
  info->uri = _T("file://");
  char* fnameEscaped = escape_string(info->filename.mb_str());
  info->uri += wxString(fnameEscaped, *wxConvCurrent);
  if(fnameEscaped)
    free(fnameEscaped);
#endif
  return info;
}

wxString wxThumbnailFactory::GetThumbPath(ThumbInfo& info, ThumbType type)
{
#ifdef GNOME2
  char* thumbnail_path =
    gnome_thumbnail_factory_lookup(thumbnail_factory, info.uri, info.mtime);
  if (thumbnail_path == NULL)
  	thumbnail_path = gnome_thumbnail_path_for_uri(info.uri, GNOME_THUMBNAIL_SIZE_NORMAL);
  if (!thumbnail_path || thumbnail_path[0] != '/')
	return wxEmptyString;
  wxString thumbPath = wxString(thumbnail_path, *wxConvCurrent);
  if (type == THUMBNAIL_FAILED) {
  	int idx = thumbPath.Find(wxString(wxFILE_SEP_PATH) + wxT("normal") + wxString(wxFILE_SEP_PATH));
  	if (idx > 0)
  		thumbPath = thumbPath.Mid(0, idx+1) + wxT("fail") + thumbPath.Mid(idx+7);
  	else
  		thumbPath = wxEmptyString;
  }
  return thumbPath;
#else
  unsigned char digest[16];
  thumb_md5(info.uri.mb_str(), digest);
  wxString md5 = wxString(thumb_digest_to_ascii(digest), *wxConvCurrent);
  wxString file = md5 + _T(".png");
  wxString dir = THUMBNAILS_DIR;
  dir += wxFILE_SEP_PATH;
  dir += type == THUMBNAIL_FAILED ? _T("fail") : _T("normal");
  return dir + wxFILE_SEP_PATH + file;
#endif
}

wxImage wxThumbnailFactory::LoadThumbnail(ThumbInfo& info, ThumbType type)
{
  wxImage img;
  
  if (info.mimeType == _T("image"))
  {
	wxStructStat fInfo;
	if (wxStat(info.filename, &fInfo) == 0 && fInfo.st_size < maxFileSize) {
	  img = GenerateThumbnail(info, false);
	  return img;
	}
  }
  
  // load thumbnail for video
  wxString thumbPath = GetThumbPath(info, type);
#ifdef GNOME2
  GdkPixbuf* pixbuf = NULL;
  if (thumbPath.Length())
	pixbuf = gdk_pixbuf_new_from_file(thumbPath.mb_str(), NULL);
  if (pixbuf)
  {
	if (gnome_thumbnail_is_valid(pixbuf, info.uri, info.mtime))
	{
	  wxLogNull log;
	  img.LoadFile(thumbPath); // img = pixbuf2image(pixbuf); failed on some systems
	}
	g_object_unref(pixbuf);
  }
#else
  wxLogNull log;
  img.LoadFile(thumbPath);
#endif
  return img;
}

bool wxThumbnailFactory::CanThumbnail(ThumbInfo& info)
{
  if (info.mimeType == _T("unknown"))
    return false;
#ifdef GNOME2
  if (info.mimeType != _T("image"))
    return gnome_thumbnail_factory_can_thumbnail(thumbnail_factory, info.uri, info.mime_type, info.mtime);
#endif
  // check if we already tried to generate this thumbnail and it fails
  wxString thumbPath = GetThumbPath(info, THUMBNAIL_FAILED);
  return !wxFileExists(thumbPath);
}

unsigned char GetBarColor(int x, int y) {
  y = y % 7;
  if (x == 0 || x == 6 || y <= 1 || y >= 5)
     return 0x00;
  if (x == 1 || x == 5 || y == 2 || y == 4)
     return (x == 1 || x == 5) && (y == 2 || y == 4) ? 0xA0 : 0xD0;
  return 0xFF;
}

wxImage wxThumbnailFactory::GenerateThumbnail(ThumbInfo& info, bool save)
{
  wxImage img;
  wxString thumbPath = GetThumbPath(info);
  if (info.mimeType == _T("image"))
  {
	wxLogNull log;
	img.Create(1,1);
	img.SetOption(_T("max_width"), info.width);
	img.SetOption(_T("max_height"), info.height);
	LoadImageFile(img, info.filename);
	if (img.Ok() && info.filename.length() > 4
			&& info.filename.Mid(info.filename.length()-4).Lower() == wxT(".jpg")) {
	  int orient = ExifHandler::getOrient(info.filename);
      switch (orient) {
        case 3:  // rotated 180
          img = img.Rotate90().Rotate90();
          break;
        case 6:  // 90 CW
          img = img.Rotate90();
          break;
        case 8:  // 90 CCW
          img = img.Rotate90(false);
          break;
        default:
		  break;
	  }
	}
  }
  else
  {
#ifdef GNOME2
    info.pixbuf = gnome_thumbnail_factory_generate_thumbnail(
      thumbnail_factory, info.uri, info.mime_type);
    if (info.pixbuf)
    {
	  gnome_thumbnail_factory_save_thumbnail(thumbnail_factory, info.pixbuf, info.uri, info.mtime);
      img.LoadFile(thumbPath);
    }
    else
      gnome_thumbnail_factory_create_failed_thumbnail(thumbnail_factory, info.uri, info.mtime);
    return img;
#elif defined(WX_SVG) || defined(FFMPEG)
    wxFfmpegMediaDecoder ffmpeg;
    if (ffmpeg.Load(info.filename) && ffmpeg.BeginDecode(96, 80)) {
      if (ffmpeg.GetDuration() > 0)
        ffmpeg.SetPosition(ffmpeg.GetDuration() * 0.05);
      img = ffmpeg.GetNextFrame();
    }
    if (img.Ok())
    {
        unsigned char* data = img.GetData();
        for (int y = 0; y<img.GetHeight(); y++) {
            for (int x = 0; x<7; x++) {
                data[0] = data[1] = data[2] = GetBarColor(x,y);
                data += 3;
            }
            data += (img.GetWidth()-14)*3;
            for (int x = 0; x<7; x++) {
                data[0] = data[1] = data[2] = GetBarColor(x,y);
                data += 3;
            }
        }
    }
#endif
  }
  if (!save)
    return img;
  // save thumbnail
  if (!img.Ok())
  {
    img.Create(1,1);
    img.SaveFile(GetThumbPath(info, THUMBNAIL_FAILED), wxBITMAP_TYPE_PNG);
    wxRemoveFile(thumbPath);
  }
  else
  {
  	if (img.GetWidth() > info.width || img.GetHeight() > info.height) {
      float scale = (float) info.width / img.GetWidth();
      if (scale > (float) info.height / img.GetHeight())
        scale = (float) info.height / img.GetHeight();
	  img.Rescale((int) (img.GetWidth()*scale), (int) (img.GetHeight()*scale));
    }
#ifdef GNOME2
	// create gdk pixbuf
	int width = img.GetWidth();
	int height = img.GetHeight();
	GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, width, height);
	// copy data from image
    const unsigned char* in = img.GetData();
    unsigned char *out = gdk_pixbuf_get_pixels(pixbuf);
    int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - 3 * width;
    for (int y = 0; y < height; y++, out += rowpad) {
        for (int x = 0; x < width; x++, out += 3, in += 3) {
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
        }
    }
	// save
	gnome_thumbnail_factory_save_thumbnail(thumbnail_factory,
	  pixbuf, info.uri, info.mtime);
#else
    img.SaveFile(thumbPath, wxBITMAP_TYPE_PNG);
#endif
  }
  return img;
}
