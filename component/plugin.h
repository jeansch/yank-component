/*
 * plugin.h
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

#ifndef YANK_PLUGIN_H
#define YANK_PLUGIN_H
	
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include "plugindefs.h"
#include "app.h"

#include <gnome.h>
#include <dirent.h>
#include <stdlib.h>

void cb_plugin_load(GtkWidget *, gpointer);
void cb_plugin_unload(GtkWidget *, gpointer);
void cb_plugin_refresh(GtkWidget *, gpointer);
void cb_plugin_toggle_preload(GtkWidget *, gpointer);

void       plugin_create_list(void);
void       plugin_read_dir(gchar *);
gint       plugin_fetch_info(plugin_list_entry **, gchar *);
GtkWidget* plugin_select_dialog(void);
void       plugin_fill_clist(GtkCList *, GList *);
void       plugin_create_preload_list(gint *, gchar ***);
void       plugin_preload_list(gint, gchar **);
gchar*     list_entry_by_pathname(GList *, gchar *);
gint       clist_row_by_pathname(GtkCList *clist, gchar *path);

#endif  /* YANK_PLUGIN_H */
