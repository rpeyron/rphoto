/////////////////////////////////////////////////////////////////////////////
// Name:        imagejpg.h
// Purpose:     wxImage JPEG handler
// Author:      Rémi Peyronnet, based on imagjpeg by Vaclav Slavik
// RCS-ID:      $Id: imagejpg.h 400 2008-10-11 15:29:21Z remi $
// Copyright:   (c)  Vaclav Slavik, modified by Rémi Peyronnet
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGEJPG_H_
#define _WX_IMAGEJPG_H_

#include "wx/defs.h"

//-----------------------------------------------------------------------------
// wxJPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG

#include "wx/image.h"

class WXDLLEXPORT wxEJPGHandler: public wxImageHandler
{
public:
	// 
	class WXDLLEXPORT wxImageOpt
	{
	public:
		wxImageOpt();
		wxImageOpt(const wxImage & img);

		void getImageOptions(const wxImage & img);
		void setImageOptions(wxImage & img, bool overwrite = false);

	public:
		wxArrayString idsToSave;
	protected:
		wxArrayString ids;
		wxArrayString values;
	};

public:
    wxEJPGHandler() : wxImageHandler()
    {
        m_name = wxT("Extended JPEG file");
        m_extension = wxT("jpg");
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT("image/jpeg");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
    virtual bool DoCanRead( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxEJPGHandler)
};

#endif // wxUSE_LIBJPEG

#endif // _WX_IMAGEJPG_H_

