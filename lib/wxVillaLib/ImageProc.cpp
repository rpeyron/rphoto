/////////////////////////////////////////////////////////////////////////////
// Name:        ImageProc.cpp
// Purpose:     Image processing functions.
// Author:      Alex Thuering
// Created:		18.06.2003
// RCS-ID:      $Id: ImageProc.cpp,v 1.1 2003/12/29 15:22:26 remi Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "ImageProc.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////
///////////////////////////// Brightness /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void wxAdjustBrightness(wxImage& img, char n)
{
  wxAdjustBrightness(img, wxRect(0,0,img.GetWidth()-1,img.GetHeight()-1), n);
}

void wxAdjustBrightness(wxImage& img, wxRect rect, char n)
{
  int w = img.GetWidth();
  int h = img.GetHeight();
  
  int k = wxMax(rect.GetLeft(),0);
  rect.width += rect.x-k; rect.x = k; //rect.SetLeft(k);
  k = wxMax(rect.GetTop(),0);
  rect.height += rect.y-k; rect.y = k; //rect.SetTop(k);
  rect.SetRight(wxMin(rect.GetRight(),w-1));
  rect.SetBottom(wxMin(rect.GetBottom(),h-1));
  
  unsigned char* data = img.GetData() + rect.GetTop()*w*3;
  for (int y=rect.GetTop(); y<=rect.GetBottom(); y++)
  {
    data += rect.GetLeft()*3;
    for (int x=rect.GetLeft(); x<=rect.GetRight(); x++)
    {
      wxAdjustBrightness(*data, *(data+1), *(data+2), n);
      data += 3;
    }
    data += (w-rect.GetRight()-1)*3;
  }
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////// Contrast /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

wxArrayInt contrastTable;

void InitializateContrastTable()
{
  if (contrastTable.IsEmpty())
    for (int i=0; i<256; i++)
    {
      double brightness = double(i)/255;
      contrastTable.Add(int(
        1000*((sin(3.1415*(brightness-0.5))+1)/2-brightness)));
    }
}

// Adjust contrast: dark color become darker, light color become lighter.
inline void wxAdjustContrast(
 unsigned char& r, unsigned char& g, unsigned char& b, char n)
{
  int brightness = wxGetBrightness(r,g,b);
  wxAdjustBrightness(r,g,b, contrastTable[brightness]*n/100);
}

void wxAdjustContrast(wxImage& img, char n)
{
  wxAdjustContrast(img, wxRect(0,0,img.GetWidth()-1,img.GetHeight()-1), n);
}

void wxAdjustContrast(wxImage& img, wxRect rect, char n)
{
  InitializateContrastTable();
  unsigned char* data = img.GetData() + rect.GetTop()*img.GetWidth()*3;
  for (int y=rect.GetTop(); y<=rect.GetBottom(); y++)
  {
    data += rect.GetLeft()*3;
    for (int x=rect.GetLeft(); x<=rect.GetRight(); x++)
    {
      wxAdjustContrast(*data, *(data+1), *(data+2), n);
      data += 3;
    }
    data += (img.GetWidth()-rect.GetRight()-1)*3;
  }
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////// Convert functions //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Convert RGB to HSL
void RGB2HSL(const wxColour colour,
  double& hue, double& saturation, double& luminosity)
{
  double r = (double) colour.Red()/255;
  double g = (double) colour.Green()/255;
  double b = (double) colour.Blue()/255;
  double max = wxMax(r, wxMax(g,b));
  double min = wxMin(r, wxMin(g,b));
  hue = 0;
  saturation = 0;
  luminosity = (min+max)/2;
  double delta=max-min;
  if (delta == 0)
    return;
  saturation = delta/((luminosity <= 0.5) ? (min+max) : (2-max-min));
  if (r == max)
    hue = (g == min ? 5+(max-b)/delta : 1-(max-g)/delta);
  else if (g == max)
    hue = (b == min ? 1+(max-r)/delta : 3-(max-b)/delta);
  else
    hue = (r == min ? 3+(max-g)/delta : 5-(max-r)/delta);
  hue /= 6;
}

// Convert HSL to RGB
wxColour HSL2RGB(double hue, double saturation, double luminosity)
{
  wxColour colour;
  
  if (saturation == 0)
  {
    int c = (int) (255*luminosity+0.5);
    colour.Set(c, c, c);
    return colour;
  }
  
  double v=(luminosity <= 0.5) ? (luminosity*(1+saturation)) :
    (luminosity+saturation-luminosity*saturation);
  double y=2*luminosity-v;
  double x=y+(v-y)*(6*hue-floor(6*hue));
  double z=v-(v-y)*(6*hue-floor(6*hue));
  
  double r,g,b;
  switch ((int) (6*hue))
  {
    case 0: r=v; g=x; b=y; break;
    case 1: r=z; g=v; b=y; break;
    case 2: r=y; g=v; b=x; break;
    case 3: r=y; g=z; b=v; break;
    case 4: r=x; g=y; b=v; break;
    case 5: r=v; g=y; b=z; break;
    default: r=v; g=x; b=y; break;
  }
  
  colour.Set((int) (255*r+0.5), (int) (255*g+0.5), (int) (255*b+0.5));
  return colour;
}
