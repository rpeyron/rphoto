REM call upx_compress.bat
cd ..\..\bin
mkdir ..\distrib

@REM zip -9 -r -j rphoto.htb ..\doc\use
@REM zip -9 ..\distrib\rphoto_bin.zip RPhoto.exe jpegtran.exe jhead.exe wrjpgcom.exe rphoto.htb lang\fr\RPhoto.mo lang\cs\RPhoto.mo lang\ru\RPhoto.mo
@REM cd ..\lib
@REM zip -9 -r ..\distrib\wxrecttracker.zip wxRectTracker
@REM zip -9 -r ..\distrib\wxmisc.zip wxmisc
@REM zip -9 -r ..\distrib\wxjpegexifhanfler.zip wxJPEGExifHandler


7z a -tzip -r rphoto.htb ..\doc\use\*.*
7z a -tzip -r  ..\distrib\rphoto_bin.zip RPhoto.exe jpegtran.exe jhead.exe wrjpgcom.exe rphoto.htb lang\fr\RPhoto.mo lang\cs\RPhoto.mo lang\ru\RPhoto.mo
7z a -tzip -r  ..\distrib\wxrecttracker.zip wxRectTracker
7z a -tzip -r  ..\distrib\wxmisc.zip wxmisc
7z a -tzip -r  ..\distrib\wxjpegexifhanfler.zip wxJPEGExifHandler