/*
 * preferences.c
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

#include "preferences.h"
#include "privates.h"

/*
 * defaults
 */

gchar default_yank_crash_file[] = ".yank.crash";
gchar default_yank_def_file[] = ".yank.default";
gchar default_fg_color[] = "#000000000000";
gchar default_bg_color[] = "#ffffffffffff";

extern SuperPrivate *sp;

static yank_preferences preferences;
static GnomePropertyBox *pref_dialog = NULL;
static GtkWidget        *b_auto_save_on_exit;
static GtkWidget        *b_use_backup_file;
static GtkObject        *s_auto_save_minutes;
static GtkWidget        *b_auto_save_preferences;
static GtkObject        *s_compression;
static GtkObject        *s_max_recent_files;
static GtkWidget        *b_wordwrap;
static GtkWidget        *b_linewrap;
static GtkWidget        *b_completing_todo_note_sets_done;
static GtkWidget        *b_recursive_completion;
static GtkWidget        *b_auto_save_modified_notes;
static GtkWidget        *e_yank_crash_file;
static GtkWidget        *e_yank_def_file;
static GtkWidget        *b_save_yank_geometry;
static GtkWidget        *b_hide_toolbar;
static GtkWidget        *b_hide_statusbar;
static int              i_sorting_mode;
static GtkWidget        *w_todo_bg_color_today;
static GtkWidget	*w_todo_fg_color_today;
static GtkWidget        *b_todo_use_color_today;
static GtkWidget        *w_todo_bg_color_past;
static GtkWidget	*w_todo_fg_color_past;
static GtkWidget        *b_todo_use_color_past;
static GtkWidget        *b_note_fontp;
static GtkWidget        *b_note_tree_fontp;
static GtkWidget        *b_todolist_fontp;
static GtkWidget        *b_use_custom_font;


/* ------------------------------------------------------ */
/*
 * select preferences for yank
 */

void
cb_pref_dialog(GtkWidget *w, gpointer p)
{
    GtkWidget *title;
    gchar     *t_str;
    
    if (pref_dialog == NULL)
    {        
        pref_dialog = GNOME_PROPERTY_BOX(gnome_property_box_new());
        t_str = g_strdup_printf(_("%s - preferences"), PACKAGE);
        gtk_window_set_title(GTK_WINDOW(pref_dialog), t_str);
        g_free(t_str);
        gnome_dialog_set_parent(GNOME_DIALOG(pref_dialog),
                                yank_root_win(NULL));
        gtk_signal_connect(GTK_OBJECT(pref_dialog), "apply",
                           GTK_SIGNAL_FUNC(cb_pref_apply), pref_dialog);
        gnome_dialog_close_hides(GNOME_DIALOG(pref_dialog), TRUE);
        
        /*
         * files
         */
        
/*         title = gtk_label_new(_("Files")); */
/*         gtk_notebook_append_page(GTK_NOTEBOOK((GNOME_PROPERTY_BOX( */
/*             pref_dialog))->notebook), pref_files_p(), title); */
/*          */
/*         gtk_signal_connect(GTK_OBJECT(b_auto_save_on_exit), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(b_use_backup_file), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(s_auto_save_minutes), "value_changed", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(s_compression), "value_changed", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(e_yank_crash_file), "changed", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(e_yank_def_file), "changed", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(b_auto_save_preferences), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/* 	gtk_signal_connect(GTK_OBJECT(s_max_recent_files), "value_changed", */
/* 			   GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
        
        /*
         * notes
         */
        
        title = gtk_label_new(_("Notes"));
        gtk_notebook_append_page(GTK_NOTEBOOK((GNOME_PROPERTY_BOX(
            pref_dialog))->notebook), pref_notes_p(), title);
        
        gtk_signal_connect(GTK_OBJECT(b_wordwrap), "toggled",
                           GTK_SIGNAL_FUNC(pref_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_linewrap), "toggled",
                           GTK_SIGNAL_FUNC(pref_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_completing_todo_note_sets_done),
                           "toggled", GTK_SIGNAL_FUNC(pref_changed),
                           pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_recursive_completion),
                           "toggled", GTK_SIGNAL_FUNC(pref_changed),
                           pref_dialog);
 	gtk_signal_connect(GTK_OBJECT(b_auto_save_modified_notes), "toggled",
 			   GTK_SIGNAL_FUNC(pref_changed), pref_dialog);        

        /*
         * todolist
         */
        
        title = gtk_label_new(_("Todolist"));
        gtk_notebook_append_page(GTK_NOTEBOOK((GNOME_PROPERTY_BOX(
            pref_dialog))->notebook), pref_todolist_p(), title);

        /* Note: the option menu (for sorting) has its own way of calling
         * pref_changed(); see pref_todolist_p() */

        gtk_signal_connect(GTK_OBJECT(w_todo_bg_color_today), "color_set",
                           GTK_SIGNAL_FUNC(pref_color_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(w_todo_fg_color_today), "color_set",
			   GTK_SIGNAL_FUNC(pref_color_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_todo_use_color_today), "toggled",
                           GTK_SIGNAL_FUNC(pref_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(w_todo_bg_color_past), "color_set",
                           GTK_SIGNAL_FUNC(pref_color_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(w_todo_fg_color_past), "color_set",
			   GTK_SIGNAL_FUNC(pref_color_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_todo_use_color_past), "toggled",
                           GTK_SIGNAL_FUNC(pref_changed), pref_dialog);
        
        /*
         * gui
         */
        
        title = gtk_label_new(_("GUI"));
        gtk_notebook_append_page(GTK_NOTEBOOK((GNOME_PROPERTY_BOX(
            pref_dialog))->notebook), pref_gui_p(), title);
        
/*         gtk_signal_connect(GTK_OBJECT(b_save_yank_geometry), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(b_hide_toolbar), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
/*         gtk_signal_connect(GTK_OBJECT(b_hide_statusbar), "toggled", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
        gtk_signal_connect(GTK_OBJECT(b_note_fontp), "font-set",
                           GTK_SIGNAL_FUNC(pref_font_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_note_tree_fontp), "font-set",
                           GTK_SIGNAL_FUNC(pref_font_changed), pref_dialog);
        gtk_signal_connect(GTK_OBJECT(b_todolist_fontp), "font-set",
                           GTK_SIGNAL_FUNC(pref_font_changed), pref_dialog);
	gtk_signal_connect(GTK_OBJECT(b_use_custom_font), "toggled",
	                   GTK_SIGNAL_FUNC(pref_changed), pref_dialog);

        /*
         * plugins
         */
        
/*         title = gtk_label_new(_("Plugins")); */
/*         gtk_notebook_append_page(GTK_NOTEBOOK((GNOME_PROPERTY_BOX( */
/*             pref_dialog))->notebook), plugin_select_dialog(), title); */
        
/*         gtk_signal_connect(GTK_OBJECT(plugin_preload_changed), "clicked", */
/*                            GTK_SIGNAL_FUNC(pref_changed), pref_dialog); */
        
        gtk_widget_grab_focus(b_auto_save_on_exit);
    }
    
    gtk_widget_show_all(GTK_WIDGET(pref_dialog));
    gdk_window_raise(GTK_WIDGET(pref_dialog)->window);    
}

/* ------------------------------------------------------ */
/*
 * apply preferences
 */

void
cb_pref_apply(GnomePropertyBox *pref, gint page, gpointer data)
{
    preferences.auto_save_on_exit = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_auto_save_on_exit));
    preferences.use_backup_file = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_use_backup_file));
    
    preferences.auto_save_minutes = GTK_ADJUSTMENT(s_auto_save_minutes)->value;
    autosave_notes(NULL);
    
    preferences.save_compression = GTK_ADJUSTMENT(s_compression)->value;
    
    g_free(preferences.yank_crash_file);
    preferences.yank_crash_file = g_strdup(gtk_entry_get_text(
        GTK_ENTRY(e_yank_crash_file)));
    
    g_free(preferences.yank_def_file);
    preferences.yank_def_file = g_strdup(gtk_entry_get_text(
        GTK_ENTRY(e_yank_def_file)));
    
    preferences.auto_save_preferences = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_auto_save_preferences));
    preferences.max_recent_files = GTK_ADJUSTMENT(s_max_recent_files)->value;
