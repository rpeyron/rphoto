/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Alex Thuering
// Created:		23.10.2003
// RCS-ID:      $Id: utils.cpp,v 1.1 2003/12/29 15:22:27 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "utils.h"

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
	realpath(appPath.c_str(), (char*)realname);
	appPath = realname;
#endif
	appPath = wxPathOnly(appPath);
  }
#endif
  return appPath;
}

void wxSetAppPath(wxString value)
{
  appPath = value;
}
