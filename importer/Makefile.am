#Process this file with automake to generate Makefile.in
#Copyright (C)2001 Jean Schurger

YANK_LIBS = @YANK_LIBS@
ORIG_CFLAGS = @CFLAGS@
YANK_CFLAGS = @YANK_CFLAGS@

# the OAF data

oafdir = $(datadir)/oaf
oaf_in_files = GNOME_Evolution_Yank_Importer.oaf.in 
oaf_DATA = $(oaf_in_files:.oaf.in=.oaf)

@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = $(oaf_in_files)

CFLAGS	= 			-g		\
	$(ORIG_CFLAGS) \
	-I/usr/include/gal \
	-DLOCALE_DIRECTORY=\"$(localedir)\"	\
	-DG_LOG_DOMAIN=\"evolution-yank\" 	\
	-DEVOLUTION_DATADIR=\"$(datadir)\"	\
	-DEVOLUTION_GLADEDIR=\"$(datadir)/evolution/glade\"	\
	$(EVOLUTION_CFLAGS) 			\
	$(YANK_CFLAGS)

# The main executable.

bin_PROGRAMS = evolution-yank-importer

evolution_yank_importer_SOURCES =	\
	evolution-yank-importer.c

evolution_yank_importer_LDADD =	\
	-leshell 		\
	-levolution-importer
	$(YANK_LIBS)
