helpdir = $(bindir)
help_DATA = rphoto.htb

rphoto_htb_SRC = \
        help_en.html \
        help_fr.html \
        contents.hhc \
        rphoto.hhp \
        arrow_left.ico \
        arrow_right.ico \
        classic_crop.jpg \
        classic_main.jpg \
        classic_orient.jpg \
        combo_ratio.jpg \
        crop.ico \
        fullscrn.ico \
        inclinaison.ico \
        landorpo.ico \
        landscap.ico \
        light_green.ico \
        light_red.ico \
        main_comments.jpg \
        main_exif.jpg \
        main_explorer.jpg \
        main_move.jpg \
        main_scr.jpg \
        main_status.jpg \
        main_toolbar.jpg \
        main_resize.jpg \
        mode_crop.ico \
        mode_inclinaison.ico \
        open.ico \
        portrait.ico \
        prefs.jpg \
        reopen.ico \
        rotate_left.ico \
        rotate_right.ico \
        save.ico \
        undo.ico

EXTRA_DIST = $(rphoto_htb_SRC)

rphoto.htb: $(rphoto_htb_SRC)
	zip -9 -r -j rphoto.htb * -x rphoto.htb
