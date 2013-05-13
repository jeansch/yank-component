/*
 * run_pipe.c
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

#include "run_pipe.h"

static GtkWidget *pentry;
static GtkWidget *r_replace;
static GtkWidget *r_append;
static GtkWidget *r_external;
static GtkWidget *e_external;
static gchar     *selection;
static GtkWidget *t_entry;

/* ------------------------------------------------------ */
/*
 * filter selection
 */

void
cb_run_pipe(GtkWidget *w, gpointer p)
{
    const gchar      *buttons[] =
    {
        GNOME_STOCK_BUTTON_OK,
        GNOME_STOCK_BUTTON_CLOSE,
        NULL
    };
    static GtkWidget *pdialog = NULL;
    GtkWidget        *frame;
    GtkWidget        *mvbox;
    GtkWidget        *hbox;
    GtkWidget        *vbox;
    GtkWidget        *gentry;
    GSList           *b_list;
    
    t_entry = NULL;
    selection = get_text_selection(&t_entry);
    if (selection == NULL)
    {
        return;
    }
        
    if (pdialog == NULL)
    {
        pdialog = gnome_dialog_newv(_("Run pipe"), buttons);
        gnome_dialog_set_parent(GNOME_DIALOG(pdialog), yank_root_win(NULL));
        gnome_dialog_set_default(GNOME_DIALOG(pdialog), 0);
        mvbox = gtk_vbox_new(FALSE, 0);
        gtk_widget_show(mvbox);
        gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(pdialog)->vbox), mvbox, FALSE,
                           FALSE, 2);
        frame = gtk_frame_new(_("Run pipe (like: 'sort | nl')"));
        gtk_widget_show(frame);
        gtk_box_pack_start(GTK_BOX(mvbox), frame, FALSE, FALSE, 2);
        gentry = gnome_entry_new(_("yank pipe history"));
        gtk_widget_show(gentry);
        gtk_container_add(GTK_CONTAINER(frame), gentry);
        pentry = gnome_entry_gtk_entry(GNOME_ENTRY(gentry));
        
        frame = gtk_frame_new(_("Options"));
        gtk_widget_show(frame);
        gtk_box_pack_start(GTK_BOX(mvbox), frame, FALSE, FALSE, 2);
        vbox = gtk_vbox_new(FALSE, 0);
        gtk_widget_show(vbox);
        gtk_container_add(GTK_CONTAINER(frame), vbox);
        hbox = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(hbox);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
        r_replace = gtk_radio_button_new_with_label(NULL,
                                                    _("Replace selection"));
        gtk_box_pack_start(GTK_BOX(hbox), r_replace, FALSE, FALSE, 2);
        gtk_widget_show(r_replace);
        b_list = gtk_radio_button_group(GTK_RADIO_BUTTON(r_replace));
        r_append = gtk_radio_button_new_with_label(
            b_list, _("Append after selection"));
        gtk_box_pack_end(GTK_BOX(hbox), r_append, FALSE, FALSE, 2);
        gtk_widget_show(r_append);
        b_list = gtk_radio_button_group(GTK_RADIO_BUTTON(r_append));
        hbox = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(hbox);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
        r_external = gtk_radio_button_new_with_label(b_list,
                                                     _("Use external viewer"));
        gtk_box_pack_start(GTK_BOX(hbox), r_external, FALSE, FALSE, 2);
        gtk_widget_show(r_external);
        b_list = gtk_radio_button_group(GTK_RADIO_BUTTON(r_external));
        e_external = gtk_entry_new();
        gtk_box_pack_end(GTK_BOX(hbox), e_external, FALSE, FALSE, 2);
        gtk_widget_show(e_external);
        /* FIXME: select this in settings */
        gtk_entry_set_text(GTK_ENTRY(e_external), "gless");
        
        gnome_dialog_button_connect(GNOME_DIALOG(pdialog), 0,
                                    GTK_SIGNAL_FUNC(cb_run_pipe_ok), pdialog);
        gnome_dialog_button_connect(GNOME_DIALOG(pdialog), 1,
                                    GTK_SIGNAL_FUNC(cb_run_pipe_close),
                                    pdialog);
        gtk_signal_connect(GTK_OBJECT(pdialog), "destroy",
                           GTK_SIGNAL_FUNC(gtk_widget_destroyed), &pdialog);
        
        gnome_dialog_editable_enters(GNOME_DIALOG(pdialog),
                                     GTK_EDITABLE(pentry));
        gnome_dialog_editable_enters(GNOME_DIALOG(pdialog),
                                     GTK_EDITABLE(e_external));
        gtk_window_set_modal(GTK_WINDOW(pdialog), TRUE);
        gtk_widget_grab_focus(pentry);
        gtk_widget_show(pdialog);
    }
    else
    {
        g_assert(GTK_WIDGET_REALIZED(pdialog));
        gdk_window_show(pdialog->window);
        gdk_window_raise(pdialog->window);
    }
}

