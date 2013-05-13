/*
 * menus.c
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

#include "menus.h"

/* #include "text.xpm" */
/* #include "box2.xpm" */
/* #include "circle2.xpm" */
/* #include "gnome-note.xpm" */

/* ------------------------------------------------------ */

static GnomeUIInfo file_menu [] =
{
    GNOMEUIINFO_MENU_OPEN_ITEM(cb_file_load_dialog, NULL),    
    GNOMEUIINFO_MENU_SAVE_ITEM(cb_save_file, NULL),
    GNOMEUIINFO_MENU_SAVE_AS_ITEM(cb_file_save_dialog, NULL),
#ifdef USE_GNOME_PRINT
    {
        GNOME_APP_UI_ITEM, N_("Page setup"), N_("Select paper dimensions"),
        cb_page_setup, NULL, NULL, 0, 0, (gchar) NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_MENU_PRINT_ITEM(cb_print_notes, NULL),
#endif  /* USE_GNOME_PRINT */
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_CLOSE_ITEM(cb_close, NULL),
/*     GNOMEUIINFO_MENU_EXIT_ITEM(cb_exit, NULL), */
    GNOMEUIINFO_END
};

static GnomeUIInfo add_menu [] =
{
    {
        GNOME_APP_UI_ITEM, N_("T_extNote"),
        N_("Add a text note to the tree"),
        cb_new_text, NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        GDK_e, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("C_heckNote"),
        N_("Add a check note to the tree"),
        cb_new_check, NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        GDK_h, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("T_odoNote"),
        N_("Add a todo note to the tree"),
        cb_new_todo, NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        GDK_o, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

static GnomeUIInfo sort_menu [] =
{
    {
        GNOME_APP_UI_ITEM, N_("list by date"),
        N_("Sort todo-list by date"), cb_sort_todo_date,
        NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("list by priority"),
        N_("Sort todo-list by priority"), cb_sort_todo_prio,
        NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("list by completion"),
        N_("Sort todo-list by completion"), cb_sort_todo_complete,
        NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("list by title"),
        N_("Sort todo-list by title"), cb_sort_todo_title,
        NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("list by date, then priority"),
        N_("Sort todo-list by date, then priority"),
        cb_sort_todo_date_then_prio, NULL, NULL, 0, 0,
        (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("list by priority, then date"),
        N_("Sort todo-list by priority, then date"),
        cb_sort_todo_prio_then_date, NULL, NULL, 0, 0,
        (gchar)NULL, GDK_CONTROL_MASK
    },

    GNOMEUIINFO_END
};

static GnomeUIInfo modify_menu [] =
{
    {
        GNOME_APP_UI_ITEM, N_("into a TextNote"),
        N_("Convert current note to a textnote"), cb_modify_text, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("into a CheckNote"),
        N_("Convert current note to a checknote"), cb_modify_check, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("into a TodoNote"),
        N_("Convert current note to a todonote"), cb_modify_todo, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

static GnomeUIInfo text_selection_popup [] =
{
    {
        GNOME_APP_UI_ITEM, N_("Open in browser"),
        N_("Open selection in browser"), cb_open_in_browser, NULL, NULL,
        0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    {
        GNOME_APP_UI_ITEM, N_("View as mime"), N_("View selection as mime"),
        cb_view_as_mime, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    {
        GNOME_APP_UI_ITEM, N_("Run command"), N_("Run selection as command"),
        cb_run_command, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    {
        GNOME_APP_UI_ITEM, N_("Run pipe"), N_("Filter selection"),
        cb_run_pipe, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    GNOMEUIINFO_END
};

static GnomeUIInfo edit_menu [] =
{
    GNOMEUIINFO_MENU_CUT_ITEM(cb_edit_cut, NULL),
    GNOMEUIINFO_MENU_COPY_ITEM(cb_edit_copy, NULL),
    GNOMEUIINFO_MENU_PASTE_ITEM(cb_edit_paste, NULL),
    GNOMEUIINFO_MENU_SELECT_ALL_ITEM(cb_edit_selall, NULL),
    GNOMEUIINFO_SUBTREE(N_("Sort todolist"), sort_menu),
    {
        GNOME_APP_UI_ITEM, N_("Sort tree"), N_("Sort the selected subtree(s)"),
        cb_sort_subtree, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    GNOMEUIINFO_SUBTREE(N_("Transform"), modify_menu),
    GNOMEUIINFO_SUBTREE(N_("Text Selection"), text_selection_popup),
    {
        GNOME_APP_UI_ITEM, N_("_Delete"), N_("Delete note(s)"),
        cb_delete_dialog, NULL, NULL, GNOME_APP_PIXMAP_STOCK,
        GNOME_STOCK_PIXMAP_TRASH, GDK_d, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_MENU_FIND_ITEM(cb_search_dialog, NULL),
    GNOMEUIINFO_MENU_PREFERENCES_ITEM(cb_pref_dialog, NULL),
    {
        GNOME_APP_UI_ITEM, N_("Save Preferences"),
        N_("Save preferences for yank"), cb_save_preferences, NULL, NULL,
        0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

static GnomeUIInfo misc_menu [] =
{
    GNOMEUIINFO_END
};

static GnomeUIInfo help_menu [] =
{
/* FIXME */
/*   GNOMEUIINFO_HELP ("gnome-yank"), */
    {
        GNOME_APP_UI_ITEM, N_("yank homepage"), 0, cb_yank_homepage,
        NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_HOME, 0, 0, 0
    },
    GNOMEUIINFO_MENU_ABOUT_ITEM(cb_about, NULL),
    GNOMEUIINFO_END
};

static GnomeUIInfo menu [] =
{
    GNOMEUIINFO_MENU_FILE_TREE(file_menu),
    GNOMEUIINFO_MENU_EDIT_TREE(edit_menu),
    GNOMEUIINFO_SUBTREE(N_("A_dd"), add_menu),
    GNOMEUIINFO_SUBTREE(N_("M_isc"), misc_menu),
    GNOMEUIINFO_MENU_HELP_TREE(help_menu),
    GNOMEUIINFO_END
};

static GnomeUIInfo toolbar [] =
{
    {
        GNOME_APP_UI_ITEM, N_("TextNote"), N_("Add a TextNote"), cb_new_text,
        NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("CheckNote"), N_("Add a CheckNote"),
        cb_new_check, NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("TodoNote"), N_("Add a TodoNote"), cb_new_todo,
        NULL, NULL, GNOME_APP_PIXMAP_DATA, 0,
        (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_ITEM_STOCK(N_("Delete"), N_("Delete note(s)"),
                           cb_delete_dialog, GNOME_STOCK_PIXMAP_TRASH),
    GNOMEUIINFO_ITEM_STOCK(N_("Find"), N_("Find in notes"), cb_search_dialog,
                           GNOME_STOCK_PIXMAP_SEARCH),
#ifdef USE_GNOME_PRINT
    GNOMEUIINFO_ITEM_STOCK(N_("Print"), N_("Print selected note(s)"),
                           cb_print_notes, GNOME_STOCK_PIXMAP_PRINT),
#endif  /* USE_GNOME_PRINT */
    GNOMEUIINFO_SEPARATOR,
/*     GNOMEUIINFO_ITEM_STOCK(N_("Quit"), N_("Quit yank"), cb_exit, */
/*                            GNOME_STOCK_PIXMAP_QUIT), */
    GNOMEUIINFO_END
};

static GnomeUIInfo tree_item_edit_popup [] =
{
    GNOMEUIINFO_MENU_CUT_ITEM(cb_edit_cut_tree, NULL),
    GNOMEUIINFO_MENU_COPY_ITEM(cb_edit_copy_tree, NULL),
    GNOMEUIINFO_MENU_PASTE_ITEM(cb_edit_paste_tree, NULL),
    GNOMEUIINFO_END
};

static GnomeUIInfo tree_item_popup [] =
{
    GNOMEUIINFO_SUBTREE(N_("Add"), add_menu),
    GNOMEUIINFO_MENU_EDIT_TREE(tree_item_edit_popup),
    GNOMEUIINFO_MENU_SELECT_ALL_ITEM(cb_edit_selall_tree, NULL),
    {
        GNOME_APP_UI_ITEM, N_("Sort"), N_("Sort subtree"),
        cb_sort_subtree, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    GNOMEUIINFO_SUBTREE(N_("Transform"), modify_menu),
    GNOMEUIINFO_SEPARATOR,
    {
        GNOME_APP_UI_ITEM, N_("Delete"), N_("Delete note(s)"),
        cb_delete_dialog, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

static GnomeUIInfo modify_todo_menu [] =
{
    {
        GNOME_APP_UI_ITEM, N_("into a TextNote"),
        N_("Convert current note to a textnote"), cb_todo_modify_text, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("into a CheckNote"),
        N_("Convert current note to a checknote"), cb_todo_modify_check, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    {
        GNOME_APP_UI_ITEM, N_("into a TodoNote"),
        N_("Convert current note to a todonote"), cb_todo_modify_todo, NULL,
        NULL, GNOME_APP_PIXMAP_DATA, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

static GnomeUIInfo todo_item_popup [] =
{
    GNOMEUIINFO_SUBTREE(N_("Sort"), sort_menu),
    GNOMEUIINFO_SUBTREE(N_("Transform"), modify_todo_menu),
    {
        GNOME_APP_UI_ITEM, N_("Open note in tree"),
        N_("Open the corresponding note"), cb_todo_open_corr, NULL,
        NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    GNOMEUIINFO_SEPARATOR,
    {
        GNOME_APP_UI_ITEM, N_("Delete note"),
        N_("Delete note from list& tree"), cb_delete_sel_todo, NULL,
        NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },    
    GNOMEUIINFO_END
};

static GnomeUIInfo text_item_edit_popup [] =
{
    GNOMEUIINFO_MENU_CUT_ITEM(cb_edit_cut_note, NULL),
    GNOMEUIINFO_MENU_COPY_ITEM(cb_edit_copy_note, NULL),
    GNOMEUIINFO_MENU_PASTE_ITEM(cb_edit_paste_note, NULL),
    GNOMEUIINFO_END
};

static GnomeUIInfo text_item_popup [] =
{
    GNOMEUIINFO_MENU_EDIT_TREE(text_item_edit_popup),
    GNOMEUIINFO_SUBTREE(N_("Selection"), text_selection_popup),
    GNOMEUIINFO_MENU_SELECT_ALL_ITEM(cb_edit_selall_note, NULL),
    GNOMEUIINFO_END
};

/* ------------------------------------------------------ */
/*
 * create the main menus
 */

void
yank_install_menus_and_toolbar(GtkWidget *app)
{
    gnome_app_create_toolbar_with_data(GNOME_APP(app), toolbar, app);
    gnome_app_create_menus_with_data(GNOME_APP(app), menu, app);
    gnome_app_install_menu_hints(GNOME_APP(app), menu);
    yank_recent_files_setup(app);
}

/* ------------------------------------------------------ */
/*
 * create the context menu for items in the note-tree
 */

void
yank_tree_item_context_menu(GtkWidget *tree)
{
    GtkWidget *menu;

    menu = gnome_popup_menu_new(tree_item_popup);
    gnome_popup_menu_attach(menu, tree, NULL);
}

/* ------------------------------------------------------ */
/*
 * create the context menu for items in the todo-list
 */

void
yank_todo_item_context_menu(GtkWidget *todo)
{
    GtkWidget *menu;
    
    menu = gnome_popup_menu_new(todo_item_popup);
    gnome_popup_menu_attach(menu, todo, NULL);
}

/* ------------------------------------------------------ */
/*
 * create the context menu for items in the text-widget
 */

void
yank_text_item_context_menu(GtkWidget *text)
{
    GtkWidget *menu;
    
    menu = gnome_popup_menu_new(text_item_popup);
    gnome_popup_menu_attach(menu, text, NULL);
}

/* ------------------------------------------------------ */
/*
 * Set up recent files list in File menu
 */

void
yank_recent_files_setup(GtkWidget * app)
{
    GList *rf;
    rf = recent_files_get_list();
    recent_files_menu_fill(app, rf);
}

/* 
 * Numbers in front of the recent files
 * 1. are useful accelerators, and
 * 2. are commonly used in other applications
 *
 * Because numbers are reassigned with each insertion, we have to remove all
 * the old entries and insert new ones.
 */

void
yank_recent_files_update(GtkWidget * app, gchar const * filename)
{
    gchar *del_name;
    GList *rf;

    /* Get the recent files list */
    rf = recent_files_get_list();

    if (rf && strcmp((gchar *) rf->data, filename) == 0)
	/* Do nothing if filename already at head of list */
	return;

    recent_files_menu_flush(app, rf);	/* Remove the old entries */

    /* Update the recent files list */
    del_name = recent_files_update_list(filename);
    g_free(del_name);

    /* Fill the menus */
    rf = recent_files_get_list();
    recent_files_menu_fill(app, rf);
}

/*
 * This function will be used by the options dialog when the list size is
 * reduced by the user.
 */

void
yank_recent_files_shrink(GtkWidget * app, gint new_max)
{
    GList *rf;
    gint length;
    gchar *del_name;

    /* Check if the list needs to be shrunk. */
    rf = recent_files_get_list();
    length = g_list_length(rf);

    if (length <= new_max)
	return;

    recent_files_menu_flush(app, rf);	/* Remove the old entries */
    for (; length > new_max; length--)
    {
	del_name = recent_files_list_shrink();
	g_free(del_name);
    }
    rf = recent_files_get_list();
    recent_files_menu_fill(app, rf);
}

/* ------------------------------------------------------ */
