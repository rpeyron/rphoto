/////////////////////////////////////////////////////////////////////////////
// Name:        SConv.h
// Purpose:     String converter
// Author:      Alex Thuering
// Created:		21.07.2003
// RCS-ID:      $Id: SConv.h,v 1.2 2004/10/03 09:43:05 ntalex Exp $
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
