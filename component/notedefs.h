/*
 * notedefs.h
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


#ifndef YANK_NOTEDEFS_H
#define YANK_NOTEDEFS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <time.h>

typedef enum
{
    None,
    TextNote,
    CheckNote,
    TodoNote
} _notetype;

typedef enum
{
    NoMode,
    Add,
    Edit,
    Delete
} _editmode;

struct _note_data
{
    _notetype notetype;
    gchar     *id;
    gchar     *title;
    time_t    expire;    /* if > -1, delete at given date */
    time_t    created;   /* creation time */
    time_t    changed;   /* modification time */
    glong     changes;   /* modification counter */
    gchar     *text;
    gint      todo;      /* 1 == done */
    time_t    deadline;
    gint      prio;
    gint      complete;
};

typedef struct _note_data note_data;

struct _yank_preferences
{
    gint  auto_save_on_exit;
    gint  use_backup_file;
    gint  auto_save_minutes;
    gint  auto_save_preferences;
    gint  save_compression;
    gint  wordwrap;
    gint  linewrap;
    gint  completing_todo_note_sets_done;
    gint  recursive_completion;
    gint  auto_save_modified_notes;
    gchar *yank_crash_file;
    gchar *yank_def_file;
    gint  save_yank_geometry;
    gint  yank_width;
    gint  yank_height;
    gint  note_tree_width;
    gint  hide_toolbar;
    gint  hide_statusbar;
    gint  sorting_mode;
    gchar *todolist_bg_color_today;
    gchar *todolist_fg_color_today;
    gint  todolist_use_color_today;
    gchar *todolist_bg_color_past;
    gchar *todolist_fg_color_past;
    gint  todolist_use_color_past;
    gchar *note_font;
    gchar *note_tree_font;
    gchar *todolist_font;
    gint  use_custom_font;
    gint  preload_plugins;                /* number of plugins to load */
    gchar **preload_plugin;               /* plugin names */
    gint  max_recent_files;
    GList *recent_files;
};

typedef struct _yank_preferences yank_preferences;


#endif  /* YANK_NOTEDEFS_H */

