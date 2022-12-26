# Microsoft Developer Studio Project File - Name="RPhoto" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RPhoto - Win32 Debug wx26
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RPhoto.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RPhoto.mak" CFG="RPhoto - Win32 Debug wx26"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RPhoto - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RPhoto - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "RPhoto - Win32 Debug wx26" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RPhoto - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\lib" /I "./lib" /I "../../lib" /I "../../lib/jpeg-6b-crop" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX"RatioPrecomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlib.lib regex.lib png.lib jpeg.lib tiff.lib wxmsw.lib libexif_a.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"libc.lib" /libpath:"Release"

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RPhoto___Win32_Debug"
# PROP BASE Intermediate_Dir "RPhoto___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "jpeg-6b-crop" /I "../../lib" /I "../../lib/jpeg-6b-crop" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"RatioPrecomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlib.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib libexif_a.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /nodefaultlib /force

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug wx26"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RPhoto___Win32_Debug_wx26"
# PROP BASE Intermediate_Dir "RPhoto___Win32_Debug_wx26"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RPhoto___Win32_Debug_wx26"
# PROP Intermediate_Dir "RPhoto___Win32_Debug_wx26"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "jpeg-6b-crop" /I "../../lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"RatioPrecomp.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Program Files\wxWidgets-2.6.2\include\msvc" /I "jpeg-6b-crop" /I "C:\Program Files\wxWidgets-2.6.2\include" /I "../../lib" /I "../../lib/jpeg-6b-crop" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"RatioPrecomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib wxVillaLib.lib libexif_a.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"Debug"
# SUBTRACT BASE LINK32 /nodefaultlib /force
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlib.lib regexd.lib pngd.lib jpegd.lib tiffd.lib libexif_a.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"../../bin/RPhoto.exe" /pdbtype:sept /libpath:"C:\Program Files\wxWidgets-2.6.2\lib\vc_lib"
# SUBTRACT LINK32 /nodefaultlib /force

!ENDIF 

# Begin Target

# Name "RPhoto - Win32 Release"
# Name "RPhoto - Win32 Debug"
# Name "RPhoto - Win32 Debug wx26"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\lib\libexif-0.6.12\compat.cpp"
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioImageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioPrecomp.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RPhoto.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\lib\jpeg-6b-crop\jmorecfg.h"
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioFrame.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioImageBox.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RatioPrecomp.h
# End Source File
# Begin Source File

SOURCE=.\..\..\src\RPhoto.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\..\..\resources\arrow_left.bmp
# End Source File
# Begin Source File

SOURCE=..\..\resources\arrow_left.ICO
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\arrow_right.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\colours.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\copy.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\crop.bmp
# End Source File
# Begin Source File

SOURCE=..\..\resources\wx\msw\csquery.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\csquery.bmp
# End Source File
# Begin Source File

SOURCE=..\..\resources\wx\msw\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\cut.bmp
# End Source File
# Begin Source File

SOURCE=..\..\resources\wx\msw\disable.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\disable.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\explorer.BMP
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\explorer.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\FilesPhoto.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\fullscrn.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\help.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\landorpo.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\landscap.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\new.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\open.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\page.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\paste.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\portrait.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\preview.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\print.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\resources.rc
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\rotate_left.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\rotate_right.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\save.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\undo.bmp
# End Source File
# Begin Source File

SOURCE=.\..\..\resources\wx\msw\watch1.cur
# End Source File
# Begin Source File

SOURCE=.\..\..\src\wx\msw\watch1.cur
# End Source File
# Begin Source File

SOURCE=..\..\resources\wxbitmap.bmp
# End Source File
# End Group
# Begin Group "wxFix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\..\lib\wxFix\dirctrlg.cpp

!IF  "$(CFG)" == "RPhoto - Win32 Release"

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug"

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug wx26"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxFix\dirctrlg.h

!IF  "$(CFG)" == "RPhoto - Win32 Release"

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug"

!ELSEIF  "$(CFG)" == "RPhoto - Win32 Debug wx26"

!ENDIF 

# End Source File
# End Group
# Begin Group "wxRectTracker"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\LineTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\LineTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\RectTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\RectTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\RectTrackerRatio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxRectTracker\RectTrackerRatio.h
# End Source File
# End Group
# Begin Group "wxMisc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\wxmisc\ConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxmisc\ConfigDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxmisc\ListCtrlResize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxmisc\ListCtrlResize.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxmisc\str64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxmisc\str64.h
# End Source File
# End Group
# Begin Group "wxExifJPEG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\wxJPEGExifHandler\imagejpg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxJPEGExifHandler\imagejpg.h
# End Source File
# End Group
# Begin Group "wxVillaLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageBox.h
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageFilters.h
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageProc.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\ImageProc.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxVillaLib\ThumbnailFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\wxVillaLib\ThumbnailFactory.h
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\Thumbnails.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\Thumbnails.h
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\utils.h
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\VerticalToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\lib\wxVillaLib\VerticalToolbar.h
# End Source File
# End Group
# End Target
# End Project
