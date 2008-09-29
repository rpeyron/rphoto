call upx_compress.bat
cd ..\..\bin
mkdir ..\distrib
zip -9 -r -j rphoto.htb ..\doc\use
zip -9 ..\distrib\rphoto_bin.zip RPhoto.exe jpegtran.exe wrjpgcom.exe rphoto.htb fr\RPhoto.mo
