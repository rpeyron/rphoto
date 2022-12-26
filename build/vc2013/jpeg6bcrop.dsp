# Microsoft Developer Studio Project File - Name="jpeg6bcrop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpeg6bcrop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeg6bcrop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeg6bcrop.mak" CFG="jpeg6bcrop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeg6bcrop - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg6bcrop - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpeg6bcrop - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\jpeg6bcrop.lib"

!ELSEIF  "$(CFG)" == "jpeg6bcrop - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\jpeg6bcrop.lib"

!ENDIF 

# Begin Target

# Name "jpeg6bcrop - Win32 Release"
# Name "jpeg6bcrop - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcapimin.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcapistd.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jccoefct.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jccolor.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jchuff.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcinit.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcmainct.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcmarker.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcparam.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcphuff.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcprepct.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jcsample.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jctrans.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jdapimin.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jdapistd.c
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\jpeg-6b-crop\jdatadst.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
