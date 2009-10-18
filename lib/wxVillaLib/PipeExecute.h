/////////////////////////////////////////////////////////////////////////////
// Name:        PipeExecute.h
// Purpose:     Pipe execute
// Author:      Alex Thuering
// Created:     23.08.2004
// RCS-ID:      $Id: PipeExecute.h 365 2008-06-21 23:15:43Z remi $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifndef PIPE_EXECUTE_H
#define PIPE_EXECUTE_H

#include <wx/string.h>

/** Executes pipe commands and optional read input from file
  * and write output to other file.
  * It also allow you to process output messages f.e. for logging.
  */
class wxPipeExecute
{
  public:
    virtual ~wxPipeExecute() {}
	bool Execute(wxString command, wxString inputFile = wxEmptyString,
	  wxString outputFile = wxEmptyString);
	virtual void ProcessOutput(wxString line) = 0;
	virtual bool IsCanceled() = 0;
};

#endif // PIPE_EXECUTE_H
