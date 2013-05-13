/*
 * deleteop.h
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

#ifndef YANK_DELETEOP_H
#define YANK_DELETEOP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>

#include "callbacks.h"
#include "app.h"
#include "preferences.h"

void cb_delete_dialog(GtkWidget *, gpointer);
void cb_delete_dialog_ok(GtkWidget *, gpointer);
void cb_delete_dialog_close(GtkWidget *, gpointer);

void remove_node_from_tree(GtkCTree *, GtkCTreeNode *, gpointer);
void free_tree_node(GtkCTree *, GtkCTreeNode *, gpointer);
void expire_tree_node(GtkCTree *, GtkCTreeNode *, gpointer);
void clear_tree_from_node(GtkCTree *, GtkCTreeNode *);

void cb_delete_sel_todo(GtkWidget *, gpointer);

#endif  /* YANK_DELETEOP_H */

