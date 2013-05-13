/*
 * fileio.h
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

#ifndef YANK_FILEIO_H
#define YANK_FILEIO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "notedefs.h"
#include "callbacks.h"
#include "preferences.h"
#include "menus.h"
#include "util.h"
#include "app.h"

/*
 * loading
 */

void       cb_file_load_dialog(GtkWidget *, gpointer);
void       cb_load_file_ok(GtkWidget *, GtkWidget *);
gint       note_load(gchar *, GtkCTree *, GtkCTreeNode *);
void       load_notelist(GtkCTreeNode *, xmlNodePtr);
gint       load_notes_from_cli(gchar *, GtkCTree *, GtkCTreeNode *);
note_data* get_note(xmlDocPtr, xmlNodePtr, gint *);
int        get_notes(GtkCTreeNode *, xmlDocPtr, xmlNodePtr, GDate*);
void       cb_recent_files(GtkWidget *, GtkWidget *);

/*
 * saving
 */

void cb_file_save_dialog(GtkWidget *, gpointer);
void cb_save_file_ok(GtkWidget *, GtkFileSelection *);
void note_save(gchar *, GtkCTree *);
void build_save_file(GtkCTree *, GtkCTreeNode *, gpointer);
void cb_save_file(GtkWidget *, gpointer);

/*
 * misc
 */

void                cb_close(GtkWidget *, gpointer);
void                txt_append_file(GtkText *, gchar *);
note_data*          make_note_from_file(gchar *);
inline const gchar* get_filename(void);
void                handle_sigterm(int);
gint                autosave_notes(gpointer p);
gint                get_crash_file(void);
void                get_def_file(void);

gint load_file(gchar *);

#endif  /* YANK_FILEIO_H */

