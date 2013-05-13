/*
 * app.h
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

#ifndef YANK_APP_H
#define YANK_APP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "menus.h"
#include "notedefs.h"
#include "preferences.h"
#include "time_label.h"
#include "e-yank-type.h"

/*GtkWidget   *yank_app_new(const gchar *, const gchar *);*/
GtkWidget   *yank_app_new(EYank *);

/*
 * utilities
 */

inline void show_todo_tab(void);
inline void show_text_tab(void);
void        clear_text_page(gboolean, gint *);
void        view_note_as(_notetype);
note_data   *fill_note_data_from_page(_notetype);
void        fill_page_from_note_data(note_data *);
GtkWindow   *yank_root_win(GtkWindow *);
void        show_tool_status(GnomeApp *, yank_preferences *);
void        default_gui_size(GtkWidget *, gpointer);
inline void set_status_text(const gchar *);
GtkWidget   *yank_main_app(GtkWidget *);
void        set_note_font_str(gchar *);
void        set_note_tree_font_str(gchar *);
void        set_todolist_font_str(gchar *);
void        set_default_font();
void        yank_app_exit(GtkWidget *);

#endif  /* YANK_APP_H */

