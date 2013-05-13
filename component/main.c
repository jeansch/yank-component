/*
 * main.c: Main file for the Yank
 *
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Authors: Jean Schurger
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkrgb.h>

#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomeui/gnome-init.h>

#include <bonobo/bonobo-main.h>
#include <liboaf/liboaf.h>

#include <libgnomevfs/gnome-vfs.h>
#include <glade/glade.h>

#include "component-factory.h"

int
main (int argc,
      char **argv)
{
	CORBA_ORB orb;

	bindtextdomain (PACKAGE, LOCALE_DIRECTORY);
	textdomain (PACKAGE);

	gnome_init_with_popt_table ("Evolution Yank", VERSION,
				    argc, argv, oaf_popt_options, 0, NULL);
	orb = oaf_init (argc, argv);

	gdk_rgb_init ();
	if (bonobo_init (orb, CORBA_OBJECT_NIL, CORBA_OBJECT_NIL) == FALSE) {
		g_error (_("Yank component could not initialize Bonobo.\n"));
		exit (1);
	}

	glade_gnome_init ();
	gnome_vfs_init ();

	/* Start our component */
	component_factory_init ();

	bonobo_main ();

	return 0;
}
