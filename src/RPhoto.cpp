/* ****************************************************************************
 * RPhoto.cpp                                                                 *
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

// wxMinimal : wxWindows project sample

#include "RatioPrecomp.h"
#include "RatioFrame.h"

class RPhotoApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(RPhotoApp)

bool RPhotoApp::OnInit()
{
	wxInitAllImageHandlers();

	wxFrame * frame = new RatioFrame(NULL, -1, RT_NAME);
	//wxFrame * frame = new wxFrame(NULL, -1, "RPhoto");
	SetTopWindow(frame);
	frame->Show(true);
	
	return true;
}
