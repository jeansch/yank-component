/*
 * pgp5.c
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

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "plugindefs.h"
#include "notedefs.h"

extern GtkWidget    *yank_main_app(GtkWidget *);
extern GtkWindow    *yank_root_win(GtkWindow *);
extern void         ask_commit_changes(void);
extern GtkCTreeNode *selected_node;
extern GtkWidget    *note_tree;
extern GtkWidget    *text_entry;

static void cb_pgp5_dialog(GtkWidget *, gpointer);
static gint destructor(plugin_list_entry **);

static plugin_func_basic my_functions =
{
    &(destructor),
    NULL
};

static gchar             name[]      = N_("Pgp5");
static e_plugin_type     plugin_type = PLUGIN_BASIC;
static plugin_list_entry *list_entry = NULL;
/*
 * FIXME: set from main app
 */
static gchar             menu_base_txt[] = N_("M_isc/");
static gchar             menu_entry_txt[] = N_("Pgp5");
static GnomeUIInfo       pgp5_menu [] =
{
    {
        GNOME_APP_UI_ITEM, menu_entry_txt, N_("en-/decrypt with pgp5"),
        cb_pgp5_dialog, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};

/* ------------------------------------------------------ */
/*
 * init plugin
 */

gint
constructor(plugin_list_entry **entry)
{
    gint ret;
    
    ret = 0;

    (*entry)->uses = 1;                    /* just loaded */
    (*entry)->func = NULL;
    list_entry = (*entry);
    list_entry->func = &my_functions;
    
    /* install menu */
    gnome_app_insert_menus(GNOME_APP(yank_main_app(NULL)), menu_base_txt,
                           pgp5_menu);
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * destroy plugin
 */

gint
destructor(plugin_list_entry **entry)
{
    gchar *menu;
    gint  ret;

    ret = 0;

    if ((*entry)->uses == 1)
    {
        /* uninstall menu */
        menu = g_strconcat(menu_base_txt, menu_entry_txt, NULL);
        gnome_app_remove_menus(GNOME_APP(yank_main_app(NULL)), menu, 1);
        g_free(menu);
        (*entry)->uses = 0;
        list_entry = NULL;
    }
    else
    {
        ret = 1;
    }
    
    return (ret);
}
              
/* ------------------------------------------------------ */
/*
 * tell name
 */

gchar*
return_name(void)
{
    return (name);
}

/* ------------------------------------------------------ */
/*
 * tell type
 */

e_plugin_type
return_plugin_type(void)
{
    return (plugin_type);
}

/* ------------------------------------------------------ */
/*
 * inc reference
 */

static void
use_me(plugin_list_entry *entry)
{
    entry->uses++;
}

/* ------------------------------------------------------ */
/*
 * dec reference
 */

static void
unuse_me(plugin_list_entry *entry)
{
    entry->uses--;
}

/* ------------------------------------------------------ */
/*
 * stolen from mutt
 */

static int 
compare_stat (struct stat *osb, struct stat *nsb)
{
  if (osb->st_dev != nsb->st_dev || osb->st_ino != nsb->st_ino ||
      osb->st_rdev != nsb->st_rdev)
  {
    return -1;
  }

  return 0;
}

int
safe_open (const char *path, int flags)
{
  struct stat osb, nsb;
  int fd;

  if ((fd = open (path, flags, 0600)) < 0)
    return fd;

  /* make sure the file is not symlink */
  if (lstat (path, &osb) < 0 || fstat (fd, &nsb) < 0 ||
      compare_stat(&osb, &nsb) == -1)
  {
    close (fd);
    return (-1);
  }

  return (fd);
}

/* ------------------------------------------------------ */
/*
 * stolen from pgp-integration(1) 
 */

/*Runs cmd, and hooks in to the process' stdin, and out to its stdout.
 *Inspiration stolen from Michael Elkins; see mutt for how to do this
 *properly with error checking, etc.
 */

static pid_t
run_pgp(char *cmd, FILE **in, FILE **out, FILE **err)
{
    gint  pin[2], pout[2], perr[2];
    pid_t child_pid;
    
    *in = *out = *err = NULL;
    child_pid = 0;
    
    if (pipe(pin) == -1)
    {
        return (-1);
    }
    if (pipe(pout) == -1)
    {
        close(pin[0]);
        close(pin[1]);
        return (-1);
    }
    if (pipe(perr) == -1)
    {
        close(pin[0]);
        close(pin[1]);
        close(pout[0]);
        close(pout[1]);
        return (-1);
    }
    
    if (!(child_pid = fork()))
    {
        /*We're the child.*/
        close(pin[1]);
        dup2(pin[0], 0);
        close(pin[0]);
        
        close(pout[0]);
        dup2(pout[1], 1);
        close(pout[1]);
        
        close (perr[0]);
        dup2 (perr[1], 2);
        close (perr[1]);
        
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(127);
    }
    else
    {
        /* FIXME: add a warning here */
        if (child_pid == -1)
        {
            close(pin[0]);
            close(pin[1]);
            close(pout[0]);
            close(pout[1]);
            close(perr[0]);
            close(perr[1]);
            return (-1);
        }
    }
    
    /*Only get here if we're the parent.*/
    close(pout[1]);
    *out = fdopen(pout[0], "r");
    
    close(pin[0]);
    *in = fdopen(pin[1], "w");
    
    close (perr[1]);
    *err = fdopen (perr[0], "r");

    return (child_pid);
}

/* ------------------------------------------------------ */
/*
 * encrypt a note
 */

static void
cb_encrypt(GtkWidget *w, gpointer p)
{
    GtkWidget    *pass_entry;
    GtkWidget    *warn_w;
    FILE         *pgpin;
    FILE         *pgpout;
    FILE         *pgperr;
    static gchar sys_buf[] = "pgpe -catfq +batchmode";
    gchar        tmpbuf[1024];
    note_data    *note;
    
    g_return_if_fail(p != NULL);
    g_return_if_fail(GNOME_IS_DIALOG(p));

    if (selected_node == NULL)
    {
        warn_w = gnome_warning_dialog_parented(
            _("Select a note first!"), yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }

    ask_commit_changes();

    note = (note_data *) gtk_ctree_node_get_row_data(
        GTK_CTREE(note_tree), GTK_CTREE_NODE(selected_node));
    
    if (!note->text || !strlen(note->text))
    {
        warn_w = gnome_warning_dialog_parented(_("Nothing to encrypt!"),
                                               yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }
    
    pass_entry = gtk_object_get_user_data(GTK_OBJECT(p));
    setenv("PGPPASSFD", "0", 1);
    run_pgp(sys_buf, &pgpin, &pgpout, &pgperr); /*Execute PGP*/

    if(pgpin && pgpout)
    {
        fprintf(pgpin, "%s\n", gtk_entry_get_text(GTK_ENTRY(pass_entry)));
        /*Output buffer to PGP:*/
        fwrite(note->text, sizeof(gchar), strlen(note->text), pgpin);
        fclose(pgpin);

        gtk_text_freeze(GTK_TEXT(text_entry));
        gtk_editable_delete_text(GTK_EDITABLE(text_entry), 0,
                                 gtk_text_get_length(GTK_TEXT(text_entry)));
        /*Now, read the result back from PGP:*/
        do
        {
            fgets(tmpbuf, sizeof(tmpbuf), pgpout);
            if (!feof(pgpout))
            {
                gtk_text_insert(GTK_TEXT(text_entry), NULL, NULL, NULL,
                                tmpbuf, strlen(tmpbuf));
            }
        } while (!feof(pgpout));
        gtk_text_thaw(GTK_TEXT(text_entry));
        
        fclose(pgpout);
    }
    unsetenv("PGPPASSFD");
}
/* ------------------------------------------------------ */
/*
 * decrypt a note
 */

static void
cb_decrypt(GtkWidget *w, gpointer p)
{
    GtkWidget    *pass_entry;
    GtkWidget    *warn_w;
    FILE         *pgpin;
    FILE         *pgpout;
    FILE         *pgperr;
    static gchar sys_buf[] = "pgpv -qzd +batchmode +force -o - %s";
    gchar        tmpbuf[1024];
    gchar        *tmpfname;
    gchar        *tmpcmd;
    note_data    *note;
    pid_t        pgp_pid;
    gint         fd;
    
    g_return_if_fail(p != NULL);
    g_return_if_fail(GNOME_IS_DIALOG(p));

    if (selected_node == NULL)
    {
        warn_w = gnome_warning_dialog_parented(
            _("Select a note first!"), yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }

    ask_commit_changes();

    note = (note_data *) gtk_ctree_node_get_row_data(
        GTK_CTREE(note_tree), GTK_CTREE_NODE(selected_node));
    
    if (!note->text || !strlen(note->text))
    {
        warn_w = gnome_warning_dialog_parented(_("Nothing to decrypt!"),
                                               yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }

    tmpfname = tmpnam(NULL);
    if (tmpfname == NULL)
    {
        warn_w = gnome_warning_dialog_parented(_("Can't create tmp filename!"),
                                               yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }
    fd = safe_open(tmpfname, O_CREAT | O_EXCL | O_WRONLY);
    if (fd == -1)
    {
        warn_w = gnome_warning_dialog_parented(_("Can't open tmp file!"),
                                               yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        return;
    }
    write(fd, note->text, strlen(note->text));
    close(fd);

    tmpcmd = g_strdup_printf(sys_buf, tmpfname);
    
    pass_entry = gtk_object_get_user_data(GTK_OBJECT(p));
    setenv("PGPPASSFD", "0", 1);
    
    pgp_pid = run_pgp(tmpcmd, &pgpin, &pgpout, &pgperr);

    if (pgp_pid == -1)
    {
        warn_w = gnome_warning_dialog_parented(_("Error while running pgp!"),
                                               yank_root_win(NULL));
        gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
        
        unsetenv("PGPPASSFD");
        unlink(tmpfname);
        g_free(tmpcmd);
        return;
    }
    
    if (pgpin && pgpout)
    {
        fprintf(pgpin, "%s\n", gtk_entry_get_text(GTK_ENTRY(pass_entry)));
        fwrite(note->text, sizeof (gchar), strlen(note->text), pgpin);
        fclose(pgpin);

        gtk_text_freeze(GTK_TEXT(text_entry));
        gtk_editable_delete_text(GTK_EDITABLE(text_entry), 0,
                                 gtk_text_get_length(GTK_TEXT(text_entry)));
        do
        {
            fgets(tmpbuf, sizeof (tmpbuf), pgpout);
            if (!feof(pgpout))
            {
                gtk_text_insert(GTK_TEXT(text_entry), NULL, NULL, NULL,
                                tmpbuf, strlen(tmpbuf));
            }
        } while (!feof(pgpout));
        gtk_text_thaw(GTK_TEXT(text_entry));
        
        fclose(pgpout);
        fclose(pgperr);
        
        if ((gtk_text_get_length(GTK_TEXT(text_entry)) == 0) && note->text)
        {
            warn_w = gnome_warning_dialog_parented(_("Pgp didn't return any\
 text. Maybe your're using the wrong password?"), yank_root_win(NULL));
            gtk_window_set_modal(GTK_WINDOW(warn_w), TRUE);
            gtk_text_insert(GTK_TEXT(text_entry), NULL, NULL, NULL,
                            note->text, strlen(note->text));
        }
    }
    
    unlink(tmpfname);
    g_free(tmpcmd);
    unsetenv("PGPPASSFD");
}

/* ------------------------------------------------------ */
/*
 * kill the pgp5 dialog
 */

static void
cb_pgp5_close(GtkWidget *w, gpointer p)
{
    gtk_widget_hide(GTK_WIDGET(p));
    gtk_widget_destroy(GTK_WIDGET(p));
    p = NULL;
    
    unuse_me(list_entry);
}

/* ------------------------------------------------------ */
/*
 * 
 */

static void
cb_pgp5_dialog(GtkWidget *w, gpointer p)
{
    static GtkWidget *dialog = NULL;
    const gchar      *buttons[] =
    {
        _("Encrypt"),
        _("Decrypt"),
        GNOME_STOCK_BUTTON_CLOSE, 
        NULL
    };
    gchar            *title;
    GtkWidget        *hbox;
    GtkWidget        *label;
    GtkWidget        *pass_entry;
    
    use_me(list_entry);
    
    title = g_strdup_printf(_("%s - %s"), PACKAGE, name);
    dialog = gnome_dialog_newv(title, buttons);
    g_free(title);
    gnome_dialog_set_parent(GNOME_DIALOG(dialog), yank_root_win(NULL));
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);
    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), hbox, FALSE,
                       FALSE, 0);
    label = gtk_label_new(_("Password "));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, TRUE, 0);
    pass_entry = gtk_entry_new();
    gtk_object_set_user_data(GTK_OBJECT(dialog), pass_entry);
    gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);
    gtk_widget_show(pass_entry);
    gtk_box_pack_start(GTK_BOX(hbox), pass_entry, TRUE, TRUE, 0);
    
    gnome_dialog_button_connect(GNOME_DIALOG(dialog), 0,
                                GTK_SIGNAL_FUNC(cb_encrypt), dialog);
    gnome_dialog_button_connect(GNOME_DIALOG(dialog), 1,
                                GTK_SIGNAL_FUNC(cb_decrypt), dialog);
    gnome_dialog_button_connect(GNOME_DIALOG(dialog), 2,
                                GTK_SIGNAL_FUNC(cb_pgp5_close), dialog);
    gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
                       GTK_SIGNAL_FUNC(cb_pgp5_close), dialog);
    
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 2);
    gnome_dialog_editable_enters(GNOME_DIALOG(dialog),
                                 GTK_EDITABLE(pass_entry));
    gtk_widget_grab_focus(pass_entry);
    
    gtk_widget_show(dialog);
}

/* ------------------------------------------------------ */
