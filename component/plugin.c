/*
 * plugin.c
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

#include "plugin.h"

static GList     *plugin_list = NULL;
static GtkWidget *plugin_clist = NULL;
static gchar     *plugin_typelist [] =
{
    N_("Basic"),
    N_("Note"),
    N_("View"),
    NULL
};
static gchar     *titles[] =
{
    N_("Name"),
    N_("Path"),
    N_("Type"),
    N_("Uses"),
    N_("Preload")
};
static gint      num_titles = sizeof (titles) / sizeof (titles[0]);
static gchar     txt_do_preload[] = N_("Yes");
static gchar     txt_no_preload[] = N_("No");

GtkWidget        *plugin_preload_changed;      /* preload button */


/* ------------------------------------------------------ */
/*
 * initialize or update the plugin list 
 */

void
plugin_create_list(void)
{
    plugin_list_entry *p_entry;
    gchar             *home;
    gchar             *dir;
    GList             *l;
    gboolean          clear;
    gint              row;
    
    if (plugin_list)
    {    
        clear = TRUE;
        l = g_list_first(plugin_list);
        while (l)
        {
            p_entry = (plugin_list_entry*) l->data;
            if (p_entry && p_entry->uses == 0 && p_entry->preload == FALSE)
            {
                row = clist_row_by_pathname(GTK_CLIST(plugin_clist),
                                            p_entry->path);
                g_free(p_entry->name);
                g_free(p_entry->path);
                if (row > -1)
                {
                    gtk_clist_remove(GTK_CLIST(plugin_clist), row);
                }
                plugin_list = g_list_remove(plugin_list, p_entry);
                g_free(p_entry);
                clear = FALSE;
            }
            if (clear)
            {
                l = g_list_next(l);
            }
            else
            {
                clear = TRUE;
                l = g_list_first(plugin_list);
            }
        }
    }
    
    /*
     * FIXME: use a PATH-entry from preferences here
     */

    plugin_read_dir("./Plugins/Palm/.libs/");
    plugin_read_dir("./Plugins/Pgp5/.libs/");
    
    home = g_get_home_dir();
    if (home)
    {
        dir = g_strdup_printf("%s/.%s/plugins/%s/", home, PACKAGE, VERSION);
        plugin_read_dir(dir);
        g_free(dir);
    }
    
    dir = gnome_unconditional_libdir_file(PACKAGE "/plugins/" VERSION "/");
    plugin_read_dir(dir);
    g_free(dir);    
}

/* ------------------------------------------------------ */
/*
 * read all plugins in directory
 */

void
plugin_read_dir(gchar *path)
{
    plugin_list_entry *p_entry;
    DIR               *dir;
    struct dirent     *entry;
    gchar             *fname;
    gboolean          res;
    gchar             *found;
    
    dir = opendir(path);
    if (dir)
    {
        while ((entry = readdir(dir)))
        {
            if (! strcmp(&(entry->d_name[strlen(entry->d_name)-3]), ".so"))
            {
                fname = g_strconcat(path, entry->d_name, NULL);
                found = list_entry_by_pathname(plugin_list, fname);
                if (!found)
                {
                    p_entry = g_new(plugin_list_entry, 1);
                    if (p_entry)
                    {
                        res = plugin_fetch_info(&p_entry, fname);
                        if (!res)
                        {
                            plugin_list = g_list_append(plugin_list, p_entry);
                        }
                        else
                        {
                            g_free(p_entry);
                        }
                    }
                    else
                    {
                        g_warning(_("Can't malloc."));
                    }
                }
                g_free(fname);
            }
        }
        closedir(dir);
    }
}

/* ------------------------------------------------------ */
/*
 * fetch basic plugin information
 * return FALSE upon success
 */

