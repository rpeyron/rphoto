SUBDIRS=lib src resources
EXTRA_DIST = \
        README \
        COPYING \
        AUTHORS \
        ChangeLog \
        INSTALL \
        NEWS \
        TODO \
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
        build/debian/postinst \
        build/debian/postrm \
        build/debian/ratiophoto.xpm \
        locales/fr.po \
        locales/po_build.bat \
        locales/po_update.bat \
        doc/HOWTO-Package-Debian.txt

debian:
	ln -sf build/debian .
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean
