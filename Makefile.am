SUBDIRS= po lib src resources doc
EXTRA_DIST = config.rpath m4/ChangeLog  config.rpath m4/ChangeLog  \
        README \
        COPYING \
        AUTHORS \
        ChangeLog \
        INSTALL \
        NEWS \
        TODO \
        build/vc/RPhoto.vcproj \
        build/vc/RPhoto.sln \
        build/vc/RPhoto.dsp \
        build/vc/RPhoto.dsw \
        build/vc/RPhoto.iss \
        build/vc/jpeg6bcrop.dsp \
        build/vc/wxVillaLib.dsp \
        build/vc/make_zip.bat \
        build/vc/upx_compress.bat \
        build/debian/rules \
        build/debian/RPhoto.1 \
        build/debian/conffiles \
        build/debian/changelog \
        build/debian/control \
        build/debian/copyright \
        build/debian/menu \
        build/debian/rphoto.desktop \
        build/debian/postinst \
        build/debian/postrm \
        build/debian/rphoto.xpm \
        po/fr.po \
        po/po_build.bat \
        po/po_update.bat

deb:
	ln -sf build/debian .
	chmod +x debian/rules
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean

ACLOCAL_AMFLAGS = -I m4
