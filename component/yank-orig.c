/*
 * yank.c
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

#include "yank-orig.h"

/* ------------------------------------------------------ */

/*  GtkWidget *application;  */
/*  */
/* int */
/* main(int argc, char* argv[]) */
/* { */
/*     poptContext pctx; */
/*     const char  **args; */
/*     GnomeClient *client; */
/*  */
/* #ifdef ENABLE_NLS     */
/*     bindtextdomain(PACKAGE, GNOMELOCALEDIR); */
/*     textdomain(PACKAGE); */
/* #endif */
/*      */
/*     gnome_init_with_popt_table(PACKAGE, VERSION, argc, argv, */
/*                                options, 0, &pctx); */
/*      */
     /* Argument parsing */ 
/*      */
/*     args = poptGetArgs(pctx); */
/*      */
/*     poptFreeContext(pctx); */
/*      */
     /* Session Management */ 
/*      */
/*     client = gnome_master_client (); */
/*     gtk_signal_connect(GTK_OBJECT (client), "save_yourself", */
/*                        GTK_SIGNAL_FUNC(save_session), argv[0]); */
/*     gtk_signal_connect(GTK_OBJECT (client), "die", */
/*                        GTK_SIGNAL_FUNC(session_die), NULL); */
/*      */
/*      */
     /* Main app */ 
/*      */
/*     application = yank_app_new(geometry, file); */
/*      */
/*     gtk_widget_show(application); */
/*  */
/*     show_tool_status(GNOME_APP(application), get_preferences()); */
/*  */
/*     cb_todo_timeout(NULL);  regularly calls set_todolist_colors() */ 
/*      */
/*     gtk_main(); */
/*      */
/*     return (0); */
/* } */

/* ------------------------------------------------------ */

/* static gint */
/* save_session(GnomeClient *client, gint phase, GnomeSaveStyle save_style, */
/*               gint is_shutdown, GnomeInteractStyle interact_style, */
/*               gint is_fast, gpointer client_data) */
/* { */
/*     gchar** argv; */
/*     guint argc; */
    
    /* allocate 0-filled, so it will be NULL-terminated */
/*     argv = g_malloc0(sizeof(gchar*)*4); */
/*     argc = 1; */
    
/*     argv[0] = client_data; */

/*     if (geometry) */
/*     { */
/*         argv[1] = "--geometry"; */
/*         argv[2] = geometry; */
/*         argc = 3; */
/*     } */
    
/*     gnome_client_set_clone_command(client, argc, argv); */
/*     gnome_client_set_restart_command(client, argc, argv); */
    
/*     return (TRUE); */
/* } */

/* ------------------------------------------------------ */

/* static void */
/* session_die(GnomeClient* client, gpointer client_data) */
/* { */
/*     gtk_main_quit(); */
/* } */

/* ------------------------------------------------------ */


