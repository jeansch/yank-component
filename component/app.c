/*
 * app.c
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

#include  <gal/e-paned/e-hpaned.h>
#include  <gal/e-paned/e-vpaned.h>

#include "e-yank.h"
  
#include "app.h"
#include "text.xpm"
#include "box.xpm"
#include "box2.xpm"
#include "circle.xpm"
#include "circle2.xpm"
#include "privates.h"

/*
 * widgets
 */


SuperPrivate *sp;


/*
 * pixmaps
 */

GtkWidget *pix_text_xpm;
GtkWidget *pix_box_xpm;
GtkWidget *pix_box2_xpm;
GtkWidget *pix_circle_xpm;
GtkWidget *pix_circle2_xpm;

/*
 * prototypes
 */

static GtkWidget  *gui_ext_flags(void);
static gint       determine_use_fontset(void);
static void       set_font_str(GtkWidget *, gchar *);
static void       set_font(GtkWidget *, GdkFont *);


/* ------------------------------------------------------ */
/*
 * draw the main gui
 */

  
 GtkWidget * yank_app_new(EYank *yank) 
{
    yank_preferences      *prefs;


    static GtkTargetEntry drop_types[] =
    {
        { "text/uri-list", 0, 1 },
    };
    static gint n_drop_types = sizeof (drop_types)/sizeof (drop_types [0]);
    gchar     *treetitle[] =
    {
        _("NoteTree"),
        NULL
    };
    gchar     *todotitles[] =
    {
        _("Deadline"),
        _("Priority"),
        _("Complete"),
        _("Title"),
        NULL
    };
     GtkWidget   *app; 
    GtkWidget   *scrolledwindow1;
    GtkWidget   *scrolledwindow2;
    GtkWidget   *scrolledwindow3;
    GtkWidget   *hbox;
    GtkWidget   *hbox3;
    GtkWidget   *vbox;
    GtkWidget   *label;
    GdkFont     *font;
    GtkObject   *prio_adjust;
/*     gint        got_crash; */
    GtkWidget   *b_toggle_ext_flags;
    GtkWidget   *hbox_ext_flags;
		GtkWidget		*hpaned;
    
		hpaned = app = yank->priv->hpaned; 
    yank_main_app(app);  


	 sp->edit_tree = NULL;

	 sp->title_entry =NULL;
	 sp->todo_check =NULL;
	 sp->prio_entry =NULL;
	 sp->de_deadline =NULL;
	 sp->ad_complete =NULL;
	 sp->tlabel_created =NULL;
	 sp->tlabel_changed =NULL;
	 sp->label_changes =NULL;
	 sp->tlabel_expire =NULL;
	 sp->note_id_entry =NULL;
	 sp->text_entry =NULL;
	 sp->note_tree =NULL;
	 sp->b_ok =NULL;
	 sp->b_apply =NULL;
	 sp->b_cancel =NULL;
	 sp->todolist =NULL; 
	 sp->notebook =NULL;
	 sp->status =NULL;  
	 sp->prio_date_box =NULL; 
	 sp->edit_tree=NULL;

   
    yank_root_win(GTK_WINDOW(app)); 
   


    /*
     * main tree
     */
    
    scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    /* gtk_widget_show will be called later */
/*     gtk_container_add(GTK_CONTAINER(hpaned), scrolledwindow1); */
		e_paned_add1(E_PANED(hpaned), scrolledwindow1);

    sp->note_tree = gtk_ctree_new_with_titles(1, 0, treetitle);
    gtk_widget_show(sp->note_tree);
    gtk_container_add(GTK_CONTAINER(scrolledwindow1), sp->note_tree);
    gtk_clist_set_column_width(GTK_CLIST(sp->note_tree), 0, 80);
    gtk_clist_set_column_auto_resize(GTK_CLIST(sp->note_tree), 0, TRUE);
    gtk_clist_set_row_height(GTK_CLIST(sp->note_tree), 19);
    gtk_clist_set_selection_mode(GTK_CLIST(sp->note_tree), GTK_SELECTION_EXTENDED);
    gtk_clist_set_reorderable(GTK_CLIST(sp->note_tree), TRUE);
    gtk_clist_set_use_drag_icons(GTK_CLIST(sp->note_tree), TRUE);
    gtk_clist_column_titles_show(GTK_CLIST(sp->note_tree));
    gtk_signal_connect(GTK_OBJECT(sp->note_tree), "tree_select_row",
                       GTK_SIGNAL_FUNC (note_tree_row_selected), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->note_tree), "tree_unselect_row",
                       GTK_SIGNAL_FUNC (note_tree_row_unselected), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->note_tree), "click_column",
                       GTK_SIGNAL_FUNC (cb_note_tree_col_selected), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->note_tree), "drag_data_received",
		       GTK_SIGNAL_FUNC(cb_note_tree_reordered), NULL);
    /*
     * FIXME:
     * dnd on the note-tree disables reordering of notes by dnd
     */
/*     gtk_signal_connect(GTK_OBJECT(sp->note_tree), "drag_data_received", */
/*                        GTK_SIGNAL_FUNC(cb_note_tree_drop), NULL); */
/*     gtk_drag_dest_set(sp->note_tree, GTK_DEST_DEFAULT_MOTION | */
/*                       GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP, */
/*                       drop_types, n_drop_types, GDK_ACTION_COPY); */

    sp->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(sp->notebook), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(sp->notebook), FALSE);
    gtk_widget_show(sp->notebook);
/*     gtk_container_add(GTK_CONTAINER(hpaned), sp->notebook); */
		e_paned_add2(E_PANED(hpaned), sp->notebook);

    /*
     * todo-list
     */
    
    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_show(scrolledwindow2);
    gtk_container_add(GTK_CONTAINER(sp->notebook), scrolledwindow2);
    
    sp->todolist = gtk_clist_new_with_titles(4, todotitles);
    gtk_signal_connect(GTK_OBJECT(sp->todolist), "select_row",
                       GTK_SIGNAL_FUNC (cb_todo_row_selected), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->todolist), "click_column",
                       GTK_SIGNAL_FUNC (cb_todo_col_selected), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->todolist), "drag_data_received",
                       GTK_SIGNAL_FUNC(cb_todo_list_drop), NULL);
    gtk_drag_dest_set(sp->todolist, GTK_DEST_DEFAULT_MOTION |
                      GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
                      drop_types, n_drop_types, GDK_ACTION_COPY);    
    gtk_widget_show(sp->todolist);
    gtk_container_add(GTK_CONTAINER(scrolledwindow2), sp->todolist);
    gtk_clist_column_titles_show(GTK_CLIST(sp->todolist));
    gtk_clist_set_column_auto_resize(GTK_CLIST(sp->todolist), 0 , TRUE);
    gtk_clist_set_column_auto_resize(GTK_CLIST(sp->todolist), 3 , TRUE);
    gtk_clist_set_column_justification(GTK_CLIST(sp->todolist), 1,
                                       GTK_JUSTIFY_RIGHT);
    gtk_clist_set_column_justification(GTK_CLIST(sp->todolist), 2,
                                       GTK_JUSTIFY_RIGHT);
    

    /*
     * text/ data
     */
    
    vbox = gtk_vbox_new(FALSE, 3);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER (sp->notebook), vbox);
    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

    sp->prio_date_box = gtk_vbox_new(FALSE, 3);
    gtk_widget_show(sp->prio_date_box);
    gtk_box_pack_start(GTK_BOX(vbox), sp->prio_date_box, FALSE, FALSE, 0);
    
    hbox_ext_flags = gui_ext_flags();
    gtk_box_pack_start(GTK_BOX(vbox), hbox_ext_flags, FALSE, FALSE, 3);

    label = gtk_label_new(_("Title:"));
    font = gtk_widget_get_style(label)->font;
    gtk_widget_set_usize(label, -1, gdk_string_height(font, "W") + 10);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

    sp->title_entry = gtk_entry_new();
    gtk_widget_set_usize(sp->title_entry, -1, gdk_string_height(font, "W") + 10);
    gtk_widget_show(sp->title_entry);
    gtk_box_pack_start(GTK_BOX(hbox), sp->title_entry, TRUE, TRUE, 5);

    sp->todo_check = gtk_check_button_new_with_label(_("Done"));
    gtk_widget_set_usize(sp->todo_check, -1, gdk_string_height(font, "W") + 10);
    gtk_widget_show(sp->todo_check);
    gtk_box_pack_start(GTK_BOX(hbox), sp->todo_check, FALSE, FALSE, 5);

    /*
     * button for extra flags& data
     */
    
    b_toggle_ext_flags = gtk_button_new();
    {
        GtkWidget *p_up;
        GtkWidget *p_down;
        GtkWidget *box;
        
        box = gtk_hbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(b_toggle_ext_flags), box);
        gtk_widget_show(box);
        p_up = gnome_stock_pixmap_widget_at_size(
            GTK_WIDGET(yank_root_win(NULL)), GNOME_STOCK_BUTTON_UP, 12, 14);
        p_down = gnome_stock_pixmap_widget_at_size(
            GTK_WIDGET(yank_root_win(NULL)), GNOME_STOCK_BUTTON_DOWN, 12, 14);
        gtk_widget_hide(p_up);
        gtk_widget_show(p_down);
        gtk_container_add(GTK_CONTAINER(box), p_up);
        gtk_container_add(GTK_CONTAINER(box), p_down);
        gtk_object_set_data(GTK_OBJECT(b_toggle_ext_flags), "up", p_up);
        gtk_object_set_data(GTK_OBJECT(b_toggle_ext_flags), "down", p_down);
    }
    gtk_widget_show(b_toggle_ext_flags);
    gtk_box_pack_start(GTK_BOX(hbox), b_toggle_ext_flags, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(b_toggle_ext_flags), "clicked",
                       GTK_SIGNAL_FUNC(cb_toggle_ext_flags), hbox_ext_flags);
    
    /*
     * deadline/ prio/ complete
     */
    
    {
        GtkWidget *prio_hbox1;
        GtkWidget *prio_hbox2;
        GtkWidget *sc_complete;
        
        prio_hbox1 = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(prio_hbox1);
        gtk_box_pack_start(GTK_BOX(sp->prio_date_box), prio_hbox1, FALSE, FALSE,
                           0);
        prio_hbox2 = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(prio_hbox2);
        gtk_box_pack_start(GTK_BOX(sp->prio_date_box), prio_hbox2, FALSE, FALSE,
                           0);
        
        label = gtk_label_new(_("Deadline:"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(prio_hbox1), label, FALSE, FALSE, 5);
        sp->de_deadline = gnome_date_edit_new(0, 1, 1);
        gnome_date_edit_set_popup_range(GNOME_DATE_EDIT(sp->de_deadline), 0, 23);
        gtk_widget_set_sensitive(GNOME_DATE_EDIT(sp->de_deadline)->date_entry,
                                 FALSE);
        gtk_widget_set_sensitive(GNOME_DATE_EDIT(sp->de_deadline)->time_entry,
                                 FALSE);
        gtk_widget_show(sp->de_deadline);
        gtk_box_pack_start(GTK_BOX(prio_hbox1), sp->de_deadline, FALSE, FALSE, 0);
        
        label = gtk_label_new(_("Priority:"));
        gtk_widget_set_usize(label, -1, gdk_string_height(font, "W") + 10);
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(prio_hbox1), label, FALSE, FALSE, 5);
        
        prio_adjust = gtk_adjustment_new(0, 0, 9999, 1, 10, 10);
        sp->prio_entry = gtk_spin_button_new(GTK_ADJUSTMENT(prio_adjust), 1, 0);
        /* NOTE: this is note 100% ok */
        gtk_widget_set_usize(GTK_WIDGET(sp->prio_entry),
                             gdk_string_width(font, "999999") + 10,
                             gdk_string_height(font, "W") + 10);
        gtk_widget_show(sp->prio_entry);
        gtk_box_pack_start(GTK_BOX(prio_hbox1), sp->prio_entry, FALSE, FALSE, 0);
        
        label = gtk_label_new(_("Complete:"));
        gtk_widget_set_usize(label, -1, gdk_string_height(font, "W") + 10);
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(prio_hbox2), label, FALSE, FALSE, 5);
        
        sp->ad_complete = gtk_adjustment_new(0, 0, 101, 1, 1, 1);
        sc_complete = gtk_hscale_new(GTK_ADJUSTMENT(sp->ad_complete));
        gtk_scale_set_digits(GTK_SCALE(sc_complete), 0);
        gtk_scale_set_value_pos(GTK_SCALE(sc_complete), GTK_POS_LEFT);
        gtk_widget_show(sc_complete);
        gtk_box_pack_start(GTK_BOX(prio_hbox2), sc_complete, TRUE, TRUE, 5);

        label = gtk_label_new(_("%"));
        gtk_widget_set_usize(label, -1, gdk_string_height(font, "W") + 10);
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(prio_hbox2), label, FALSE, FALSE, 5);
        
    }
    
    /*
     * additional note data
     */

    sp->note_id_entry = gtk_entry_new();
/*     gtk_widget_show(sp->note_id_entry); */
    gtk_widget_hide(sp->note_id_entry);
    gtk_box_pack_start(GTK_BOX(hbox), sp->note_id_entry, FALSE, FALSE, 5);

    /*
     * note text
     */
    
    scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow3),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_show(scrolledwindow3);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow3, TRUE, TRUE, 0);
                       
    sp->text_entry = gtk_text_new(NULL, NULL);
    gtk_text_set_editable(GTK_TEXT(sp->text_entry), TRUE);
    gtk_widget_show(sp->text_entry);
    gtk_container_add(GTK_CONTAINER(scrolledwindow3), sp->text_entry);

    /* text dnd */
    gtk_signal_connect(GTK_OBJECT(sp->text_entry), "drag_data_received",
                       GTK_SIGNAL_FUNC(cb_text_entry_drop), NULL);
    gtk_drag_dest_set(sp->text_entry, GTK_DEST_DEFAULT_MOTION |
                      GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
                      drop_types, n_drop_types, GDK_ACTION_COPY);

    hbox3 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox3);
    gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 3);

    sp->b_ok = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
    gtk_widget_show(sp->b_ok);
    gtk_box_pack_start(GTK_BOX(hbox3), sp->b_ok, TRUE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(sp->b_ok), "clicked",
                       GTK_SIGNAL_FUNC(cb_b_ok), NULL);

    sp->b_apply = gnome_stock_button(GNOME_STOCK_BUTTON_APPLY);
    gtk_widget_show(sp->b_apply);
    gtk_box_pack_start(GTK_BOX(hbox3), sp->b_apply, TRUE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(sp->b_apply), "clicked",
                       GTK_SIGNAL_FUNC(cb_b_apply), NULL);
        
    sp->b_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
    gtk_widget_show(sp->b_cancel);
    gtk_box_pack_start(GTK_BOX(hbox3), sp->b_cancel, TRUE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(sp->b_cancel), "clicked",
                       GTK_SIGNAL_FUNC(cb_b_cancel), NULL);

    /*
     * a bit suboptimal
     */
    
    gtk_signal_connect(GTK_OBJECT(sp->title_entry), "changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->de_deadline), "date-changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->de_deadline), "time-changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->tlabel_expire), "time_changed",
		       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->prio_entry), "changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->ad_complete), "value_changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->ad_complete), "value_changed",
                       GTK_SIGNAL_FUNC(cb_complete_updates_done), sp->todo_check);
    gtk_signal_connect(GTK_OBJECT(sp->todo_check), "toggled",
                       GTK_SIGNAL_FUNC(note_changed), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->text_entry), "changed",
                       GTK_SIGNAL_FUNC(note_changed), NULL);

    /*
     * keyboard-control
     */
    
    gtk_signal_connect(GTK_OBJECT(sp->title_entry), "activate",
                       GTK_SIGNAL_FUNC(cb_finished_note), NULL);
    gtk_signal_connect(GTK_OBJECT(sp->text_entry), "activate",
                       GTK_SIGNAL_FUNC(cb_finished_note), NULL);

    /*
     * create pixmaps
     */
    
    pix_text_xpm = gnome_pixmap_new_from_xpm_d(text_xpm);
    gtk_widget_show(pix_text_xpm);
    pix_box_xpm = gnome_pixmap_new_from_xpm_d(box_xpm);
    gtk_widget_show(pix_box_xpm);
    pix_box2_xpm = gnome_pixmap_new_from_xpm_d(box2_xpm);
    gtk_widget_show(pix_box2_xpm);
    pix_circle_xpm = gnome_pixmap_new_from_xpm_d(circle_xpm);
    gtk_widget_show(pix_circle_xpm);
    pix_circle2_xpm = gnome_pixmap_new_from_xpm_d(circle2_xpm);
    gtk_widget_show(pix_circle2_xpm);

    /*
     * menus
     */
    
