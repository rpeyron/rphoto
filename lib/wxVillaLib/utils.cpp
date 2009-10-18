/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Alex Thuering
// Created:		23.10.2003
// RCS-ID:      $Id: utils.cpp 263 2005-11-21 23:14:19Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "utils.h"
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/filename.h>

#ifdef __UNIX_LIKE__
#include <stdlib.h>
#include <sys/param.h>
#endif

wxString appPath;

wxString wxGetAppPath()
{
  if (appPath.length() == 0)
#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    appPath = wxGetWorkingDirectory();
#else
  {
    if (wxIsAbsolutePath(wxTheApp->argv[0]))
      appPath = wxTheApp->argv[0];
    else
    {
	  // Is it a relative path?
	  wxString currentDir(wxGetWorkingDirectory());
	  if (currentDir.Last() != wxFILE_SEP_PATH)
		currentDir += wxFILE_SEP_PATH;
	  wxString str = currentDir + wxTheApp->argv[0];
	  if (wxFileExists(str))
		appPath = str;
	  else
	  {
		// OK, it's neither an absolute path nor a relative path.
		// Search PATH.
		wxPathList pathList;
		pathList.AddEnvList(wxT("PATH"));
		str = pathList.FindAbsoluteValidPath(wxTheApp->argv[0]);
		if (!str.IsEmpty())
		  appPath = str;
	  }
    }
#ifdef __UNIX_LIKE__
	// realfullname
	char realname[MAXPATHLEN];
	realpath(appPath.mb_str(), (char*)realname);
	appPath = wxString(realname, *wxConvCurrent);
#endif
	appPath = wxPathOnly(appPath);
  }
#endif
  if (appPath.Last() != wxFILE_SEP_PATH)
    appPath += wxFILE_SEP_PATH;
  return appPath;
}

void wxSetAppPath(wxString value)
{
  appPath = value;
}

wxString wxFindDataDirectory(wxString dir)
{
  wxString d = wxGetAppPath() + dir;
  if (wxDir::Exists(d))
	return d;
  wxFileName dname(wxGetAppPath() + wxT("..") +
     wxFILE_SEP_PATH + dir + wxFILE_SEP_PATH);
  dname.Normalize();
  if (wxDir::Exists(dname.GetFullPath()))
	return dname.GetFullPath();
#ifdef DATADIR
  return wxString(DATADIR,wxConvLocal) + wxFILE_SEP_PATH + dir + wxFILE_SEP_PATH;
#else
  return wxGetAppPath() + dir + wxFILE_SEP_PATH;
#endif
}

wxString wxFindDataFile(wxString filename)
{
  wxString d = wxGetAppPath() + filename;
  if (wxFileExists(d))
	return d;
  wxFileName fname(wxGetAppPath() + wxT("..") + wxFILE_SEP_PATH + filename);
  fname.Normalize();
  if (wxFileExists(fname.GetFullPath()))
	return fname.GetFullPath();
#ifdef DATADIR
  return wxString(DATADIR,wxConvLocal) + wxFILE_SEP_PATH + filename;
#else
  return wxGetAppPath() + filename;
#endif
}
