/////////////////////////////////////////////////////////////////////////////
// Name:        PipeExecute.cpp
// Purpose:     Pipe execute
// Author:      Alex Thuering
// Created:     23.08.2004
// RCS-ID:      $Id: PipeExecute.cpp,v 1.3 2007/11/14 21:40:39 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "PipeExecute.h"
#include "utils.h"
#include <wx/process.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/log.h>

#define BUF_SIZE 4096
#define MAX_PROCESSES 5

#ifndef __WXMSW__

#include <unistd.h>
#include <fcntl.h>
class wxOutputStreamUnblocker: public wxFileOutputStream
{
  public:
	void SetBlock(bool block = true)
	{
	  int flags = fcntl(m_file->fd(), F_GETFL) | O_NONBLOCK;
	  if (block)
		flags -= O_NONBLOCK;
	  fcntl(m_file->fd(), F_SETFL, flags);
	}
};

#else

#include <windows.h>
class wxPipeOutputStream: public wxOutputStream
{
  public:
    wxPipeOutputStream(HANDLE hOutput);
    virtual ~wxPipeOutputStream();

  protected:
    size_t OnSysWrite(const void *buffer, size_t len);

  protected:
    HANDLE m_hOutput;

    DECLARE_NO_COPY_CLASS(wxPipeOutputStream)
};
class wxOutputStreamUnblocker: public wxPipeOutputStream
{
  public:
    wxOutputStreamUnblocker(HANDLE hOutput) : wxPipeOutputStream(hOutput) {}
    
	void SetBlock(bool block = true)
	{
      DWORD mode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
	  if (block)
		mode -= PIPE_NOWAIT;
      if (!SetNamedPipeHandleState(m_hOutput, &mode, NULL, NULL))
       wxLogLastError(_T("SetNamedPipeHandleState(PIPE_NOWAIT)"));
	}
};

#endif

void wxUnblockOutputStream(wxFileOutputStream& stream)
{
  ((wxOutputStreamUnblocker&)stream).SetBlock(false);
}

class PipeProcess: public wxProcess
{
  public:
    PipeProcess(wxPipeExecute* parent, PipeProcess* nextProc = NULL,
      wxFileInputStream* input = NULL, wxFileOutputStream* output = NULL):
        m_parent(parent), m_nextProc(nextProc), m_input(input), m_output(output)
    {
      m_isRunning = true;
      m_exitCode = -1;
	  m_lastRead = m_lastWrite = 0;
      Redirect();
    }
    
	/**
	  * Reads data from given stream and prints out (see ProcessOutput)
	  */
    void DoGetFromStream(wxInputStream& in)
    {
	  wxTextInputStream tis(in);
      while (in.CanRead())
	  {
		wxYieldIfNeeded();
        wxString line;
        while (in.CanRead()) //tis.ReadLine()
        {
          wxChar c = tis.GetChar();
          if (c == wxT('\n') || c == wxT('\r'))
            break;
          line += c;
        }
        if (line.length())
          m_parent->ProcessOutput(line);
	  }
    }
	
	/**
	  * Reads from given stream and writes into process input stream
	  */
	bool DoPutToStream(wxInputStream& in)
	{
	  wxUnblockOutputStream((wxFileOutputStream&)*GetOutputStream());
	  if (m_lastWrite == m_lastRead)
	  {
		m_lastRead = in.Read(m_buffer, sizeof(m_buffer)).LastRead();
		wxLogNull log;
		m_lastWrite = GetOutputStream()->Write(m_buffer, m_lastRead).LastWrite();
	  }
	  else
	  {
		wxLogNull log;
		m_lastWrite += GetOutputStream()->Write(m_buffer+m_lastWrite,
		  m_lastRead-m_lastWrite).LastWrite();
	  }
	  return m_lastWrite == m_lastRead;
	}
	
	/**
	  * Is some data lefts to write after last call of DoPutToStream()?
	  */
	bool HasDataToPut()
	{
	  return m_lastWrite != m_lastRead;
	}
	
