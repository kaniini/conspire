include buildsys.mk

SUBDIRS   = po pixmaps src
MAN	  = conspire.1

install-extra:
install-extra:
	for i in conspire.pc; do \
		${INSTALL_STATUS}; \
		if ${MKDIR_P} ${DESTDIR}${libdir}/pkgconfig && ${INSTALL} -m 644 $$i ${DESTDIR}${libdir}/pkgconfig/$$i; then \
			${INSTALL_OK}; \
		else \
			${INSTALL_FAILED}; \
		fi \
	done

	for i in conspire.desktop; do \
		${INSTALL_STATUS}; \
		if ${MKDIR_P} ${DESTDIR}${datadir}/applications && ${INSTALL} -m 644 $$i ${DESTDIR}${datadir}/applications/$$i; then \
			${INSTALL_OK}; \
		else \
			${INSTALL_FAILED}; \
		fi \
	done

uninstall-extra:
	for i in conspire.pc; do \
		if test -f ${DESTDIR}${libdir}/pkgconfig/$$i; then \
			if rm -f ${DESTDIR}${libdir}/pkgconfig/$$i; then \
				${DELETE_OK}; \
			else \
				${DELETE_FAILED}; \
			fi \
		fi \
	done

	for i in conspire.desktop; do \
		if test -f ${DESTDIR}${datadir}/applications/$$i; then \
			if rm -f ${DESTDIR}${datadir}/applications/$$i; then \
				${DELETE_OK}; \
			else \
				${DELETE_FAILED}; \
			fi \
		fi \
	done

