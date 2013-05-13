/*
 * preferences.h
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

#ifndef YANK_PREFERENCES_H
#define YANK_PREFERENCES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>

#include "notedefs.h"
#include "app.h"
#include "todolist.h"
#include "plugin.h"

extern GtkWidget* application;

void cb_pref_dialog(GtkWidget *, gpointer);
void cb_pref_apply(GnomePropertyBox *, gint, gpointer);

GtkWidget* pref_files_p(void);
GtkWidget* pref_notes_p(void);
GtkWidget* pref_todolist_p(void);
GtkWidget* pref_gui_p(void);

void                     load_preferences(void);
void                     save_preferences(yank_preferences *);
void                     cb_save_preferences(GtkWidget *, gpointer);
inline yank_preferences* get_preferences(void);
void                     pref_changed(GtkWidget *, GnomePropertyBox *);
void                     pref_color_changed(GnomeColorPicker *, guint, guint,
                                            guint, guint, GnomePropertyBox *);
void                     pref_font_changed(GtkWidget *, gchar *,
                                           GnomePropertyBox *);

void select_fname(GtkWidget *, gpointer);
void select_fname_ok(GtkWidget *, gpointer);
void select_fname_cancel(GtkWidget *, gpointer);
void cb_yank_def_file(GtkWidget *, gpointer);
void cb_yank_def_cmd(GtkWidget *, gpointer);

gchar* get_color_text(GnomeColorPicker *);
void   parse_color_text(gchar *, guint *, guint *, guint *);

GList* recent_files_get_list(void);
gchar* recent_files_update_list(gchar const *);
gchar* recent_files_list_shrink(void);
void   recent_files_write_config(void);

#endif  /* YANK_PREFERENCES_H */
