REM Update from code
SET PATH=D:\Util\Prog\GetText\bin;%PATH%
xgettext.exe --c++ --keyword=_ --from-code=ISO-8859-1  ../src/*.cpp ../src/*.h
REM Merge files
msgmerge --update fr.po messages.po

