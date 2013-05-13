/*
 * deleteop.c
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Huﬂmann <m.hussmann@home.ins.de>
 *
 * This program is free software; you  can redistribute it and/or sp->modify it
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

#include "deleteop.h"
#include "e-yank.h"

/* globals */
static GSList       *where_group;
static GtkWidget    *rb_selected_notes;
static GtkWidget    *rb_selected_trees;
static GtkWidget    *rb_all_notes;

static GSList       *what_group;
static GtkWidget    *rb_del_all_notes;
static GtkWidget    *rb_del_finished_notes;

static void get_tree_selection_list(GtkCTree *, GList **);

/* ------------------------------------------------------ */
/*
 * open the delete-dialog
 */

void
cb_delete_dialog(GtkWidget *w, gpointer p)
{
    const gchar      *buttons[] =
    {
        GNOME_STOCK_BUTTON_OK,
        GNOME_STOCK_BUTTON_CANCEL,
        NULL
    };
    static GtkWidget *dialog = NULL;
    GtkWidget        *hbox1;
    GtkWidget        *frame2;
    GtkWidget        *vbox1;
    GtkWidget        *frame3;
    GtkWidget        *vbox2;
    gchar            *t_str;
    
    if (dialog == NULL)
    {
        t_str = g_strdup_printf(_("%s - delete"), PACKAGE);
        dialog = gnome_dialog_newv(t_str, buttons);
        g_free(t_str);
        gnome_dialog_set_parent(GNOME_DIALOG(dialog), yank_root_win(NULL));
        
        hbox1 = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), hbox1,
                           FALSE, FALSE, 0);
        
        frame2 = gtk_frame_new(_("Where"));
        gtk_box_pack_start(GTK_BOX(hbox1), frame2, TRUE, TRUE, 0);
        gtk_container_border_width(GTK_CONTAINER(frame2), 4);
        
        vbox1 = gtk_vbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(frame2), vbox1);
        gtk_container_border_width(GTK_CONTAINER(vbox1), 4);
        
        rb_selected_notes = gtk_radio_button_new_with_label(NULL,
                                                       _("selected notes"));
        where_group = gtk_radio_button_group(GTK_RADIO_BUTTON(
            rb_selected_notes));
        gtk_box_pack_start(GTK_BOX(vbox1), rb_selected_notes, TRUE, TRUE, 0);
        
        rb_selected_trees = gtk_radio_button_new_with_label(where_group,
                                                       _("selected trees"));
        where_group = gtk_radio_button_group(GTK_RADIO_BUTTON(
            rb_selected_trees));
        gtk_box_pack_start(GTK_BOX(vbox1), rb_selected_trees, TRUE, TRUE, 0);
        
        rb_all_notes = gtk_radio_button_new_with_label(where_group,
                                                       _("all notes"));
        where_group = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_all_notes));
        gtk_box_pack_start(GTK_BOX(vbox1), rb_all_notes, TRUE, TRUE, 0);
        
        frame3 = gtk_frame_new(_("What"));
        gtk_box_pack_start(GTK_BOX(hbox1), frame3, TRUE, TRUE, 0);
        gtk_container_border_width(GTK_CONTAINER(frame3), 4);
        
        vbox2 = gtk_vbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(frame3), vbox2);
        gtk_container_border_width(GTK_CONTAINER(vbox2), 4);
        
        rb_del_all_notes = gtk_radio_button_new_with_label(NULL,
                                                           _("all notes"));
        what_group = gtk_radio_button_group(GTK_RADIO_BUTTON(
            rb_del_all_notes));
        gtk_box_pack_start(GTK_BOX(vbox2), rb_del_all_notes, TRUE, TRUE, 0);
        
        rb_del_finished_notes = gtk_radio_button_new_with_label(what_group,
                                                       _("finished notes"));
        what_group = gtk_radio_button_group(GTK_RADIO_BUTTON(
            rb_del_finished_notes));
        gtk_box_pack_start(GTK_BOX(vbox2), rb_del_finished_notes,
                           TRUE, TRUE, 0);

        gnome_dialog_button_connect(GNOME_DIALOG(dialog), 0, GTK_SIGNAL_FUNC
                                    (cb_delete_dialog_ok), dialog);
        gnome_dialog_button_connect(GNOME_DIALOG(dialog), 1, GTK_SIGNAL_FUNC
                                    (cb_delete_dialog_close), dialog);
        gtk_signal_connect(GTK_OBJECT(dialog), "destroy", GTK_SIGNAL_FUNC
                           (gtk_widget_destroyed), &dialog);
    }

    if (!GTK_WIDGET_VISIBLE(dialog))
    {
        gtk_window_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
    }
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 1);
    gtk_widget_show_all(dialog);
    gdk_window_raise(dialog->window);
}

/* ------------------------------------------------------ */
/*
 * copy tree selection to list
 */

static void
get_tree_selection_list(GtkCTree *tree, GList **sel)
{
    GList *t_sel;
    
    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(sel != NULL);

    /*
     * clear old list
     */
    
    if (*sel != NULL)
    {
        (*sel) = g_list_first(*sel);
        g_list_free(*sel);
        (*sel) = NULL;
    }
    t_sel = GTK_CLIST(tree)->selection;    
    while (t_sel != NULL)
    {
        (*sel) = g_list_append((*sel), t_sel->data);
        t_sel = t_sel->next;
    }
    (*sel) = g_list_first(*sel);
}

/* ------------------------------------------------------ */
/*
 * start to delete
 */

