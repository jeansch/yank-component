/*
 * search.h
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Hu�mann <m.hussmann@home.ins.de>
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

#ifndef YANK_SEARCH_H
#define YANK_SEARCH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif   /* HAVE_CONFIG_H */

#include <gnome.h>
/* this works for me */
#include <libgnome/gnome-regex.h>
#include <regex.h>
#include "notedefs.h"
#include "app.h"

void cb_search_dialog(GtkWidget *, gpointer);

#endif  /* YANK_SEARCH_H */

