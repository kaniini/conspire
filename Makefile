include buildsys.mk

SUBDIRS   = po pixmaps src
MAN	  = conspire.1

install-extra:
	for i in conspire.desktop; do \
		${INSTALL_STATUS}; \
		if ${MKDIR_P} ${DESTDIR}${datadir}/applications && ${INSTALL} -m 644 $$i ${DESTDIR}${datadir}/applications/$$i; then \
			${INSTALL_OK}; \
		else \
			${INSTALL_FAILED}; \
		fi \
	done

uninstall-extra:
	for i in conspire.desktop; do \
		if test -f ${DESTDIR}${datadir}/applications/$$i; then \
			if rm -f ${DESTDIR}${datadir}/applications/$$i; then \
				${DELETE_OK}; \
			else \
				${DELETE_FAILED}; \
			fi \
		fi \
	done