/*     gnome_app_set_contents(GNOME_APP(app), hpaned); */
/*     sp->status = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_USER); */
/*     gnome_app_set_statusbar(GNOME_APP(app), sp->status); */
/*     yank_install_menus_and_toolbar(app); */
    yank_tree_item_context_menu(sp->note_tree);
    yank_todo_item_context_menu(sp->todolist);
    yank_text_item_context_menu(sp->title_entry);
    yank_text_item_context_menu(sp->text_entry);
    
/*     gtk_signal_connect(GTK_OBJECT(app), "delete_event", */
/*                        GTK_SIGNAL_FUNC(cb_delete_event), NULL); */

    /*
     * initialize preferences
     * also preloads plugins
     */
    
    load_preferences();
    prefs = get_preferences();

    gtk_text_set_word_wrap(GTK_TEXT(sp->text_entry), prefs->wordwrap);
    gtk_text_set_line_wrap(GTK_TEXT(sp->text_entry), prefs->linewrap);
    
    
/*     if (prefs->yank_width * prefs->yank_height) */
/*     { */
/*         gtk_window_set_default_size(GTK_WINDOW(app), prefs->yank_width, */
/*                                     prefs->yank_height); */
/*     } */
/*     else */
/*     { */
/*         gtk_window_set_default_size(GTK_WINDOW(app), 600, 400); */
/*     } */
    
    if (prefs->use_custom_font)
    {
	if (prefs->note_font)
        {
	    set_note_font_str(prefs->note_font);
	}
	if (prefs->note_tree_font)
	{
	    set_note_tree_font_str(prefs->note_tree_font);
        }
        if (prefs->todolist_font)
        {
            set_todolist_font_str(prefs->todolist_font);
        }
    }
    else
    {
        set_default_font();
    }
    
    
    /*
     * parse geometry if given
     */
    
/*     if (geometry != NULL) */
/*     { */
/*         gint x, y, w, h; */
/*         if (gnome_parse_geometry(geometry, &x, &y, &w, &h )) */
/*         { */
/*             if (x != -1) */
/*             { */
/*                 gtk_widget_set_uposition(app, x, y); */
/*             } */
/*              */
/*             if (w != -1) */
/*             { */
/*                 gtk_window_set_default_size(GTK_WINDOW(app), w, h); */
/*             } */
/*         } */
/*         else */
/*         { */
/*             g_error(_("Could not parse geometry string `%s'"), geometry); */
/*         } */
/*     } */
/*      */
/*     if (prefs->note_tree_width) */
/*     { */
/*         gtk_widget_set_usize(scrolledwindow1, prefs->note_tree_width, -1); */
/*     } */
/*     else */
/*     { */
/*         gtk_widget_set_usize(scrolledwindow1, 200, -1); */
/*     } */
/*     gtk_widget_show(scrolledwindow1); */

    /*
     * try to read files from killed sessions
     */

