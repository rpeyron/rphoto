/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dir.h
// Purpose:     wxDir is a class for enumerating the files in a directory
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id: dir.h 333 2006-07-16 17:28:53Z remi $
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FIXEDDIR_H_
#define _WX_FIXEDDIR_H_

#include <wx/dir.h>

// ----------------------------------------------------------------------------
// wxDir: portable equivalent of {open/read/close}dir functions
// ----------------------------------------------------------------------------

class wxFixedDirData;

class  wxFixedDir : public wxDir
{
public:
    // test for existence of a directory with the given name
    static bool Exists(const wxString& dir);

    // ctors
    // -----

    // default, use Open()
    wxFixedDir() { m_data = NULL; }

    // opens the directory for enumeration, use IsOpened() to test success
    wxFixedDir(const wxString& dir);

    // dtor cleans up the associated ressources
    ~wxFixedDir();

    // open the directory for enumerating
    bool Open(const wxString& dir);

    // returns true if the directory was successfully opened
    bool IsOpened() const;

    // get the full name of the directory (without '/' at the end)
    wxString GetName() const;

    // file enumeration routines
    // -------------------------

    // start enumerating all files matching filespec (or all files if it is
    // empty) and flags, return true on success
    bool GetFirst(wxString *filename,
                  const wxString& filespec = wxEmptyString,
                  int flags = wxDIR_DEFAULT) const;

    // get next file in the enumeration started with GetFirst()
    bool GetNext(wxString *filename) const;

    // return true if this directory has any files in it
    bool HasFiles(const wxString& spec = wxEmptyString);

    // return true if this directory has any subdirectories
    bool HasSubDirs(const wxString& spec = wxEmptyString);

    // enumerate all files in this directory and its subdirectories
    //
    // return the number of files found
    size_t Traverse(wxDirTraverser& sink,
                    const wxString& filespec = wxEmptyString,
                    int flags = wxDIR_DEFAULT) const;

    // simplest version of Traverse(): get the names of all files under this
    // directory into filenames array, return the number of files
    static size_t GetAllFiles(const wxString& dirname,
                              wxArrayString *files,
                              const wxString& filespec = wxEmptyString,
                              int flags = wxDIR_DEFAULT);

private:
    friend class wxFixedDirData;

    wxFixedDirData *m_data;

    DECLARE_NO_COPY_CLASS(wxFixedDir)
};

#endif // _WX_DIR_H_

