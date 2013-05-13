/*
 * menus.h
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


#ifndef YANK_MENUS_H
#define YANK_MENUS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#define UGLY_GNOME_UI_KEY "RecentFilename"

#include <gnome.h>
#include "app.h"
#include "callbacks.h"
#include "search.h"
#include "fileio.h"
#include "deleteop.h"
#include "recent.h"
#include "run_pipe.h"
#include "preferences.h"
#include "print.h"

void yank_install_menus_and_toolbar(GtkWidget *);
void yank_tree_item_context_menu(GtkWidget *);
void yank_todo_item_context_menu(GtkWidget *);
void yank_text_item_context_menu(GtkWidget *);

void yank_recent_files_setup(GtkWidget *);
void yank_recent_files_update(GtkWidget *, gchar const *);
void yank_recent_files_shrink(GtkWidget *, gint);

#endif  /* YANK_MENUS_H */