/*     yank_recent_files_shrink(application, preferences.max_recent_files); */

    preferences.wordwrap = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_wordwrap));
    gtk_text_set_word_wrap(GTK_TEXT(sp->text_entry), preferences.wordwrap);    
    preferences.linewrap = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_linewrap));
    gtk_text_set_line_wrap(GTK_TEXT(sp->text_entry), preferences.linewrap);
    preferences.completing_todo_note_sets_done =
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(b_completing_todo_note_sets_done));
    preferences.recursive_completion = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_recursive_completion));
    preferences.auto_save_modified_notes = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_auto_save_modified_notes));
    
    preferences.save_yank_geometry = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_save_yank_geometry));

    preferences.sorting_mode = i_sorting_mode;
    sort_by_sorting_mode(preferences.sorting_mode);
    
    g_free(preferences.todolist_bg_color_today);
    preferences.todolist_bg_color_today = g_strdup(
        get_color_text(GNOME_COLOR_PICKER(w_todo_bg_color_today)));
    
    g_free(preferences.todolist_fg_color_today);
    preferences.todolist_fg_color_today = g_strdup(
	get_color_text(GNOME_COLOR_PICKER(w_todo_fg_color_today)));
    preferences.todolist_use_color_today = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_todo_use_color_today));
    
    g_free(preferences.todolist_bg_color_past);
    preferences.todolist_bg_color_past = g_strdup(
        get_color_text(GNOME_COLOR_PICKER(w_todo_bg_color_past)));
    
    g_free(preferences.todolist_fg_color_past);
    preferences.todolist_fg_color_past = g_strdup(
	get_color_text(GNOME_COLOR_PICKER(w_todo_fg_color_past)));
    preferences.todolist_use_color_past = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(b_todo_use_color_past));
    set_todolist_colors();
    
/*     preferences.hide_toolbar = gtk_toggle_button_get_active( */
/*         GTK_TOGGLE_BUTTON(b_hide_toolbar)); */
/*     preferences.hide_statusbar = gtk_toggle_button_get_active( */
/*         GTK_TOGGLE_BUTTON(b_hide_statusbar)); */
/*     show_tool_status(GNOME_APP(yank_root_win(NULL)), &preferences); */

    g_free(preferences.note_font);
    preferences.note_font = g_strdup(gnome_font_picker_get_font_name(
        GNOME_FONT_PICKER(b_note_fontp)));
    g_free(preferences.note_tree_font);
    preferences.note_tree_font = g_strdup(gnome_font_picker_get_font_name(
        GNOME_FONT_PICKER(b_note_tree_fontp)));
    g_free(preferences.todolist_font);
    preferences.todolist_font = g_strdup(gnome_font_picker_get_font_name(
        GNOME_FONT_PICKER(b_todolist_fontp)));
    preferences.use_custom_font = gtk_toggle_button_get_active(
	GTK_TOGGLE_BUTTON(b_use_custom_font));
    if (preferences.use_custom_font)
    {
	set_note_font_str(preferences.note_font);
	set_note_tree_font_str(preferences.note_tree_font);
	set_todolist_font_str(preferences.todolist_font);
    }
    else
      set_default_font();
    
    if (preferences.auto_save_preferences == TRUE)
    {
        save_preferences(get_preferences());
    }
}

/* ------------------------------------------------------ */
/*
 * file preferences tab
 */

/* GtkWidget * */
/* pref_files_p(void) */
/* { */
/*     GtkWidget *vbox; */
/*     GtkWidget *vbox1; */
/*     GtkWidget *frame; */
/*     GtkWidget *hbox1; */
/*     GtkWidget *label; */
/*     GtkWidget *s_comp_factor; */
/*     GtkWidget *button; */
/*     gchar     *fname; */
/*      */
/*     vbox = gtk_vbox_new(FALSE, 2); */
/*     gtk_widget_show(vbox); */
/*  */
    /*
     * saving
     */