/* ------------------------------------------------------ */
/*
 * finally smoke the pipe ... 
 */

void
cb_run_pipe_ok(GtkWidget *w, gpointer p)
{
    gchar       *command;
    gchar       *external;
    gchar       *e_cmd;
    FILE        *pp;
    gchar       *tmpfile;
    struct stat statbuf;
    gint        s_start;
    gint        s_end;
    gint        s_end2;
    gchar       *r_buffer;
    size_t      numb;
    GtkWidget   *warn_w;
    gint        p_ret;
    
    tmpfile = NULL;
    command = gtk_entry_get_text(GTK_ENTRY(pentry));
    external = gtk_entry_get_text(GTK_ENTRY(e_external));

    if (command == NULL)
    {
        warn_w = gnome_warning_dialog_parented(
            _("Please enter a command to execute!"), yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }
    else
    {
        if (!strlen(command))
        {
            warn_w = gnome_warning_dialog_parented(
                _("Please enter a command to execute!"), yank_root_win(NULL));
            gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
            return;
        }
    }
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_external)))
    {
        if (external == NULL)
        {
            warn_w = gnome_warning_dialog_parented(
                _("External viewer command is missing!"), yank_root_win(NULL));
            gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
            return;
        }
        else
        {
            if (!strlen(external))
            {
                warn_w = gnome_warning_dialog_parented(
                    _("External viewer command is missing!"),
                    yank_root_win(NULL));
                gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                return;
            }
        }
        
        /*
         * view selection in external viewer
         */
        
        e_cmd = g_strdup_printf("%s|%s&", command, external);
        pp = popen(e_cmd, "w");
        fprintf(pp, "%s", selection);
        pclose(pp);
        g_free(e_cmd);
    }
    else
    {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_append)) ||
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(r_replace)))
        {
            /*
             * FIXME: this should be done by using coprocesses
             */
            
            tmpfile = tmpnam(NULL);
            e_cmd = g_strdup_printf("%s>%s", command, tmpfile);
            pp = popen(e_cmd, "w");
            fprintf(pp, "%s", selection);
            p_ret = pclose(pp);
            if (p_ret != 0)
            {
                unlink(tmpfile);
                warn_w = gnome_warning_dialog_parented(
                    _("Error while running pipe!"), yank_root_win(NULL));
                gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
                return;
            }
            g_free(e_cmd);
            stat(tmpfile, &statbuf);
            if (statbuf.st_size > 0)
            {
                s_start = GTK_EDITABLE(t_entry)->selection_start_pos;
                s_end = GTK_EDITABLE(t_entry)->selection_end_pos;
                pp = fopen(tmpfile, "r");
                if (pp != NULL)
                {
                    r_buffer = g_malloc((gulong) statbuf.st_size);
                    numb = fread(r_buffer, statbuf.st_size, 1, pp);
                    if (numb > 0)
                    {
                        s_end2 = s_end;
                        gtk_editable_insert_text(GTK_EDITABLE(t_entry),
                                                 r_buffer, statbuf.st_size,
                                                 &s_end2);
                        if (gtk_toggle_button_get_active(
                            GTK_TOGGLE_BUTTON(r_replace)))
                        {
                            gtk_editable_delete_text(GTK_EDITABLE(t_entry),
                                                     s_start, s_end);
                        }
                    }
                    g_free(r_buffer);
                    fclose(pp);
                }
            }
            unlink(tmpfile);
        }
    }
    cb_run_pipe_close(w, p);
}

/* ------------------------------------------------------ */
/*
 * close 'run pipe' dialog
 */

void
cb_run_pipe_close(GtkWidget *w, gpointer p)
{
    gtk_widget_hide(GTK_WIDGET(p));
    gtk_widget_destroy(GTK_WIDGET(p));
}

/* ------------------------------------------------------ */
/*
 * handler for sigpipe
 */

void
handle_sigpipe(int signum)
{
    g_error(_("caught sigpipe\n"));
}

/* ------------------------------------------------------ */
