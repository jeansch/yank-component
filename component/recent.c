/*
 * recent.c
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Huﬂmann <m.hussmann@home.ins.de>
 *
 * This file: Copyright (C) 2000 Fred Wilson Horch <fhorch@ecoaccess.org>
 * based on the gnumeric history.c by Mike Kestner (mkestner@ameritech.net)
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

#include <config.h>
#include <gnome.h>

#include "app.h"
#include "recent.h"
#include "menus.h"
#include "preferences.h"
#include "fileio.h"

/*
 * Returns a newly allocated string containing the name of the file
 */

static gchar *
recent_files_item_label(const gchar * name, const gint accel_number)
{
    int   i, count;
    char *label;
    GString *menuname;

    /* Duplicate underscores so menu will not underline the following char. */
    name = g_basename(name);
    menuname = g_string_new(name);
    count = 0;
    for (i = 0; name[i]; i++)
      if (name[i] == '_')
      {
	  menuname = g_string_insert_c(menuname, i + count, '_');
	  count++;
      }
    label = g_strdup_printf("_%d %s", accel_number, menuname->str);
    g_string_free(menuname, 1);

    return label;
}

/*
 * Create a recent files menu item in a menu at a given position.
 */

static void
recent_files_menu_item_create(GtkWidget * app, gchar * name, gint accel_number,
			      GtkWidget * menu, gint pos)
{
    GnomeUIInfo info[] =
    {
	{GNOME_APP_UI_ITEM, NULL, NULL, cb_recent_files, NULL},
	GNOMEUIINFO_END
    };

    info[0].hint = name;
    info[0].label = recent_files_item_label(name, accel_number);
    info[0].user_data = app;

    gnome_app_fill_menu(GTK_MENU_SHELL(menu), info,
			GNOME_APP(app)->accel_group, TRUE,
			pos);
    gtk_object_set_data(GTK_OBJECT(info[0].widget), UGLY_GNOME_UI_KEY, name);
    gnome_app_install_menu_hints(GNOME_APP(app), info);

    g_free(info[0].label);
}

typedef struct
{
    GtkWidget *menu;
    gint pos;
}
MenuPos;

/*
 * Insert the recent files separator. Return its position.
 */

static MenuPos
recent_files_menu_insert_separator(GtkWidget * app)
{
    MenuPos ret;

    GtkWidget *item;
    char const *menu_name = "_File/<Separator>";

    ret.menu = gnome_app_find_menu_pos(GNOME_APP(app)->menubar,
				       menu_name, &ret.pos);
    if (ret.menu != NULL)
    {
	item = gtk_menu_item_new();
	gtk_widget_show(item);
	gtk_menu_shell_insert(GTK_MENU_SHELL(ret.menu), item, ret.pos);
    }
    return ret;
}

/*
 * Add the recent files items list to the file menu.
 */

static void
recent_files_menu_insert_items(GtkWidget * app, MenuPos * mp, GList * name_list)
{
    gint accel_number;
    GList *l;

    if (name_list == NULL) return;

    /* Add a new menu item for each item in the recent file list */
    accel_number = 1;
    for (l = name_list; l; l = l->next)
	recent_files_menu_item_create(app, (gchar *) l->data, accel_number++,
				      mp->menu, (mp->pos)++);
}

/*
 * Remove the recent file list items from the file menu.
 */

static void
recent_files_menu_remove_items(GtkWidget * app, GList * name_list)
{
    if (name_list)
    {
	gchar *path = "_File/<Separator>";
	gnome_app_remove_menus(GNOME_APP(app), path,
			       g_list_length(name_list) + 1);
    }
}

/*
 * Make the file menu show the new recent file list.
 * Precondition - the old entries have been removed with recent_files_menu_flush.
 */

void
recent_files_menu_fill(GtkWidget * app, GList * name_list)
{
    MenuPos mp;

    if (name_list == NULL) return;

    /* Insert separator and get its position */
    mp = recent_files_menu_insert_separator(app);
    /* Insert the items */
    recent_files_menu_insert_items(app, &mp, name_list);
}

/*
 * Remove the recent file list items from the file menu.
 *
 * All changes require that the list is removed and a new one built, because we
 * add a digit in front of the file name.
 */

void
recent_files_menu_flush(GtkWidget * app, GList * name_list)
{
    /* Update the file menu */
    recent_files_menu_remove_items(app, name_list);
}
