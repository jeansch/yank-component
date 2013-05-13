/*
 * callbacks.h
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


#ifndef YANK_CALLBACKS_H
#define YANK_CALLBACKS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include <gnome-xml/tree.h>
#include <gnome-xml/parser.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "app.h"
#include "notedefs.h"
#include "daycalc.h"
#include "todolist.h"
#include "util.h"
#include "time_label.h"

/*
 * 'real' callbacks
 */

void cb_about(GtkWidget *, gpointer);
void cb_new_todo(GtkWidget *, gpointer);
void cb_new_text(GtkWidget *, gpointer);
void cb_new_check(GtkWidget *, gpointer);
void cb_b_ok(GtkWidget *, gpointer);
void cb_b_apply(GtkWidget *, gpointer);
void cb_b_cancel(GtkWidget *, gpointer);
void note_tree_row_selected(GtkCTree *, GtkCTreeNode *, gint, gpointer);
void note_tree_row_unselected (GtkCTree *, GtkCTreeNode *, gint);
gint cb_delete_event(GtkWidget *, GdkEventAny *, gpointer);
void cb_open_date_sel(GtkWidget *, gpointer);
void cb_date_sel_ok(GtkWidget *, gpointer);
void cb_date_sel_clear(GtkWidget *, gpointer);
void cb_todo_row_selected(GtkCList *, gint, gint, GdkEvent *);
void cb_todo_col_selected(GtkCList *, gint);
gint cb_todo_timeout(gpointer);
void cb_sort_todo_date(GtkWidget *, gpointer);
void cb_sort_todo_prio(GtkWidget *, gpointer);
void cb_sort_todo_complete(GtkWidget *, gpointer);
void cb_sort_todo_title(GtkWidget *, gpointer);
void cb_sort_todo_date_then_prio(GtkWidget *, gpointer);
void cb_sort_todo_prio_then_date(GtkWidget *, gpointer);
void cb_exit(GtkWidget *, gpointer);
void cb_modify_text(GtkWidget *, gpointer);
void cb_modify_check(GtkWidget *, gpointer);
void cb_modify_todo(GtkWidget *, gpointer);
void cb_todo_modify_text(GtkWidget *, gpointer);
void cb_todo_modify_check(GtkWidget *, gpointer);
void cb_todo_modify_todo(GtkWidget *, gpointer);
void cb_text_entry_drop(GtkWidget *, GdkDragContext *, gint, gint,
                        GtkSelectionData *, guint, guint);
void cb_todo_list_drop(GtkWidget *, GdkDragContext *, gint, gint,
                       GtkSelectionData *, guint, guint);
void cb_note_tree_drop(GtkWidget *, GdkDragContext *, gint, gint,
                       GtkSelectionData *, guint, guint);
void cb_finished_note(GtkWidget *, gpointer);
void cb_note_tree_col_selected(GtkCList *, gint);
void cb_note_tree_reordered(GtkWidget *, GdkDragContext *, gint, gint,
                            GtkSelectionData *, guint, guint);
void cb_todo_open_corr(GtkWidget *, gpointer);
void cb_open_in_browser(GtkWidget *, gpointer);
void cb_view_as_mime(GtkWidget *, gpointer);
void cb_run_command(GtkWidget *, gpointer);

/* Edit functions */
void      cb_edit_cut(GtkWidget *, gpointer);
void      cb_edit_copy(GtkWidget *, gpointer);
void      cb_edit_paste(GtkWidget *, gpointer);
void      cb_edit_selall(GtkWidget *, gpointer);
GtkWidget *get_cut_copy_paste_focus(void);
GtkWidget *get_note_cut_copy_paste_focus(void);
void      cb_edit_cut_note(GtkWidget *, gpointer);
void      cb_edit_copy_note(GtkWidget *, gpointer);
void      cb_edit_paste_note(GtkWidget *, gpointer);
void      cb_edit_selall_note(GtkWidget *, gpointer);

void      cb_edit_cut_tree(GtkWidget *, gpointer);
void      cb_edit_copy_tree(GtkWidget *, gpointer);
void      cb_edit_paste_tree(GtkWidget *, gpointer);
void      cb_edit_selall_tree(GtkWidget *, gpointer);

/*
 * functions to do some work
 */

gint         apply_note(void);
gint         ok_note(void);
void         note_changed(void);
void         prepare_add(_notetype);
GtkCTreeNode *do_add_note(note_data *, GtkCTreeNode *);
void         do_edit_note(note_data *, GtkCTreeNode *);
GtkWidget    *choose_note_icon(note_data *);
void         free_note(note_data *);
void         modify_note(_notetype, GtkCTreeNode *);
void         modify_selection(_notetype, GList *);
GtkCTreeNode *tree_node_from_todo_selection(void);
GtkCTreeNode *tree_node_from_todo_selection_msg(void);
void         ask_commit_changes(void);
gchar*       get_text_selection(GtkWidget **);
void         handle_sigchild(int);

void         tree_cut_selection(GtkCTree *, GtkCTreeNode *, gpointer);
void         tree_copy_selection(GtkCTree *, GtkCTreeNode *, gpointer);
void         tree_paste_selection(GtkCTreeNode *, GNode *);
void         edit_tree_node_clear(GNode *, gpointer);
note_data*   copy_note(note_data *);
void         tree_node_save_childs(GtkCTree *, GtkCTreeNode *, gpointer);
void         cb_sort_subtree(GtkWidget *, gpointer);
void         cb_toggle_ext_flags(GtkWidget *, gpointer);
void         cb_complete_updates_done(GtkWidget *, gpointer);
void         cb_yank_homepage(GtkWidget *, gpointer);
void         tree_recursive_completion(GtkCTree *, GtkCTreeNode *);

#endif  /* YANK_CALLBACKS_H */

