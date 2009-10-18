/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Alex Thuering
// Created:		23.10.2003
// RCS-ID:      $Id: utils.h 263 2005-11-21 23:14:19Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_UTILS_H
#define WXVILLALIB_UTILS_H

#include <wx/wx.h>
#include <wx/log.h>

#if defined(DEBUG) || defined(__WXDEBUG__)
#define wxLogTraceE(msg)\
 wxLogMessage(wxString(_T("%s:%d: trace: ")) + msg, __TFILE__, __LINE__)
#else
#define wxLogTraceE
#endif

wxString wxGetAppPath();
void wxSetAppPath(wxString value);

wxString wxFindDataDirectory(wxString dir);
wxString wxFindDataFile(wxString filename);

#endif // WXVILLALIB_UTILS_H