gint
plugin_fetch_info(plugin_list_entry **plist, gchar *name)
{
    gchar         *(*return_name) (void);
    e_plugin_type (*return_plugin_type)(void);
    gchar         *msg;
    gint          res;
    gint          err;

    err = FALSE;
    (*plist)->handle = g_module_open(name, G_MODULE_BIND_LAZY);
    if ((*plist)->handle)
    {
        res = g_module_symbol((*plist)->handle, "return_name",
                              (gpointer *) &return_name);
        if (res)
        {
            /* get name */
            (*plist)->name = g_strdup(return_name());
            res = g_module_symbol((*plist)->handle, "return_plugin_type",
                              (gpointer *) &return_plugin_type);
            if (res)
            {
                (*plist)->preload = FALSE;
                /* get type */
                (*plist)->plugin_type = return_plugin_type();
                /* add full path */
                (*plist)->path = g_strdup(name);
                /* init uses */
                (*plist)->uses = 0;
            }
            else
            {
                err = TRUE;
                g_free((*plist)->name);
            }
        }
        else
        {
            err = TRUE;
        }
        g_module_close((*plist)->handle);
        (*plist)->handle = NULL;
    }
    else
    {
        msg = g_strdup_printf(_("Can't load plugin %s"), name);
        g_warning(msg);
        g_free(msg);
        err = TRUE;
    }
    
    return (err);
}

/* ------------------------------------------------------ */
/*
 * create the plugin list dialog
 */

GtkWidget*
plugin_select_dialog(void)
{
    static GtkWidget   *vbox = NULL;
    GtkWidget          *scrolledwindow;
    GtkWidget          *hbuttonbox;
    static const gchar *b_label[] =
    {
        N_("Load plugin"),
        N_("Unload plugin"),
        N_("Refresh list"),
        N_("Preload"),
    };
    gint               buttons = sizeof (b_label) / sizeof (b_label[0]);
    GtkWidget          *button[buttons];
    gint               i;
    GdkFont            *font;
    static int         translated;
    
    if (!translated)
    {
	for (i = 0; i < num_titles; i++)
	  titles[i] = _(titles[i]);
	translated = 1;
    }
    
    if (vbox == NULL)
    {        
        vbox = gtk_vbox_new(FALSE, 2);
        gtk_widget_show(vbox);
        
        scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_set_border_width(GTK_CONTAINER(scrolledwindow), 0);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
                                       GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_AUTOMATIC);
        gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
        gtk_widget_show(scrolledwindow);
        
        plugin_clist = gtk_clist_new_with_titles(num_titles, titles);
        gtk_clist_column_titles_passive(GTK_CLIST(plugin_clist));
        gtk_clist_set_selection_mode(GTK_CLIST(plugin_clist),
                                     GTK_SELECTION_EXTENDED);
        font = gtk_widget_get_style(plugin_clist)->font;
        for (i = 0; i < num_titles; i++)
        {
            gtk_clist_set_column_min_width(GTK_CLIST(plugin_clist), i,
                                           gdk_string_width(font, _(titles[i])));
        }        
        gtk_widget_show(plugin_clist);
        gtk_container_add(GTK_CONTAINER(scrolledwindow),
                          GTK_WIDGET(plugin_clist));
        
        hbuttonbox = gtk_hbutton_box_new();
        gtk_widget_show(hbuttonbox);
        gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, TRUE, 0);
        
        for (i = 0; i < buttons; i++)
        {
            button[i] = gtk_button_new_with_label(_(b_label[i]));
            gtk_widget_show(button[i]);
            GTK_WIDGET_SET_FLAGS(button[i], GTK_CAN_DEFAULT);
            gtk_container_add(GTK_CONTAINER(hbuttonbox), button[i]);
        }
        
        plugin_preload_changed = button[3];
        
        gtk_signal_connect(GTK_OBJECT(button[0]), "clicked",
                           GTK_SIGNAL_FUNC(cb_plugin_load), plugin_clist);
        gtk_signal_connect(GTK_OBJECT(button[1]), "clicked",
                           GTK_SIGNAL_FUNC(cb_plugin_unload), plugin_clist);
        gtk_signal_connect(GTK_OBJECT(button[2]), "clicked",
                           GTK_SIGNAL_FUNC(cb_plugin_refresh), NULL);
        gtk_signal_connect(GTK_OBJECT(button[3]), "clicked",
                           GTK_SIGNAL_FUNC(cb_plugin_toggle_preload),
                           plugin_clist);
    }
    
    return (vbox);
}