void
cb_delete_dialog_ok(GtkWidget *w, gpointer p)
{
    static GList *selection = NULL;
    GList        *sel;
    GtkCTreeFunc method;

    /*
     * get delete method
     */
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_del_all_notes)))
    {
        method = (*remove_node_from_tree);
    }
    else
    {
        method = (*expire_tree_node);
    }
    
    gtk_clist_freeze(GTK_CLIST(sp->note_tree));

    /*
     * delete all notes
     */
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_all_notes)))
    {
        gtk_ctree_post_recursive(GTK_CTREE(sp->note_tree), NULL, method, NULL);
    }
    else
    {        
        sel = GTK_CLIST(sp->note_tree)->selection;    
        if (sel == NULL)
        {
            gtk_clist_thaw(GTK_CLIST(sp->note_tree));
            gnome_ok_dialog_parented(_("Select a note first!"),
                                     yank_root_win(NULL));
            return;
            /* notreached */
        }

        get_tree_selection_list(GTK_CTREE(sp->note_tree), &selection);
        
        /*
         * delete selected notes
         */
        
        if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(rb_selected_notes)))
        {
            while (selection)
            {
                (method)(GTK_CTREE(sp->note_tree), GTK_CTREE_NODE(selection->data),
                         NULL);
                selection = selection->next;
            }
        }
        
        /*
         * delete selected trees
         */
        
        if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(rb_selected_trees)))
        {
            while (selection)
            {
                gtk_ctree_post_recursive(GTK_CTREE(sp->note_tree),
                                         GTK_CTREE_NODE(selection->data),
                                         method, NULL);
                /* rebuild selection list */
                get_tree_selection_list(GTK_CTREE(sp->note_tree), &selection);
            }
        }
    }
    
    gtk_clist_thaw(GTK_CLIST(sp->note_tree));
    cb_delete_dialog_close(NULL, p);
}

/* ------------------------------------------------------ */
/*
 * delete a note& data
 */

void
free_tree_node(GtkCTree *tree, GtkCTreeNode *node, gpointer p)
{
    static note_data *data;
    GNode            *item;
    
    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(node != NULL);

				printf("free_tree_node: sp = %0x\n", sp);
    
    data = gtk_ctree_node_get_row_data(GTK_CTREE(tree), GTK_CTREE_NODE(node));

    if (data == NULL)
    {
        return;
        /* notreached */
    }
    
    /*
     * update todolist
     */

    if (data->notetype == TodoNote && data->todo == 0)
    {
        delete_todo_item(data);
    }

    /*
     * update edit_tree
     */

    if (sp->edit_tree)
    {        

        item = g_node_find(sp->edit_tree, G_PRE_ORDER, G_TRAVERSE_ALL, data);
        if (item)
        {
            g_node_destroy(item);
        }
    }

       
    if (get_preferences()->recursive_completion && GTK_CTREE_ROW(node)->parent
        && ((data->notetype == CheckNote) || (data->notetype == TodoNote)))
    {
        /*
         * textnotes don't count
         */
        
        data->notetype = TextNote;
        tree_recursive_completion(GTK_CTREE(sp->note_tree),
                                  GTK_CTREE_ROW(node)->parent);
    }

    gtk_ctree_remove_node(tree, node);
    free_note(data);

    sp->modified = TRUE;
}

/* ------------------------------------------------------ */
/*
 * expire note(s)
 */

void
expire_tree_node(GtkCTree *tree, GtkCTreeNode *node, gpointer p)
{
    note_data *data;

    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(node != NULL);

    data = gtk_ctree_node_get_row_data(GTK_CTREE(tree), GTK_CTREE_NODE(node));

    if (data->todo == 1 &&
        (data->notetype == TodoNote || data->notetype == CheckNote))
    {
        remove_node_from_tree(GTK_CTREE(sp->note_tree), node, NULL);
    }
}

/* ------------------------------------------------------ */
/*
 * clear a subtree
 */

void
clear_tree_from_node(GtkCTree *tree, GtkCTreeNode *node)
{
    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
/*     g_return_if_fail(node != NULL); */
    
    gtk_clist_freeze(GTK_CLIST(tree));
    gtk_ctree_post_recursive(tree, node, (*free_tree_node), NULL);
    gtk_clist_thaw(GTK_CLIST(tree));
}

/* ------------------------------------------------------ */
/*
 * delete a selected note from the todo-list
 */

void
cb_delete_sel_todo(GtkWidget *w, gpointer p)
{
    GtkCTreeNode *tnode;

    tnode = tree_node_from_todo_selection_msg();
    if (tnode != NULL)
    {
        remove_node_from_tree(GTK_CTREE(sp->note_tree), tnode, NULL);
    }
}

/* ------------------------------------------------------ */
/*
 * kill the dialog
 */

void
cb_delete_dialog_close(GtkWidget *w, gpointer p)
{
    gtk_widget_hide(GTK_WIDGET(p));
    gtk_widget_destroy(GTK_WIDGET(p));
}

/* ------------------------------------------------------ */
/*
 * remove a node from the tree
 */

void
remove_node_from_tree(GtkCTree *tree, GtkCTreeNode *node, gpointer p)
{
    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(node != NULL);
    
    tree_node_save_childs(tree, node, p);
    
    /*
     * delete note from tree
     * also clears it from todolist, edit_tree
     */
    
    free_tree_node(tree, node, NULL);
    sp->modified = TRUE;
}

/* ------------------------------------------------------ */
