
MANPAGES = \
	SceneViewer.1 \
	iv2toiv1.1 \
	ivcat.1 \
	ivdowngrade.1 \
	ivfix.1 \
	ivinfo.1 \
	ivview.1 \
	inventor.1

install:
	@mkdir -p $(IVROOT)/usr/man/man1
	@for i in $(MANPAGES) ; do \
		/bin/cp -f $$i $(IVROOT)/usr/man/man1 ; \
	done

clean clobber:
	@for i in $(MANPAGES) ; do \
		/bin/rm -f $(IVROOT)/usr/man/man1/$$i ; \
	done