/* ------------------------------------------------------ */
/*
 * show list of plugins in dialog
 */

void
plugin_fill_clist(GtkCList *clist, GList *list)
{
    plugin_list_entry *entry;
    gchar             *line[num_titles];
    GList             *l;
    gint              row;
    gint              s_row;
    gchar             *txt;
    gboolean          ok;
    
    g_return_if_fail(clist != NULL);
    if (list == NULL)
    {
        return;
    }
    
    gtk_clist_freeze(clist);
    l = g_list_first(list);
    while (l)
    {
        ok = FALSE;
        entry = (plugin_list_entry*) l->data;
        s_row = clist_row_by_pathname(clist, entry->path);
        if (entry->uses == 0)
        {
            if (s_row != -1)
            {
                gtk_clist_get_text(clist, s_row, 4, &txt);
                if (!strcmp(_(txt_no_preload), txt))
                {
                    ok = TRUE;
                }
            }
            else
            {
                ok = TRUE;
            }            
            if (ok)
            {
                line[0] = entry->name;
                line[1] = entry->path;
                line[2] = _(plugin_typelist[entry->plugin_type]);
                line[3] = g_strdup_printf("%d", entry->uses);
                line[4] = _(txt_no_preload);
                row = gtk_clist_append(clist, line);
                g_free(line[3]);
                gtk_clist_set_row_data(clist, row, entry);
            }
        }
        else
        {
            txt = g_strdup_printf("%d", entry->uses);
            gtk_clist_set_text(clist, s_row, 3, txt);
            g_free(txt);
        }
        l = g_list_next(l);
    }
    
    gtk_clist_columns_autosize(clist);    
    gtk_clist_thaw(clist);
}

/* ------------------------------------------------------ */
/*
 * load plugins
 */

void
cb_plugin_load(GtkWidget *w, gpointer list)
{
    GtkCList          *clist;
    GList             *s_list;
    GtkWidget         *warn_w;
    plugin_list_entry *entry;
    gint              row;
    gchar             *buf;
    gint              (*constructor) (plugin_list_entry **);
    gint              res;
    
    g_return_if_fail(list != NULL);

    clist = GTK_CLIST(list);
    s_list = clist->selection;
    while (s_list)
    {
        row = (gint) s_list->data;
        entry = (plugin_list_entry *) gtk_clist_get_row_data(clist, row);
        if (entry->uses == 0)
        {
            entry->handle = g_module_open(entry->path, G_MODULE_BIND_LAZY);
            if (entry->handle)
            {
                res = g_module_symbol(entry->handle, "constructor",
                                      (gpointer *) &constructor);
                if (res)
                {
                    res = constructor(&entry);
                    if (!res)
                    {
                        buf = g_strdup_printf("%d", entry->uses);
                        gtk_clist_set_text(clist, row, 3, buf);
                        g_free(buf);
                        buf = g_strdup_printf(
                            _("Loaded plugin \'%s\' from %s"),
                            entry->name, entry->path);
                        set_status_text(buf);
                        g_free(buf);
                    }
                    else
                    {
                        buf = g_strdup_printf(_("Construtor for %s failed"),
                                              entry->name);
                        warn_w = gnome_warning_dialog_parented(
                            buf, yank_root_win(NULL));
                        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                        g_free(buf);
                        g_module_close(entry->handle);
                    }
                }
                else
                {
                    buf = g_strdup_printf(_("Can't load construtor for %s"),
                                          entry->name);
                    warn_w = gnome_warning_dialog_parented(
                        buf, yank_root_win(NULL));
                    gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                    g_free(buf);
                    g_module_close(entry->handle);
                }
                
            }
            else
            {
                buf = g_strdup_printf(_("Can't load %s"), entry->name);
                warn_w = gnome_warning_dialog_parented(buf,
                                                       yank_root_win(NULL));
                gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                g_free(buf);
            }
        }
        s_list = g_list_next(s_list);
    }
}

