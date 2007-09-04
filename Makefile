include mk/init.mk
include mk/rules.mk
include mk/objective.mk

SUBDIRS = po src
entrydir = ${datadir}/applications
OBJECTIVE_DATA = \
	conspire.desktop:${entrydir}
