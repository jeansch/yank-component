/*
 * plugindefs.h
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

#ifndef YANK_PLUGINDEFS_H
#define YANK_PLUGINDEFS_H
	
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
#include <gmodule.h>

typedef enum
{
    PLUGIN_BASIC,
    PLUGIN_NOTE,
    PLUGIN_VIEW,
    PLUGIN_FILE
} e_plugin_type;

typedef struct _plugin_list_entry
{
    gboolean      preload;                /* load upon start? */
    gchar         *name;
    gchar         *path;
    e_plugin_type plugin_type;
    guint         uses;                   /* reference counter */
    GModule       *handle;
    void          *func;                  /* plugin_func_[basic|note|...] */
} plugin_list_entry;

typedef struct _plugin_func_basic
{
    gint          (*destructor) (plugin_list_entry **);
    void          (*func);
} plugin_func_basic;

typedef struct _plugin_func_note
{
    gint          (*destructor) (plugin_list_entry **);
    void          (*func);
} plugin_func_note;

typedef struct _plugin_func_view
{
    gint          (*destructor) (plugin_list_entry **);
    void          (*func);
} plugin_func_view;

typedef struct _plugin_func_file
{
    gint          (*destructor) (plugin_list_entry **);
    void          (*func);
} plugin_func_file;

#endif  /* YANK_PLUGINDEFS_H */
