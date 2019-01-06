SET PATH=D:\Util\Prog\GetText\bin;%PATH%

mkdir ..\bin\lang

REM fr
msgfmt -o fr.mo fr.po
mkdir ..\bin\lang\fr
copy fr.mo ..\bin\lang\fr\rphoto.mo

REM cs
msgfmt -o cs.mo cs.po
mkdir ..\bin\lang\cs
copy cs.mo ..\bin\lang\cs\rphoto.mo

REM ru
msgfmt -o ru.mo ru.po
mkdir ..\bin\lang\ru
copy ru.mo ..\bin\lang\ru\rphoto.mo

pause