/*      */
/*     frame = gtk_frame_new(_("Load & Save")); */
/*     gtk_widget_show(frame); */
/*     gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2); */
/*      */
/*     vbox1 = gtk_vbox_new(FALSE, 2); */
/*     gtk_widget_show(vbox1); */
/*     gtk_container_add(GTK_CONTAINER(frame), vbox1); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*      */
/*     b_auto_save_on_exit = gtk_check_button_new_with_label( */
/*         _("Autosave notes on exit")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_auto_save_on_exit), */
/*                                  preferences.auto_save_on_exit); */
/*     gtk_widget_show(b_auto_save_on_exit); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), b_auto_save_on_exit, FALSE, TRUE, 2); */
/*  */
/*     b_use_backup_file = gtk_check_button_new_with_label(_("Use backup file")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_use_backup_file), */
/*                                  preferences.use_backup_file); */
/*     gtk_widget_show(b_use_backup_file); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), b_use_backup_file, FALSE, TRUE, 2); */

/*     label = gtk_label_new(_("Autosave after ")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     s_auto_save_minutes = gtk_adjustment_new(preferences.auto_save_minutes, */
/*                                              0, 999, 1, 1, 1); */
/*     button = gtk_spin_button_new(GTK_ADJUSTMENT(s_auto_save_minutes), 0.0, 0); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*      */
/*     label = gtk_label_new(_("minutes (0 = off)")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*      */
/*     label = gtk_label_new(_("Compression for saving (0 = off) ")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     s_compression = gtk_adjustment_new(preferences.save_compression, 0, 10, */
/*                                        1, 1, 1); */
/*     s_comp_factor = gtk_hscale_new(GTK_ADJUSTMENT(s_compression)); */
/*     gtk_scale_set_digits(GTK_SCALE(s_comp_factor), 0); */
/*     gtk_scale_set_value_pos(GTK_SCALE(s_comp_factor), GTK_POS_LEFT); */
/*     gtk_widget_show(s_comp_factor); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), s_comp_factor, TRUE, TRUE, 2); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*  */
/*     label = gtk_label_new(_("Filename for crashes (SIGTERM) ")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     e_yank_crash_file = gtk_entry_new(); */
/*     if (preferences.yank_crash_file == NULL) */
/*     { */
/*         fname = g_strdup_printf("%s/%s", getenv("HOME"), */
/*                                 default_yank_crash_file); */
/*         gtk_entry_set_text(GTK_ENTRY(e_yank_crash_file), fname); */
/*         g_free(fname); */
/*     } */
/*     else */
/*     { */
/*         gtk_entry_set_text(GTK_ENTRY(e_yank_crash_file), */
/*                            preferences.yank_crash_file); */
/*     } */
/*     gtk_object_set_user_data(GTK_OBJECT(e_yank_crash_file), */
/*                              default_yank_crash_file); */
/*     gtk_widget_show(e_yank_crash_file); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), e_yank_crash_file, TRUE, TRUE, 2); */
/*  */
/*     button = gtk_button_new_with_label(_("Browse")); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*     gtk_signal_connect(GTK_OBJECT(button), "clicked", */
/*                        GTK_SIGNAL_FUNC(select_fname), e_yank_crash_file); */
/*      */
/*     button = gtk_button_new_with_label(_("Default")); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*     gtk_signal_connect(GTK_OBJECT(button), "clicked", */
/*                        GTK_SIGNAL_FUNC(cb_yank_def_file), e_yank_crash_file); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*  */
/*     label = gtk_label_new(_("Default filename ")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     e_yank_def_file = gtk_entry_new(); */
/*     if (preferences.yank_def_file == NULL) */
/*     { */
/*         fname = g_strdup_printf("%s/%s", getenv("HOME"), */
/*                                 default_yank_def_file); */
/*         gtk_entry_set_text(GTK_ENTRY(e_yank_def_file), fname); */
/*         g_free(fname); */
/*     } */
/*     else */
/*     { */
/*         gtk_entry_set_text(GTK_ENTRY(e_yank_def_file), */
/*                            preferences.yank_def_file); */
/*     } */
/*     gtk_object_set_user_data(GTK_OBJECT(e_yank_def_file), */
/*                              default_yank_def_file); */
/*     gtk_widget_show(e_yank_def_file); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), e_yank_def_file, TRUE, TRUE, 2); */
/*  */
/*     button = gtk_button_new_with_label(_("Browse")); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*     gtk_signal_connect(GTK_OBJECT(button), "clicked", */
/*                        GTK_SIGNAL_FUNC(select_fname), e_yank_def_file); */
/*      */
/*     button = gtk_button_new_with_label(_("Default")); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*     gtk_signal_connect(GTK_OBJECT(button), "clicked", */
/*                        GTK_SIGNAL_FUNC(cb_yank_def_file), e_yank_def_file); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*      */
/*     label = gtk_label_new(_("Number of recently used files to show in " */
/* 			    "\"File\" menu:")); */
/*     gtk_widget_show(label); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2); */
/*      */
/*     s_max_recent_files = gtk_adjustment_new(preferences.max_recent_files, */
/* 					    0, 10, 1, 1, 1); */
/*     button = gtk_spin_button_new(GTK_ADJUSTMENT(s_max_recent_files), 0.0, 0); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*      */
/*     frame = gtk_frame_new(_("Preferences")); */
/*     gtk_widget_show(frame); */
/*     gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2); */
/*      */
/*     vbox1 = gtk_vbox_new(FALSE, 2); */
/*     gtk_widget_show(vbox1); */
/*     gtk_container_add(GTK_CONTAINER(frame), vbox1); */
/*  */
/*     b_auto_save_preferences = gtk_check_button_new_with_label( */
/*         _("Autosave preferences")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_auto_save_preferences), */
/*                                  preferences.auto_save_preferences); */
/*     gtk_widget_show(b_auto_save_preferences); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), b_auto_save_preferences, */
/*                        FALSE, TRUE, 2); */
/*      */
/*     return (vbox); */
/* } */

