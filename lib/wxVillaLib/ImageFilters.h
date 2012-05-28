/////////////////////////////////////////////////////////////////////////////
// Name:        ImageFilters.h
// Purpose:     Image Filters (middle-value filter, Gauss filter, etc.)
// Author:      Alex Thuering
// Created:		21.06.2003
// RCS-ID:      $Id: ImageFilters.h 658 2017-05-01 11:19:59Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_IMAGE_FILTERS_H
#define WXVILLALIB_IMAGE_FILTERS_H

#include <wx/wx.h>
#include <wx/image.h>

#ifndef uchar
#define uchar unsigned char
#endif

/** The base class of all image processing filters.*/
class wxImageFilter
{
  public:
    /** Constructs a filter with initial filter size. */
    wxImageFilter(wxImage& image, int size = 3);
    virtual ~wxImageFilter() {}
    void Run();
    virtual void AdjustPixel(uchar* image, int x, int y) = 0;
    
  protected:
    wxImage& m_img;
    wxImage m_origImg;
    int m_size;
};

/** An image processing filter of median filter.
  * The result is stored in the original image. */
class wxMedianImageFilter: public wxImageFilter
{
  public:
    wxMedianImageFilter(wxImage& image, int size = 3): wxImageFilter(image, size){}
    virtual void AdjustPixel(uchar* image, int x, int y);
};

enum wxLinearImageFilterType
{ lftMiddleValue, lftGauss };

/** An image processing filter of linears filters (middle value, Gauss).
  * The result is stored in the original image. */
class wxLinearImageFilter: public wxImageFilter
{
  public:
    wxLinearImageFilter(wxImage& image, wxLinearImageFilterType type,int size = 3);
    virtual void AdjustPixel(uchar* image, int x, int y);
    
  protected:
    wxArrayInt m_pattern;
};

#endif // WXVILLALIB_IMAGE_FILTERS_H

