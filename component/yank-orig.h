/*
 * yank.h
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Huﬂmann <m.hussmann@home.ins.de>
 *
 * This program is free software; you  can redistribute it and/or modify it
 * under the terms of  the GNU General  Public License as published  by the
 * Free Software Foundation;  either version 2 of  the License, or (at your
 * option) any later version.
 *
 * This program  is  distributed in the  hope  that it will  be useful, but
 * WITHOUT  ANY   WARRANTY;  without   even   the  implied    warranty   of
 * MERCHANTABILITY or    FITNESS FOR A PARTICULAR   PURPOSE.   See  the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write to the  Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef YANK_YANK_H
#define YANK_YANK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include "app.h"
#include "preferences.h"
#include "todolist.h"

static char *geometry = 0;
static char *file = 0;


static void session_die(GnomeClient* client, gpointer client_data);

static gint save_session(GnomeClient *client, gint phase,
                         GnomeSaveStyle save_style,
                         gint is_shutdown, GnomeInteractStyle interact_style,
                         gint is_fast, gpointer client_data);



struct poptOption options[] = {
    {
        "geometry",
        'g',
        POPT_ARG_STRING,
        &geometry,
        0,
        N_("Specify the geometry of the main window"),
        N_("GEOMETRY")
    },
    {
        "file",
        'f',
        POPT_ARG_STRING,
        &file,
        0,
        N_("Load notes from this file"),
        N_("FILE")
    },
    { NULL, '\0', 0, NULL, 0, NULL, NULL }
};


#endif  /* YANK_YANK_H */

