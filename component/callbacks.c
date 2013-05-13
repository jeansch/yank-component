/*
 * callbacks.c
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

#include "callbacks.h"

#include "e-yank.h"

extern char *pix_text_xpm;
extern char *pix_circle2_xpm;
extern char *pix_circle_xpm;
extern char *pix_box2_xpm;
extern char *pix_box_xpm;

/* type of current note we're dealing with */
static _notetype    notetype = None;

/* what should we do with the note? */
static _editmode    editmode = NoMode;

/* selected node in the ctree on the left  -- for inserts */
GtkCTreeNode        *selected_node = NULL;

/* current opened node -- for edits */
static GtkCTreeNode *opened_node = NULL;

/* changed-data flag -- any notes modified since last save */
/* gint                modified = FALSE; */

/* commit flag -- whether current opened node has uncommitted changes */
static gint         uncommitted = FALSE;

/* tree structure for cut, copy, paste operations on the tree widget */
/* GNode               *edit_tree = NULL; */

/* flag toggled from cut, copy */
gboolean            edit_tree_free = FALSE;

#define YANK_HOMEPAGE "http://home.ins.de/~m.hussmann/software/yank/"

/*
 * prototypes
 */

gint calc_completion_status(GtkCTree *, GtkCTreeNode *);

/* ------------------------------------------------------ */
/*
 * about message
 */

void
cb_about(GtkWidget* widget, gpointer data)
{
    static GtkWidget *dialog = NULL;
    GtkWidget        *app;
    
    app = (GtkWidget*) data;
    
    if (dialog != NULL)
    {
        g_assert(GTK_WIDGET_REALIZED(dialog));
        gdk_window_show(dialog->window);
        gdk_window_raise(dialog->window);
    }
    else
    {
        const gchar *authors[] =
        {
            "Michael Huﬂmann <m.hussmann@home.ins.de>",
	    "Thomas Schultz <tststs@gmx.de>",
            NULL
        };

        dialog = gnome_about_new (_(PACKAGE), VERSION,
                                  "(C) 1999, 2000, 2001 Michael Huﬂmann", authors,
                                  N_("yet another NoteKeeper - \
compiled: "__DATE__" "__TIME__"\n"YANK_HOMEPAGE""), "yank.png");
        gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
                           GTK_SIGNAL_FUNC(gtk_widget_destroyed), &dialog);
        
        gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
        
        gtk_widget_show(dialog);
    }
}

/* ------------------------------------------------------ */
/*
 * exit with check
 */

/* void */
/* cb_exit(GtkWidget *widget, gpointer data) */
/* { */
/*     static GtkWidget *dialog = NULL; */
/*     gint             sel; */
/*     yank_preferences *pref; */
    
    /*
     * there's an error hidden here
     */

/*     ask_commit_changes(); */

/*     pref = get_preferences(); */

/*     if (pref->save_yank_geometry) */
/*     { */
/*         gnome_config_push_prefix("/yank/Global/"); */
/*         gnome_config_set_int("yank height", GTK_WIDGET(yank_root_win(NULL)) */
/*                              ->allocation.height); */
/*         gnome_config_set_int("yank width", GTK_WIDGET(yank_root_win(NULL)) */
/*                              ->allocation.width); */
/*         gnome_config_set_int("note tree width", */
/*                              sp->note_tree->parent->allocation.width); */
/*         gnome_config_pop_prefix(); */
/*         gnome_config_sync(); */
/*     } */
    
/*     if (modified) */
/*     { */
/*         if (pref->auto_save_on_exit && get_filename()) */
/*         { */
/*             cb_save_file(NULL, NULL); */
/*         } */
/*          */
/*         if (dialog == NULL) */
/*         { */
/*             dialog = gnome_question_dialog_modal_parented( */
/*                 _("Notes modified. Do you want to save them?"), NULL, NULL, */
/*                 yank_root_win(NULL)); */
/*             gnome_dialog_set_default(GNOME_DIALOG(dialog), 0); */
/*             sel = gnome_dialog_run_and_close(GNOME_DIALOG(dialog)); */
/*             if (sel == 0) */
/*             { */
/*                 cb_save_file(NULL, NULL); */
/*             } */
/*             if (sel == 1) */
/*             { */
/* 		yank_app_exit((GtkWidget *) data); */
/*             } */
/*         } */
/*         else */
/*         { */
/*             g_assert(GTK_WIDGET_REALIZED(dialog)); */
/*             gdk_window_show(dialog->window); */
/*             gdk_window_raise(dialog->window); */
/* 	    yank_app_exit((GtkWidget *) data); */
/*         } */
/*     } */
/*     else */
/*     { */
/* 	yank_app_exit((GtkWidget *) data); */
/*     } */
/* } */

/* ------------------------------------------------------ */
/*
 * start adding a new text-note
 */

void
cb_new_text(GtkWidget *widget, gpointer data)
{
    prepare_add(TextNote);
}

/* ------------------------------------------------------ */
/*
 * start adding a new check-note
 */

void
cb_new_check(GtkWidget *widget, gpointer data)
{
    prepare_add(CheckNote);
}

/* ------------------------------------------------------ */
/*
 * start adding a new todo-note
 */

void
cb_new_todo(GtkWidget *widget, gpointer data)
{
    prepare_add(TodoNote);
}

/* ------------------------------------------------------ */
/*
 * generic stuff when starting to add a note
 */

void
prepare_add(_notetype typ)
{
    ask_commit_changes();
    notetype = typ;
    editmode = Add;
    clear_text_page(TRUE, &uncommitted);
    view_note_as(notetype);
    gtk_widget_set_sensitive(sp->b_ok, TRUE);
    gtk_widget_set_sensitive(sp->b_apply, FALSE);
    gtk_widget_set_sensitive(sp->b_cancel, TRUE);
    gtk_widget_grab_focus(sp->title_entry);
    show_text_tab();
}

/* ------------------------------------------------------ */
/*
 * OK-button in the note has been clicked
 */

void
cb_b_ok(GtkWidget* widget, gpointer data)
{
    gint ret;
    
    if (notetype == TextNote || notetype == CheckNote || notetype == TodoNote)
    {
        ret = ok_note();
        if (ret)
        {
            gnome_warning_dialog_parented(_("Note has not been added!"),
                                          yank_root_win(NULL));
        }        
    }
    else
    {
        g_warning(_("You shouldn't see this warning - please report."));
    }
}

/* ------------------------------------------------------ */
/*
 * apply-button in the note has been clicked
 */

void
cb_b_apply(GtkWidget* widget, gpointer data)
{
    gint ret;
    
    if (notetype == TextNote || notetype == CheckNote || notetype == TodoNote)
    {
        ret = apply_note();
        if (ret)
        {
            gnome_warning_dialog_parented(_("Note has not been added!"),
                                          yank_root_win(NULL));
        }
    }
    else
    {
        g_warning(_("You shouldn't see this warning - please report."));
    }
}

/* ------------------------------------------------------ */
/*
 * cancel-button in the note has been clicked
 */

void
cb_b_cancel(GtkWidget* widget, gpointer data)
{
    editmode = NoMode;
    show_todo_tab();
    notetype = None;
}