/* ------------------------------------------------------ */
/*
 * return pointer to preferences
 */

inline
yank_preferences*
get_preferences(void)
{
    return (&preferences);
}

/* ------------------------------------------------------ */
/*
 * load yanks preferences
 */

void
load_preferences(void)
{
    gint  i;
    gchar **p_list;
    
    gnome_config_push_prefix("/yank/Global/");
    
    preferences.auto_save_on_exit = gnome_config_get_int("auto save on exit");
    preferences.use_backup_file = gnome_config_get_int("use backup file");
    preferences.auto_save_minutes = gnome_config_get_int("auto save minutes");
    preferences.save_compression = gnome_config_get_int("save compression");
    preferences.yank_crash_file = gnome_config_get_string(
        "yank crash file");
    if (preferences.yank_crash_file == NULL)
    {
        preferences.yank_crash_file = g_strdup_printf("%s/%s", getenv("HOME"),
                                                      default_yank_crash_file);
    }
    preferences.yank_def_file = gnome_config_get_string(
        "yank default file");
    if (preferences.yank_def_file == NULL)
    {
        preferences.yank_def_file = g_strdup_printf("%s/%s", getenv("HOME"),
                                                      default_yank_def_file);
    }
    preferences.auto_save_preferences = gnome_config_get_int
        ("auto save preferences");
    preferences.wordwrap = gnome_config_get_int("wordwrap");
    preferences.linewrap = gnome_config_get_int("linewrap");
    preferences.completing_todo_note_sets_done =
        gnome_config_get_int("completing todo note sets done");
    preferences.recursive_completion =
        gnome_config_get_int("recursive completion");
    preferences.auto_save_modified_notes = 
        gnome_config_get_int("auto save modified notes");
 
    preferences.save_yank_geometry = gnome_config_get_int
        ("save yank geometry");
    preferences.yank_width = gnome_config_get_int("yank width");
    preferences.yank_height = gnome_config_get_int("yank height");
    preferences.note_tree_width = gnome_config_get_int("note tree width");
    preferences.hide_toolbar = gnome_config_get_int("hide toolbar");
    preferences.hide_statusbar = gnome_config_get_int("hide statusbar");

    preferences.note_font = gnome_config_get_string("note font");
    preferences.note_tree_font = gnome_config_get_string(
        "note tree font");
    preferences.todolist_font = gnome_config_get_string(
        "todolist font");
    preferences.use_custom_font = gnome_config_get_int("use custom font");
    
    gnome_config_pop_prefix();
    
    gnome_config_push_prefix("/yank/TodoList/");

    preferences.sorting_mode = gnome_config_get_int("sorting mode");
    i_sorting_mode = preferences.sorting_mode;
    preferences.todolist_use_color_today = gnome_config_get_int(
        "use color today");
    preferences.todolist_bg_color_today = gnome_config_get_string(
        "bg color today");
    if (preferences.todolist_bg_color_today == NULL)
      preferences.todolist_bg_color_today = g_strdup(default_bg_color);
    preferences.todolist_fg_color_today = gnome_config_get_string(
        "fg color today");
    if (preferences.todolist_fg_color_today == NULL)
      preferences.todolist_fg_color_today = g_strdup(default_fg_color);
    preferences.todolist_use_color_past = gnome_config_get_int(
        "use color past");
    preferences.todolist_bg_color_past = gnome_config_get_string(
        "bg color past");
    if (preferences.todolist_bg_color_past == NULL)
      preferences.todolist_bg_color_past = g_strdup(default_bg_color);
    preferences.todolist_fg_color_past = gnome_config_get_string(
	"fg color past");
    if (preferences.todolist_fg_color_past == NULL)
      preferences.todolist_fg_color_past = g_strdup(default_fg_color);
    
    gnome_config_pop_prefix();
    
    gnome_config_push_prefix("/yank/Plugins/");
    preferences.preload_plugins = gnome_config_get_int("preload plugins");
    gnome_config_get_vector("preload plugin", &preferences.preload_plugins,
        &p_list);
    plugin_preload_list(preferences.preload_plugins, p_list);
    if (preferences.preload_plugins)
    {
        for (i = 0; i < preferences.preload_plugins; i++)
        {
            g_free(p_list[i]);
        }
        g_free(p_list);
    }
    
    gnome_config_pop_prefix();
    
    preferences.max_recent_files =
      gnome_config_get_int("/yank/Recent/MaxFiles=4");
    preferences.recent_files = NULL;
    recent_files_get_list();
    
    gnome_config_sync();
}

/* ------------------------------------------------------ */
/*
 * save yanks preferences
 */

