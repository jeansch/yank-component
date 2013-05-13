/*
 * search.c
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

#include "search.h"
#include "privates.h"

/* globals */
static GtkWidget *r_list;
static GSList    *rb_area;
static GSList    *rb_method;
static GtkWidget *rb_all_notes;
static GtkWidget *rb_selected_trees;
static GtkWidget *rb_regex;
static GtkWidget *rb_substring;

extern SuperPrivate *sp;

/* prototypes */
static void          cb_search_ok(GtkWidget *, gpointer);
static void          cb_search_close(GtkWidget *, gpointer);
static void          cb_search_row_selected(GtkCList *, gint, gint, GdkEvent*);
static void          cb_search_row_unselected(GtkCList *, gint, gint,
                                              GdkEvent *);
static void          search_tree_node(GtkCTree *, GtkCTreeNode *, gpointer);
static void          search_tree_node_regex(GtkCTree *, GtkCTreeNode *, 
                                            gpointer);
static void          cb_search_select_results(GtkWidget *, gpointer);
static void          cb_search_unselect_results(GtkWidget *, gpointer);
static gint          search_append_result2list(GtkCList *, note_data *);
static GtkCTreeNode* search_list_row2tree_node(GtkCList *, GtkCTree *, gint);

/* ------------------------------------------------------ */
/*
 * open the search dialog
 */

void
cb_search_dialog(GtkWidget *w, gpointer p)
{
    const gchar      *buttons[] =
    {
        _("Search"),
        _("Select results"),
        _("Unselect results"),
        GNOME_STOCK_BUTTON_CLOSE, 
        NULL
    };
    static GtkWidget *sdialog = NULL;
    gchar            *titles[] = { _("Results"), NULL };
    GtkWidget        *scrolledwindow;
    GtkWidget        *gentry;
    GtkWidget        *entry;
    GtkWidget        *app;
    GtkWidget        *frame1;
    GtkWidget        *mhbox;
    GtkWidget        *hbox1;
    GtkWidget        *vbox1;
    GtkWidget        *frame2;
    GtkWidget        *vbox3;
    gchar            *buf;
    
    app = GTK_WIDGET(p);

    if (sdialog == NULL)
    {
        buf = g_strdup_printf(_("%s - find"), PACKAGE);
        sdialog = gnome_dialog_newv(buf, buttons);
        g_free(buf);
        gnome_dialog_set_parent(GNOME_DIALOG(sdialog), GTK_WINDOW(app));
        gnome_dialog_set_default(GNOME_DIALOG(sdialog), 0);

        mhbox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(sdialog)->vbox), mhbox, TRUE,
                           TRUE, 0);

        frame1 = gtk_frame_new (_("Find"));
        gtk_box_pack_start(GTK_BOX(mhbox), frame1, TRUE, TRUE, 0);
        gtk_container_border_width (GTK_CONTAINER (frame1), 4);
        
        hbox1 = gtk_hbox_new(FALSE, 4);
        gtk_container_add(GTK_CONTAINER(frame1), hbox1);
        gtk_container_border_width(GTK_CONTAINER(hbox1), 4);
        
        vbox1 = gtk_vbox_new(FALSE, 4);
        gtk_box_pack_start(GTK_BOX(hbox1), vbox1, FALSE, TRUE, 0);

        gentry = gnome_entry_new("yank search history");
        gtk_box_pack_start(GTK_BOX(vbox1), gentry, TRUE, TRUE, 0);
        entry = gnome_entry_gtk_entry(GNOME_ENTRY(gentry));
        
        frame2 = gtk_frame_new(_("Options"));
        gtk_box_pack_start(GTK_BOX(vbox1), frame2, TRUE, TRUE, 0);
        
        vbox3 = gtk_vbox_new(FALSE, 4);
        gtk_container_add(GTK_CONTAINER(frame2), vbox3);
        gtk_container_border_width(GTK_CONTAINER(vbox3), 2);
        
        rb_all_notes = gtk_radio_button_new_with_label(
            NULL, _("Search all notes"));
        rb_area = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_all_notes));
        gtk_box_pack_start(GTK_BOX(vbox3), rb_all_notes, TRUE, TRUE, 0);
        
        rb_selected_trees = gtk_radio_button_new_with_label(
            rb_area, _("Search selected trees"));
        rb_area = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_selected_trees));
        gtk_box_pack_start(GTK_BOX(vbox3), rb_selected_trees, TRUE, TRUE, 0);
        
        rb_regex = gtk_radio_button_new_with_label(
            NULL, _("Regular expression"));
        rb_method = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_regex));
        gtk_box_pack_start(GTK_BOX(vbox3), rb_regex, TRUE, TRUE, 0);
        
        rb_substring = gtk_radio_button_new_with_label(
            rb_method, _("Substring"));
        rb_method = gtk_radio_button_group(GTK_RADIO_BUTTON(rb_substring));
        gtk_box_pack_start(GTK_BOX(vbox3), rb_substring, TRUE, TRUE, 0);
        
        scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_set_border_width(GTK_CONTAINER(scrolledwindow), 0);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
                                       GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_AUTOMATIC);
        gtk_box_pack_start(GTK_BOX(hbox1), scrolledwindow, TRUE, TRUE, 0);
        r_list = gtk_clist_new_with_titles(1, titles);
        gtk_clist_column_titles_passive(GTK_CLIST(r_list));
        gtk_clist_set_selection_mode(
            GTK_CLIST(r_list), GTK_SELECTION_EXTENDED);
        gtk_signal_connect(GTK_OBJECT(r_list), "select_row", 
                           GTK_SIGNAL_FUNC(cb_search_row_selected), NULL); 
        gtk_signal_connect(GTK_OBJECT(r_list), "unselect_row", 
                           GTK_SIGNAL_FUNC(cb_search_row_unselected), NULL); 
        gtk_container_add(GTK_CONTAINER(scrolledwindow), r_list);
        
        gnome_dialog_button_connect(GNOME_DIALOG(sdialog), 0,
                                    GTK_SIGNAL_FUNC(cb_search_ok), entry);
        gnome_dialog_button_connect(GNOME_DIALOG(sdialog), 1, GTK_SIGNAL_FUNC(
            cb_search_select_results), (gpointer) r_list);
        gnome_dialog_button_connect(GNOME_DIALOG(sdialog), 2, GTK_SIGNAL_FUNC(
            cb_search_unselect_results), (gpointer) r_list);
        gnome_dialog_button_connect(GNOME_DIALOG(sdialog), 3,
                                    GTK_SIGNAL_FUNC(cb_search_close), sdialog);
        gtk_signal_connect(GTK_OBJECT(sdialog), "destroy",
                           GTK_SIGNAL_FUNC(gtk_widget_destroyed), &sdialog);

        gnome_dialog_editable_enters(GNOME_DIALOG(sdialog),
                                     GTK_EDITABLE(entry));
        gtk_widget_grab_focus(entry);
    }

    if (!GTK_WIDGET_VISIBLE(sdialog))
    {
        gtk_window_position(GTK_WINDOW(sdialog), GTK_WIN_POS_MOUSE);
    }
    gtk_widget_show_all(sdialog);
    gdk_window_raise(sdialog->window);
}

