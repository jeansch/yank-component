/*
 * todolist.c
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

#include "todolist.h"
#include "privates.h"

extern SuperPrivate *sp;
/* listwidget */

/* ------------------------------------------------------ */
/*
 * add an entry to the list
 */

void
add_todo_item(note_data *item)
{
    static gchar *txt[] = { NULL, NULL, NULL, NULL, NULL };
    gchar  *prio_txt;
    gchar  *comp_txt;
    gint   row;
    
    if (item)
    {
        txt[0] = time2isodate(&(item->deadline));
        prio_txt = g_strdup_printf(_("%d"), item->prio);
        txt[1] = prio_txt;
        comp_txt = g_strdup_printf(_("%d"), item->complete);
        txt[2] = comp_txt;
        txt[3] = item->title;
        gtk_clist_freeze(GTK_CLIST(sp->todolist));
        row = gtk_clist_append(GTK_CLIST(sp->todolist), txt);
        gtk_clist_set_row_data(GTK_CLIST(sp->todolist), row, item);
        set_todolist_row_colors(GTK_CLIST(sp->todolist), row);
	if (GTK_CLIST(sp->todolist)->rows)
	{
	    gtk_clist_sort(GTK_CLIST(sp->todolist));
	    GTK_CLIST(sp->todolist)->row_list->prev = NULL;
	}
        gtk_clist_thaw(GTK_CLIST(sp->todolist));
        g_free(prio_txt);
        g_free(comp_txt);
    }
}

/* ------------------------------------------------------ */
/*
 * modify/ remove an entry
 */

void
edit_todo_item(note_data *old_i, note_data *new_i)
{
    gint pos;

    /* Delete from list if edit changed status from not done to done */
    if ((new_i->todo == 1) && (old_i->todo != 1))
    {
        delete_todo_item(old_i);
    }
    else
    {
        pos = gtk_clist_find_row_from_data(GTK_CLIST(sp->todolist), old_i);
        if (pos != -1)
        {
            gchar *buf;
            
	    gtk_clist_freeze(GTK_CLIST(sp->todolist));
            gtk_clist_set_text(GTK_CLIST(sp->todolist), pos, 0,
                               time2isodate((&new_i->deadline)));
            buf = g_strdup_printf(_("%d"), new_i->prio);
            gtk_clist_set_text(GTK_CLIST(sp->todolist), pos, 1, buf);
            g_free(buf);
            buf = g_strdup_printf(_("%d"), new_i->complete);
            gtk_clist_set_text(GTK_CLIST(sp->todolist), pos, 2, buf);
            g_free(buf);
            gtk_clist_set_text(GTK_CLIST(sp->todolist), pos, 3, new_i->title);
            gtk_clist_set_row_data(GTK_CLIST(sp->todolist), pos, new_i);
            set_todolist_row_colors(GTK_CLIST(sp->todolist), pos);
	    if (GTK_CLIST(sp->todolist)->rows)
	    {
		gtk_clist_sort(GTK_CLIST(sp->todolist));
		GTK_CLIST(sp->todolist)->row_list->prev = NULL;
	    }
	    gtk_clist_thaw(GTK_CLIST(sp->todolist));
        }
        else
        {
            add_todo_item(new_i);
        }
    }
}

/* ------------------------------------------------------ */
/*
 * remove an entry from the list
 */

void
delete_todo_item(note_data *item)
{
    gint pos;

    pos = gtk_clist_find_row_from_data(GTK_CLIST(sp->todolist), item);
    if (pos != -1)
    {
        gtk_clist_remove(GTK_CLIST(sp->todolist), pos);
    }
    else
    {
        gnome_warning_dialog_parented(_("Todolist (delete): Can't find note!"),
                                      yank_root_win(NULL));
    }
}

/* ------------------------------------------------------ */
/*
 * sort notes by date
 */

gint
sort_todo_items_date(GtkCList *list, gconstpointer ptr1,
                          gconstpointer ptr2)
{
    GtkCListRow *row1;
    GtkCListRow *row2;
    time_t      dl1;
    time_t      dl2;

    row1 = (GtkCListRow *)ptr1;
    row2 = (GtkCListRow *)ptr2;

    dl1 = ((note_data *)row1->data)->deadline;
    dl2 = ((note_data *)row2->data)->deadline;
    
    if (dl1 == -1)
    {
        if (dl2 == -1)
        {
	    return 0;
        }
        else
        {
	    return G_MAXINT;
        }
    }
    if (dl2 == -1)
    {
        return G_MININT;
    }
    
    return (dl1 - dl2);
}

/* ------------------------------------------------------ */
/*
 * sort notes by priority
 */

gint
sort_todo_items_prio(GtkCList *list, gconstpointer ptr1,
                     gconstpointer ptr2)
{
    GtkCListRow *row1;
    GtkCListRow *row2;
    gint        p1;
    gint        p2;
    
    row1 = (GtkCListRow *)ptr1;
    row2 = (GtkCListRow *)ptr2;

    p1 = ((note_data *)row1->data)->prio;
    p2 = ((note_data *)row2->data)->prio;
    
    return (p1 - p2);
}

/* ------------------------------------------------------ */
/*
 * sort notes by completion status
 */