void
save_preferences(yank_preferences *prefs)
{
    gnome_config_push_prefix("/yank/Global/");
    
    gnome_config_set_int("auto save on exit", prefs->auto_save_on_exit);
    gnome_config_set_int("use backup file", prefs->use_backup_file);
    gnome_config_set_int("auto save minutes", prefs->auto_save_minutes);
    gnome_config_set_int("save compression", prefs->save_compression);
    gnome_config_set_string("yank crash file", prefs->yank_crash_file);
    gnome_config_set_string("yank default file", prefs->yank_def_file);
    gnome_config_set_int("auto save preferences",
                         prefs->auto_save_preferences);
    gnome_config_set_int("wordwrap", prefs->wordwrap);
    gnome_config_set_int("linewrap", prefs->linewrap);
    gnome_config_set_int("completing todo note sets done",
                         prefs->completing_todo_note_sets_done);
    gnome_config_set_int("recursive completion", prefs->recursive_completion);
    gnome_config_set_int("auto save modified notes",
                         prefs->auto_save_modified_notes);
    
    gnome_config_set_int("save yank geometry", prefs->save_yank_geometry);
    gnome_config_set_int("hide toolbar", prefs->hide_toolbar);
    gnome_config_set_int("hide statusbar", prefs->hide_statusbar);

    gnome_config_set_string("note font", prefs->note_font);
    gnome_config_set_string("note tree font", prefs->note_tree_font);
    gnome_config_set_string("todolist font", prefs->todolist_font);
    gnome_config_set_int("use custom font", prefs->use_custom_font);

    gnome_config_pop_prefix();

    gnome_config_push_prefix("/yank/TodoList/");

    gnome_config_set_int("sorting mode", prefs->sorting_mode);
    gnome_config_set_int("use color today", prefs->todolist_use_color_today);
    gnome_config_set_string("bg color today", prefs->todolist_bg_color_today);
    gnome_config_set_string("fg color today", prefs->todolist_fg_color_today);
    gnome_config_set_int("use color past", prefs->todolist_use_color_past);
    gnome_config_set_string("bg color past", prefs->todolist_bg_color_past);
    gnome_config_set_string("fg color past", prefs->todolist_fg_color_past);
    
    gnome_config_pop_prefix();

    gnome_config_push_prefix("/yank/Plugins/");
    plugin_create_preload_list(&(prefs->preload_plugins),
                               &(prefs->preload_plugin));
    gnome_config_set_int("preload plugins", prefs->preload_plugins);
    gnome_config_set_vector("preload plugin", prefs->preload_plugins,
                            (const gchar**)prefs->preload_plugin);
    g_free(prefs->preload_plugin);
    prefs->preload_plugin = NULL;
    
    gnome_config_pop_prefix();
    
    gnome_config_set_int("/yank/Recent/MaxFiles", prefs->max_recent_files);
    
    gnome_config_sync();
}

/* ------------------------------------------------------ */
/*
 * save from the menu
 */

void
cb_save_preferences(GtkWidget *w, gpointer p)
{
    save_preferences(get_preferences());
}

/* ------------------------------------------------------ */
/*
 * preferences have been changed 
 */

void
pref_changed(GtkWidget *w, GnomePropertyBox *prefs)
{
    gnome_property_box_changed(prefs);
}

/* ------------------------------------------------------ */
/*
 * color preferences have been changed 
 */

void
pref_color_changed(GnomeColorPicker *widget, guint r, guint g, guint b,
                   guint a, GnomePropertyBox *prefs)
{
    gnome_property_box_changed(prefs);
}

/* ------------------------------------------------------ */
/*
 * font preferences have been changed 
 */

void
pref_font_changed(GtkWidget *w, gchar *str, GnomePropertyBox *prefs)
{
    gnome_property_box_changed(prefs);
}

/* ------------------------------------------------------ */

static void
cb_set_sortmode(GtkWidget *w, gpointer data)
{
   i_sorting_mode = GPOINTER_TO_INT(data);
   pref_changed(NULL, pref_dialog);
}

/* ------------------------------------------------------ */

GtkWidget *
pref_notes_p(void)
{
    GtkWidget *vbox;
    GtkWidget *vbox1;
    GtkWidget *frame;

    vbox = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox);

    /*
     * editing
     */
    
    frame = gtk_frame_new(_("Editing"));
    gtk_widget_show(frame);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2);
    
    vbox1 = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(frame), vbox1);
    
    b_wordwrap = gtk_check_button_new_with_label(
        _("Use wordwrap in note text"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_wordwrap),
                                 preferences.wordwrap);
    gtk_widget_show(b_wordwrap);
    gtk_box_pack_start(GTK_BOX(vbox1), b_wordwrap, FALSE, TRUE, 2);
    
    b_linewrap = gtk_check_button_new_with_label(
        _("Use linewrap in note text"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_linewrap),
                                 preferences.linewrap);
    gtk_widget_show(b_linewrap);
    gtk_box_pack_start(GTK_BOX(vbox1), b_linewrap, FALSE, TRUE, 2);

    b_completing_todo_note_sets_done = gtk_check_button_new_with_label(
        _("Completing todo note sets done flag"));
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(b_completing_todo_note_sets_done),
        preferences.completing_todo_note_sets_done);
    gtk_widget_show(b_completing_todo_note_sets_done);
    gtk_box_pack_start(GTK_BOX(vbox1), b_completing_todo_note_sets_done,
                       FALSE, TRUE, 2);

    b_recursive_completion = gtk_check_button_new_with_label(
        _("Recursive completion"));
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(b_recursive_completion),
        preferences.recursive_completion);
    gtk_widget_show(b_recursive_completion);
    gtk_box_pack_start(GTK_BOX(vbox1), b_recursive_completion,
                       FALSE, TRUE, 2);

    frame = gtk_frame_new(_("Miscellaneous"));
    gtk_widget_show(frame);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2);
    
    vbox1 = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(frame), vbox1);

    b_auto_save_modified_notes = gtk_check_button_new_with_label(
        _("Save modified notes without prompting"));			
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_auto_save_modified_notes),
                                 preferences.auto_save_modified_notes);
    gtk_widget_show(b_auto_save_modified_notes);
    gtk_box_pack_start(GTK_BOX(vbox1), b_auto_save_modified_notes, 
                       FALSE, TRUE, 2);
    
    return (vbox);
}

/* ------------------------------------------------------ */
/*
 * open fileselection
 */

void
select_fname(GtkWidget *w, gpointer p)
{
    GtkWidget *fs;
    gchar     *txt;
    
    txt = gtk_entry_get_text(GTK_ENTRY(p));
    fs = gtk_file_selection_new(_("Select filename"));
    if (txt != NULL)
    {
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), txt);
    }
    gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(fs));
    gtk_object_set_user_data(GTK_OBJECT(fs), p);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                       "clicked", GTK_SIGNAL_FUNC(select_fname_ok), fs);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
                       "clicked", GTK_SIGNAL_FUNC(select_fname_cancel), fs);
    gtk_widget_show(fs);
}

/* ------------------------------------------------------ */
/*
 * fileselection ok
 */

void
select_fname_ok(GtkWidget *w, gpointer p)
{
    GtkEntry *entry;
    
    entry = GTK_ENTRY(gtk_object_get_user_data(GTK_OBJECT(p)));
    gtk_entry_set_text(entry, gtk_file_selection_get_filename(
        GTK_FILE_SELECTION(p)));
    gtk_widget_destroy(GTK_WIDGET(p));
}