/* ------------------------------------------------------ */
/*
 * start searching
 */

static void
cb_search_ok(GtkWidget *w, gpointer entry)
{
    static gchar    *sstring = NULL;
    GList           *sel;
    GnomeRegexCache *rxc = NULL;
    GtkCTreeFunc    func;
    gpointer        arg;

    g_return_if_fail(entry != NULL);
    g_return_if_fail(GTK_IS_ENTRY(entry));

    if (sstring)
    {
        g_free(sstring);
    }
    sstring = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
    gtk_clist_clear(GTK_CLIST(r_list));
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_substring)))
    {
        func = (*search_tree_node);
	arg = sstring;
    }
    else
    {
        func = (*search_tree_node_regex);
	rxc = gnome_regex_cache_new();
	arg = gnome_regex_cache_compile(rxc, sstring, REG_EXTENDED|REG_ICASE);
    }
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_all_notes)))
    {
        gtk_ctree_pre_recursive(GTK_CTREE(sp->note_tree), NULL, func, arg);
    }
    else
    {
        sel = GTK_CLIST(sp->note_tree)->selection;
        while (sel)
        {
            gtk_ctree_pre_recursive(GTK_CTREE(sp->note_tree), 
                                    GTK_CTREE_NODE(sel->data), func, arg);
            sel = g_list_next(sel);
        }
    }
    
    if (rxc != NULL)
    {
        gnome_regex_cache_destroy(rxc);
    }
}

/* ------------------------------------------------------ */
/*
 * stop searching& kill the dialog
 */

static void
cb_search_close(GtkWidget *w, gpointer p)
{
    gtk_widget_hide(GTK_WIDGET(p));
    gtk_widget_destroy(GTK_WIDGET(p));
}

/* ------------------------------------------------------ */
/*
 * search a single note
 */