/*     got_crash = get_crash_file(); */
    
    
    /*
     * load file from the command-line
     */
    
/*     if (file != NULL && ! got_crash) */
/*     { */
/*         load_notes_from_cli((gchar *)file, GTK_CTREE(sp->note_tree), NULL); */
/*     } */

    /*
     * read default file
     */
    
/*     if (file == NULL && ! got_crash) */
/*     { */
/*         get_def_file(); */
/*     } */
    
    /*
     * register some signal-handlers
     */

/*     if (signal(SIGTERM, handle_sigterm) == SIG_ERR) */
/*     { */
/*         g_warning("Can't handle SIGTERM"); */
/*     } */
/*     if (signal(SIGCHLD, handle_sigchild) == SIG_ERR) */
/*     { */
/*         g_warning("Can't handle SIGCHILD"); */
/*     } */
/*     if (signal(SIGPIPE, handle_sigpipe) == SIG_ERR) */
/*     { */
/*         g_warning("Can't handle SIGPIPE"); */
/*     } */

    /*
     * init autosave
     */
    
/*     if (prefs->auto_save_minutes) */
/*     { */
/*         autosave_notes(NULL); */
/*     } */
    
    return (hpaned);
}

/* ------------------------------------------------------ */
/*
 * adjust the sp->notebook
 */

inline void
show_todo_tab(void)
{
    gtk_notebook_set_page(GTK_NOTEBOOK(sp->notebook), 0);
}

/* ------------------------------------------------------ */
/*
 * adjust the sp->notebook
 */

inline void
show_text_tab(void)
{
    gtk_notebook_set_page(GTK_NOTEBOOK(sp->notebook), 1);
}

/* ------------------------------------------------------ */
/*
 * wipe the text fields
 */

void
clear_text_page(gboolean add, gint *uncommitted)
{
    guint len;
    gint  ucm;

    ucm = *uncommitted;
    gtk_entry_set_text(GTK_ENTRY(sp->title_entry), "");
    if (add == TRUE)
    {
        time_label_set_time(TIME_LABEL(sp->tlabel_created), time(NULL));
    }
    else
    {
        time_label_set_time(TIME_LABEL(sp->tlabel_created), -1);
    }
    time_label_set_time(TIME_LABEL(sp->tlabel_changed), -1);
    if (add == TRUE)
    {
        gtk_label_set_text(GTK_LABEL(sp->label_changes), _("0"));
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(sp->label_changes), "");
    }
    time_label_set_time(TIME_LABEL(sp->tlabel_expire), -1);
    gtk_entry_set_text(GTK_ENTRY(sp->note_id_entry), "");
    gnome_date_edit_set_time(GNOME_DATE_EDIT(sp->de_deadline), 0);
    gtk_entry_set_text(GTK_ENTRY(sp->prio_entry), "");
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(sp->todo_check), FALSE);
    gtk_adjustment_set_value(GTK_ADJUSTMENT(sp->ad_complete), 0);
    len = gtk_text_get_length(GTK_TEXT(sp->text_entry));
    if (len)
    {
        gtk_text_freeze(GTK_TEXT(sp->text_entry));
        gtk_text_set_point(GTK_TEXT(sp->text_entry), 0);
        gtk_text_forward_delete(GTK_TEXT(sp->text_entry), len);
        gtk_text_thaw(GTK_TEXT(sp->text_entry));
    }
    *uncommitted = ucm;
}

/* ------------------------------------------------------ */
/*
 * show the correct portion of the note
 */

void
view_note_as(_notetype type)
{
    switch (type)
    {
    case TextNote:
        gtk_widget_hide(sp->prio_date_box);
        gtk_widget_hide(sp->todo_check);
        break;
    case CheckNote:
        gtk_widget_hide(sp->prio_date_box);
        gtk_widget_show(sp->todo_check);
        break;
    case TodoNote:
        gtk_widget_show(sp->prio_date_box);
        gtk_widget_show(sp->todo_check);
        break;
    case None:
    default:
        g_warning(_("You shouldn't see this warning - please report."));
        break;
    };
}

/* ------------------------------------------------------ */
/*
 * insert data into a struct _note_data
 */