/* ------------------------------------------------------ */
/*
 * fileselection cancel
 */

void
select_fname_cancel(GtkWidget *w, gpointer p)
{
    gtk_widget_destroy(GTK_WIDGET(p));
}

/* ------------------------------------------------------ */
/*
 * set default file for crash, load
 */

void
cb_yank_def_file(GtkWidget *w, gpointer p)
{
    GtkEntry *entry;
    gchar    *fname1;
    gchar    *fname2;
    gchar    *def;
    
    entry = GTK_ENTRY(p);
    def = gtk_object_get_user_data(GTK_OBJECT(entry));
    fname1 = g_strdup_printf("%s/%s", getenv("HOME"), def);
    fname2 = gtk_entry_get_text(entry);
    if (strcmp(fname1, fname2))
    {
        gtk_entry_set_text(entry, fname1);
    }
    g_free(fname1);
}

/* ------------------------------------------------------ */
/*
 * set default filename
 */

void
cb_yank_def_cmd(GtkWidget *w, gpointer p)
{
    GtkEntry *entry;
    gchar    *fname1;
    gchar    *fname2;
    
    entry = GTK_ENTRY(p);
    fname1 = gtk_object_get_user_data(GTK_OBJECT(entry));
    fname2 = gtk_entry_get_text(entry);
    if (strcmp(fname1, fname2))
    {
        gtk_entry_set_text(entry, fname1);
    }
}

/* ------------------------------------------------------ */
/*
 * todolist preferences tab
 */

