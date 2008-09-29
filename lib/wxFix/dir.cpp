/////////////////////////////////////////////////////////////////////////////
// Name:        unix/dir.cpp
// Purpose:     wxDir implementation for Unix/POSIX systems
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id: dir.cpp 372 2008-09-27 18:40:03Z remi $
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "dir.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxMatchWild

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

#include "dir.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_DIR       ((wxFixedDirData *)m_data)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxFixedDirData
{
public:
    wxFixedDirData(const wxString& dirname);
    ~wxFixedDirData();

    bool IsOk() const { return m_dir != NULL; }

    void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
    void SetFlags(int flags) { m_flags = flags; }

    void Rewind() { rewinddir(m_dir); }
    bool Read(wxString *filename);

    const wxString& GetName() const { return m_dirname; }

private:
    DIR     *m_dir;

    wxString m_dirname;
    wxString m_filespec;

    int      m_flags;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

#if !defined( __VMS__ ) || ( __VMS_VER >= 70000000 )

wxFixedDirData::wxFixedDirData(const wxString& dirname)
         : m_dirname(dirname)
{
    m_dir = NULL;

    // throw away the trailing slashes
    size_t n = m_dirname.length();
    wxCHECK_RET( n, _T("empty dir name in wxDir") );

    while ( n > 0 && m_dirname[--n] == '/' )
        ;

    m_dirname.Truncate(n + 1);

    // do open the dir
    m_dir = opendir(m_dirname.fn_str());
}

wxFixedDirData::~wxFixedDirData()
{
    if ( m_dir )
    {
        if ( closedir(m_dir) != 0 )
        {
            wxLogLastError(_T("closedir"));
        }
    }
}

bool wxFixedDirData::Read(wxString *filename)
{
    dirent *de = (dirent *)NULL;    // just to silence compiler warnings
    bool matches = false;

    // speed up string concatenation in the loop a bit
    wxString path = m_dirname;
    path += _T('/');
    path.reserve(path.length() + 255);

    wxString de_d_name;
    wxString s1;
    wxString s2;

    while ( !matches )
    {
        de = readdir(m_dir);
        if ( !de )
            return false;

#if wxUSE_UNICODE
        de_d_name = wxConvFileName->cMB2WC( de->d_name );
#else
        de_d_name = de->d_name;
#endif

        // don't return "." and ".." unless asked for
        if ( de->d_name[0] == '.' &&
             ((de->d_name[1] == '.' && de->d_name[2] == '\0') ||
              (de->d_name[1] == '\0')) )
        {
            if ( !(m_flags & wxDIR_DOTDOT) )
                continue;

            // we found a valid match
            break;
        }

        // check the type now
        if ( !(m_flags & wxDIR_FILES) && !wxDir::Exists(path + de_d_name) )
        {
            // it's a file, but we don't want them
            continue;
        }
        else if ( !(m_flags & wxDIR_DIRS) && wxDir::Exists(path + de_d_name) )
        {
            // it's a dir, and we don't want it
            continue;
        }

        // finally, check the name
        if ( m_filespec.empty() )
        {
            matches = m_flags & wxDIR_HIDDEN ? true : de->d_name[0] != '.';
        }
        else
        {
            // test against the pattern
            s1 = m_filespec.Upper();
            s2 = de_d_name.Upper();
            matches = wxMatchWild(s1, s2,  !(m_flags & wxDIR_HIDDEN));
        }
    }

    *filename = de_d_name;

    return true;
}

#else // old VMS (TODO)

wxFixedDirData::wxFixedDirData(const wxString& WXUNUSED(dirname))
{
    wxFAIL_MSG(_T("not implemented"));
}

wxFixedDirData::~wxFixedDirData()
{
}

bool wxFixedDirData::Read(wxString * WXUNUSED(filename))
{
    return false;
}

#endif // not or new VMS/old VMS

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxFixedDir::Exists(const wxString& dir)
{
    return wxDirExists(dir);
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxFixedDir::wxFixedDir(const wxString& dirname)
{
    m_data = NULL;

    (void)Open(dirname);
}

bool wxFixedDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxFixedDirData(dirname);

    if ( !M_DIR->IsOk() )
    {
        wxLogSysError(_("Can not enumerate files in directory '%s'"),
                      dirname.c_str());

        delete M_DIR;
        m_data = NULL;

        return false;
    }

    return true;
}

bool wxFixedDir::IsOpened() const
{
    return m_data != NULL;
}

wxString wxFixedDir::GetName() const
{
    wxString name;
    if ( m_data )
    {
        name = M_DIR->GetName();
        if ( !name.empty() && (name.Last() == _T('/')) )
        {
            // chop off the last (back)slash
            name.Truncate(name.length() - 1);
        }
    }

    return name;
}

wxFixedDir::~wxFixedDir()
{
    delete M_DIR;
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxFixedDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    M_DIR->Rewind();

    M_DIR->SetFileSpec(filespec);
    M_DIR->SetFlags(flags);

    return GetNext(filename);
}

bool wxFixedDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, false, _T("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}

bool wxFixedDir::HasSubDirs(const wxString& spec)
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    if ( spec.empty() )
    {
        // faster check for presence of any subdirectory: normally each subdir
        // has a hard link to the parent directory and so, knowing that there
        // are at least "." and "..", we have a subdirectory if and only if
        // links number is > 2 - this is just a guess but it works fairly well
        // in practice
        //
        // note that we may guess wrongly in one direction only: i.e. we may
        // return true when there are no subdirectories but this is ok as the
        // caller will learn it soon enough when it calls GetFirst(wxDIR)
        // anyhow
        wxStructStat stBuf;
        if ( wxStat(M_DIR->GetName().c_str(), &stBuf) == 0 )
        {
            switch ( stBuf.st_nlink )
            {
                case 2:
                    // just "." and ".."
                    return false;

                case 0:
                case 1:
                    // weird filesystem, don't try to guess for it, use dumb
                    // method below
                    break;

                default:
                    // assume we have subdirs - may turn out to be wrong if we
                    // have other hard links to this directory but it's not
                    // that bad as explained above
                    return true;
            }
        }
    }

    // just try to find first directory
    wxString s;
    return GetFirst(&s, spec, wxDIR_DIRS | wxDIR_HIDDEN);
}

// dircmn.cpp


// ----------------------------------------------------------------------------
// wxDir::HasFiles() and HasSubDirs()
// ----------------------------------------------------------------------------

// dumb generic implementation

bool wxFixedDir::HasFiles(const wxString& spec)
{
    wxString s;
    return GetFirst(&s, spec, wxDIR_FILES | wxDIR_HIDDEN);
}

// we have a (much) faster version for Unix
#if (defined(__CYGWIN__) && defined(__WINDOWS__)) || !defined(__UNIX_LIKE__) || defined(__WXMAC__) || defined(__EMX__) || defined(__WINE__)

bool wxFixedDir::HasSubDirs(const wxString& spec)
{
    wxString s;
    return GetFirst(&s, spec, wxDIR_DIRS | wxDIR_HIDDEN);
}

#endif // !Unix

// ----------------------------------------------------------------------------
// wxDir::Traverse()
// ----------------------------------------------------------------------------

size_t wxFixedDir::Traverse(wxDirTraverser& sink,
                       const wxString& filespec,
                       int flags) const
{
    return 0;
}

// ----------------------------------------------------------------------------
// wxDir::GetAllFiles()
// ----------------------------------------------------------------------------



/* static */
size_t wxFixedDir::GetAllFiles(const wxString& dirname,
                          wxArrayString *files,
                          const wxString& filespec,
                          int flags)
{
    return 0;
}