/* ------------------------------------------------------ */
/*
 * add/ edit a note
 */

gint
apply_note(void)
{
    note_data *note;

    note = fill_note_data_from_page(notetype);
    if (note == NULL)
    {
        return (1);
        /* notreached */
    }
    if (editmode == Add)
    {
        note->id = create_note_id();
        opened_node = do_add_note(note, selected_node);
	editmode = Edit;
    }
    else
    {
        if (editmode == Edit)
        {
            do_edit_note(note, opened_node);
        }
    }
    uncommitted = FALSE;
   gtk_widget_set_sensitive(sp->b_apply, FALSE);
    return (0);
}

/* ------------------------------------------------------ */
/*
 * 'OK' callback
 */

gint
ok_note(void)
{
    gint ret;
    
    if (uncommitted)
    {
        ret = apply_note();
    }
    else
    {
        ret = 0;
    }
    if (ret)
    {
        gnome_warning_dialog_parented(_("Note has not been added!"),
                                      yank_root_win(NULL));
    }        
    show_todo_tab();
    if (selected_node != NULL)
    {
        gtk_ctree_unselect(GTK_CTREE(sp->note_tree), selected_node);
    }
/*     gtk_window_set_focus(yank_root_win(NULL), sp->note_tree); */
    notetype = None;
    return (ret);
	return 0;
}

/* ------------------------------------------------------ */
/*
 * somebody has clicked into the sp->note_tree 
 */

void
note_tree_row_selected(GtkCTree *tree, GtkCTreeNode *node, gint col,
                       gpointer gpo)
{
    note_data *note;

    gtk_signal_handler_block_by_func(GTK_OBJECT(sp->note_tree),
                                     GTK_SIGNAL_FUNC(note_tree_row_selected),
                                     NULL);
    ask_commit_changes();
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(sp->note_tree),
                                       GTK_SIGNAL_FUNC(note_tree_row_selected),
                                       NULL);

    selected_node = node;
    opened_node = node;
    clear_text_page(FALSE, &uncommitted);
    note = (note_data *)gtk_ctree_node_get_row_data(GTK_CTREE(sp->note_tree),
                                                    selected_node);
    if (note != NULL)
    {
        notetype = note->notetype;
        view_note_as(note->notetype);
        fill_page_from_note_data(note);
	gtk_widget_set_sensitive(sp->b_ok, TRUE);
        gtk_widget_set_sensitive(sp->b_apply, FALSE);
        gtk_widget_set_sensitive(sp->b_cancel, TRUE);
	uncommitted = FALSE;
        editmode = Edit;
        show_text_tab();
    }
    else
    {
        gnome_warning_dialog_parented(_("Critical error: Note without data!"),
                                      yank_root_win(NULL));
    }
}

/* ------------------------------------------------------ */
/*
 * somebody has unselected an item in the sp->note_tree
 */

void
note_tree_row_unselected(GtkCTree *tree, GtkCTreeNode *node, gint col)
{
    /*
     * note_tree_row_selected has to be called _after_ ask_commit_changes
     */
    
    gtk_signal_handler_block_by_func(GTK_OBJECT(sp->note_tree),
                                     GTK_SIGNAL_FUNC(note_tree_row_selected),
                                     NULL);
    ask_commit_changes();
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(sp->note_tree),
                                       GTK_SIGNAL_FUNC(note_tree_row_selected),
                                       NULL);
    selected_node = NULL;
    opened_node = NULL;
    show_todo_tab();
}


/* ------------------------------------------------------ */
/*
 * the note has been changed
 */

void
note_changed(void)
{
    uncommitted = TRUE;
    gtk_widget_set_sensitive(sp->b_apply, TRUE);
}


/* ------------------------------------------------------ */
/*
 * got the delete-event from wm
 */

gint
cb_delete_event(GtkWidget* window, GdkEventAny* e, gpointer data)
{
/*     cb_exit(NULL, NULL); */
    return (TRUE);
}

/* ------------------------------------------------------ */
/*
 * add a note to the tree
 */

GtkCTreeNode *
do_add_note(note_data *note, GtkCTreeNode *parent)
{
    gchar        *title[] = { NULL, NULL};
    GtkWidget    *icon;
    GtkCTreeNode *n_node;

    icon = choose_note_icon(note);
		if (icon == NULL) {
			exit(0);
		}
    title[0] = note->title;
    n_node = gtk_ctree_insert_node(GTK_CTREE(sp->note_tree), parent,
                                   NULL, title, 2,
                                   GNOME_PIXMAP(icon)->pixmap,
                                   GNOME_PIXMAP(icon)->mask,
                                   GNOME_PIXMAP(icon)->pixmap,
                                   GNOME_PIXMAP(icon)->mask,
                                   FALSE, TRUE);
    gtk_ctree_node_set_row_data(GTK_CTREE(sp->note_tree), n_node, note);



    /*
     * update sp->todolist
     */
    
    if (note->notetype == TodoNote && note->todo == 0)
    {
        add_todo_item(note);
    }

    if (get_preferences()->recursive_completion && parent != NULL)
    {
        tree_recursive_completion(GTK_CTREE(sp->note_tree), parent);
    }
    
    sp->modified = TRUE;
    
    return (n_node);
}

/* ------------------------------------------------------ */
/*
 * edit a note
 */

void
do_edit_note(note_data *n_note, GtkCTreeNode *node)
{
    static note_data *o_note;
    GtkWidget        *icon;
    gchar            *text;
    guint8           spacing;
    GdkPixmap        *pixmap_closed;
    GdkBitmap        *mask_closed;
    GdkPixmap        *pixmap_opened;
    GdkBitmap        *mask_opened;
    gboolean         is_leaf;
    gboolean         expanded;

    o_note = (note_data *) gtk_ctree_node_get_row_data(GTK_CTREE(sp->note_tree),
                                                       GTK_CTREE_NODE(node));
    if (o_note != NULL)
    {
        /*
         * update sp->todolist
         */
        
        if (n_note->notetype == TodoNote && n_note->todo == 1)
        {
            if (gtk_clist_find_row_from_data(GTK_CLIST(sp->todolist), o_note)
                != -1)
            {
                delete_todo_item(o_note);
            }
        }
        if (n_note->notetype == TodoNote && n_note->todo == 0)
        {
            edit_todo_item(o_note, n_note);
        }

        /*
         * update tree
         */
        
        icon = choose_note_icon(n_note);
        gtk_ctree_get_node_info(GTK_CTREE(sp->note_tree),
                                GTK_CTREE_NODE(node), &text,
                                &spacing, &pixmap_closed, &mask_closed,
                                &pixmap_opened, &mask_opened, &is_leaf,
                                &expanded);
        gtk_ctree_set_node_info(GTK_CTREE(sp->note_tree),
                                GTK_CTREE_NODE(node), n_note->title,
                                2, GNOME_PIXMAP(icon)->pixmap,
                                GNOME_PIXMAP(icon)->mask,
                                GNOME_PIXMAP(icon)->pixmap,
                                GNOME_PIXMAP(icon)->mask, 0, expanded);
        gtk_ctree_node_set_row_data(GTK_CTREE(sp->note_tree), node, n_note);
        gtk_widget_queue_draw(sp->note_tree);
        sp->modified = TRUE;
        free_note(o_note);

        if (get_preferences()->recursive_completion &&
            GTK_CTREE_ROW(node)->parent != NULL)
        {
            tree_recursive_completion(GTK_CTREE(sp->note_tree),
                                      GTK_CTREE_ROW(node)->parent);
        }
    }
    else
    {
        gnome_warning_dialog_parented(_("Trying to edit NULL note!"),
                                      yank_root_win(NULL));
    }
}

