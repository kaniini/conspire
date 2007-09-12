include mk/init.mk
include mk/rules.mk
include mk/objective.mk

SUBDIRS   = po src
entrydir  = ${datadir}/applications
pixmapdir = ${datadir}/pixmaps
OBJECTIVE_DATA = \
	conspire.desktop:${entrydir} \
	src/pixmaps/conspire.png:${pixmapdir}
