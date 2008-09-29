/////////////////////////////////////////////////////////////////////////////
// Name:        ExifHandler.h
// Purpose:     ExifHandler class
// Author:      Alex Thuering
// Created:		30.12.2007
// RCS-ID:      $Id: ExifHandler.h,v 1.1 2007/12/30 22:45:02 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_EXIFHANDLER_H
#define WXVILLALIB_EXIFHANDLER_H

#include <wx/string.h>

class ExifHandler {
  public:
	static int getOrient(wxString filename);
};

#endif // WXVILLALIB_EXIFHANDLER_H
