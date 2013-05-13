/*
 * recent.h
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

#ifndef YANK_RECENT_H
#define YANK_RECENT_H

#include <gnome.h>

void recent_files_menu_fill  (GtkWidget *, GList *);
void recent_files_menu_flush (GtkWidget *, GList *);

#endif