	/**
	  * Process of input/output data
	  */
    bool HasInput()
	{
	  bool hasInput = false;
	  
	  // Read from input stream and write to process input
	  if (IsRunning() && m_input && (m_input->CanRead() || HasDataToPut()))
	  {
        if (DoPutToStream(*m_input) && m_input->Eof())
		  CloseOutput();
		else
		  hasInput = true;
	  }
      
	  // read from process input
	  if (IsInputAvailable() || (m_nextProc && m_nextProc->HasDataToPut()))
	  {
	    if (m_nextProc) // and write to next process
		{
		  if (m_nextProc->IsRunning())
			m_nextProc->DoPutToStream(*GetInputStream());
		  else
			return false;
		}
	    else if (m_output) // and write in output fstream
	    {
	      while (GetInputStream()->CanRead())
	      {
			wxYield();
	        char buffer[BUF_SIZE];
	        int cnt = GetInputStream()->Read(buffer, sizeof(buffer)).LastRead();
            m_output->Write(buffer, cnt);
          }
	    }
	    else // and print out
	      DoGetFromStream(*GetInputStream());
        hasInput = true;
      }
      
	  // read from process error stream and print out
      if (IsErrorAvailable())
	  {
	    DoGetFromStream(*GetErrorStream());
        hasInput = true;
      }
      
	  // close input from next process if this process finished
      if (m_nextProc && !hasInput && !IsRunning())
      {
        m_nextProc->CloseOutput();
        m_nextProc = NULL;
      }
      
      return hasInput;
	}
	
	void OnTerminate(int pid, int status)
	{
	  m_exitCode = status;
	  m_isRunning = false;
	}
	
	bool IsRunning() const
	{
	  return m_isRunning;
	}
	
	int GetExitCode() const
	{
	  return m_exitCode;
	}
    
  protected:
    wxPipeExecute* m_parent;
    bool m_isRunning;
    int  m_exitCode;
    PipeProcess* m_nextProc;
    wxFileInputStream*  m_input;
    wxFileOutputStream* m_output;
	int  m_lastRead, m_lastWrite;
	char m_buffer[BUF_SIZE];
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Execute /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool wxPipeExecute::Execute(wxString command,
  wxString inputFile, wxString outputFile)
{
  wxArrayString cmds;
  while (cmds.Count()<MAX_PROCESSES)
  {
    int pos = command.find(wxT('|'));
    if (pos<0)
    {
      cmds.Add(command.Strip(wxString::both));
      break;
    }
    cmds.Add(command.Mid(0,pos).Strip(wxString::both));
    command.Remove(0,pos+1);
  }
  int cnt = cmds.Count();
  
  wxFileInputStream* input = NULL;;
  wxFileOutputStream* output = NULL;
  if (inputFile.length())
    input = new wxFileInputStream(inputFile);
  if (input && !input->IsOk())
    return false;
  if (outputFile.length())
    output = new wxFileOutputStream(outputFile);
  if (output && !output->IsOk())
    return false;
  
  PipeProcess* proc[MAX_PROCESSES];
  long pid[MAX_PROCESSES];
  PipeProcess* prevProc = NULL;
  for (int i=cnt-1; i>=0; i--)
  {
    proc[i] = new PipeProcess(this,
     prevProc, i == 0 ? input : NULL, i == cnt-1 ? output : NULL);
    prevProc = proc[i];
  }
  for (int i=0; i<cnt; i++)
  {
    wxString cmd = cmds[i];
    pid[i] = wxExecute(cmd, wxEXEC_ASYNC, proc[i]);
    if (pid[i]<=0)
      return false;
  }
  bool hasInput = true;
  while (proc[cnt-1]->IsRunning() || hasInput)
  {
    wxYield();
    if (IsCanceled())
    {
	  for (int i=0; i<cnt; i++)
		wxProcess::Kill(pid[i], wxSIGTERM);
      break;
    }
	hasInput = false;
	for (int i=0; i<cnt; i++)
	  hasInput = hasInput || proc[i]->HasInput();
	if (!hasInput)
	wxMilliSleep(50);
  }
  bool res = proc[cnt-1]->GetExitCode() == 0;
  for (int i=0; i<cnt; i++)
	if (IsCanceled())
	  proc[i]->Detach();
	else
	  delete proc[i];
  if (input)
	delete input;
  if (output)
	delete output;
  
  return res;
}
