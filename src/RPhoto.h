/* ****************************************************************************
 * RPhoto.h                                                                   *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * RPhoto : image tool for a quick and efficient use with digital photos      *
 * Copyright (C) 2004 - Rmi Peyronnet <remi+rphoto@via.ecp.fr>               *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * http://www.gnu.org/copyleft/gpl.html                                       *
 * ************************************************************************** */

#define RPHOTO_NAME wxT("RPhoto")
#define RPHOTO_VERSION wxT("v0.4.2")
#define RPHOTO_COPYRIGHT wxT("2004-2012 - Remi Peyronnet")

#define RT_DEFAULT_IMAGE wxT("test.jpg")

// lowercase AND uppercare to be able to see both in *nix ; but NOT for Windows (else files are seen twice)
#ifdef __WINDOWS__ 
#define RPHOTO_FILTERIMGLIST_ALL wxString(wxT("Image Files|*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;*.ico;*.ani;*.pcx;*.pnm;*.xpm"))
#define RPHOTO_FILTERIMGLIST wxString(wxT("BMP Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png|TIFF Files (*.tif; *.tiff)|*.tif;*.tiff"))
#else
#define RPHOTO_FILTERIMGLIST_ALL wxString(wxT("Image Files|*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;*.ico;*.ani;*.pcx;*.pnm;*.xpm;*.JPG;*.JPEG;*.PNG;*.BMP;*.GIF;*.TIF;*.TIFF;*.ICO;*.ANI;*.PCX;*.PNM;*.XPM"))
#define RPHOTO_FILTERIMGLIST wxString(wxT("BMP Files (*.bmp)|*.bmp;*.BMP|GIF Files (*.gif)|*.gif;*.GIF|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg;*.JPG;*.JPEG|PNG Files (*.png)|*.png;*.PNG|TIFF Files (*.tif; *.tiff)|*.tif;*.tiff;*.TIF;*.TIFF"))
#endif
#define RPHOTO_FILTERIMGLIST_WORLD	(wxString(wxT("All files (*.*)|*.*|")) + RPHOTO_FILTERIMGLIST_ALL + wxT("|") + RPHOTO_FILTERIMGLIST)

#define RPHOTO_TEMP_PREFIX wxT("rphoto_")

#define RPHOTO_DEFAULT_EXIF_FAVORITES wxT("DateTime,ExposureTime,FocalLength,FNumber,ISOSpeedRatings,Flash,ExposureBiasValue,MaxApertureValue,Orientation,PixelXDimension,PixelYDimension,Make,Model,Copyright,ImageDescription,Contrast,Saturation,Sharpness")
#define RPHOTO_DEFAULT_FORCED_RATIO _("0:0 (Free)|4:3 (Digital Photo)|3:2 (Paper Photo)|16:9 (Digital Photo - TV|1:1  (Square)")
#define RPHOTO_DEFAULT_CUSTOM_RATIO _("13:9 (Paper Photo - German 1)|18:13 (Paper Photo - German 2)|176x240 (Fixed Size)")
#define RPHOTO_DEFAULT_FORCED_RESIZE _("Select new size|50% (Percentage)|640x480 (Width x Height)|800x0 (With ratio)|42kb (File size)")
#define RPHOTO_DEFAULT_CUSTOM_RESIZE _("800x600|1280x1024|100kb|300kb")
#define RPHOTO_DEFAULT_FORCED_GUIDE_RATIO _("0 (Free)|0.33 (Third)|1.618 (Golden Rule)")
#define RPHOTO_DEFAULT_CUSTOM_GUIDE_RATIO _("0.5 (Middle)")

#define RPHOTO_DEFAULT_COMMENT_ENCODING _("iso8859-15")

#ifdef _DEBUG
#define RPHOTO_NOT_IMPLEMENTED
#define RPHOTO_NOT_IMPLEMENTED_RET(x)
#else
#define RPHOTO_NOT_IMPLEMENTED  \
        wxMessageBox(_("This function is not yet implemented, \nor is being developped.\n"),_("Function not implemented."),wxICON_INFORMATION | wxOK ); \
        return;
#define RPHOTO_NOT_IMPLEMENTED_RET(x)  \
        wxMessageBox(_("This function is not yet implemented, \nor is being developped.\n"),_("Function not implemented."),wxICON_INFORMATION | wxOK ); \
        return(x);
#endif

class RPhotoApp : public wxApp
{
public:
	virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};

DECLARE_APP(RPhotoApp)

// Visual Leak Detector (in project definition)
// #include <vld.h>
