/////////////////////////////////////////////////////////////////////////////
// Name:        str64.cpp
// Purpose:     Base64 encoding
// Author:      Remi Peyronnet  <remi+wx@via.ecp.fr>  (based on base64.cpp, see below)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/defs.h"

#include "str64.h"
#include <wx/log.h>
#include <wx/utils.h>

int   decode_base64(unsigned char *dest, const char *src);
char *encode_base64(int size, const unsigned char *src);


/**
 * Encode some data in a base64 string
 *
 * @param src the data to encode in base64
 * @param size the length of the data
 *
 * @return the newly created wxStringBase64 storing data encoded in base64
 */
wxStringBase64 wxStringBase64::EncodeBase64(const unsigned char * src, int size)
{
	wxStringBase64 ret = wxString(wxT(""));
	char * base64;
	base64 = encode_base64(size, src);
	if (base64 != NULL)
	{
		ret = wxStringBase64::FromAscii(base64);
		free(base64);
	}
	return ret;
}

/**
 * Encode some data in a base64 string
 *
 * @param src the string to encode in base64
 */
wxStringBase64 wxStringBase64::EncodeBase64(const wxString & str)
{
	return wxStringBase64::EncodeBase64((const unsigned char *)(const char *)str.c_str(), str.Len() * sizeof(wxChar));
}

/**
 * Encode some data in a base64 string in a special encoding
 *
 * WARNING : if the original string contains a NUL, this may truncate the string
 *    this should be fixed in a future release of wxWidgets
 *
 * @param src the string to encode in base64
 * @param conv wxMBConv convertor to use
 */
wxStringBase64 wxStringBase64::EncodeBase64(const wxString & str, wxMBConv& conv)
{
	return wxStringBase64::EncodeBase64((const unsigned char *)(const char *)str.mb_str(conv));
}


/**
 * Decode the base64 string
 *
 * @param ret a reference to a pointer for the buffer (to be freed)
 * @return the number of decoded bytes.
 */
int wxStringBase64::DecodeBase64(unsigned char * & ret)
{
	int len;
	ret = (unsigned char *)malloc(Len()+1);
	if (ret == NULL) return 0;
	len = decode_base64(ret, ToAscii());
	wxASSERT((len >= 0) && (len <= (int)Len())); 
	ret[len] = 0;
	return len;
}

/**
 * Decode the base64 string into a wxString
 *
 * WARNING : if the original string contains a NUL, this may truncate the string
 *    this should be fixed in a future release of wxWidgets
 *
 * @return the decoded wxString.
 */
wxString wxStringBase64::DecodeBase64()
{
	wxString ret = wxT("");
	unsigned char * str;
	int len;
	len = DecodeBase64(str);
	if (str != NULL)
	{
		ret = wxString((wxChar *)str, len / sizeof(wxChar));
		free(str);
	}
	return ret;
}

/**
 * Decode the base64 string into a wxString from the given encoding
 *
 * WARNING : if the original string contains a NUL, this may truncate the string
 *    this should be fixed in a future release of wxWidgets
 *
 * @param conv wxMVConv convertor
 *
 * @return the decoded wxString.
 */
wxString wxStringBase64::DecodeBase64(wxMBConv& conv)
{
	wxString ret = wxT("");
	unsigned char * str;
	int len;
	len = DecodeBase64(str);
	if (str != NULL)
	{
		ret = wxString((const char *)str, conv, len);
		free(str);
	}
	return ret;
}


bool wxStringBase64::test()
{
	bool ret = true;
	int len;
	unsigned char cini[40]="Test ééàà ééàà ?", * cfin;
	wxString wini, wfin = wxT("");
	wxStringBase64 stst;
	wini = wxT("Test ééàà ééàà é ?");
	stst = wxStringBase64::EncodeBase64(cini, strlen((char *)cini));
	len = stst.DecodeBase64(cfin);
	cfin[len]=0;
	stst = wxStringBase64::EncodeBase64(wini);
	wfin = stst.DecodeBase64();
	if ((strcmp((char *)cini, (char *)cfin) != 0) || (wini != wfin))
	{
		wxLogDebug(wxT("unsigned char * : '") + wxString((char *)cini,wxConvLocal,strlen((char *)cini)) + wxT("' -> '") + wxString((char *)cfin,wxConvLocal,strlen((char *)cfin)) + wxT("'\n"));
		wxLogDebug(wxT("wxString: '") + wini + wxT("' -> '") + wfin + wxT("' (") + stst + wxT(") \n"));
		ret = false;
	}
	if (cfin != NULL) free(cfin);
	return ret;
}




/*
 * Copyright (C), 2000-2007 by the monit project group.
 * All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Private prototypes */
