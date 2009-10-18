/////////////////////////////////////////////////////////////////////////////
// Name:        SConv.h
// Purpose:     String converter
// Author:      Alex Thuering
// Created:		21.07.2003
// RCS-ID:      $Id: SConv.h 263 2005-11-21 23:14:19Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_SCONV_H
#define WXVILLALIB_SCONV_H

#include <wx/wx.h>

class SConv
{
  public:
	static wxString ToString(wxColour value, bool withNumberSign = true);
	static wxColour ToColour(wxString value);
	
	static wxString ToString(wxFont value);
	static wxFont ToFont(wxString value);
};

#endif // WXVILLALIB_SCONV_H