GtkWidget*
pref_todolist_p(void)
{
    GtkWidget *vbox;
    GtkWidget *vbox1;
    GtkWidget *hbox1;
    GtkWidget *table;
    GtkWidget *align;
    GtkWidget *frame;
    GtkWidget *label;
    GtkWidget *optionmenu;
    GtkWidget *menu;
    GtkWidget *menuitem;   
    int       sort_index;
    static const char *sorting_methods[] = { N_("(no sorting)"),
	                                     N_("date"),
	                                     N_("priority"),
                                             N_("complete"),
	                                     N_("title"),
                                             N_("date, then priority"),
                                             N_("priority, then date"),
                                             NULL 
	                                   };
    guint     r;
    guint     g;
    guint     b;
    
    vbox = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox);

    /*
     * sorting
     */

    frame = gtk_frame_new(_("Sorting"));
    gtk_widget_show(frame);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2);
   
    vbox1 = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(frame), vbox1);
   
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2);

    label = gtk_label_new(_("Sort todo list by "));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, TRUE, 2);
    
    optionmenu = gtk_option_menu_new();
    menu = gtk_menu_new();
    for (sort_index = 0; sorting_methods[sort_index] != NULL; ++sort_index)
     {
	menuitem = 
	  gtk_menu_item_new_with_label(_(sorting_methods[sort_index]));
        gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(cb_set_sortmode),
			   (gpointer)sort_index);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_widget_show(menuitem);
     }
    gtk_menu_set_active((GtkMenu*) menu, preferences.sorting_mode);
    gtk_option_menu_set_menu((GtkOptionMenu*) optionmenu, menu);
    gtk_widget_show(optionmenu);
    gtk_box_pack_start(GTK_BOX(hbox1), optionmenu, FALSE, TRUE, 2);
    
    /*
     * colors
     */

    r = g = b = 0;
    
    frame = gtk_frame_new(_("Colors"));
    gtk_widget_show(frame);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2);

    table = gtk_table_new(2, 6, FALSE);
    gtk_widget_show(table);
    gtk_container_add(GTK_CONTAINER(frame), table);

    label = gtk_label_new(_("Deadlines today"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, 0, 2, 2);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_table_set_col_spacing(GTK_TABLE(table), 0, 20);

    label = gtk_label_new(_("Foreground:"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, 0, 0, 2, 2);

    w_todo_fg_color_today = gnome_color_picker_new();
    parse_color_text(preferences.todolist_fg_color_today, &r, &g, &b);
    gnome_color_picker_set_i16(GNOME_COLOR_PICKER(w_todo_fg_color_today),
			       r, g, b, 0);
    gtk_widget_show(w_todo_fg_color_today);
    gtk_table_attach(GTK_TABLE(table), w_todo_fg_color_today, 2, 3, 0, 1,
		     0, 0, 2, 2);
    gtk_table_set_col_spacing(GTK_TABLE(table), 2, 10);

    label = gtk_label_new(_("Background:"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 3, 4, 0, 1, 0, 0, 2, 2);
    
    w_todo_bg_color_today = gnome_color_picker_new();
    parse_color_text(preferences.todolist_bg_color_today, &r, &g, &b);
    gnome_color_picker_set_i16(GNOME_COLOR_PICKER(w_todo_bg_color_today),
			       r, g, b, 0);
    gtk_widget_show(w_todo_bg_color_today);
    gtk_table_attach(GTK_TABLE(table), w_todo_bg_color_today, 4, 5, 0, 1,
		     0, 0, 2, 2);
   
    b_todo_use_color_today = gtk_check_button_new_with_label(_("Use color"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_todo_use_color_today),
                                 preferences.todolist_use_color_today);
    gtk_widget_show(b_todo_use_color_today);
    align = gtk_alignment_new(1, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), b_todo_use_color_today);
    gtk_table_attach(GTK_TABLE(table), align, 5, 6, 0, 1,
		     GTK_EXPAND|GTK_FILL, 0, 2, 2);

    label = gtk_label_new(_("Past deadlines"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, 0, 2, 2);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
   
    label = gtk_label_new(_("Foreground:"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 1, 2, 0, 0, 2, 2);
    
    w_todo_fg_color_past = gnome_color_picker_new();
    parse_color_text(preferences.todolist_fg_color_past, &r, &g, &b);
    gnome_color_picker_set_i16(GNOME_COLOR_PICKER(w_todo_fg_color_past),
			       r, g, b, 0);
    gtk_widget_show(w_todo_fg_color_past);
    gtk_table_attach(GTK_TABLE(table), w_todo_fg_color_past, 2, 3, 1, 2,
		     0, 0, 2, 2);
   
    label = gtk_label_new(_("Background:"));
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(table), label, 3, 4, 1, 2, 0, 0, 2, 2);
       
    w_todo_bg_color_past = gnome_color_picker_new();
    parse_color_text(preferences.todolist_bg_color_past, &r, &g, &b);
    gnome_color_picker_set_i16(GNOME_COLOR_PICKER(w_todo_bg_color_past),
			       r, g, b, 0);
    gtk_widget_show(w_todo_bg_color_past);
    gtk_table_attach(GTK_TABLE(table), w_todo_bg_color_past, 4, 5, 1, 2,
		     0, 0, 2, 2);
    
    b_todo_use_color_past = gtk_check_button_new_with_label(_("Use color"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_todo_use_color_past),
                                 preferences.todolist_use_color_past);
    gtk_widget_show(b_todo_use_color_past);
    align = gtk_alignment_new(1, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), b_todo_use_color_past);
    gtk_table_attach(GTK_TABLE(table), align, 5, 6, 1, 2,
		     GTK_EXPAND|GTK_FILL, 0, 2, 2);

    return (vbox);
}

/* ------------------------------------------------------ */
/*
 * gui preferences tab
 */

GtkWidget*
pref_gui_p(void)
{
    GtkWidget *vbox;
    GtkWidget *vbox1;
    GtkWidget *hbox1;
    GtkWidget *hbox2;
    GtkWidget *frame;
    GtkWidget *button;
    GtkWidget *label;
    
    vbox = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox);

/*     frame = gtk_frame_new(_("Main window")); */
/*     gtk_widget_show(frame); */
/*     gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2); */
/*      */
/*     vbox1 = gtk_vbox_new(FALSE, 2); */
/*     gtk_widget_show(vbox1); */
/*     gtk_container_add(GTK_CONTAINER(frame), vbox1); */
/*      */
/*     hbox1 = gtk_hbox_new(FALSE, 0); */
/*     gtk_widget_show(hbox1); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2); */
/*  */
/*     b_save_yank_geometry = gtk_check_button_new_with_label( */
/*         _("Save geometry on exit")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_save_yank_geometry), */
/*                                  preferences.save_yank_geometry); */
/*     gtk_widget_show(b_save_yank_geometry); */
/*     gtk_box_pack_start(GTK_BOX(hbox1), b_save_yank_geometry, FALSE, TRUE, 2); */
/*  */
/*     button = gtk_button_new_with_label(_("Restore default gui size")); */
/*     gtk_widget_show(button); */
/*     gtk_box_pack_end(GTK_BOX(hbox1), button, FALSE, TRUE, 2); */
/*     gtk_signal_connect(GTK_OBJECT(button), "clicked", */
/*                        GTK_SIGNAL_FUNC(default_gui_size), NULL); */
    
/*     b_hide_toolbar = gtk_check_button_new_with_label(_("Hide toolbar")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_hide_toolbar), */
/*                                  preferences.hide_toolbar); */
/*     gtk_widget_show(b_hide_toolbar); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), b_hide_toolbar, FALSE, TRUE, 2); */
/*  */
/*     b_hide_statusbar = gtk_check_button_new_with_label(_("Hide statusbar")); */
/*     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_hide_statusbar), */
/*                                  preferences.hide_statusbar); */
/*     gtk_widget_show(b_hide_statusbar); */
/*     gtk_box_pack_start(GTK_BOX(vbox1), b_hide_statusbar, FALSE, TRUE, 2); */

    frame = gtk_frame_new(_("Fonts"));
    gtk_widget_show(frame);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 2);
    
    vbox1 = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(frame), vbox1);

    b_use_custom_font = gtk_check_button_new_with_label(_("Use custom fonts"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b_use_custom_font),
				 preferences.use_custom_font);
    gtk_widget_show(b_use_custom_font);
    gtk_box_pack_start(GTK_BOX(vbox1), b_use_custom_font, FALSE, TRUE, 2);
    
    hbox1 = gtk_hbox_new(FALSE, 10);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2);
    
    hbox2 = gtk_hbox_new(FALSE, 4);
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox2, TRUE, TRUE, 2);
    
    label = gtk_label_new(_("Note font:"));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, TRUE, 0);

    b_note_fontp = gnome_font_picker_new();
    if (preferences.note_font)
    {
        gnome_font_picker_set_font_name(GNOME_FONT_PICKER(b_note_fontp),
                                        preferences.note_font);
    }
    gnome_font_picker_set_title(GNOME_FONT_PICKER(b_note_fontp),
                                _("Note font"));
    gnome_font_picker_set_mode(GNOME_FONT_PICKER(b_note_fontp),
                               GNOME_FONT_PICKER_MODE_FONT_INFO);
    gtk_widget_show(b_note_fontp);
    gtk_box_pack_start(GTK_BOX(hbox2), b_note_fontp, FALSE, TRUE, 0);
    
    hbox2 = gtk_hbox_new(FALSE, 4);
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox2, TRUE, TRUE, 2);
    
    label = gtk_label_new(_("NoteTree:"));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, TRUE, 0);

    b_note_tree_fontp = gnome_font_picker_new();
    if (preferences.note_tree_font)
    {
        gnome_font_picker_set_font_name(GNOME_FONT_PICKER(b_note_tree_fontp),
                                        preferences.note_tree_font);
    }
    gnome_font_picker_set_title(GNOME_FONT_PICKER(b_note_tree_fontp),
                                _("NoteTree font"));
    gnome_font_picker_set_mode(GNOME_FONT_PICKER(b_note_tree_fontp),
                               GNOME_FONT_PICKER_MODE_FONT_INFO);
    gtk_widget_show(b_note_tree_fontp);
    gtk_box_pack_start(GTK_BOX(hbox2), b_note_tree_fontp, FALSE, TRUE, 0);
    
    hbox2 = gtk_hbox_new(FALSE, 4);
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox2, TRUE, TRUE, 2);
    
    label = gtk_label_new(_("Todolist:"));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, TRUE, 0);

    b_todolist_fontp = gnome_font_picker_new();
    if (preferences.todolist_font)
    {
        gnome_font_picker_set_font_name(GNOME_FONT_PICKER(b_todolist_fontp),
                                        preferences.todolist_font);
    }
    gnome_font_picker_set_title(GNOME_FONT_PICKER(b_todolist_fontp),
                                _("Todolist font"));
    gnome_font_picker_set_mode(GNOME_FONT_PICKER(b_todolist_fontp),
                               GNOME_FONT_PICKER_MODE_FONT_INFO);
    gtk_widget_show(b_todolist_fontp);
    gtk_box_pack_start(GTK_BOX(hbox2), b_todolist_fontp, FALSE, TRUE, 0);
    
    return (vbox);
}