/* ------------------------------------------------------ */
/*
 * unload plugins
 */

void
cb_plugin_unload(GtkWidget *w, gpointer list)
{
    GtkCList          *clist;
    GList             *s_list;
    plugin_list_entry *entry;
    gint              row;
    GtkWidget         *warn_w;
    gint              res;
    gint              (*destructor) (plugin_list_entry **);
    gchar             *buf;
    
    g_return_if_fail(list != NULL);

    clist = GTK_CLIST(list);
    s_list = clist->selection;
    s_list = g_list_first(s_list);
    while (s_list)
    {
        row = (gint) s_list->data;
        entry = (plugin_list_entry *) gtk_clist_get_row_data(clist, row);
        if (entry->uses == 1)
        {
            switch (entry->plugin_type)
            {
            case PLUGIN_BASIC:
                destructor = ((plugin_func_basic*)(entry->func))->destructor;
                break;
            case PLUGIN_NOTE:
                destructor = ((plugin_func_note*)(entry->func))->destructor;
                break;
            case PLUGIN_VIEW:
                destructor = ((plugin_func_view*)(entry->func))->destructor;
                break;
            case PLUGIN_FILE:
                destructor = ((plugin_func_file*)(entry->func))->destructor;
                break;
            default :
                g_warning(_("Unknown plugin type."));
                return;
            };
            
            res = destructor(&entry);
            if (!res)
            {
                buf = g_strdup_printf("%d", entry->uses);
                gtk_clist_set_text(clist, row, 3, buf);
                g_free(buf);
                g_module_close(entry->handle);
                buf = g_strdup_printf(_("Removed plugin \'%s\' from %s"),
                                      entry->name, entry->path);
                set_status_text(buf);
                g_free(buf);
                
            }
            else
            {
                buf = g_strdup_printf(_("Destrutor for %s failed"),
                                      entry->name);
                warn_w = gnome_warning_dialog_parented(
                    buf, yank_root_win(NULL));
                gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                g_free(buf);
            }
        }
        else
        {
            if (entry->uses > 1)
            {
                buf = g_strdup_printf(_("Plugin %s is still in use!"),
                                      entry->name);
                warn_w = gnome_warning_dialog_parented(
                    buf, yank_root_win(NULL));
                gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                g_free(buf);
            }
        }
        s_list = g_list_next(s_list);
    }
}

/* ------------------------------------------------------ */
/*
 * refresh the plugin list if possible
 */

