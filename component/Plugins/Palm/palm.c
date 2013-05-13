/*
 * palm.c
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

#include <pi-source.h>
#include <pi-socket.h>
#include <pi-memo.h>
#include <pi-todo.h>
#include <pi-dlp.h>

#include "plugindefs.h"
#include "notedefs.h"

/* FIXME: let the user decide */
#define PILOTPORT "/dev/pilot"


extern GtkWidget    *yank_main_app(GtkWidget *);
extern GtkWindow    *yank_root_win(GtkWindow *);
extern GtkCTreeNode *do_add_note(note_data *, GtkCTreeNode *);
extern void         do_edit_note(note_data *, GtkCTreeNode *);
extern long int     isodate2days(char *);
extern gchar        *create_note_id(void);
extern GtkCTreeNode *selected_node;
extern GtkWidget    *note_tree;


static void cb_palm_dialog(GtkWidget *, gpointer);
static gint destructor(plugin_list_entry **);

static plugin_func_basic my_functions =
{
    &(destructor),
    NULL
};

static gchar             name[]      = "Palm HotSync";
static e_plugin_type     plugin_type = PLUGIN_BASIC;
static plugin_list_entry *list_entry = NULL;
static GnomeUIInfo       palm_menu [] =
{
    {
        GNOME_APP_UI_ITEM, N_("HotSync"), N_("HotSync with a palm pilot"),
        cb_palm_dialog, NULL, NULL, 0, 0, (gchar)NULL, GDK_CONTROL_MASK
    },
    GNOMEUIINFO_END
};
static GtkWidget         *b_memos = NULL;
static GtkWidget         *b_todo = NULL;

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
    gnome_app_insert_menus(GNOME_APP(yank_main_app(NULL)), N_("M_isc/"),
                           palm_menu);
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * destroy plugin
 */