/* ------------------------------------------------------ */
/*
 * create text string from selected color
 */

gchar*
get_color_text(GnomeColorPicker *widget)
{
    static gchar c_text[100];
    gushort      r;
    gushort      g;
    gushort      b;
    gushort      a;

    if (GNOME_IS_COLOR_PICKER(widget))
    {
        gnome_color_picker_get_i16(widget, &r, &g, &b, &a);
        sprintf(c_text, "#%04x%04x%04x", r, g, b);
        return (c_text);
    }
    else
    {
        return (NULL);
    }    
}

/* ------------------------------------------------------ */
/*
 * parse txt into numeric values
 * stolen from gnome-pim
 */

void
parse_color_text(gchar *txt, guint *r, guint *g, guint *b)
{
    g_return_if_fail (txt != NULL);
    g_return_if_fail (r != NULL);
    g_return_if_fail (g != NULL);
    g_return_if_fail (b != NULL);

    if (sscanf (txt, "#%04x%04x%04x", r, g, b) != 3)
    {
        g_warning ("Invalid color specification %s, returning black", txt);
        *r = *g = *b = 0;
    }
}

/* ------------------------------------------------------ */
/* 
 * recent_files_get_list:
 * 
 *  This function returns a pointer to the recent files list,
 *  creating it if neccessary.
 * 
 * Return value: the list./
 **/
GList *
recent_files_get_list(void)
{
    gchar *filename, *key;
    gint max_entries, i;
    gboolean do_set = FALSE;

    /* If the list is already populated, return it. */
    if (preferences.recent_files)
	return preferences.recent_files;

    gnome_config_push_prefix("/yank/Recent/");

    /* Get maximum number of history entries.  Write default value to 
     * config file if no entry exists. */
    max_entries = gnome_config_get_int_with_default("MaxFiles=4", &do_set);
    if (do_set)
	gnome_config_set_int("MaxFiles", 4);

    /* Read the history filenames from the config file */
    for (i = 0; i < max_entries; i++)
    {
	key = g_strdup_printf("File%d", i);
	filename = gnome_config_get_string(key);
	if (filename == NULL)
	{
	    /* Ran out of filenames. */
	    g_free(key);
	    break;
	}
	preferences.recent_files = g_list_append(preferences.recent_files,
						 filename);
	g_free(key);
    }
    gnome_config_pop_prefix();

    return preferences.recent_files;
}

/*
 * recent_files_update_list:
 * @filename: 
 * 
 * This function updates the recent files list. 
 * 
 * Return value: The return value is a pointer to the filename that was
 * removed if the list was already full, or NULL if no items were removed.
 */

gchar *
recent_files_update_list(gchar const * filename)
{
    gchar *name, *old_name = NULL;
    GList *l = NULL;
    GList *new_list = NULL;
    gint max_entries, count = 0;
    gboolean found = FALSE;
    yank_preferences *prefs;

    g_return_val_if_fail(filename != NULL, NULL);

    prefs = get_preferences();
    max_entries = prefs->max_recent_files;
    if (max_entries == 0) return NULL;
    
    /* Check if this filename already exists in the list */
    for (l = preferences.recent_files; l && (count < max_entries); l = l->next)
    {
	if (!found && (!strcmp((gchar *) l->data, filename) ||
		       (count == max_entries - 1)))
	{
	    /* This is either the last item in the list, or a
	     * duplicate of the requested entry. */
	    old_name = (gchar *) l->data;
	    found = TRUE;
	}
	else			/* Add this item to the new list */
	    new_list = g_list_append(new_list, l->data);

	count++;
    }

    /* Insert the new filename to the new list and free up the old list */
    name = g_strdup(filename);
    new_list = g_list_prepend(new_list, name);
    g_list_free(preferences.recent_files);
    preferences.recent_files = new_list;

    return old_name;
}

/*
 * Remove the last item from the history list and return it.
 */

gchar *
recent_files_list_shrink(void)
{
    gchar *name;
    GList *l;

    if (preferences.recent_files == NULL)
	return NULL;

    l = g_list_last(preferences.recent_files);
    name = (gchar *) l->data;
    preferences.recent_files = g_list_remove_link(preferences.recent_files, l);
    if (preferences.recent_files == NULL)
      recent_files_write_config(); /* commit the change */

    return name;
}

/*
 * Write contents of the history list to the configuration file.
 */

void
recent_files_write_config(void)
{
    gchar *key;
    GList *l;
    gint max_entries, i = 0;
    
    max_entries = gnome_config_get_int("/yank/Recent/MaxFiles=4");
    gnome_config_clean_section("/yank/Recent");
    gnome_config_push_prefix("/yank/Recent/");
    gnome_config_set_int("MaxFiles", max_entries);

    for (l = preferences.recent_files; l; l = l->next)
    {
	key = g_strdup_printf("File%d", i++);
	gnome_config_set_string(key, (gchar *) l->data);
	g_free(l->data);
	g_free(key);
    }

    gnome_config_sync();
    gnome_config_pop_prefix();
    g_list_free(preferences.recent_files);
    preferences.recent_files = NULL;
}

/* ------------------------------------------------------ */
