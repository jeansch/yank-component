/*
 * time_label.c
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

#include <gtk/gtk.h>
#include "time_label.h"

/* signals */
enum
{
    TIME_CHANGED,
    LAST_SIGNAL
};
static gint time_label_signals[LAST_SIGNAL] = { 0 };


static void time_label_init(TimeLabel *t_label);
static void time_label_class_init(TimeLabelClass *class);
static void time_label_show_time(TimeLabel *time_label);


/* ------------------------------------------------------ */
/*
 * return the current time
 */

time_t
time_label_get_time(TimeLabel *time_label)
{
    g_return_val_if_fail(IS_TIME_LABEL(time_label), -2);
    return (time_label->dsp_time);
}

/* ------------------------------------------------------ */
/*
 * display the current time using the format_string
 */

static void
time_label_show_time(TimeLabel *time_label)
{
    gchar buf[256];

    g_return_if_fail(IS_TIME_LABEL(time_label));

    if ((time_label->format_string != NULL) && (time_label->dsp_time >= 0))
    {
        strftime(buf, (sizeof (buf) - 1), time_label->format_string,
                 localtime(&(time_label->dsp_time)));
    }
    else
    {
        buf[0] = '\0';
    }
    gtk_label_set_text(GTK_LABEL(time_label), buf);
}

/* ------------------------------------------------------ */
/*
 * set the format_string, update label
 */

void
time_label_set_format(TimeLabel *time_label, gchar *format)
{
    g_return_if_fail(IS_TIME_LABEL(time_label));

    if (time_label->format_string != NULL)
    {
        g_free(time_label->format_string);
    }
    time_label->format_string = (format != NULL) ? g_strdup(format) : NULL;
    
    time_label_show_time(time_label);
}

/* ------------------------------------------------------ */
/*
 * set the time, update label
 */

time_t
time_label_set_time(TimeLabel *time_label, time_t dsp_time)
{
    time_t old_time;

    g_return_val_if_fail(IS_TIME_LABEL(time_label), -2);
    old_time = time_label->dsp_time;
    time_label->dsp_time = dsp_time;
    time_label_show_time(time_label);
    gtk_signal_emit(GTK_OBJECT(time_label), time_label_signals[TIME_CHANGED]);
    
    return (old_time);
}


/* ------------------------------------------------------ */
/*
 * return a new time_label
 */

GtkWidget*
time_label_new(void)
{
    TimeLabel *t_label;
    
    t_label = gtk_type_new(TYPE_TIME_LABEL);
    return (GTK_WIDGET(t_label));
}

/* ------------------------------------------------------ */
/*
 * initialise the time_label
 */

static void
time_label_init(TimeLabel *t_label)
{
    GtkLabel *label;

    label = GTK_LABEL(t_label);
    gtk_label_set_text(label, "");
    t_label->dsp_time = -1;
    t_label->format_string = NULL;
}

/* ------------------------------------------------------ */
/*
 * init. the time_label class
 */

static void
time_label_class_init(TimeLabelClass *class)
{
    GtkObjectClass *object_class;
    
    object_class = (GtkObjectClass*) class;
   
    time_label_signals[TIME_CHANGED] =
        gtk_signal_new("time_changed", GTK_RUN_FIRST, object_class->type,
                       GTK_SIGNAL_OFFSET (TimeLabelClass, time_changed),
                       gtk_signal_default_marshaller, GTK_TYPE_NONE, 0);
    
    
    gtk_object_class_add_signals(object_class, time_label_signals,
                                 LAST_SIGNAL);
    
    class->time_changed = NULL;    
}

/* ------------------------------------------------------ */
/*
 * return the type
 */

GtkType
time_label_get_type(void)
{
    static GtkType time_label_type = 0;
    
    if (!time_label_type)
    {
        static const GtkTypeInfo time_label_info =
        {
            "Time_Label",
            sizeof (TimeLabel),
            sizeof (TimeLabelClass),
            (GtkClassInitFunc) time_label_class_init,
            (GtkObjectInitFunc) time_label_init,
            /* reserved_1 */ NULL,
            /* reserved_2 */ NULL,
            (GtkClassInitFunc) NULL,
        };
        time_label_type = gtk_type_unique(gtk_label_get_type(),
                                          &time_label_info);        
    }
    return (time_label_type);
}

/* ------------------------------------------------------ */
