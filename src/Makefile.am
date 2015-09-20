bin_PROGRAMS=RPhoto
RPhoto_LDADD=  ../lib/wxVillaLib/libwxvilla.a ../lib/wxFix/libwxFix.a  ../lib/wxmisc/libwxmisc.a ../lib/wxRectTracker/libwxrecttracker.a ../lib/wxJPEGExifHandler/libwxjpegexifhandler.a
RPhoto_SOURCES= \
	RatioPrecomp.cpp \
	RatioPrecomp.h \
	RatioFrame.cpp \
	wrjpgcom.inc \
	RatioFrame.h \
	RPhoto.cpp \
	RPhoto.h \
	RatioImageBox.cpp \
	RatioImageBox.h
	
INCLUDES=-I../lib/

# Pre-compiled headers
# g++ $(wx-config --cppflags) -o RatioPrecomp.h.gch -c RatioPrecomp.cpp