gint
sort_todo_items_complete(GtkCList *list, gconstpointer ptr1, gconstpointer ptr2)
{
    GtkCListRow *row1;
    GtkCListRow *row2;
    gint        p1;
    gint        p2;
    
    row1 = (GtkCListRow *)ptr1;
    row2 = (GtkCListRow *)ptr2;

    p1 = ((note_data *)row1->data)->complete;
    p2 = ((note_data *)row2->data)->complete;
    
    return (p1 - p2);
}

/* ------------------------------------------------------ */
/*
 * sort notes by date, then priority
 */

gint
sort_todo_items_date_then_prio(GtkCList *w, gconstpointer ptr1,
                               gconstpointer ptr2)
{
   gint result;
   
   result = sort_todo_items_date(w, ptr1, ptr2);
   
   if (result)
     return result;
   else
     return sort_todo_items_prio(w, ptr1, ptr2);
}

/* ------------------------------------------------------ */
/*
 * sort notes by priority, then date
 */

gint
sort_todo_items_prio_then_date(GtkCList *w, gconstpointer ptr1,
                               gconstpointer ptr2)
{
   gint result;
   
   result = sort_todo_items_prio(w, ptr1, ptr2);
   
   if (result)
     return result;
   else
     return sort_todo_items_date(w, ptr1, ptr2);
}

/* ------------------------------------------------------ */
/*
 * sort notes by title
 */

gint
sort_todo_items_title(GtkCList *w, gconstpointer ptr1, gconstpointer ptr2)
{
    GtkCListRow *row1;
    GtkCListRow *row2;
    char        *title1;
    char        *title2;
    
    row1 = (GtkCListRow *)ptr1;
    row2 = (GtkCListRow *)ptr2;

    title1 = ((note_data *)row1->data)->title;
    title2 = ((note_data *)row2->data)->title;
    
    return strcmp(title1, title2);
}

/* ------------------------------------------------------ */
/*
 * do not sort (dummy function)
 */

gint
sort_todo_items_noop(GtkCList *w, gconstpointer ptr1, gconstpointer ptr2)
{
    return 0;
}

/* ------------------------------------------------------ */
/*
 * do the actual sorting
 */

void
sort_by_sorting_mode(gint mode)
{
    if (mode == 0)
    {
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
		      (GtkCListCompareFunc)(*sort_todo_items_noop));
        return;
    }

    switch (mode)
    {
    case 1:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_date));
        break;
    case 2:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_prio));
        break;
    case 3:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_complete));
        break;
    case 4:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_title));
        break;
    case 5:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_date_then_prio));
        break;
    case 6:
        gtk_clist_set_compare_func(GTK_CLIST(sp->todolist),
                      (GtkCListCompareFunc)(*sort_todo_items_prio_then_date));
        break;
    };
    if (GTK_CLIST(sp->todolist)->rows)
    {
	gtk_clist_sort(GTK_CLIST(sp->todolist));
	GTK_CLIST(sp->todolist)->row_list->prev = NULL;
    }
}

/* ------------------------------------------------------ */
/*
 * set colors for the whole list
 */

void
set_todolist_colors(void)
{
    gint row;

    for (row = 0; row < GTK_CLIST(sp->todolist)->rows; row++)
    {
        set_todolist_row_colors(GTK_CLIST(sp->todolist), row);
    }
}

/* ------------------------------------------------------ */
/*
 * set color for one row
 */

void
set_todolist_row_colors(GtkCList *list, gint row)
{
    GdkColor         color;
    GtkStyle         *style;
    yank_preferences *pref;
    guint            r;
    guint            g;
    guint            b;
    note_data        *data;
    GDate            *c_date;
    GDate            *n_date;
    
    g_return_if_fail(GTK_IS_CLIST(list));
    g_return_if_fail(row >= 0);

    pref = get_preferences();
    data = gtk_clist_get_row_data(list, row);
    c_date = g_date_new();
    g_date_set_time(c_date, time(NULL));
    n_date = g_date_new();
    g_date_set_time(n_date, data->deadline);
    style = gtk_style_copy(GTK_WIDGET(list)->style);

    if (pref->todolist_use_color_today
        && (g_date_compare(c_date, n_date) == 0))
    {
        parse_color_text(pref->todolist_bg_color_today, &r, &g, &b);
        color.red = r;
        color.green = g;
        color.blue = b;
        style->base[GTK_STATE_NORMAL] = color;
        parse_color_text(pref->todolist_fg_color_today, &r, &g, &b);
        color.red = r;
        color.green = g;
        color.blue = b;
        style->fg[GTK_STATE_NORMAL] = color;
    }
    if (pref->todolist_use_color_past && (time(NULL) > data->deadline)
        && data->deadline > 0)
    {
        parse_color_text(pref->todolist_bg_color_past, &r, &g, &b);
        color.red = r;
        color.green = g;
        color.blue = b;
        style->base[GTK_STATE_NORMAL] = color;
        parse_color_text(pref->todolist_fg_color_past, &r, &g, &b);
        color.red = r;
        color.green = g;
        color.blue = b;
        style->fg[GTK_STATE_NORMAL] = color;
    }
    
    gtk_clist_set_row_style(list, row, style);
    g_date_free(c_date);
    g_date_free(n_date);
}

/* ------------------------------------------------------ */
