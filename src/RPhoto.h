/* ****************************************************************************
 * RPhoto.h                                                                   *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * RPhoto : image tool for a quick and efficient use with digital photos      *
 * Copyright (C) 2004 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>               *
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

#define RT_NAME wxT("RPhoto")
#define RT_VERSION wxT("v0.2.0")

#define RT_DEFAULT_IMAGE wxT("cnedra.jpg")

#define RPHOTO_FILTERIMGLIST_ALL wxString("Image Files|*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;*.ico;*.ani;*.pcx;*.pnm;*.xpm")
#define RPHOTO_FILTERIMGLIST wxString("BMP Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png|TIFF Files (*.tif; *.tiff)|*.tif;*.tiff")
#define RPHOTO_FILTERIMGLIST_WORLD	("All files (*.*)|*.*|" + RPHOTO_FILTERIMGLIST_ALL + "|" + RPHOTO_FILTERIMGLIST)

#define RPHOTO_TEMP_PREFIX wxT("rphoto_")

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

