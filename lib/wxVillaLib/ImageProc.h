/////////////////////////////////////////////////////////////////////////////
// Name:        ImageProc.h
// Purpose:     Image processing functions.
// Author:      Alex Thuering
// Created:		18.06.2003
// RCS-ID:      $Id: ImageProc.h,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_IMAGE_PROC_H
#define IMAGE_PROC_H

#include <wx/wx.h>
#include <wx/image.h>

inline int wxGetBrightness(unsigned char r, unsigned char g, unsigned char b)
{ return (wxMax(r, wxMax(g,b))+wxMin(r, wxMin(g,b)))/2; }

inline unsigned char wxAdjustBrightness(unsigned char c, char n)
{ return n<=0 ? c*(n+128)/128 : c + (255-c)*n/128; }

inline void wxAdjustBrightness(
 unsigned char& r, unsigned char& g, unsigned char& b, char n)
{
  r = wxAdjustBrightness(r,n);
  g = wxAdjustBrightness(g,n);
  b = wxAdjustBrightness(b,n);
}

void wxAdjustBrightness(wxImage& img, wxRect rect, char n);
void wxAdjustBrightness(wxImage& img, char n);

void wxAdjustContrast(wxImage& img, wxRect rect, char n);
void wxAdjustContrast(wxImage& img, char n);

#endif // WXVILLALIB_IMAGE_PROC_H