static void
search_tree_node(GtkCTree *tree, GtkCTreeNode *node, gpointer sstring)
{
    note_data *data;
    gint      found;
    
    data = gtk_ctree_node_get_row_data(tree, node);
    if (data)
    {
        found = FALSE;
        if (data->title != NULL)
        {
            if (strstr(data->title, sstring))
            {
                found = TRUE;
            }
        }
        if (data->text != NULL && found == FALSE)
        {
            if (strstr(data->text, sstring))
            {
                found = TRUE;
            }
        }
        if (found == TRUE)
        {
            search_append_result2list(GTK_CLIST(r_list), data);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * an item in the list has been selected
 */

static void
cb_search_row_selected(GtkCList *list, gint row, gint col, GdkEvent *evt)
{
    GtkCTreeNode *tnode;

    tnode = search_list_row2tree_node(list, GTK_CTREE(sp->note_tree), row);
    if (tnode == NULL)
    {
        return;
        /* notreached */
    }

    if (gtk_ctree_node_is_visible(GTK_CTREE(sp->note_tree), tnode)
        != GTK_VISIBILITY_FULL)
    {
        GtkCTreeNode *pnode;
        
        pnode = GTK_CTREE_ROW(tnode)->parent;
        while (pnode != NULL)
        {
            gtk_ctree_expand(GTK_CTREE(sp->note_tree), pnode);
            pnode = GTK_CTREE_ROW(pnode)->parent;
        }
    }
    gtk_ctree_node_moveto(GTK_CTREE(sp->note_tree), tnode, 0, 0, 0);
    gtk_ctree_select(GTK_CTREE(sp->note_tree), tnode);
}

/* ------------------------------------------------------ */
/*
 * an item in the list has been unselected
 */

static void
cb_search_row_unselected(GtkCList *list, gint row, gint col, GdkEvent *evt)
{
    GtkCTreeNode *tnode;

    tnode = search_list_row2tree_node(list, GTK_CTREE(sp->note_tree), row);
    if (tnode == NULL)
    {
        return;
        /* notreached */
    }
    gtk_ctree_unselect(GTK_CTREE(sp->note_tree), tnode);
}

/* ------------------------------------------------------ */
/*
 * regular expression searching
 */

static void
search_tree_node_regex(GtkCTree *tree, GtkCTreeNode *node, gpointer re)
{
    note_data  *data;
    regmatch_t pmatch[16];
    gint       res;
    gint       found;
    
    data = gtk_ctree_node_get_row_data(tree, node);
    if (data)
    {
        found = FALSE;
        if (data->title != NULL)
        {
            res = regexec(re, data->title, 16, pmatch, 0);
            if (res == 0)
            {
                found = TRUE;
            }
        }
        if (data->text != NULL && found == FALSE)
        {
            res = regexec(re, data->text, 16, pmatch, 0);
            if (res == 0)
            {
                found = TRUE;
            }
        }
        if (found == TRUE)
        {
            search_append_result2list(GTK_CLIST(r_list), data);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * select all found items in the note-tree
 */

static void
cb_search_select_results(GtkWidget *w, gpointer list)
{
    gint rows;
    gint row;
    
    g_return_if_fail(list != NULL);
    g_return_if_fail(GTK_IS_CLIST(list));

    rows = GTK_CLIST(list)->rows;
    for (row = 0; row < rows; row++)
    {
        gtk_clist_select_row(GTK_CLIST(list), row, 0);
    }
/*     gtk_clist_select_all(GTK_CLIST(list)); */
}

/* ------------------------------------------------------ */
/*
 * unselect all found items in the note-tree
 */

static void
cb_search_unselect_results(GtkWidget *w, gpointer list)
{
    gint rows;
    gint row;
    
    g_return_if_fail(list != NULL);
    g_return_if_fail(GTK_IS_CLIST(list));

    rows = GTK_CLIST(list)->rows;
    for (row = 0; row < rows; row++)
    {
        gtk_clist_unselect_row(GTK_CLIST(list), row, 0);
    }
/*     gtk_clist_unselect_all(GTK_CLIST(list)); */
}

/* ------------------------------------------------------ */
/*
 * append the title to the list of results
 */

static gint
search_append_result2list(GtkCList *list, note_data *note)
{
    static gchar *txt[] = { NULL, NULL };
    gint         row;
    
    g_return_val_if_fail(list != NULL, -1);
    g_return_val_if_fail(GTK_IS_CLIST(list), -1);
    g_return_val_if_fail(note != NULL, -1);

    txt[0] = note->title;
    row = gtk_clist_append(GTK_CLIST(list), txt);
    gtk_clist_set_row_data(GTK_CLIST(list), row, note);

    return (row);
}

/* ------------------------------------------------------ */
/*
 * return the corresponding treenode
 */

static GtkCTreeNode*
search_list_row2tree_node(GtkCList *list, GtkCTree *tree, gint row)
{
    GtkCTreeNode *retnode;
    note_data    *note;
    
    g_return_val_if_fail(list != NULL, NULL);
    g_return_val_if_fail(GTK_IS_CLIST(list), NULL);
    g_return_val_if_fail(tree != NULL, NULL);
    g_return_val_if_fail(GTK_IS_CTREE(tree), NULL);

    retnode = NULL;
    
    note = (note_data *)gtk_clist_get_row_data(GTK_CLIST(list), row);
    if (note == NULL)
    {
        gnome_warning_dialog_parented(_("Search: List-row without data!"),
                                      yank_root_win(NULL));
    }
    else
    {
        retnode = gtk_ctree_find_by_row_data(GTK_CTREE(tree), NULL, note);
        if (retnode == NULL)
        {
            gnome_warning_dialog_parented(_("Search: Can't find result in \
tree!\n\nPlease try to search again."), yank_root_win(NULL));
        }
    }
    return (retnode);
}

/* ------------------------------------------------------ */