note_data * 
fill_note_data_from_page(_notetype notetype)
{
    note_data *note;
    gchar     *l_ptr;
    
    note = (note_data *) malloc(sizeof (note_data));
    if (note == NULL)
    {
        g_warning(_("Error while allocating memory for note (0)"));
        return (NULL);
    }
    note->notetype = notetype;
    note->title = g_strdup(gtk_entry_get_text(GTK_ENTRY(sp->title_entry)));
    if (note->title == NULL)
    {
        g_warning(_("Error while allocating memory for note (1)"));
        return (NULL);
    }
    note->text = gtk_editable_get_chars(GTK_EDITABLE(sp->text_entry), 0,
                                        gtk_text_get_length(GTK_TEXT(
                                            sp->text_entry)));
    if (note->text == NULL)
    {
        g_warning(_("Error while allocating memory for note (2)"));
        return (NULL);
    }
    if (notetype == CheckNote || notetype == TodoNote)
    {
        note->todo = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(sp->todo_check));
    }
    else
    {
        note->todo = 0;
    }
    
    note->expire = time_label_get_time(TIME_LABEL(sp->tlabel_expire));
    note->created = time_label_get_time(TIME_LABEL(sp->tlabel_created));
    note->changed = time(NULL);
    time_label_set_time(TIME_LABEL(sp->tlabel_changed), note->changed);

    gtk_label_get(GTK_LABEL(sp->label_changes), &l_ptr);
    note->changes = atoi(l_ptr);
    note->changes++;
    l_ptr = g_strdup_printf("%ld", note->changes);
    gtk_label_set_text(GTK_LABEL(sp->label_changes), l_ptr);
    g_free(l_ptr);
    
    note->id = g_strdup(gtk_entry_get_text(GTK_ENTRY(sp->note_id_entry)));
    if (note->id == NULL)
    {
        g_warning(_("Error while allocating memory for note (3)"));
        return (NULL);
    }
    
    if (notetype == TodoNote)
    {
        note->deadline = gnome_date_edit_get_date(
            GNOME_DATE_EDIT(sp->de_deadline));
        note->prio = gtk_spin_button_get_value_as_int(
            GTK_SPIN_BUTTON(sp->prio_entry));
        note->complete = GTK_ADJUSTMENT(sp->ad_complete)->value;

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
 * display the note text
 */

void
fill_page_from_note_data(note_data *note)
{
    gint  len;
    gchar *buff;
    
    if (note->title != NULL)
    {
        gtk_entry_set_text(GTK_ENTRY(sp->title_entry), note->title);
    }
    if (note->text != NULL)
    {
        len = strlen(note->text);
        gtk_text_insert(GTK_TEXT(sp->text_entry), NULL, NULL, NULL, note->text,
                        len);
        gtk_editable_set_position(GTK_EDITABLE(sp->text_entry), 0);
    }

    time_label_set_time(TIME_LABEL(sp->tlabel_expire), note->expire);
    time_label_set_time(TIME_LABEL(sp->tlabel_created), note->created);
    time_label_set_time(TIME_LABEL(sp->tlabel_changed), note->changed);

    buff = g_strdup_printf("%ld", note->changes);
    gtk_label_set_text(GTK_LABEL(sp->label_changes), buff);
    g_free(buff);
    
    if (note->id != NULL)
    {
        gtk_entry_set_text(GTK_ENTRY(sp->note_id_entry), note->id);
    }
    
    if (note->notetype == CheckNote || note->notetype == TodoNote)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sp->todo_check),
                                     note->todo);
    }
    if (note->notetype == TodoNote)
    {
        gnome_date_edit_set_time(GNOME_DATE_EDIT(sp->de_deadline), note->deadline);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(sp->prio_entry), note->prio);

        gtk_signal_handler_block_by_func(
            GTK_OBJECT(sp->ad_complete), GTK_SIGNAL_FUNC(cb_complete_updates_done),
            sp->todo_check);
        gtk_adjustment_set_value(GTK_ADJUSTMENT(sp->ad_complete), note->complete);
        gtk_signal_handler_unblock_by_func(
            GTK_OBJECT(sp->ad_complete), GTK_SIGNAL_FUNC(cb_complete_updates_done),
            sp->todo_check);
    }
}

gchar *yank_uri(gchar *uri) {
	static gchar *ruri = NULL;

	if (uri != NULL) {
		ruri = uri;
	}	
	return ruri;
}

/* ------------------------------------------------------ */
/*
 * get/ set pointer to the root-window
 */

 GtkWindow * 
 yank_root_win(GtkWindow *win) 
 { 
     static GtkWindow *rwin = NULL; 
  
     if (win != NULL) 
     { 
         rwin = win; 
     } 
      
     return (rwin); 
 } 

/* ------------------------------------------------------ */
/*
 * (un-)display toolbar, sp->statusbar
 */

void
show_tool_status(GnomeApp *app, yank_preferences *pref)
{
    GnomeDockItem *gditem;
    
    gditem = gnome_app_get_dock_item_by_name(app, GNOME_APP_TOOLBAR_NAME);
    if (pref->hide_toolbar)
    {
        gtk_widget_hide(GTK_WIDGET(gditem));
    }
    else
    {
        gtk_widget_show(GTK_WIDGET(gditem));
    }
    if (pref->hide_statusbar)
    {
        gtk_widget_hide(app->statusbar);
    }
    else
    {
        gtk_widget_show(app->statusbar);
    }
    gtk_widget_queue_resize(GTK_WIDGET(app));
}

