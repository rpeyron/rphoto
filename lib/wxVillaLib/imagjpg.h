#ifndef _WX_IMAGJPG_H_
#define _WX_IMAGJPG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "imagjpg.h"
#endif

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxJPGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG
class WXDLLEXPORT wxJPGHandler: public wxImageHandler
{
public:
    inline wxJPGHandler()
    {
        m_name = wxT("JPEG file");
        m_extension = wxT("jpg");
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT("image/jpeg");
		m_maxWidth = -1;
		m_maxHeight = -1;
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
    virtual bool DoCanRead( wxInputStream& stream );
#endif
    
	int m_maxWidth;
	int m_maxHeight;

private:
    DECLARE_DYNAMIC_CLASS(wxJPGHandler)
};
#endif



#endif
  // _WX_IMAGJPG_H_