/* ------------------------------------------------------ */
/*
 * select an icon for a note
 */

GtkWidget *
choose_note_icon(note_data *note)
{
    GtkWidget *icon;
    
    switch (note->notetype)
    {
    case TextNote:
        icon = pix_text_xpm;
        break;
    case CheckNote:
        icon = note->todo ? pix_circle2_xpm : pix_circle_xpm;
        break;
    case TodoNote:
        icon = note->todo ? pix_box2_xpm : pix_box_xpm;
        break;
    default:
        icon = NULL;
        break;
    };
    return (icon);
}

/* ------------------------------------------------------ */
/*
 * show a calendar ...
 */

void
cb_open_date_sel(GtkWidget *w, gpointer p)
{
    const gchar      *buttons[] =
    {
        GNOME_STOCK_BUTTON_OK,
        GNOME_STOCK_BUTTON_CANCEL,
        NULL
    };
    static GtkWidget *dialog = NULL;
    static GtkWidget *cal = NULL;
    static GtkWidget *b_noexpire = NULL;
    GtkWindow        *app;
    TimeLabel        *timelabel;
    GDate            *e_date;
    time_t           labeltime;
    
    timelabel = TIME_LABEL(p);

    /*
     * create the dialog
     */
    
    if (dialog == NULL)
    {
        app = yank_root_win(NULL);
        g_return_if_fail(IS_TIME_LABEL(timelabel));
        dialog = gnome_dialog_newv("", buttons);
        cal = gtk_calendar_new();
        gtk_widget_show(cal);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), cal, FALSE,
                           FALSE, 5);

        b_noexpire = gtk_button_new_with_label(_("Do not expire"));
        gtk_object_set_user_data(GTK_OBJECT(b_noexpire), timelabel);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), b_noexpire,
			   FALSE, FALSE, 5);
        gtk_signal_connect(GTK_OBJECT(b_noexpire), "clicked",
			   GTK_SIGNAL_FUNC(cb_date_sel_clear), dialog);

        gnome_dialog_close_hides(GNOME_DIALOG(dialog), TRUE);
        gnome_dialog_set_parent(GNOME_DIALOG(dialog), app);
        gtk_object_set_user_data(
            GTK_OBJECT(GNOME_DIALOG(dialog)->buttons->data), timelabel);
        gnome_dialog_button_connect(GNOME_DIALOG(dialog), 0,
                                    GTK_SIGNAL_FUNC(cb_date_sel_ok), cal);
        /* FIXME: Can this be done without using private data? */
        gtk_signal_connect_object(
            GTK_OBJECT(cal), "day_selected_double_click",
            GTK_SIGNAL_FUNC(gtk_button_clicked),
            GTK_OBJECT(GNOME_DIALOG(dialog)->buttons->data));
    }
    else
    {
        g_assert(GTK_WIDGET_REALIZED(dialog));
        gdk_window_show(dialog->window);
        gdk_window_raise(dialog->window);
    }
    
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);

    gtk_window_set_title(GTK_WINDOW(dialog), _("Expires at"));
    e_date = g_date_new();
    labeltime = time_label_get_time(timelabel);
    if (labeltime < 0)
    {
        labeltime = time(NULL);
    }
    g_date_set_time(e_date, labeltime);
    gtk_widget_show(b_noexpire);
    gtk_calendar_select_month(GTK_CALENDAR(cal), g_date_month(e_date),
                              g_date_year(e_date));
    gtk_calendar_select_day(GTK_CALENDAR(cal), g_date_day(e_date));
    g_date_free(e_date);
    
    gnome_dialog_run_and_close(GNOME_DIALOG(dialog));
}

/* ------------------------------------------------------ */
/*
 * ok pressed in the calendar
 */

void
cb_date_sel_ok(GtkWidget *w, gpointer p)
{
    GDate     *s_date;
    gint      year;
    gint      month;
    gint      day;
    struct tm s_tm;
    TimeLabel *tl;
    
    tl = TIME_LABEL(gtk_object_get_user_data(GTK_OBJECT(w)));
    gtk_calendar_get_date(GTK_CALENDAR(p), &year, &month, &day);
    s_date = g_date_new();
    g_date_set_dmy(s_date, day, month, year);
    g_date_to_struct_tm(s_date, &s_tm);
    g_date_free(s_date);
    time_label_set_time(tl, mktime(&s_tm));
}

/* ------------------------------------------------------ */
/*
 * "do not expire" pressed in the calendar
 */

void
cb_date_sel_clear(GtkWidget *w, gpointer p)
{
    TimeLabel *tl;

    tl = TIME_LABEL(gtk_object_get_user_data(GTK_OBJECT(w)));
    time_label_set_time(tl, -1);
    gnome_dialog_close(GNOME_DIALOG(p));
}

/* ------------------------------------------------------ */
/*
 * remove note from mem
 */

void
free_note(note_data *data)
{
    if (data)
    {
        if (data->id)
        {
            free(data->id);
        }
        if (data->title)
        {
            free(data->title);
        }
        if (data->text)
        {
            free(data->text);
        }
        free(data);
    }
}

/* ------------------------------------------------------ */
/*
 * double-click or space will open a note from the todo-list
 */

void
cb_todo_row_selected(GtkCList *list, gint row, gint col, GdkEvent *evt)
{
    note_data    *note;
    GtkCTreeNode *tnode;

    if (((evt != NULL) && (evt->type == GDK_2BUTTON_PRESS))
        || (evt == NULL))
    {
        clear_text_page(FALSE, &uncommitted);
        note = (note_data *)gtk_clist_get_row_data(GTK_CLIST(list), row);
        tnode = gtk_ctree_find_by_row_data(GTK_CTREE(sp->note_tree), NULL, note);
        if (tnode != NULL)
        {
            gtk_clist_freeze(GTK_CLIST(sp->note_tree));
            gtk_ctree_unselect_recursive(GTK_CTREE(sp->note_tree), NULL);
            gtk_ctree_select(GTK_CTREE(sp->note_tree), tnode);
            gtk_clist_thaw(GTK_CLIST(sp->note_tree));    
        }
    }
}

/* ------------------------------------------------------ */
/*
 * a todo-list column-title has been clicked
 */

void
cb_todo_col_selected(GtkCList *list, gint col)
{
   sort_by_sorting_mode(col + 1); /* sorting mode 0 means "no sorting" */
}

/* ------------------------------------------------------ */
/*
 * the todo list needs to be refreshed
 */