/* ------------------------------------------------------ */
/*
 * reset gui to default sizes
 */

/* void */
/* default_gui_size(GtkWidget *w, gpointer p) */
/* { */
/*     gdk_window_resize(GTK_WIDGET(yank_root_win(NULL))->window, 600, 400); */
    
    /*
     * FIXME: this works only once
     */
    
/*     gtk_widget_set_usize(GTK_PANED(hpaned)->child1, 200, -1); */
/* } */

/* ------------------------------------------------------ */
/*
 * sets the text in sp->statusbar ...
 */

inline
void
set_status_text(const gchar *txt)
{
    gnome_appbar_set_status(GNOME_APPBAR(sp->status), txt);
}

/* ------------------------------------------------------ */
/*
 * set/ get main widget
 */

GtkWidget*
yank_main_app(GtkWidget *app)
{
    static GtkWidget *main_app = NULL;

    if (app != NULL)
    {
        main_app = app;
    }

    return (main_app);
}
 
/* ------------------------------------------------------ */
/*
 * change font style
 */

static void
set_font(GtkWidget *w, GdkFont *font)
{
    GtkStyle *style;
    
    style = gtk_style_copy(gtk_widget_get_style(w));
    gdk_font_unref(style->font);
    gdk_font_ref(font);
    style->font = font;
    gtk_widget_set_style(w, style);
    gtk_style_unref(style);
}

/* ------------------------------------------------------ */
/*
 * change font style 
 */

static void
set_font_str(GtkWidget *w, gchar *fontstr)
{
    GdkFont *font;
    gchar   *msg;

    if (determine_use_fontset())
    {
        font = gdk_fontset_load(fontstr);
    }
    else
    {
        font = gdk_font_load(fontstr);
    }
    
    if (font != NULL)
    {
        set_font(w, font);
    }
    else
    {
        msg = g_strdup_printf(_("Can't load font %s"), fontstr);
        set_status_text(msg);
        g_free(msg);
    }   
}

/* ------------------------------------------------------ */
/*
 * change fonts for notes
 */

void
set_note_font_str(gchar *fontstr)
{
    GdkFont *font;
    gint    size;
    gint    width;
    
    gtk_widget_ensure_style(sp->text_entry);
    set_font_str(sp->text_entry, fontstr);
    gtk_widget_ensure_style(sp->title_entry);
    set_font_str(sp->title_entry, fontstr);
    gtk_widget_ensure_style(sp->de_deadline);
    set_font_str(sp->de_deadline, fontstr);
    gtk_widget_ensure_style(sp->prio_entry);
    set_font_str(sp->prio_entry, fontstr);
    
    font = gtk_widget_get_style(sp->de_deadline)->font;
    size = gdk_string_height(font, "W") + 10;
    width = gdk_string_width(font, _("99/99/99999"));
    gtk_widget_set_usize(sp->de_deadline, -1, size);
    gtk_widget_set_usize(GNOME_DATE_EDIT(sp->de_deadline)->date_entry,
                         width, size);
    width = gdk_string_width(font, _("99:999"));
    gtk_widget_set_usize(GNOME_DATE_EDIT(sp->de_deadline)->time_entry,
                         width, size);
    gtk_widget_set_usize(sp->title_entry, -1, size);
    gtk_widget_set_usize(sp->prio_entry, -1, size);
}

/* ------------------------------------------------------ */
/*
 * set tree font
 */

void
set_note_tree_font_str(gchar *fontstr)
{
    gtk_widget_ensure_style(sp->note_tree);
    set_font_str(sp->note_tree, fontstr);
}

/* ------------------------------------------------------ */
/*
 * set sp->todolist font
 */

void
set_todolist_font_str(gchar *fontstr)
{
    gtk_widget_ensure_style(sp->todolist);
    set_font_str(sp->todolist, fontstr);
}

/* ------------------------------------------------------ */
/*
 * set default font
 */

void
set_default_font()
{
    GtkWidget *dummy_widget;
    GdkFont   *font;
    gint      size;
    gint      width;
    
    dummy_widget = gtk_label_new("");
    gtk_widget_ensure_style(dummy_widget);
    font = gtk_widget_get_style(dummy_widget)->font;
    
    gtk_widget_ensure_style(sp->text_entry);
    set_font(sp->text_entry, font);
    gtk_widget_ensure_style(sp->title_entry);
    set_font(sp->title_entry, font);
    gtk_widget_ensure_style(sp->de_deadline);
    set_font(sp->de_deadline, font);
    gtk_widget_ensure_style(sp->prio_entry);
    set_font(sp->prio_entry, font);
    
    size = gdk_string_height(font, "W") + 10;
    
    gtk_widget_set_usize(sp->title_entry, -1, size);
    width = gdk_string_width(font, _("99/99/99999"));
    gtk_widget_set_usize(sp->de_deadline, -1, size);
    gtk_widget_set_usize(GNOME_DATE_EDIT(sp->de_deadline)->date_entry,
                         width, size);
    width = gdk_string_width(font, _("99:999"));
    gtk_widget_set_usize(GNOME_DATE_EDIT(sp->de_deadline)->time_entry,
                         width, size);
    gtk_widget_set_usize(sp->prio_entry, -1, size);
    
    gtk_widget_ensure_style(sp->note_tree);
    set_font(sp->note_tree, font);
    gtk_widget_ensure_style(sp->todolist);
    set_font(sp->todolist, font);
    
    gtk_widget_destroy(dummy_widget);
}


