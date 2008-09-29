/////////////////////////////////////////////////////////////////////////////
// Name:        str64.h
// Purpose:     wxStringBase64
// Author:      Rmi Peyronnet
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STR64_H_
#define _WX_STR64_H_

#include "wx/defs.h"

#include "wx/string.h"

class wxStringBase64 : public wxString
{
public:
	wxStringBase64() : wxString() {};
	wxStringBase64(wxString str) : wxString(str) {};

	static wxStringBase64 EncodeBase64(const unsigned char * src, int size = 0);
	static wxStringBase64 EncodeBase64(const wxString & str);
	static wxStringBase64 EncodeBase64(const wxString & str, wxMBConv& conv);
	int DecodeBase64(unsigned char * & ret);
	wxString DecodeBase64();
	wxString DecodeBase64(wxMBConv& conv);

	static bool test();

};

#endif // _WX_STR64_H_

