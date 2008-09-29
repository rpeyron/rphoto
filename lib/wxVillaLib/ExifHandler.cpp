/////////////////////////////////////////////////////////////////////////////
// Name:        ExifHandler.cpp
// Purpose:     ExifHandler class
// Author:      Alex Thuering
// Created:		30.12.2007
// RCS-ID:      $Id: ExifHandler.cpp,v 1.1 2007/12/30 22:45:02 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "ExifHandler.h"
#include <libexif/exif-loader.h>

int ExifHandler::getOrient(wxString filename) {
	ExifData* exifData = exif_data_new_from_file(filename.mb_str());
	if (!exifData)
		return -1;
	if (!exif_content_get_entry(exifData->ifd[EXIF_IFD_EXIF], EXIF_TAG_EXIF_VERSION))
		return -1;
	gint orient = -1;
	ExifEntry* entry = exif_content_get_entry(exifData->ifd[EXIF_IFD_0], EXIF_TAG_ORIENTATION);
	if (entry) {
		ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
		orient = exif_get_short(entry->data, byteOrder);
	}
	exif_data_unref(exifData);
	return (int) orient;
}