void
cb_plugin_refresh(GtkWidget *w, gpointer p)
{
    GtkWidget *warn_w;
    
    if (!g_module_supported ())
    {
        warn_w = gnome_warning_dialog_parented(
            _("No support for plugins here."), yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
    }
    else
    {
        plugin_create_list();
        plugin_fill_clist(GTK_CLIST(plugin_clist), plugin_list);
    }
}

/* ------------------------------------------------------ */
/*
 * toggle the preload flag
 */

void
cb_plugin_toggle_preload(GtkWidget *w, gpointer p)
{
    GtkCList          *list;
    GList             *sel;
    gchar             *txt;
    plugin_list_entry *entry;
    
    g_return_if_fail(p != NULL);
    g_return_if_fail(GTK_IS_CLIST(p));

    list = GTK_CLIST(p);
    sel = list->selection;
    if (sel != NULL)
    {
        gtk_clist_freeze(list);
        sel = g_list_first(sel);
        while (sel != NULL)
        {
            entry = (plugin_list_entry *) gtk_clist_get_row_data(
                list, (gint) sel->data);
            
            if (entry->preload == TRUE)
            {
                entry->preload = FALSE;
                txt = _(txt_no_preload);
            }
            else
            {
                entry->preload = TRUE;
                txt = _(txt_do_preload);
            }
            gtk_clist_set_text(list, (gint) sel->data, 4, txt);
            
            sel = g_list_next(sel);
        }
        gtk_clist_thaw(list);
    }
}

/* ------------------------------------------------------ */
/*
 * return number of items, list of new item-pointers
 */

void
plugin_create_preload_list(gint *list_length, gchar ***pre_list)
{
    plugin_list_entry *entry;
    GList             *list;
    gint              pos;

    list = g_list_first(plugin_list);
    *list_length = 0;
    while (list)
    {
        entry = (plugin_list_entry*) list->data;
        if (entry->preload == TRUE)
        {
            *list_length += 1;
        }
        list = g_list_next(list);
    }

    if (*list_length > 0)
    {
        *pre_list = g_new(gchar*, *list_length);
        pos = 0;
        list = g_list_first(plugin_list);
        while (list != NULL)
        {
            entry = (plugin_list_entry*) list->data;
            if (entry->preload == TRUE && pos < *list_length)
            {
                (*pre_list)[pos] = entry->name;
                pos++;
            }
            list = g_list_next(list);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * set list entries& preload them
 */

void
plugin_preload_list(gint items, gchar **list)
{
    plugin_list_entry *entry;
    gint              item;
    gint              list_item;
    gboolean          found;
    gchar             *txt;
    
    if (!g_module_supported())
    {
        return;
    }
    
    plugin_select_dialog();

    if (items == 0)
    {
        return;
    }
    
    g_return_if_fail(plugin_clist != NULL);
    g_return_if_fail(list != NULL);
    
    cb_plugin_refresh(NULL, NULL);
    item = 0;
    while (item < items)
    {
        found = FALSE;
        list_item = 0;
        while (list_item < GTK_CLIST(plugin_clist)->rows && found == FALSE)
        {
            gtk_clist_get_text(GTK_CLIST(plugin_clist), list_item, 0, &txt);
            if (txt != NULL)
            {
                if (!strcmp(txt, list[item]))
                {
                    found = TRUE;

                    entry = (plugin_list_entry *) gtk_clist_get_row_data(
                        GTK_CLIST(plugin_clist), list_item);
                    entry->preload = TRUE;
                    
                    gtk_clist_select_row(GTK_CLIST(plugin_clist),
                                         list_item, 0);
                    cb_plugin_load(NULL, plugin_clist);
                    gtk_clist_unselect_row(GTK_CLIST(plugin_clist),
                                           list_item, 0);
                    
                    txt = _(txt_do_preload);
                    gtk_clist_set_text(GTK_CLIST(plugin_clist),
                                       list_item, 4, txt);
                }
            }
            list_item++;
        }
        item++;
    }
}

/* ------------------------------------------------------ */
/*
 * search path in list
 */

gchar*
list_entry_by_pathname(GList *list, gchar *path)
{
    plugin_list_entry *tmp_entry;
    GList             *tmp_list;
    gchar             *ret;
    
    tmp_list = g_list_first(list);
    ret = NULL;
    
    while (tmp_list && !ret)
    {
        tmp_entry = (plugin_list_entry*) tmp_list->data;
        if (tmp_entry)
        {
            if (!strcmp(tmp_entry->path, path))
            {
                ret = tmp_entry->path;
            }
        }
        tmp_list = g_list_next(tmp_list);
    }
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * search path in clist widget
 */

gint
clist_row_by_pathname(GtkCList *clist, gchar *path)
{
    gint     row;
    gint     pos;
    gboolean found;
    gchar    *txt;

    if (!GTK_IS_CLIST(clist))
    {
        return (-1);
    }
    
    row = -1;
    pos = 0;
    found = FALSE;
    while (pos < clist->rows && !found)
    {
        gtk_clist_get_text(clist, pos, 1, &txt);
        if (!strcmp(txt, path))
        {
            found = TRUE;
            row = pos;
        }
        else
        {
            pos++;
        }
    }
    return (row);
}

/* ------------------------------------------------------ */