gint
destructor(plugin_list_entry **entry)
{
    gint ret;

    ret = 0;

    if ((*entry)->uses == 1)
    {
        /* uninstall menu */
        gnome_app_remove_menus(GNOME_APP(yank_main_app(NULL)),
                               N_("M_isc/HotSync"), 1);
        
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
 * kill the hotsync dialog
 */

static void
cb_palm_close(GtkWidget *w, gpointer p)
{
    gtk_widget_hide(GTK_WIDGET(p));
    gtk_widget_destroy(GTK_WIDGET(p));
    p = NULL;
    
    unuse_me(list_entry);
}

/* ------------------------------------------------------ */
/*
 * search node in category
 */

static GtkCTreeNode*
find_node_by_id(GtkCTreeNode *start, gchar *id)
{
    GtkCTreeNode *node;
    GtkCTreeNode *ret;
    note_data    *data;
    
    ret = NULL;
    
    if (start)
    {
        node = GTK_CTREE_ROW(start)->children;
    }
    else
    {
        node = GTK_CTREE_NODE(GTK_CLIST(note_tree)->row_list);
    }

    while (node && !ret)
    {
        data = gtk_ctree_node_get_row_data(GTK_CTREE(note_tree),
                                           GTK_CTREE_NODE(node));
        if (data)
        {
            if (!strcmp(data->id, id))
            {
                ret = node;
            }
        }
        else
        {
            g_warning(_("Note without data!\n"));
        }
        node = GTK_CTREE_NODE_NEXT(node);
    }
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * search node in childs
 */

static GtkCTreeNode*
find_or_add_node_by_name(GtkCTreeNode *start, gchar *name)
{
    GtkCTreeNode *node;
    GtkCTreeNode *ret;
    note_data    *data;
    
    ret = NULL;
    
    if (start)
    {
        node = GTK_CTREE_ROW(start)->children;
    }
    else
    {
        node = GTK_CTREE_NODE(GTK_CLIST(note_tree)->row_list);
    }
    
    while (node && !ret)
    {
        data = gtk_ctree_node_get_row_data(GTK_CTREE(note_tree),
                                           GTK_CTREE_NODE(node));
        if (data)
        {
            if (!strcmp(data->title, name))
            {
                ret = node;
            }
        }
        else
        {
            g_warning(_("Note without data!\n"));
        }
        node = GTK_CTREE_ROW(node)->sibling;
    }
    
    if (!ret)
    {
        data = g_new(note_data, 1);
        data->notetype = TextNote;
        data->title = g_strdup(name);
        data->id = create_note_id();
        data->text = NULL;
        data->created = time(NULL);
        data->changed = data->created;
        data->changes = 0;
        data->expire = -1;
        ret = do_add_note(data, start);
    }
    
    return (ret);
}

/* ------------------------------------------------------ */
/*
 * dump palm memo database
 */

static void
palm_sync_memos(gint sd, GtkText *log, struct PilotUser *piu)
{
    gint               db;
    static gchar       database[] = "MemoDB";
    static gchar       start_m [] = "Reading Memos\n";
    unsigned char      buffer[0xffff];
    char               appblock[0xffff];
    struct MemoAppInfo mai;
    gint               i;
    GtkCTreeNode       *parent;
    GtkCTreeNode       *category_parent;
    GtkCTreeNode       *n_node;
    note_data          *note;
    gint               titlelen;
    gint               j;
    gchar              *msg;
    
    titlelen = 80;

    gtk_text_insert(log, NULL, NULL, NULL, start_m, strlen(start_m));
    
    if (dlp_OpenDB(sd, 0, 0x80|0x40, database, &db) < 0)
    {
        msg = g_strdup_printf("Unable to open %s\n", database);
        gtk_text_insert(log, NULL, NULL, NULL, msg, strlen(msg));
        dlp_AddSyncLogEntry(sd, msg);
        g_free(msg);
        return;
    }
    
    parent = find_or_add_node_by_name(selected_node, database);

    dlp_ReadAppBlock(sd, db, 0, (unsigned char *)appblock, 0xffff);
    unpack_MemoAppInfo(&mai, (unsigned char *)appblock, 0xffff);

    for (i=0;;i++)
    {
        struct Memo m;
        int         attr;
        int         category;
        recordid_t  id;
        gchar       *buf;
        gint        len;
        
        len = dlp_ReadRecordByIndex(sd, db, i, buffer, &id, 0, &attr,
                                    &category);
        if (len < 0)
        {
            break;
    	}
        
        /* Skip deleted records */
        if ((attr & dlpRecAttrDeleted) || (attr & dlpRecAttrArchived))
        {
            continue;
        }

        
        unpack_Memo(&m, buffer, len);

        category_parent = find_or_add_node_by_name(
            parent, mai.category.name[category]);

        note = g_new(note_data, 1);
        note->notetype = TextNote;
        note->title = g_new(gchar, titlelen);
        j = 0;
        while (j < titlelen - 1 && m.text[j] != '\0' && m.text[j] != '\n')
        {
            note->title[j] = m.text[j];
            j++;
        }
        note->title[j] = '\0';
        g_renew(gchar, note->title, j);
        note->text = g_strdup(m.text);
/*         buf = g_strdup_printf("%lu:%s", id, create_note_id()); */
        buf = g_strdup_printf("%lu", id);
        note->id = buf;
        note->created = time(NULL);
        note->changed = note->created;
        note->changes = 0;
        note->expire = -1;

        n_node = find_node_by_id(parent, note->id);
        if (attr & dlpRecAttrDirty)
        {
            /* note has been modified on pilot */
            
            if (n_node != NULL)
            {
                g_message("found note with id :%s", note->id);
                do_edit_note(note, n_node);
            }
            else
            {
                g_message("adding note with id :%s", note->id);
                do_add_note(note, category_parent);
            }
        }
        else
        {
            if (n_node == NULL)
            {
                do_add_note(note, category_parent);
            }
        }
        free_Memo(&m);
    }
    dlp_ResetSyncFlags(sd, db);
    dlp_CleanUpDatabase(sd, db);
    dlp_CloseDB(sd, db);    
}

/* ------------------------------------------------------ */
/*
 * dump palm todo database
 */

static void
palm_sync_todos(gint sd, GtkText *log)
{
    struct ToDoAppInfo tai;
    gint               db;
    gint               i;
    static gchar       database[] = "ToDoDB";
    static gchar       start_m [] = "Reading ToDos\n";
    gchar              *msg;
    GtkCTreeNode       *parent;
    GtkCTreeNode       *category_parent;
    GtkCTreeNode       *n_node;
    unsigned char      buffer[0xffff];
    note_data          *note;
    
    gtk_text_insert(log, NULL, NULL, NULL, start_m, strlen(start_m));
    
    if(dlp_OpenDB(sd, 0, 0x80|0x40, "ToDoDB", &db) < 0)
    {
        msg = g_strdup_printf("Unable to open %s\n", database);
        gtk_text_insert(log, NULL, NULL, NULL, msg, strlen(msg));
        dlp_AddSyncLogEntry(sd, msg);
        g_free(msg);
        return;        
    }

    parent = find_or_add_node_by_name(selected_node, database);
    
    dlp_ReadAppBlock(sd, db, 0, buffer, 0xffff);
    unpack_ToDoAppInfo(&tai, buffer, 0xffff);
    
    for (i=0;;i++)
    {
  	struct ToDo t;
  	gint        attr;
        gint        category;
        recordid_t  id;
        gchar       *buf;
        gint        len;
        
  	len = dlp_ReadRecordByIndex(sd, db, i, buffer, &id, 0, &attr,
                                    &category);
  	if (len < 0)
        {
            break;
        }
        
  	/* Skip deleted records */
  	if ((attr & dlpRecAttrDeleted) || (attr & dlpRecAttrArchived))
        {
            continue;
        }
        
	unpack_ToDo(&t, buffer, len);

        category_parent = find_or_add_node_by_name(
            parent, tai.category.name[category]);

        note = g_new(note_data, 1);
        note->notetype = (mktime(&t.due) != 0) ? TodoNote : CheckNote;
        note->title = g_strdup(t.description);
        buf = g_strdup_printf("%lu", id);
        note->id = buf;
        note->created = time(NULL);
        note->changed = note->created;
        note->changes = 0;
        note->expire = -1;
        note->deadline = mktime(&(t.due));
        note->todo = t.complete;
        note->complete = note->todo ? 100 : 0;
        note->prio = t.priority;
        note->text = g_strdup(t.note);
/*         do_add_note(note, category_parent); */
        n_node = find_node_by_id(parent, note->id);
        if (attr & dlpRecAttrDirty)
        {
            /* note has been modified on pilot */
            
            if (n_node != NULL)
            {
                g_message("found note with id :%s", note->id);
                do_edit_note(note, n_node);
            }
            else
            {
                g_message("adding note with id :%s", note->id);
                do_add_note(note, category_parent);
            }
        }
        else
        {
            if (n_node == NULL)
            {
                do_add_note(note, category_parent);
            }
        }
	free_ToDo(&t);
    }
    dlp_ResetSyncFlags(sd, db);
    dlp_CleanUpDatabase(sd, db);
    dlp_CloseDB(sd, db);    
}

/* ------------------------------------------------------ */
/*
 * hotsync now
 */

static void
cb_palm_sync(GtkWidget *w, gpointer p)
{
    struct pi_sockaddr addr;
    struct PilotUser   U;
    gchar              *ptr;
    gint               sd;
    gint               err;
    gint               ret;
    GtkText            *log;
    
    g_return_if_fail(GTK_IS_TEXT(p));
    log = GTK_TEXT(p);
    
    err = FALSE;
    ptr = getenv("PILOTPORT");
    if (ptr)
    {
        strcpy(addr.pi_device, ptr);
    }
    else
    {
        strcpy(addr.pi_device, PILOTPORT);
    }

    ptr = g_strdup_printf(_("Press HotSync on %s\n"), addr.pi_device);
    gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
    g_free(ptr);
    
    if (!(sd = pi_socket(PI_AF_SLP, PI_SOCK_STREAM, PI_PF_PADP)))
    {
        ptr = g_strdup_printf("Error: pi_socket\n");
        gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
        g_free(ptr);
        err = TRUE;
    }
    else
    {
        addr.pi_family = PI_AF_SLP;
  
        ret = pi_bind(sd, (struct sockaddr*)&addr, sizeof(addr));
        if (ret == -1)
        {
            ptr = g_strdup_printf("Error: pi_bind\n");
            gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
            g_free(ptr);
            err = TRUE;
        }
        else
        {
            ret = pi_listen(sd, 1);
            if(ret == -1)
            {
                ptr = g_strdup_printf("Error: pi_listen\n");
                gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
                g_free(ptr);
                err = TRUE;
            }
            else
            {
                sd = pi_accept(sd, 0, 0);
                if(sd == -1)
                {
                    ptr = g_strdup_printf("Error: pi_accept\n");
                    gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
                    g_free(ptr);
                    err = TRUE;
                }
                else
                {
                    /* Ask the pilot who it is. */
                    dlp_ReadUserInfo(sd, &U);

                    ptr = g_strdup_printf("Got user \"%s\" with ID %lu\n",
                                          U.username, U.userID);
                    gtk_text_insert(log, NULL, NULL, NULL, ptr,
                                    strlen(ptr));
                    g_free(ptr);
                    
                    /* Tell user (via Pilot) that we are
                       starting things up */
                    dlp_OpenConduit(sd);
                    
                    if (gtk_toggle_button_get_active(
                        GTK_TOGGLE_BUTTON(b_memos)))
                    {
                        palm_sync_memos(sd, log, &U);
                    }
                    if (gtk_toggle_button_get_active(
                        GTK_TOGGLE_BUTTON(b_todo)))
                    {
                        palm_sync_todos(sd, log);
                    }
                    
                    dlp_EndOfSync(sd, 0);                    
                    pi_close(sd);
                    
                    ptr = g_strdup("Done\n");
                    gtk_text_insert(log, NULL, NULL, NULL, ptr, strlen(ptr));
                    g_free(ptr);
                }
            }
        }
    }
}

/* ------------------------------------------------------ */
/*
 * hotsync dialog
 */

static void
cb_palm_dialog(GtkWidget *w, gpointer p)
{
    static GtkWidget *dialog = NULL;
    const gchar      *buttons[] =
    {
        _("Start"),
        GNOME_STOCK_BUTTON_CLOSE, 
        NULL
    };
    gchar            *title;
    GtkWidget        *label1;
    GtkWidget        *frame1;
    GtkWidget        *vbox1;
    GtkWidget        *hbox1;
    GtkWidget        *log;
    
    use_me(list_entry);
    
    title = g_strdup_printf(_("%s - HotSync"), PACKAGE);
    dialog = gnome_dialog_newv(title, buttons);
    g_free(title);
    gnome_dialog_set_parent(GNOME_DIALOG(dialog), yank_root_win(NULL));
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 1);
    
    label1 = gtk_label_new(_("Select a tree node and a db"));
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
                       label1, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);
    
    frame1 = gtk_frame_new(_("DB"));
    gtk_widget_show(frame1);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
                       frame1, TRUE, TRUE, 0);
    
    vbox1 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(frame1), vbox1);
    
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 0);
    
    b_memos = gtk_check_button_new_with_label(_("Memos"));
    gtk_widget_show(b_memos);
    gtk_box_pack_start(GTK_BOX(hbox1), b_memos, FALSE, FALSE, 0);
    
    b_todo = gtk_check_button_new_with_label (_("Todos"));
    gtk_widget_show(b_todo);
    gtk_box_pack_end(GTK_BOX(hbox1), b_todo, FALSE, FALSE, 0);
    
    frame1 = gtk_frame_new(_("Log"));
    gtk_widget_show(frame1);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),
                       frame1, TRUE, TRUE, 0);
    
    log = gtk_text_new(NULL, NULL);
    gtk_text_set_editable(GTK_TEXT(log), FALSE);
    gtk_widget_show(log);
    gtk_container_add(GTK_CONTAINER(frame1), log);

    gnome_dialog_button_connect(GNOME_DIALOG(dialog), 0,
                                GTK_SIGNAL_FUNC(cb_palm_sync), log);
    gnome_dialog_button_connect(GNOME_DIALOG(dialog), 1,
                                GTK_SIGNAL_FUNC(cb_palm_close), dialog);
    gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
                       GTK_SIGNAL_FUNC(cb_palm_close), dialog);
    
    gtk_widget_show(dialog);
}

/* ------------------------------------------------------ */
