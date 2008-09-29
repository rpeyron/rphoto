/* ****************************************************************************
 * RPhoto.cpp                                                                 *
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

// wxMinimal : wxWindows project sample

#include <wx/cmdline.h>

#include "RatioPrecomp.h"
#include "RatioFrame.h"

#include <wx/filesys.h>
#include <wx/fs_zip.h>

#include "../lib/wxJPEGExifHandler/imagejpg.h"

IMPLEMENT_APP(RPhotoApp)

bool RPhotoApp::OnInit()
{
	wxInitAllImageHandlers();
	wxImage::RemoveHandler(wxT("JPEG file"));
	wxImage::InsertHandler(new wxEJPGHandler());
	wxFileSystem::AddHandler(new wxZipFSHandler); 

	wxFrame * frame = new RatioFrame(NULL, -1, RPHOTO_NAME);
	SetTopWindow(frame);
	wxApp::OnInit();
	frame->Show(true);
	
	return true;
}

void RPhotoApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("%s %s (c) 2004 - Remi Peyronnet - http://www.via.ecp.fr/~remi"), RPHOTO_NAME, RPHOTO_VERSION));
    parser.AddParam(_("File to open"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

bool RPhotoApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    unsigned int i;
	wxString str;
    for (i=0; i < parser.GetParamCount(); i++)
    {
        ((RatioFrame *)GetTopWindow())->LoadFile(parser.GetParam(i));
    }
    return (TRUE);
}

/* Does not seem necessary now
// Strange Bug workaround (http://onegazhang.spaces.live.com/Blog/cns!D5E642BC862BA286!480.entry)
#include <wx/apptrait.h>
#if wxUSE_STACKWALKER && defined( __WXDEBUG__ )
// silly workaround for the link error with debug configuration:
// \src\common\appbase.cpp
wxString wxAppTraitsBase::GetAssertStackTrace()
{
   return wxT("");
}
#endif
// End 
*/


