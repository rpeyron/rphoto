SUBDIRS= po lib src resources doc build
EXTRA_DIST = config.rpath  config.rpath m4/ChangeLog  config.rpath m4/ChangeLog  \
        README \
        COPYING \
        AUTHORS \
        ChangeLog \
        INSTALL \
        NEWS \
        TODO \
        TODO \
	build/win32/upx_compress.bat \
	build/win32/RPhoto.iss \
	build/win32/make_zip.bat \
	build/win32/make_wxRectTracker_zip.bat \
	build/dev-cpp/.cvsignore \
	build/dev-cpp/RPhoto_private.h \
	build/dev-cpp/RPhoto_private.res \
	build/dev-cpp/RPhoto.layout \
	build/dev-cpp/RPhoto_private.rc \
	build/dev-cpp/classfolders.dcf \
	build/dev-cpp/RPhoto.ico \
	build/dev-cpp/Makefile.win \
	build/dev-cpp/RPhoto.dev \
	build/vc/RPhoto.dsp \
	build/vc/RPhoto.dsw \
	build/vc/.cvsignore \
	build/vc/upx_compress.bat \
	build/vc/RPhoto.vcproj \
	build/vc/RPhoto.vcxproj \
	build/vc/RPhoto.iss \
	build/vc/RPhoto.sln \
	build/vc/jpeg6bcrop.dsp \
	build/vc/make_zip.bat \
	build/vc/make_wxRectTracker_zip.bat \
	build/vc/wxVillaLib.dsp \
	build/vc2013/RPhoto.vcxproj \
	build/vc2013/RPhoto.sln \
	build/vc2013/RPhoto.vcxproj.filters \
    build/vc2017/RPhoto.vcxproj \
	build/vc2017/RPhoto.sln \
	build/vc2017/RPhoto.vcxproj.filters \
	build/vc2017/wxWidgets.props \
	build/vc2017/wxWidgets30.props \
	    build/snap/snapcraft.xml \
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
        po/de.po \
        po/cs.po \
        po/ru.po \
        po/po_build.bat \
        po/po_update.bat

old-deb:  dist
	ln -sf build/debian .
	chmod +x debian/rules
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean
    
DEBSOURCEPKG=$(PACKAGE)_$(VERSION).orig.tar.gz
DEBSOURCEDIR=$(PACKAGE)-$(VERSION)
deb: dist
	ln -sf build/debian .
	scite debian/control
	scite debian/changelog
	@echo "Building Debian package..."; \
	if test ! -d debian; then \
		echo "The files needed for building a Debian package are not" \
		"included by default in the distribution. To build a package, check" \
		"out the project from source control."; \
		exit 1; \
	fi; \
	mv $(PACKAGE)-$(VERSION).tar.gz $(DEBSOURCEPKG) && \
	tar --extract --gunzip --file $(DEBSOURCEPKG) && \
	mkdir -p $(DEBSOURCEDIR)/debian && \
	cp \
		debian/* \
		$(DEBSOURCEDIR)/debian && \
	cd $(DEBSOURCEDIR) && \
	debuild --no-tgz-check -rfakeroot -G -D ; \
	cd .. ; \
	#rm --force $(DEBSOURCEPKG); \
	rm --recursive --force $(DEBSOURCEDIR)
	aptly -force-replace=true repo add local $(PACKAGE)_$(VERSION)*.dsc $(PACKAGE)_$(VERSION)*.deb
	@echo "To publish :"
	@echo aptly -gpg-key="remi+debian@via.ecp.fr" -force-overwrite publish update stable filesystem:web:debian

ppa:
	scite debian/control
	scite debian/changelog
	debuild -S -sa
	dput ppa:rpeyron/ppa ../*.changes

snap:
	echo sudo snap install --classic snapcraft
	snap run snapcraft


ACLOCAL_AMFLAGS = -I m4