static int is_base64(char c);
static char encode(unsigned char u);
static unsigned char decode(char c);



/**
 *  Implementation of base64 encoding/decoding. 
 *
 *  @author Jan-Henrik Haukeland, <hauk@tildeslash.com>
 *
 *  @version \ $Id : base64.c,v 1.19 2007/07/25 12:54:31 hauk Exp $
 *
 *  @file
 */



/* ------------------------------------------------------------------ Public */



/**
 * Base64 encode and return size data in 'src'. The caller must free the
 * returned string.
 * @param size The size of the data in src
 * @param src The data to be base64 encode
 * @return encoded string otherwise NULL
 */
char *encode_base64(int size, const unsigned char *src) {

  int i;
  char *out, *p;

  if(!src)
    return NULL;

  if(!size)
    size= strlen((char *)src);
    
  out= (char *)malloc(sizeof(char) * (size*4/3+4+1));
  
  p= out;
    
  for(i=0; i<size; i+=3) {
      
    unsigned char b1=0, b2=0, b3=0, b4=0, b5=0, b6=0, b7=0;
      
    b1 = src[i];
      
    if(i+1<size)
      b2 = src[i+1];
      
    if(i+2<size)
      b3 = src[i+2];
      
    b4= b1>>2;
    b5= ((b1&0x3)<<4)|(b2>>4);
    b6= ((b2&0xf)<<2)|(b3>>6);
    b7= b3&0x3f;
      
    *p++= encode(b4);
    *p++= encode(b5);
      
    if(i+1<size) {
      *p++= encode(b6);
    } else {
      *p++= '=';
    }
      
    if(i+2<size) {
      *p++= encode(b7);
    } else {
      *p++= '=';
    }

  }
  
  *p++ = 0;

  return out;

}


/**
 * Decode the base64 encoded string 'src' into the memory pointed to by
 * 'dest'. The dest buffer is <b>not</b> NUL terminated.
 * @param dest Pointer to memory for holding the decoded string.
 * Must be large enough to recieve the decoded string.
 * @param src A base64 encoded string.
 * @return TRUE (the length of the decoded string) if decode
 * succeeded otherwise FALSE.
 */
int decode_base64(unsigned char *dest, const char *src) {

  if(src && *src) {
  
    unsigned char *p= dest;
    int k, l= strlen(src)+1;
    unsigned char *buf= (unsigned char *)malloc(sizeof(unsigned char) * l);

    
    /* Ignore non base64 chars as per the POSIX standard */
    for(k=0, l=0; src[k]; k++) {
      
      if(is_base64(src[k])) {
	
	buf[l++]= src[k];
	
      }
      
    } 
    
    for(k=0; k<l; k+=4) {
      
      char c1='A', c2='A', c3='A', c4='A';
      unsigned char b1=0, b2=0, b3=0, b4=0;
      
      c1= buf[k];
      
      if(k+1<l) {
	
	c2= buf[k+1];
	
      }
      
      if(k+2<l) {
	
	c3= buf[k+2];
	
      }
      
      if(k+3<l) {
	
	c4= buf[k+3];
	
      }
      
      b1= decode(c1);
      b2= decode(c2);
      b3= decode(c3);
      b4= decode(c4);
      
      *p++=((b1<<2)|(b2>>4) );
      
      if(c3 != '=') {
	
	*p++=(((b2&0xf)<<4)|(b3>>2) );
	
      }
      
      if(c4 != '=') {
	
	*p++=(((b3&0x3)<<6)|b4 );
	
      }
      
    }
    
    free(buf);
    
    return(p-dest);

  }

  return FALSE;
  
}


/* ----------------------------------------------------------------- Private */


/**
 * Base64 encode one byte
 */
static char encode(unsigned char u) {

  if(u < 26)  return 'A'+u;
  if(u < 52)  return 'a'+(u-26);
  if(u < 62)  return '0'+(u-52);
  if(u == 62) return '+';
  
  return '/';

}


/**
 * Decode a base64 character
 */
static unsigned char decode(char c) {
  
  if(c >= 'A' && c <= 'Z') return(c - 'A');
  if(c >= 'a' && c <= 'z') return(c - 'a' + 26);
  if(c >= '0' && c <= '9') return(c - '0' + 52);
  if(c == '+')             return 62;
  
  return 63;
  
}


/**
 * Return TRUE if 'c' is a valid base64 character, otherwise FALSE
 */
static int is_base64(char c) {

  if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
     (c >= '0' && c <= '9') || (c == '+')             ||
     (c == '/')             || (c == '=')) {
    
    return TRUE;
    
  }
  
  return FALSE;

}