/* ------------------------------------------------------ */
/*
 * stolen from gedit
 */

static gint
determine_use_fontset(void)
{
    GtkWidget    *dummy_widget;
    static  gint ret = -1;

    if (ret == -1)
    {
        dummy_widget = gtk_text_new(NULL, NULL);
        gtk_widget_ensure_style(dummy_widget);
        if (dummy_widget->style->font->type == GDK_FONT_FONTSET)
        {
            ret = 1;
        }
        else
        {
            ret = 0;
        }
        gtk_widget_destroy(dummy_widget);
    }
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * Save recent file list and quit
 */

/* void */
/* yank_app_exit(GtkWidget * app) */
/* { */
/*     gchar const *filename; */
/*  */
/*     if ((filename = get_filename ())) */
/*     { */
/*         if (app == NULL) */
/*         { */
/*             app = yank_main_app(NULL); */
/*         } */
	/*
	 * Add filename to list of recent files 
	 */
/* 	yank_recent_files_update (app, filename); */
/*     } */
/*     recent_files_write_config (); */
/*     gtk_main_quit (); */
/* } */

/* ------------------------------------------------------ */
/*
 * extended flags& data
 */

static GtkWidget*
gui_ext_flags(void)
{
    GdkFont   *font;
    GtkWidget *b_expire;
    GtkWidget *hbox1;
    GtkWidget *table1;
    GtkWidget *label1;
    GtkWidget *label3;
    GtkWidget *label6;
    
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_hide(hbox1);
    
    table1 = gtk_table_new(2, 4, FALSE);
    gtk_widget_show(table1);
    gtk_box_pack_start(GTK_BOX(hbox1), table1, FALSE, FALSE, 3);
    gtk_table_set_col_spacings(GTK_TABLE(table1), 4);
    gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
    
    label1 = gtk_label_new(_("Created:"));
    font = gtk_widget_get_style(label1)->font;
    gtk_widget_show(label1);
    gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label1), 1, 0.5);
    
    sp->tlabel_created = time_label_new();
    time_label_set_format(TIME_LABEL(sp->tlabel_created), "%b %d %Y %X");
    gtk_widget_show(sp->tlabel_created);
    gtk_table_attach(GTK_TABLE(table1), sp->tlabel_created, 1, 2, 0, 1,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(sp->tlabel_created), 0, 0.5);
    
    label3 = gtk_label_new(_("Changed:"));
    gtk_widget_show(label3);
    gtk_table_attach(GTK_TABLE(table1), label3, 2, 3, 0, 1,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label3), 1, 0.5);
    
    sp->tlabel_changed = time_label_new();
    time_label_set_format(TIME_LABEL(sp->tlabel_changed), "%b %d %Y %X");
    gtk_widget_show(sp->tlabel_changed);
    gtk_table_attach(GTK_TABLE(table1), sp->tlabel_changed, 3, 4, 0, 1,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(sp->tlabel_changed), 0, 0.5);
    
    label6 = gtk_label_new(_("Changes:"));
    gtk_widget_show(label6);
    gtk_table_attach(GTK_TABLE(table1), label6, 2, 3, 1, 2,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label6), 1, 0.5);
    
    sp->label_changes = gtk_label_new("");
    gtk_widget_show(sp->label_changes);
    gtk_table_attach(GTK_TABLE(table1), sp->label_changes, 3, 4, 1, 2,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(sp->label_changes), 0, 0.5);
    
    b_expire = gtk_button_new_with_label(_("Expires:"));
    gtk_widget_set_usize(b_expire, -1, gdk_string_height(font, "W") + 10);
    gtk_widget_show(b_expire);
    gtk_table_attach(GTK_TABLE(table1), b_expire, 0, 1, 1, 2,
                     (GtkAttachOptions)(GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    
    sp->tlabel_expire = time_label_new();
    time_label_set_format(TIME_LABEL(sp->tlabel_expire), "%b %d %Y");
    gtk_widget_show(sp->tlabel_expire);
    gtk_table_attach(GTK_TABLE(table1), sp->tlabel_expire, 1, 2, 1, 2,
                     (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                     (GtkAttachOptions)(0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(sp->tlabel_expire), 0, 0.5);
    
    gtk_signal_connect(GTK_OBJECT(b_expire), "clicked",
		       GTK_SIGNAL_FUNC(cb_open_date_sel), sp->tlabel_expire);

    return (hbox1);
}

/* ------------------------------------------------------ */