gint
cb_todo_timeout(gpointer p)
{
    time_t cur_time_t;
    struct tm *cur_time_tm;
    gint32 timeouttime;
    
    set_todolist_colors(); /* do the actual work */
    
    /* install a new timeout and stop this one */
    time(&cur_time_t);
    cur_time_tm = localtime(&cur_time_t);
    
    timeouttime = ((23 - cur_time_tm->tm_hour) * 3600000 +
		   (59 - cur_time_tm->tm_min)  *   60000 +
		   (60 - cur_time_tm->tm_sec)  *    1000);
    if (timeouttime <= 0) /* leap second */
      timeouttime = 1000;
    gtk_timeout_add(timeouttime, cb_todo_timeout, NULL);
    
    return FALSE;
}

/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by date
 */

void
cb_sort_todo_date(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(1);
}

/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by priority
 */

void
cb_sort_todo_prio(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(2);
}

/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by completion status
 */

void
cb_sort_todo_complete(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(3);
}


/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by title
 */

void
cb_sort_todo_title(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(4);
}

/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by date, then priority
 */

void
cb_sort_todo_date_then_prio(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(5);
}

/* ------------------------------------------------------ */
/*
 * sort the sp->todolist by priority, then date
 */

void
cb_sort_todo_prio_then_date(GtkWidget *w, gpointer p)
{
    sort_by_sorting_mode(6);
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_modify_text(GtkWidget *w, gpointer p)
{
    modify_selection(TextNote, GTK_CLIST(sp->note_tree)->selection);
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_modify_check(GtkWidget *w, gpointer p)
{
    modify_selection(CheckNote, GTK_CLIST(sp->note_tree)->selection);
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_modify_todo(GtkWidget *w, gpointer p)
{
    modify_selection(TodoNote, GTK_CLIST(sp->note_tree)->selection);
}

/* ------------------------------------------------------ */
/*
 * general proxy
 */

void
modify_selection(_notetype t_type, GList *selection)
{
    GList *sel;

    if (selection == NULL)
    {
        gnome_ok_dialog_parented(_("Select a note first!"),
                                 yank_root_win(NULL));
        return;
        /* notreached */
    }
    
    for (sel = selection; sel != NULL; sel = g_list_next(sel))
    {
        modify_note(t_type, GTK_CTREE_NODE(sel->data));
    }
}

/* ------------------------------------------------------ */
/*
 * change a notes type
 */

void
modify_note(_notetype t_type, GtkCTreeNode *node)
{
    note_data *data;
    GtkWidget *icon;
    gchar     *text;
    guint8    spacing;
    GdkPixmap *pixmap_closed;
    GdkBitmap *mask_closed;
    GdkPixmap *pixmap_opened;
    GdkBitmap *mask_opened;
    gboolean  is_leaf;
    gboolean  expanded;

    if (node == NULL)
    {
        gnome_ok_dialog_parented(_("Select a note first!"),
                                 yank_root_win(NULL));
        return;
        /* notreached */
    }
    
    data = (note_data *) gtk_ctree_node_get_row_data(GTK_CTREE(sp->note_tree),
                                                     GTK_CTREE_NODE(node));

    if (data)
    {
        data->changes++;
        data->changed = time(NULL);
        
        /*
         * update the todo-list
         */

        if (t_type == TodoNote && data->deadline == 0)
        {
            data->deadline = time(NULL);
        }

        if (t_type == TodoNote && data->todo == 0)
        {
            add_todo_item(data);
        }
        if (data->notetype == TodoNote && data->todo == 0)
        {
            delete_todo_item(data);
        }
        
        data->notetype = t_type;
        
        sp->modified = TRUE;

        /*
         * update the tree
         */
        
        icon = choose_note_icon(data);
        gtk_ctree_get_node_info(GTK_CTREE(sp->note_tree), GTK_CTREE_NODE(node),
                                &text, &spacing, &pixmap_closed, &mask_closed,
                                &pixmap_opened, &mask_opened, &is_leaf,
                                &expanded);
        gtk_ctree_set_node_info(GTK_CTREE(sp->note_tree), GTK_CTREE_NODE(node),
                                data->title, 2, GNOME_PIXMAP(icon)->pixmap,
                                GNOME_PIXMAP(icon)->mask,
                                GNOME_PIXMAP(icon)->pixmap,
                                GNOME_PIXMAP(icon)->mask, 0, expanded);
        show_todo_tab();
    }
    else
    {
        gnome_warning_dialog_parented(_("Trying to modify NULL note!"),
                                      yank_root_win(NULL));
    }
}

/* ------------------------------------------------------ */
/*
 * drop-request on the text-entry
 */

void
cb_text_entry_drop(GtkWidget *entry, GdkDragContext *context, gint x, gint y,
                   GtkSelectionData *selection_data, guint info, guint time)
{
    GList *names;

    names = gnome_uri_list_extract_filenames((char *)selection_data->data);
    while (names)
    {
        txt_append_file(GTK_TEXT(entry), (gchar *)names->data);
        names = names->next;
    }
    note_changed();
}

/* ------------------------------------------------------ */
/*
 * drop-request on the todo-list
 */

void
cb_todo_list_drop(GtkWidget *todo, GdkDragContext *context, gint x, gint y,
                  GtkSelectionData *selection_data, guint info, guint time)
{
    GList     *names;
    note_data *note;
    
    names = gnome_uri_list_extract_filenames((char *)selection_data->data);
    while (names)
    {
        note = make_note_from_file((gchar *)names->data);
        do_add_note(note, selected_node);
        names = names->next;
    }
    sp->modified = TRUE;
}

/* ------------------------------------------------------ */
/*
 * drop-request on the note-tree
 */

void
cb_note_tree_drop(GtkWidget *tree, GdkDragContext *context, gint x, gint y,
                  GtkSelectionData *selection_data, guint info, guint time)
{
    GList *names;
    
    names = gnome_uri_list_extract_filenames((char *)selection_data->data);
    while (names)
    {
        load_notes_from_cli((gchar *)names->data, GTK_CTREE(tree),
                            selected_node);
        names = names->next;
    }
    sp->modified = TRUE;
}

/* ------------------------------------------------------ */
/*
 * activate-signal from sp->title_entry, sp->text_entry
 */

void
cb_finished_note(GtkWidget *w, gpointer p)
{
    if (GTK_WIDGET_IS_SENSITIVE(sp->b_ok))
    {
        gtk_widget_grab_focus(sp->b_ok);
    }
    else
    {
        gtk_widget_grab_focus(sp->b_cancel);
    }
}

/* ------------------------------------------------------ */
/*
 * unselect row in tree to make adding new trees more easy
 */

void
cb_note_tree_col_selected(GtkCList *l, gint i)
{
    if (l->selection != NULL)
    {        
        gtk_clist_unselect_all(l);
    }
}

/* ------------------------------------------------------ */
/*
 * a row in note_tree was moved
 */

void
cb_note_tree_reordered(GtkWidget *tree, GdkDragContext *context, gint x,
	  gint y, GtkSelectionData *selection_data, guint info, guint time)
{
    sp->modified = TRUE;
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_todo_modify_text(GtkWidget *w, gpointer p)
{
    modify_note(TextNote, tree_node_from_todo_selection());
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_todo_modify_check(GtkWidget *w, gpointer p)
{
    modify_note(CheckNote, tree_node_from_todo_selection());
}

/* ------------------------------------------------------ */
/*
 * proxy func
 */

void
cb_todo_modify_todo(GtkWidget *w, gpointer p)
{
    modify_note(TodoNote, tree_node_from_todo_selection());
}

/* ------------------------------------------------------ */
/*
 * return the correspoding tree-node from the sp->todolist
 */

GtkCTreeNode *
tree_node_from_todo_selection(void)
{
    GtkCTreeNode *snode;
    GList        *slist;
    gint         sitem;
    note_data    *note;
    
    snode = NULL;
    slist = GTK_CLIST(sp->todolist)->selection;
    if (slist != NULL)
    {
        sitem = (gint) slist->data;
        note = (note_data *)gtk_clist_get_row_data(GTK_CLIST(sp->todolist), sitem);
        snode = gtk_ctree_find_by_row_data(GTK_CTREE(sp->note_tree), NULL, note);
    }
    
    return (snode);
}

/* ------------------------------------------------------ */
/*
 * return the correspoding tree-node from the sp->todolist
 * and give a message if no note is selected
 */


GtkCTreeNode *
tree_node_from_todo_selection_msg(void)
{
    GtkCTreeNode *snode;

    snode = tree_node_from_todo_selection();
    if (snode == NULL)
    {
        gnome_ok_dialog_parented(_("Select a note first!"), 
                                 yank_root_win(NULL));
    }
    return (snode);
}

/* ------------------------------------------------------ */
/*
 * search corresponding note in tree, expand all parents and select it
 */

void
cb_todo_open_corr(GtkWidget *w, gpointer p)
{
    GtkCTreeNode *target;
    GtkCTreeNode *parent;

    target = tree_node_from_todo_selection_msg();
    if (target != NULL)
    {
        parent = GTK_CTREE_ROW(target)->parent;
        while (parent)
        {
            gtk_ctree_expand(GTK_CTREE(sp->note_tree), parent);
            parent = GTK_CTREE_ROW(parent)->parent;
        }
        gtk_ctree_select(GTK_CTREE(sp->note_tree), target);
    }
}

/* ------------------------------------------------------ */
/*
 * ask whether to commit uncommitted changes
 */

void
ask_commit_changes(void)
{
    static GtkWidget *dialog = NULL;
    note_data        *data;
    gchar            *msg;
    gint             sel;
    yank_preferences *pref;
 
    pref = get_preferences();

    if ((notetype != None) && (uncommitted))
    {
	if (editmode == Add)
	{
	    data = fill_note_data_from_page(notetype);
	}
	else
	{
	    data = (note_data *)gtk_ctree_node_get_row_data(
				GTK_CTREE(sp->note_tree), selected_node);
	}
        if (! pref->auto_save_modified_notes)
 	{
            msg = g_strdup_printf(
                _("Note '%s' modified. Do you want to commit changes?"),
                data->title);
            dialog = gnome_question_dialog_modal_parented(msg, NULL, NULL,
                                                          yank_root_win(NULL));
            gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
            sel = gnome_dialog_run_and_close(GNOME_DIALOG(dialog));
            g_free(msg);
 	}
 	else
        {
            sel = 0;
        }
        
	if (editmode == Add)
	  free_note(data);
	
        if (sel == 0)
        {
            if (apply_note())
            {
                gnome_warning_dialog_parented(_("Note has not been modified!"),
                                              yank_root_win(NULL));
            }
        }
        if (sel == 1)
        {
            uncommitted = FALSE;
        }
    }
}

/* ------------------------------------------------------ */
/*
 * open text-selection as url
 */

void
cb_open_in_browser(GtkWidget *w, gpointer p)
{
    gchar     *url;
    
    url = get_text_selection(NULL);
    if (url)
    {
        gnome_url_show(url);
    }
}

/* ------------------------------------------------------ */
/*
 * return selection from sp->title_entry or sp->text_entry
 */

gchar*
get_text_selection(GtkWidget **widget)
{
    guint     sstart;
    guint     send;
    gchar     *ret;
    GtkWidget *entry;

    ret = NULL;
    entry = NULL;
    if (selected_node == NULL)
    {
        gnome_ok_dialog_parented(_("Select a note first!"),
                                 yank_root_win(NULL));
        return (NULL);
        /* notreached */
    }
    if (GTK_WIDGET_HAS_FOCUS(sp->title_entry))
    {
        entry = sp->title_entry;
    }
    if (GTK_WIDGET_HAS_FOCUS(sp->text_entry))
    {
        entry = sp->text_entry;
    }
    if (entry == NULL)
    {
        gnome_ok_dialog_parented(_("Select text first!"), yank_root_win(NULL));
        return (NULL);
        /* notreached */
    }
    sstart = GTK_EDITABLE(entry)->selection_start_pos;
    send = GTK_EDITABLE(entry)->selection_end_pos;
    if ((sstart - send) == 0)
    {
        gnome_ok_dialog_parented(_("Select text first!"), yank_root_win(NULL));
    }
    else
    {
        if (widget != NULL)
        {
            *widget = entry;
        }
        if (sstart > send)
        {
            guint tmp;
            
            tmp = send;
            send = sstart;
            sstart = tmp;
        }
        ret = gtk_editable_get_chars(GTK_EDITABLE(entry), sstart, send);
    }
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * view selection as mime
 */

void
cb_view_as_mime(GtkWidget *w, gpointer p)
{
    gchar       *f_name;
    const gchar *mtype;
    const gchar *mprg;
    gchar       *cmd;
    gchar       *cmd_pos;
    gchar       *cmd_full;
    gchar       *cmd_msg;
    pid_t       mpid;
    
    f_name = get_text_selection(NULL);
    if (f_name)
    {
        mtype = gnome_mime_type(f_name);
        mprg = gnome_mime_program(mtype);
        cmd = g_strdup(mprg);
        
        /*
         * not sure if this is the right way to do it ..
         */
        
        cmd_pos = strstr(cmd, "%f");
        if (cmd_pos)
        {
            cmd_pos++;
            *cmd_pos = 's';
        }
        cmd_full = g_strdup_printf(cmd, f_name);
        mpid = fork();
        if (mpid == -1)
        {
            g_warning(_("Can't fork()\n"));
        }
        else
        {
            if (mpid == 0)
            {
                system(cmd_full);
                _exit(0);
            }
        }
        cmd_msg = g_strdup_printf(_("Running : %s"), cmd_full);
        set_status_text(cmd_msg);
        g_free(cmd);
        g_free(cmd_full);
        g_free(cmd_msg);
    }
}

/* ------------------------------------------------------ */
/*
 * run selection as a command
 */

void
cb_run_command(GtkWidget *w, gpointer p)
{
    gchar *command;
    pid_t pid;
    
    command = get_text_selection(NULL);
    if (command)
    {
        pid = fork();
        if (pid == -1)
        {
            g_warning(_("Can't fork()\n"));
        }
        else
        {
            if (pid == 0)
            {
                system(command);
                _exit(0);
            }
        }
    }
}

/* ------------------------------------------------------ */
/*
 * reads status from (any) child
 */
 
void
handle_sigchild(int signum)
{
    gint st;
    
    waitpid(-1, &st, 0);
}

/* ------------------------------------------------------ */
/*
 * find out on which widget in the note the operation should work
 */

GtkWidget*
get_note_cut_copy_paste_focus(void)
{
    GtkWidget *focus;

    focus = NULL;
    if (GTK_WIDGET_HAS_FOCUS(sp->title_entry))
    {
        focus = sp->title_entry;
    }
    else
    {
        if (GTK_WIDGET_HAS_FOCUS(sp->text_entry))
        {
            focus = sp->text_entry;
        }
        else
        {
            gnome_ok_dialog_parented(_("Select text first!"),
                                     yank_root_win(NULL));
        }
    }
    return (focus);
}

/* ------------------------------------------------------ */
/*
 * find out on which widget the operation should work
 */

GtkWidget*
get_cut_copy_paste_focus(void)
{
    GtkWidget *focus;

    focus = NULL;
    
    if (GTK_WIDGET_HAS_FOCUS(sp->title_entry))
    {
        focus = sp->title_entry;
    }
    else
    {
        if (GTK_WIDGET_HAS_FOCUS(sp->text_entry))
        {
            focus = sp->text_entry;
        }
        else
        {
            if (GTK_WIDGET_HAS_FOCUS(sp->note_tree))
            {
                focus = sp->note_tree;
            }
            else
            {
                gnome_ok_dialog_parented(_("Select note or text first!"),
                                         yank_root_win(NULL));
            }
        }
    }
    
    return (focus);
}

/* ------------------------------------------------------ */
/*
 * general cut function
 */

void
cb_edit_cut(GtkWidget *w, gpointer cbdata)
{
    GtkWidget *target;

    target = get_cut_copy_paste_focus();

    if (target == sp->note_tree)
    {
        cb_edit_cut_tree(w, target);
    }
    else
    {
        if (target)
        {
            cb_edit_cut_note(w, target);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * general copy function
 */

void
cb_edit_copy(GtkWidget *w, gpointer cbdata)
{
    GtkWidget *target;

    target = get_cut_copy_paste_focus();

    if (target == sp->note_tree)
    {
        cb_edit_copy_tree(w, target);
    }
    else
    {
        if (target)
        {
            cb_edit_copy_note(w, target);
        }
    }
}
	
/* ------------------------------------------------------ */
/*
 * general paste function
 */

void
cb_edit_paste(GtkWidget *w, gpointer cbdata)
{
    GtkWidget    *target;
    
    target = get_cut_copy_paste_focus();

    if (target == sp->note_tree)
    {
        cb_edit_paste_tree(w, target);
    }
    else
    {
        if (target)
        {
            cb_edit_paste_note(w, target);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * general select function
 */

void
cb_edit_selall(GtkWidget *w, gpointer cbdata)
{
    GtkWidget *target;

    target = get_cut_copy_paste_focus();

    if (target == sp->note_tree)
    {
        cb_edit_selall_tree(w, target);
    }
    else
    {
        if (target)
        {
            cb_edit_selall_note(w, target);
        }
    }    
}

/* ------------------------------------------------------ */
/*
 * cut text to clipboard
 */

void
cb_edit_cut_note(GtkWidget *widget, gpointer cbdata)
{
    GtkWidget *w;

    w = cbdata;
    if (!cbdata)
    {
        w = get_note_cut_copy_paste_focus();
    }
    if (w)
    {
        g_return_if_fail(GTK_IS_EDITABLE(w));
        gtk_editable_cut_clipboard(GTK_EDITABLE(w));
        set_status_text(_("Text selection Cut..."));
    }
}

/* ------------------------------------------------------ */
/*
 * copy text to clipboard
 */

void
cb_edit_copy_note(GtkWidget *widget, gpointer cbdata)
{
    GtkWidget *w;

    w = cbdata;
    if (!cbdata)
    {
        w = get_note_cut_copy_paste_focus();
    }
    if (w)
    {
        g_return_if_fail(GTK_IS_EDITABLE(w));
        gtk_editable_copy_clipboard(GTK_EDITABLE(w));
        set_status_text(_("Text selection Copied..."));
    }
}

/* ------------------------------------------------------ */
/*
 * paste text from clipboard
 */

void
cb_edit_paste_note(GtkWidget *widget, gpointer cbdata)
{
    GtkWidget *w;

    w = cbdata;
    if (!cbdata)
    {
        w = get_note_cut_copy_paste_focus();
    }
    if (w)
    {
        g_return_if_fail(GTK_IS_EDITABLE(w));
        gtk_editable_paste_clipboard(GTK_EDITABLE(w));
        set_status_text(_("Text selection Pasted..."));
    }
}

/* ------------------------------------------------------ */
/*
 * select all text in the note-widget
 * defaults to the text-entry
 */

void
cb_edit_selall_note(GtkWidget *widget, gpointer cbdata)
{
    GtkWidget *w;
    gint len;

    w = cbdata;
    
    if (!cbdata)
    {
        if (GTK_WIDGET_HAS_FOCUS(sp->title_entry))
        {
            w = sp->title_entry;
        }
        else
        {
            w = sp->text_entry;
        }
    }
    
    g_return_if_fail(GTK_IS_EDITABLE(w));

    if (GTK_IS_ENTRY(w))
    {
        len = strlen(gtk_entry_get_text(GTK_ENTRY(w)));
        if (len)
        {
            gtk_entry_select_region(GTK_ENTRY(w), 0, len);
        }
    }
    else
    {
        if (GTK_IS_TEXT(w))
        {
            len = gtk_text_get_length(GTK_TEXT(w));
            if (len)
            {
                gtk_editable_select_region(GTK_EDITABLE(w), 0, len);
            }
        }    
    }
}

/* ------------------------------------------------------ */
/*
 * cut selected notes
 */

void
cb_edit_cut_tree(GtkWidget *w, gpointer cbdata)
{
    if (GTK_CLIST(sp->note_tree)->selection == NULL)
    {
        return;
    }

    if (sp->edit_tree != NULL)
    {
        edit_tree_node_clear(sp->edit_tree, NULL);
    }
    sp->edit_tree = g_node_new(NULL);
    gtk_ctree_pre_recursive(GTK_CTREE(sp->note_tree), NULL, (GtkCTreeFunc)
                            (*tree_copy_selection), sp->edit_tree);
    gtk_ctree_post_recursive(GTK_CTREE(sp->note_tree), NULL, (GtkCTreeFunc)
                             (*tree_cut_selection), NULL);
    edit_tree_free = TRUE;
}

/* ------------------------------------------------------ */
/*
 * copy selected notes
 */

void
cb_edit_copy_tree(GtkWidget *w, gpointer cbdata)
{    
    if (GTK_CLIST(sp->note_tree)->selection == NULL)
    {
        return;
    }

    if (sp->edit_tree != NULL)
    {
        edit_tree_node_clear(sp->edit_tree, NULL);
    }
    sp->edit_tree = g_node_new(NULL);
    gtk_ctree_pre_recursive(GTK_CTREE(sp->note_tree), NULL, (GtkCTreeFunc)
                            (*tree_copy_selection), sp->edit_tree);
    edit_tree_free = FALSE;
}

/* ------------------------------------------------------ */
/*
 * paste notes from edit_tree
 */

void
cb_edit_paste_tree(GtkWidget *w, gpointer cbdata)
{
    static GList *sel_list = NULL;
    GList        *sel;
    GtkCTreeNode *node;

    sel = GTK_CLIST(sp->note_tree)->selection;
    if (sp->edit_tree != NULL && sel)
    {
        g_list_free(sel_list);
        sel_list = NULL;
        while (sel)
        {
            sel_list = g_list_append(sel_list, sel->data);
            sel = g_list_next(sel);
        }
        
        sel_list = g_list_first(sel_list);        
        while (sel_list)
        {
            node = GTK_CTREE_NODE(sel_list->data);
            tree_paste_selection(node, sp->edit_tree);
            sel_list = g_list_next(sel_list);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * select all tree items
 */

void
cb_edit_selall_tree(GtkWidget *widget, gpointer cbdata)
{
    gtk_signal_handler_block_by_func(GTK_OBJECT(sp->note_tree),
                                     GTK_SIGNAL_FUNC(note_tree_row_selected),
                                     NULL);
    gtk_clist_freeze(GTK_CLIST(sp->note_tree));
    gtk_clist_select_all(GTK_CLIST(sp->note_tree));
    gtk_clist_thaw(GTK_CLIST(sp->note_tree));
    gtk_signal_handler_unblock_by_func(GTK_OBJECT(sp->note_tree),
                                       GTK_SIGNAL_FUNC(note_tree_row_selected),
                                       NULL);    
}

/* ------------------------------------------------------ */
/*
 * copy pointer of selected notes
 * this function is designed to waste cpu time
 */

void
tree_copy_selection(GtkCTree *source, GtkCTreeNode *node, gpointer dest)
{
    note_data    *data;
    GList        *selection;
    gint         found;
    GtkCTreeNode *n_parent;
    GNode        *parent;
    GNode        *new_node;
    
    g_return_if_fail(GTK_IS_CTREE(source));

    /*
     * only work on selected nodes (yes, this _is_ suboptimal)
     */
    
    selection = g_list_find(GTK_CLIST(source)->selection, node);

    if (selection)
    {
        /*
         * search 1.st parent in selection list
         */

        found = FALSE;
        n_parent = GTK_CTREE_NODE(selection->data);
        n_parent = GTK_CTREE_ROW(n_parent)->parent;
        while (n_parent && found != TRUE)
        {

            if (g_list_find(GTK_CLIST(source)->selection, n_parent))
            {
                found = TRUE;
            }
            else
            {
                n_parent = GTK_CTREE_ROW(n_parent)->parent;
            }

        }

        /*
         * find parent in edit tree
         */
        
        if (found)
        {
            data = gtk_ctree_node_get_row_data(
                source, GTK_CTREE_NODE(n_parent));
            parent = g_node_find((GNode*)dest, G_PRE_ORDER, G_TRAVERSE_ALL,
                                 data);
        }
        else
        {
            parent = (GNode*) dest;
        }

        /*
         * append pointer to copy-list
         */
        
        data = gtk_ctree_node_get_row_data(source,
                                           GTK_CTREE_NODE(selection->data));
        new_node = g_node_new(data);
        g_node_append(parent, new_node);
    }
}

/* ------------------------------------------------------ */
/*
 * clear gnode-tree from tree
 */

void
edit_tree_node_clear(GNode *tree, gpointer p)
{
    GNode *node;

    if (tree)
    {
        node = tree;
        if (node->children)
        {
            edit_tree_node_clear(node->children, NULL);
        }
        if (node->next);
        {
            edit_tree_node_clear(node->next, NULL);
        }

        if (edit_tree_free == TRUE)
        {
            free_note((note_data*)node->data);
        }
        g_node_destroy(node);
    }
}

/* ------------------------------------------------------ */
/*
 * return a pointer to a copy of the source
 */

note_data*
copy_note(note_data *source)
{
    note_data *note;

    if (source == NULL)
    {
        return (NULL);
        /* notreached */
    }
    
    note = (note_data *) malloc(sizeof (note_data));
    if (note == NULL)
    {
        g_warning(_("Error while allocating memory for note"));
        return (NULL);
        /* notreached */
    }
    note->notetype = source->notetype;
    note->title = g_strdup(source->title);
    note->text = g_strdup(source->text);
    note->expire = source->expire;
    note->id = create_note_id();
    note->created = time(NULL);
    note->changed = note->created;
    note->changes = 0;
    
    if (source->notetype == CheckNote || source->notetype == TodoNote)
    {
        note->todo = source->todo;
    }
    else
    {
        note->todo = 0;
    }    
    if (source->notetype == TodoNote)
    {
        note->deadline = source->deadline;
        note->prio = source->prio;
        note->complete = source->complete;
    }
    else
    {
        note->deadline = 0;
        note->prio = 0;
        note->complete = 0;
    }
    
    return (note);
}

/* ------------------------------------------------------ */
/*
 * generate notes from source
 */

void
tree_paste_selection(GtkCTreeNode *parent, GNode *source)
{
    note_data    *new_data;
    GtkCTreeNode *new_child;
    
    if (source)
    {        
        new_data = NULL;
        new_child = parent;
        if (source->data)
        {
            new_data = copy_note((note_data*)source->data);
            new_child = do_add_note(new_data, parent);
        }

        if (source->children)
        {
            tree_paste_selection(new_child, source->children);
        }
        if (source->next)
        {
            tree_paste_selection(parent, source->next);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * save all children of node by moving them to the parent
 */

void
tree_node_save_childs(GtkCTree *tree, GtkCTreeNode *node, gpointer p)
{
    GtkCTreeNode *childs;
    GtkCTreeNode *parent;

    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(node != NULL);
    
    parent = GTK_CTREE_ROW(node)->parent;
    
    /* prevents gtk from warning us */
    if (GTK_CTREE_ROW(node)->children)
    {
        /* move childs */
        while ((childs = GTK_CTREE_ROW(node)->children) != NULL)
        {
            gtk_ctree_expand_recursive(GTK_CTREE(tree),
                                       GTK_CTREE_ROW(childs)->parent);
            gtk_ctree_move(GTK_CTREE(tree), childs, parent, NULL);
        }
    }    
}

/* ------------------------------------------------------ */
/*
 * cut selected notes from the tree
 */

void
tree_cut_selection(GtkCTree *source, GtkCTreeNode *node, gpointer p)
{
    static note_data *data;
    GList            *selection;

    g_return_if_fail(source != NULL);
    g_return_if_fail(GTK_IS_CTREE(source));
    g_return_if_fail(node != NULL);

    /*
     * only work on selected nodes (yes, this _is_ suboptimal)
     */

    selection = g_list_find(GTK_CLIST(source)->selection, node);
    
    if (selection)
    {
        tree_node_save_childs(source, GTK_CTREE_NODE(selection->data), NULL);

        /*
         * update sp->todolist
         */
        
        data = gtk_ctree_node_get_row_data(source,
                                           GTK_CTREE_NODE(selection->data));
        if (data->notetype == TodoNote && data->todo == 0)
        {
            delete_todo_item(data);
        }
        
        gtk_ctree_remove_node(source, GTK_CTREE_NODE(selection->data));
        sp->modified = TRUE;
    }
}

/* ------------------------------------------------------ */
/*
 * sort subtrees of selected notes
 */

void
cb_sort_subtree(GtkWidget *w, gpointer p)
{
    GList *sel;
    
    sel = GTK_CLIST(sp->note_tree)->selection;

    while (sel)
    {
        gtk_ctree_sort_node(GTK_CTREE(sp->note_tree), GTK_CTREE_NODE(sel->data));
        sel = g_list_next(sel);
    }
}

/* ------------------------------------------------------ */
/*
 * toggle visibility of extra note flags
 */

void
cb_toggle_ext_flags(GtkWidget *button, gpointer box)
{
    GtkWidget *up;
    GtkWidget *down;
    
    g_return_if_fail(GTK_IS_BUTTON(button));
    g_return_if_fail(GTK_IS_HBOX(box));

    up = gtk_object_get_data(GTK_OBJECT(button), "up");
    down = gtk_object_get_data(GTK_OBJECT(button), "down");
    
    if (GTK_WIDGET_VISIBLE(GTK_WIDGET(box)) == FALSE)
    {
        gtk_widget_show(up);
        gtk_widget_hide(down);
        gtk_widget_show(GTK_WIDGET(box));
    }
    else
    {
        gtk_widget_show(down);
        gtk_widget_hide(up);
        gtk_widget_hide(GTK_WIDGET(box));
    }
}

/* ------------------------------------------------------ */
/*
 * auto-update done flag
 */

void
cb_complete_updates_done(GtkWidget *ad_complete, gpointer todo_check)
{
    if ((notetype == TodoNote)
        && (get_preferences()->completing_todo_note_sets_done))
    {
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(todo_check),
            ((GTK_ADJUSTMENT(ad_complete)->value == 100) ? 1 : 0));
    }
}

/* ------------------------------------------------------ */
/*
 * guess what it does
 */

void
cb_yank_homepage(GtkWidget *w, gpointer p)
{
    gnome_url_show(YANK_HOMEPAGE);
}

/* ------------------------------------------------------ */
/*
 * walk backwards in the tree and recalculate the completion status
 */

void
tree_recursive_completion(GtkCTree *tree, GtkCTreeNode *node)
{
    note_data *note;
    note_data *m_note;
    
    g_return_if_fail(tree != NULL);
    g_return_if_fail(GTK_IS_CTREE(tree));
    g_return_if_fail(node != NULL);

    if (calc_completion_status(tree, node) == 1)
    {
        /*
         * update note
         */
        
        note = (note_data *)gtk_ctree_node_get_row_data(tree, node);
        m_note = copy_note(note);
        if (m_note != NULL)
        {
            m_note->created = note->created;
            m_note->changed = time(NULL);
            m_note->changes = note->changes + 1;
            
            get_preferences()->recursive_completion = 0;
            do_edit_note(m_note, node);
            get_preferences()->recursive_completion = 1;
        }
        else
        {
            gnome_warning_dialog_parented(
                _("recursive completion: can't copy note"),
                yank_root_win(NULL));
        }
    }
    
    /*
     * continue with parent
     */
    
    if (GTK_CTREE_ROW(node)->parent != NULL)
    {
        tree_recursive_completion(tree, GTK_CTREE_ROW(node)->parent);
    }
}

/* ------------------------------------------------------ */
/*
 * calculate the completion status
 * returns:  0 - no changes
 *           1 - note changed
 *          -1 - error
 */

gint
calc_completion_status(GtkCTree *tree, GtkCTreeNode *node)
{
    GtkCTreeNode *child;
    note_data    *note;
    note_data    *c_note;
    glong        todonotes;
    glong        ttodonotes;
    gfloat       checknotes;
    gfloat       tchecknotes;
    glong        sum_todo_comp;
    gint         ret;
    gint         complete;
    gint         todo;
    
    g_return_val_if_fail(tree != NULL, -1);
    g_return_val_if_fail(GTK_IS_CTREE(tree), -1);
    g_return_val_if_fail(node != NULL, -1);

    note = (note_data *)gtk_ctree_node_get_row_data(tree, node);
    
    if (note == NULL)
    {
        gnome_warning_dialog_parented(
            _("calc completion status: Note without data!"),
            yank_root_win(NULL));
        return (-1);
        /* notreached */
    }

    if ((note->notetype != CheckNote) && (note->notetype != TodoNote))
    {
        return (0);
        /* notreached */
    }
    
    child = GTK_CTREE_ROW(node)->children;
    if (child == NULL)
    {
        return (0);
        /* notreached */
    }
    
    ret = 0;

    todonotes = 0;
    ttodonotes = 0;
    checknotes = 0;
    tchecknotes = 0;
    sum_todo_comp = 0;
    
    /*
     * scan childs
     */

    do
    {
        c_note = (note_data *)gtk_ctree_node_get_row_data(tree, child);
        if (c_note == NULL)
        {
            gnome_warning_dialog_parented(
                _("calc completion status (2): Note without data!"),
                yank_root_win(NULL));
            continue;
        }

        switch (c_note->notetype)
        {
        case CheckNote:
            checknotes++;
            if (c_note->todo == 0)
            {
                tchecknotes++;
            }
            break;
        case TodoNote:
            todonotes++;
            sum_todo_comp += c_note->complete;
            if (c_note->todo == 0)
            {
                ttodonotes++;
            }
            break;
        default:
            break;
        };
    } while ((child = GTK_CTREE_ROW(child)->sibling) != NULL);

    /*
     * guess todo/ completion values
     */
    
    switch (note->notetype)
    {
    case CheckNote:
        if ((checknotes + todonotes) > 0)
        {
            if (todonotes > 0)
            {
                todo = (((tchecknotes + ttodonotes) == 0) &&
                        ((sum_todo_comp / todonotes) == 100)) ? 1 : 0;
            }
            else
            {
                todo = ((tchecknotes + ttodonotes) == 0)  ? 1 : 0;
            }

            /*
             * update note
             */
            
            if (todo != note->todo)
            {
                note->todo = todo;
                ret = 1;
            }
        }
        break;

    case TodoNote:
        if (todonotes > 0)
        {
            complete = (sum_todo_comp / todonotes) - ttodonotes - tchecknotes;
        }
        else
        {
            if (checknotes > 0)
            {
                complete = ((checknotes - tchecknotes) / checknotes) * 100;
            }
            else
            {
                complete = note->complete;
            }
        }

        /*
         * update note
         */
        
        if (complete < 0)
        {
            complete = 0;
        }
        if (complete > 100)
        {
            complete = 100;
        }
        if (complete != note->complete)
        {
            note->complete = complete;
            ret = 1;
        }
        if (get_preferences()->completing_todo_note_sets_done)
        {
            todo = (complete == 100) ? 1 : 0;
        }
        else
        {
            todo = note->todo;
        }
        if (todo != note->todo)
        {
            note->todo = todo;
            ret = 1;
        }
        break;

    default:
        break;
    };
    
    return (ret);
}

/* ------------------------------------------------------ */
