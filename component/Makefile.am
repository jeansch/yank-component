#Process this file with automake to generate Makefile.in
#Copyright (C)2001 Jean Schurger

YANK_LIBS = @YANK_LIBS@
ORIG_CFLAGS = @CFLAGS@
YANK_CFLAGS = @YANK_CFLAGS@

# the OAF data

oafdir = $(datadir)/oaf
oaf_in_files = GNOME_Evolution_Yank.oaf.in 
oaf_DATA = $(oaf_in_files:.oaf.in=.oaf)

gladedir = $(datadir)/evolution/glade
glade_DATA = yank.glade

@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = $(oaf_in_files) $(glade_DATA)

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

bin_PROGRAMS = evolution-yank

evolution_yank_SOURCES =	\
	app.c \
	app.h \
	callbacks.c \
	callbacks.h \
	component-factory.c \
	component-factory.h \
	daycalc.c \
	daycalc.h \
	deleteop.c \
	deleteop.h \
	e-yank.c \
	fileio.c \
	e-yank.h \
	e-yank-type.h \
	fileio.h \
	main.c \
	menus.c \
	menus.h \
	notedefs.h \
	plugin.c \
	plugindefs.h \
	plugin.h \
	preferences.c \
	preferences.h \
	recent.c \
	recent.h \
	run_pipe.c \
	run_pipe.h \
	search.c \
	search.h \
	time_label.c \
	time_label.h \
	todolist.c \
	todolist.h \
	e-yank-factory.c \
	e-yank-factory.h \
	util.c \
	util.h \
	yank.h \
	yank-orig.c \
	yank-orig.h \
	box2.xpm \
	box.xpm \
	circle2.xpm \
	circle.xpm \
	gnome-note.xpm \
	text.xpm \
	privates.h

evolution_yank_LDADD =	\
	-leshell 		\
	$(YANK_LIBS)




