/////////////////////////////////////////////////////////////////////////////
// Name:        ImageFilters.cpp
// Purpose:     Image Filters (middle-value filter, Gauss filter, etc.)
// Author:      Alex Thuering
// Created:		21.06.2003
// RCS-ID:      $Id: ImageFilters.cpp 365 2008-06-21 23:15:43Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "ImageFilters.h"
#include "ImageProc.h"

#define DefineCompare(name, T) int name ##\
 CompareValues(T first, T second) { return first - second; }
DefineCompare(Int, int);
WX_DEFINE_SORTED_ARRAY_CMP_INT(int, IntCompareValues, wxSortedArrayInt); 

wxImageFilter::wxImageFilter(wxImage& image, int size): m_img(image)
{
  m_size = size;
  m_origImg = image.Copy();
}

void wxImageFilter::Run()
{
  uchar* data = m_img.GetData();
  for (int y=0; y<m_img.GetHeight(); y++)
  {
    for (int x=0; x<m_img.GetWidth(); x++)
    {
      AdjustPixel(data, x, y);
      data += 3;
    }
  }
}

void wxMedianImageFilter::AdjustPixel(uchar* image, int x, int y)
{
  uchar* data = m_origImg.GetData();
  wxSortedArrayInt medianR, medianG, medianB;
  int w = m_img.GetWidth()-1;
  int h = m_img.GetHeight()-1;
  for (int py=-m_size/2; py<=m_size/2; py++)
  {
    int y1 = abs(y+py);
    y1 = h - abs(h-y1); // y1>=height
    y1 *= (w+1)*3;
    for (int px=-m_size/2; px<=m_size/2; px++)
    {
      int x1 = abs(x+px); // x1<0
      x1 = w - abs(w-x1); // x1>=width
      uchar* p = data+y1+x1*3;
      medianR.Add(*(p++));
      medianG.Add(*(p++));
      medianB.Add(*(p));
    }
  }
  *(image++) = medianR[(m_size*m_size+1)/2];
  *(image++) = medianG[(m_size*m_size+1)/2];
  *(image)   = medianB[(m_size*m_size+1)/2];
}

wxLinearImageFilter::wxLinearImageFilter(wxImage& image,
 wxLinearImageFilterType type, int size): wxImageFilter(image, size)
{
  switch(type)
  {
    case lftMiddleValue:
    {
      for (int i=0; i<m_size*m_size; i++)
        m_pattern.Add(1);
      break;
    }
    case lftGauss:
    {
      for (int i=0; i<m_size*m_size; i++)
        m_pattern.Add(0);
      int k = 0;
      for (int i=0; i<m_size; i++)
      {
        k += i<=m_size/2 ? 1 : -1;
        for (int j=0; j<m_size; j++)
        {
          m_pattern[i*m_size+j] += k;
          m_pattern[j*m_size+i] += k;
        }
      }
      break;
    }
  }
  
}

void wxLinearImageFilter::AdjustPixel(uchar* image, int x, int y)
{
  uchar* data = m_origImg.GetData();
  int cf = 0;
  for (int i=0; i<m_size*m_size; i++)
    cf += m_pattern[i];
  
  int resR = 0;
  int resG = 0;
  int resB = 0;
  int pn = 0;
    
  int w = m_img.GetWidth()-1;
  int h = m_img.GetHeight()-1;
  for (int py=-m_size/2; py<=m_size/2; py++)
  {
    int y1 = abs(y+py);
    y1 = h - abs(h-y1); // y1>=height
    y1 *= (w+1)*3;
    for (int px=-m_size/2; px<=m_size/2; px++)
    {
      int x1 = abs(x+px); // x1<0
      x1 = w - abs(w-x1); // x1>=width
      uchar* p = data+y1+x1*3;
      resR += m_pattern[pn]*(*(p++));
      resG += m_pattern[pn]*(*(p++));
      resB += m_pattern[pn]*(*(p));
      pn++;
    }
  }
  
  *(image++) = resR/cf;
  *(image++) = resG/cf;
  *(image) = resB/cf;
}

