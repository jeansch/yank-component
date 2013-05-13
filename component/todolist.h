/*
 * todolist.h
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

#ifndef YANK_TODOLIST_H
#define YANK_TODOLIST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include <errno.h>

#include "notedefs.h"
#include "daycalc.h"
#include "app.h"
#include "preferences.h"

void add_todo_item(note_data *);
void edit_todo_item(note_data *, note_data *);
void delete_todo_item(note_data *);
gint sort_todo_items_date(GtkCList *, gconstpointer, gconstpointer);
gint sort_todo_items_prio(GtkCList *, gconstpointer, gconstpointer);
gint sort_todo_items_complete(GtkCList *, gconstpointer, gconstpointer);
gint sort_todo_items_date_then_prio(GtkCList *, gconstpointer, gconstpointer);
gint sort_todo_items_prio_then_date(GtkCList *, gconstpointer, gconstpointer);
gint sort_todo_items_title(GtkCList *, gconstpointer, gconstpointer);
void sort_by_sorting_mode(gint);
void set_todolist_colors(void);
void set_todolist_row_colors(GtkCList *list, gint row);

#endif  /* YANK_TODOLIST_H */

