/*
 * time_label.h
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

#ifndef __TIME_LABEL_H__
#define __TIME_LABEL_H__

#include <gtk/gtklabel.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif  /*__cplusplus */

#define TYPE_TIME_LABEL             (time_label_get_type ())
#define TIME_LABEL(obj)            (GTK_CHECK_CAST ((obj), TYPE_TIME_LABEL, TimeLabel))
#define TIME_LABEL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), TYPE_TIME_LABEL, TimeLabelClass))
#define IS_TIME_LABEL(obj)         (GTK_CHECK_TYPE ((obj), TYPE_TIME_LABEL))
#define IS_TIME_LABEL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), TYPE_TIME_LABEL))
    
typedef struct _TimeLabel       TimeLabel;
typedef struct _TimeLabelClass  TimeLabelClass;
    
struct _TimeLabel
{
    GtkLabel label;
    time_t   dsp_time;
    gchar    *format_string;
};

struct _TimeLabelClass
{
    GtkLabelClass  parent_class;

    void (*time_changed) (TimeLabel *time_label);
};

GtkType    time_label_get_type(void);
GtkWidget* time_label_new(void);
time_t     time_label_set_time(TimeLabel *time_label, time_t dsp_time);
time_t     time_label_get_time(TimeLabel *time_label);
void       time_label_set_format(TimeLabel *time_label, gchar *format);

#ifdef __cplusplus
} 
#endif  /*__cplusplus */

#endif  /* __TIME_LABEL_H__ */
