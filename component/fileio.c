/*
 * fileio.c
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

#include "fileio.h"
#include "privates.h"

/* last filename used for loading/ saving */
static gchar        *filename = NULL;
/* flag used for loading files */
static GtkWidget    *load_new = NULL;
/* flags used for saving files */
static GtkWidget    *save_all = NULL;
static gint         quit_after_save = FALSE;

extern SuperPrivate *sp;

static gchar *note_types[] =
{
    "None",
    "TextNote",
    "CheckNote",
    "TodoNote",
    NULL
};
static int num_note_types = sizeof (note_types) / sizeof (note_types[0]);

/* ------------------------------------------------------ */
/*
 * show the fileselection to get a filename
 */

void
cb_file_load_dialog(GtkWidget *widget, gpointer p)
{
    static GtkWidget *fs = NULL;
    GtkWidget        *load_add;
    GSList           *load_op;
    GtkWidget        *hbox;
    GtkWidget        *frame;

    ask_commit_changes();
    
    if (fs == NULL)
    {
        fs = gtk_file_selection_new(_("Load notes"));

        frame = gtk_frame_new(_("Options"));
        gtk_widget_show(frame);
        gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(fs)->main_vbox),
                           frame, FALSE, FALSE, 0);
        hbox = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(hbox);
        gtk_container_add(GTK_CONTAINER(frame), hbox);
        load_new = gtk_radio_button_new_with_label(
            NULL, _("Load as a new file"));
        gtk_widget_show(load_new);
        gtk_box_pack_start(GTK_BOX(hbox), load_new, FALSE, FALSE, 3);
        load_op = gtk_radio_button_group(GTK_RADIO_BUTTON(load_new));
        load_add = gtk_radio_button_new_with_label(
            load_op, _("Add notes to selection(s)"));
        gtk_widget_show(load_add);
        gtk_box_pack_end(GTK_BOX(hbox), load_add, FALSE, FALSE, 3);
        load_op = gtk_radio_button_group(GTK_RADIO_BUTTON(load_add));
        
        gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                           "clicked", GTK_SIGNAL_FUNC(cb_load_file_ok), p);
        gtk_signal_connect_object(
            GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button), "clicked",
            GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(fs));
        gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                           GTK_SIGNAL_FUNC(gtk_widget_destroyed), &fs);
        gtk_widget_show(fs);
    }
    else
    {
        g_assert(GTK_WIDGET_REALIZED(fs));
        gdk_window_show(fs->window);
        gdk_window_raise(fs->window);
    }
}

/* ------------------------------------------------------ */
/*
 * shared by cb_load_file_ok and cb_recent_files
 */

gint
load_file(gchar * name)
{
    gint stat;
    
    gtk_clist_freeze(GTK_CLIST(sp->note_tree));
    stat = note_load(name, GTK_CTREE(sp->note_tree), NULL);

    g_free(filename);
    filename = g_strdup(name);

    sp->modified = FALSE;

    gtk_clist_thaw(GTK_CLIST(sp->note_tree));
    
    return (stat);
}

/* ------------------------------------------------------ */
/*
 * got the filename - start loading it
 */

void
cb_load_file_ok(GtkWidget *w, GtkWidget *app)
{
    gchar        *txt;
    gchar        *name;
    gint         stat;
    GList        *selection;
    GtkFileSelection *fs =
      (GtkFileSelection*) gtk_widget_get_ancestor(w,GTK_TYPE_FILE_SELECTION);
    
    name = gtk_file_selection_get_filename(fs);
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(load_new)))
    {
        cb_close(NULL, app);
	stat = load_file(name);
	txt = g_strdup_printf("Loaded: %s", filename);
    }
    else
    {
        gtk_clist_freeze(GTK_CLIST(sp->note_tree));

        stat = 1;
        selection = GTK_CLIST(sp->note_tree)->selection;    
        while (selection && stat)
        {
            if (selection->data)
            {
                stat = note_load(name, GTK_CTREE(sp->note_tree), selection->data);
            }
            else
            {
                g_warning(_("Selection without data while loading!"));
            }
            selection = selection->next;
        }

        txt = g_strdup_printf("Added: %s", name);

        sp->modified = TRUE;
	
	gtk_clist_thaw(GTK_CLIST(sp->note_tree));
    }
    
    if (!stat)
    {
        g_free(txt);
        txt = g_strdup_printf("Error while loading: %s", name);
    }
    set_status_text(txt);
    g_free(txt);
    
    gtk_widget_destroy(GTK_WIDGET(fs));
}

/* ------------------------------------------------------ */
/*
 * start loading notes
 */

gint
note_load(gchar *name, GtkCTree *tree, GtkCTreeNode *parent_node)
{
    xmlNodePtr       node;
    xmlDocPtr        doc;
    gint             ret;
    gchar            *titlebuf;
    yank_preferences *pref;
    GDate            *today;

    ret = 0;
    titlebuf = NULL;
    doc = xmlParseFile(name);

    if (doc)
    {
        node = doc->root->childs;
        if (node)
        {
            if (xmlGetProp(doc->root->childs, "title"))
            {
                /*
                 * load notes created from yank-0.0.1 or yank-0.0.2
                 * will be removed in yank-1.0.0
                 */
                load_notelist(parent_node, node);
            }
            else
            {
	        today = g_date_new();
                g_date_set_time(today, time(NULL));
                get_notes(parent_node, doc, node, today);
                g_date_free(today);
            }
        }
        
        xmlFreeDoc(doc);
        titlebuf = g_strdup_printf("%s (%s)", PACKAGE, name);
        gtk_window_set_title(yank_root_win(NULL), titlebuf);
        g_free(titlebuf);
        
        pref = get_preferences();
        sort_by_sorting_mode(pref->sorting_mode);
       
        ret = 1;
    }
    return (ret);
}

/* ------------------------------------------------------ */

/*
 * wrappers around xmlGetProp() to handle memory issues
 */

static int
int_xmlGetProp(xmlNodePtr node, const xmlChar *name)
{
    xmlChar* buf;
    int result;
    if (NULL == (buf = xmlGetProp(node, name)))
      return 0;
    result = atoi(buf);
    free(buf);
    return result;
}

static long int
days_xmlGetProp(xmlNodePtr node, const xmlChar *name)
{
    xmlChar* buf;
    long int result;
    if (NULL == (buf = xmlGetProp(node, name)))
      return -1;
    result = isodate2time(buf);
    free(buf);
    return result;
}

static xmlChar*
string_xmlGetProp(xmlNodePtr node, const xmlChar *name)
{
   xmlChar* result;
   if (NULL == (result = xmlGetProp(node, name)))
     return strdup("");
   return result;
}

/*
 * read contents of a node
 */

note_data*
get_note(xmlDocPtr doc, xmlNodePtr node, gint *expanded)
{
    note_data  *note;
    gint       cont;
    gchar      *cptr;
    gint       i;
    gint       found;

    note = NULL;
    if (node)
    {
        note = (note_data *) malloc(sizeof (note_data));
        note->title = NULL;
        note->id   = NULL;
        note->text = NULL;
        note->expire = -1;
        note->created = -1;
        note->changed = -1;
        note->changed = -1;
        note->prio = 0;
        note->deadline = 0;
        note->complete = 0;
        note->notetype = None;
        note->todo = FALSE;
        
        /* Type */
        cptr = string_xmlGetProp(node, "Type");
        i = 0;
        found = FALSE;
        while (i < num_note_types && !found)
        {
            if (!strcmp(cptr, note_types[i]))
            {
                found = TRUE;
                note->notetype = i;
            }
            else
            {
                i++;
            }
        }
	free(cptr);
        if (!found)
        {
            note->notetype = TextNote;
        }
        
        note->expire = days_xmlGetProp(node, "Expire");
        note->created = days_xmlGetProp(node, "Created");
        note->changed = days_xmlGetProp(node, "Changed");
        note->changes = int_xmlGetProp(node, "Changes");
            
        if (note->notetype == CheckNote || note->notetype == TodoNote)
        {
            note->todo = int_xmlGetProp(node, "Todo");
        }
        
        if (note->notetype == TodoNote)
        {
            note->prio = int_xmlGetProp(node, "Priority");
            note->deadline = days_xmlGetProp(node, "Deadline");
            note->complete = int_xmlGetProp(node, "Complete");
        }
        
        *expanded = int_xmlGetProp(node, "Expanded");
        
        /* ID, Title, Text */
        node = node->childs;
        cont = TRUE;
        while (node != NULL && cont == TRUE)
        {
            if (!strcmp(node->name, "Node"))
            {
                /* found start of next node - don't continue here */
                cont = FALSE;
            }
            else
            {
                if (!strcmp(node->name, "ID"))
                {
                    note->id = xmlNodeListGetString(
                        doc, node->childs, 1);
                }
                if (!strcmp(node->name, "Title"))
                {
                    note->title = xmlNodeListGetString(
                        doc, node->childs, 1);
                }
                if (!strcmp(node->name, "Text"))
                {
                    note->text = xmlNodeListGetString(
                        doc, node->childs, 1);
                }
                node = node->next;
            }
        }
        
        /*
         * make sure we've an id etc. (will silently convert old files)
         */
        
        if (note->id == NULL)
        {
            note->id = create_note_id();
        }
        if (note->created == -1)
        {
            note->created = time(NULL);
        }
        if (note->changed == -1)
        {
            note->changed = time(NULL);
        }
        if (note->changes == -1)
        {
            note->changes = 0;
        }
    }

    return (note);
}

/* ------------------------------------------------------ */
/*
 * scan xml-tree
 */

int
get_notes(GtkCTreeNode *pnode, xmlDocPtr doc, xmlNodePtr node, GDate *today)
{
    GtkCTreeNode *n_node;
    note_data    *note;
    gint         expanded;
    gint         expired = 0;
    gint         rec_complete;
    GDate        *e_date;

    n_node = NULL;
    if (node)
    {
        if (!strcmp(node->name, "Node"))
        {
            note = get_note(doc, node, &expanded);
            e_date = g_date_new();
            if (note->expire != -1)
            {
                g_date_set_time(e_date, note->expire);
            }
	    if ((note->expire == -1) || (g_date_compare(e_date, today) > 0))
	    {
                rec_complete = get_preferences()->recursive_completion;
                get_preferences()->recursive_completion = 0;

		n_node = do_add_note(note, pnode);

                get_preferences()->recursive_completion = rec_complete;
		
		/*
		 * notes are expanded by default
		 */
		
		if (expanded == 0)
		{
		    gtk_ctree_collapse(GTK_CTREE(sp->note_tree), n_node);
		}
	    }
	    else
	    {
		free_note(note);
		expired = 1;
	    }
            g_date_free(e_date);
        }

        if (node->childs)
        {
	    if (expired)
	      get_notes(pnode, doc, node->childs, today);
	    else
	      get_notes(n_node, doc, node->childs, today);
        }
        if (node->next)
        {
            get_notes(pnode, doc, node->next, today);
        }
    }
    return (0);
}


/* ------------------------------------------------------ */
/*
 * load notes
 */

void
load_notelist(GtkCTreeNode *cnode, xmlNodePtr xnode)
{
    note_data    *note;
    GtkCTreeNode *n_node;
    gint         exp;
    gint         rec_complete;
    
    note = (note_data *) malloc(sizeof (note_data));
    note->title = NULL;
    note->text = NULL;
    note->prio = 0;
    note->deadline = 0;
    note->complete = 0;
    note->notetype = None;
    note->todo = FALSE;
    
    note->title = g_strdup(xmlGetProp(xnode, "title"));
    
    note->notetype = atoi(xmlGetProp(xnode, "type"));
    if (note->notetype == CheckNote || note->notetype == TodoNote)
    {
        note->todo = atoi(xmlGetProp(xnode, "todo"));
    }
    
    if (note->notetype == TodoNote)
    {
        note->prio = atoi(xmlGetProp(xnode, "prio"));
        note->deadline = isodate2time(xmlGetProp(xnode, "deadline"));
    }

    exp = atoi(xmlGetProp(xnode, "exp"));
    note->text = g_strdup(xmlGetProp(xnode, "text"));

    rec_complete = get_preferences()->recursive_completion;
    get_preferences()->recursive_completion = 0;

    n_node = do_add_note(note, cnode);

    get_preferences()->recursive_completion = rec_complete;
                
    /*
     * notes are expanded by default
     */
    
    if (exp == 0)
    {
        gtk_ctree_collapse(GTK_CTREE(sp->note_tree), n_node);
    }
    
    if (xnode->childs)
    {
        load_notelist(n_node, xnode->childs);
    }
    if (xnode->next)
    {
        load_notelist(cnode, xnode->next);
    }
}

/* ------------------------------------------------------ */
/*
 * load filename from the command-line
 */

gint
load_notes_from_cli(gchar *file, GtkCTree *tree, GtkCTreeNode *parent)
{
    gchar *txt;
    gint  succ;

    succ = 0;
    if (filename)
    {
        g_free(filename);
    }
    filename = g_strdup(file);
    if (note_load(filename, tree, parent))
    {
        txt = g_strdup_printf(_("Loaded: %s"), filename);
        set_status_text(txt);
    }
    else
    {
        txt = g_strdup_printf(_("Error while loading: %s"), filename);
        gnome_warning_dialog_parented(txt, yank_root_win(NULL));
        succ = 1;
    }
    
    g_free(txt);
    sp->modified = FALSE;

    return (succ);
}

/* ------------------------------------------------------ */
/*
 * show the fileselection to get a filename
 */

void
cb_file_save_dialog(GtkWidget *widget, gpointer p)
{
    static GtkWidget *fs = NULL;
    GtkWidget *save_tree;
    GSList    *save_op;
    GtkWidget *hbox;
    GtkWidget *frame;
    

    if (GTK_CLIST(sp->note_tree)->rows < 1)
    {
        gnome_warning_dialog_parented(_("Nothing to save!"),
                                      yank_root_win(NULL));
        return;
    }

    ask_commit_changes();
    
    if (fs == NULL)
    {
        fs = gtk_file_selection_new(_("Save notes"));

        frame = gtk_frame_new(_("Options"));
        gtk_widget_show(frame);
        gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(fs)->main_vbox),
                           frame, FALSE, FALSE, 0);
        hbox = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(hbox);
        gtk_container_add(GTK_CONTAINER(frame), hbox);
        save_all = gtk_radio_button_new_with_label(
            NULL, _("Save all notes"));
        gtk_widget_show(save_all);
        gtk_box_pack_start(GTK_BOX(hbox), save_all, FALSE, FALSE, 3);
        save_op = gtk_radio_button_group(GTK_RADIO_BUTTON(save_all));
        save_tree = gtk_radio_button_new_with_label(
            save_op, _("Save selected subtree(s)"));
        gtk_widget_show(save_tree);
        gtk_box_pack_end(GTK_BOX(hbox), save_tree, FALSE, FALSE, 3);
        save_op = gtk_radio_button_group(GTK_RADIO_BUTTON(save_tree));
        
        gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                           "clicked", GTK_SIGNAL_FUNC(cb_save_file_ok), fs);
        gtk_signal_connect_object(
            GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button), "clicked",
            GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(fs));
        gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                           GTK_SIGNAL_FUNC(gtk_widget_destroyed), &fs);
        gtk_widget_show(fs);
    }
    else
    {
        g_assert(GTK_WIDGET_REALIZED(fs));
        gdk_window_show(fs->window);
        gdk_window_raise(fs->window);
    }

    if (filename)
    {
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), filename);
    }
}

/* ------------------------------------------------------ */
/*
 * got the filename - start saving it
 */

void
cb_save_file_ok(GtkWidget *w, GtkFileSelection *fs)
{
    gchar *name;

    name = gtk_file_selection_get_filename(fs);
    note_save(name, GTK_CTREE(sp->note_tree));
    
    if (save_all == NULL ||
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(save_all)))
    {
        if (filename)
        {
            g_free(filename);
        }
        filename = g_strdup(name);
    }
    gtk_widget_destroy(GTK_WIDGET(fs));
    save_all = NULL;
}


/* ------------------------------------------------------ */
/*
 * collect notes for saving
 */

void
build_save_file(GtkCTree *ntree, GtkCTreeNode *node, gpointer p)
{
    static gchar buff[20];
    static gint  save_all = FALSE;
    gint         prev_save_all;
    note_data    *note;
    xmlNodePtr   tree;
    gchar        *cptr;
    
    if (node)
    {
        note = (note_data *) gtk_ctree_node_get_row_data(GTK_CTREE(ntree),
                                                         GTK_CTREE_NODE(node));

        tree = xmlNewChild((xmlNodePtr) p,  NULL, "Node", NULL);
        xmlSetProp(tree, "Type", note_types[note->notetype]);
        if (note->expire != -1)
	{
	    xmlSetProp(tree, "Expire", time2isodate(&(note->expire)));
	}
        if (note->created != -1)
	{
	    xmlSetProp(tree, "Created", time2isodate(&(note->created)));
	}
        if (note->changed != -1)
	{
	    xmlSetProp(tree, "Changed", time2isodate(&(note->changed)));
	}
        if (note->changes != -1)
	{
            cptr = g_strdup_printf("%ld", note->changes);
	    xmlSetProp(tree, "Changes", cptr);
            g_free(cptr);
	}
        if (note->notetype == CheckNote || note->notetype == TodoNote)
        {
            sprintf(buff, "%d", note->todo);
            xmlSetProp(tree, "Todo", buff);
        }
        if (note->notetype == TodoNote)
        {
            cptr = g_strdup_printf("%d", note->prio);
            xmlSetProp(tree, "Priority", cptr);
            g_free(cptr);
            xmlSetProp(tree, "Deadline", time2isodate(&(note->deadline)));
            cptr = g_strdup_printf("%d", note->complete);
            xmlSetProp(tree, "Complete", cptr);
            g_free(cptr);
        }

        /*
         * 0 == collapsed, 1 == expanded
         */
        
        sprintf(buff, "%d", GTK_CTREE_ROW(node)->expanded);
        xmlSetProp(tree, "Expanded", buff);

        xmlNewTextChild(tree,  NULL, "ID", note->id);
        xmlNewTextChild(tree,  NULL, "Title", note->title);
        xmlNewTextChild(tree,  NULL, "Text", note->text);

        if (GTK_CTREE_ROW(node)->children)
        {
            /*
             * dirty trick to ensure that the siblings of the first
             * child are not touched when saving subtrees
             */
            
            prev_save_all = save_all;
            save_all = TRUE;
            build_save_file(ntree, GTK_CTREE_ROW(node)->children, tree);
            save_all = prev_save_all;
        }
        
        if (GTK_CTREE_ROW(node)->sibling && save_all)
        {
            build_save_file(ntree, GTK_CTREE_ROW(node)->sibling, p);
        }
        
    }
    else
    {
        /* init the list */
        save_all = TRUE;
        build_save_file(ntree, GTK_CTREE_NODE(GTK_CLIST(ntree)->row_list), p);
        save_all = FALSE;
    }    
}

/* ------------------------------------------------------ */
/*
 * start saving the note-tree
 */

void
note_save(gchar *filename, GtkCTree *tree)
{
    xmlDocPtr        doc;
    gchar            *txt;
    yank_preferences *pref;
    gchar            *titlebuf;
    GList            *selection;
    gchar            *backup_file;

    titlebuf = NULL;
    doc = xmlNewDoc("1.0");
    doc->root = xmlNewDocNode(doc, NULL, PACKAGE, NULL);

		if (sp->modified == FALSE) {
			return;
		}
    
    if (save_all == NULL || !GTK_IS_TOGGLE_BUTTON(save_all) || 
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(save_all)))
    {
        if (GTK_CLIST(tree)->rows > 0)
        {
            build_save_file(GTK_CTREE(tree), NULL, doc->root);
        }
    }
    else
    {
        selection = GTK_CLIST(sp->note_tree)->selection;
        while (selection && GTK_CLIST(tree)->rows > 0)
        {
            if (selection->data)
            {
                build_save_file(GTK_CTREE(tree),
                                GTK_CTREE_NODE(selection->data), doc->root);
            }
            else
            {
                g_warning(_("Selection without data while saving!"));
            }
            selection = selection->next;
        }
    }
    
    
    pref = get_preferences();

    /*
     * backup file
     */
    
/*     if (pref->use_backup_file) */
/*     { */
/*         backup_file = g_strdup_printf("%s~", filename); */
/*         if (backup_file) */
/*         { */
/*             rename(filename, backup_file); */
/*             g_free(backup_file); */
/*         } */
/*         else */
/*         { */
/*             g_warning(_("Can't malloc (fileio.c)")); */
/*         } */
/*     } */
    
    /*
     * file compression
     */
    
    xmlSetDocCompressMode(doc, pref->save_compression);
    
    if (xmlSaveFile(filename, doc) != -1)
    {
/*         txt = g_strdup_printf(_("Saved: %s"), filename); */
/*         set_status_text(txt); */
/*         g_free(txt); */
        sp->modified = FALSE;
        if (quit_after_save == TRUE)
        {
            exit(0);     /* hmm ... */
/*             gtk_main_quit(); */
        }
/*         titlebuf = g_strdup_printf("%s (%s)", PACKAGE, filename); */
/*         gtk_window_set_title(yank_root_win(NULL), titlebuf); */
/*         g_free(titlebuf); */
    }
    else
    {
        txt = g_strdup_printf(_("Error while saving: %s"), filename);
        gnome_warning_dialog_parented(txt, yank_root_win(NULL));
    }
    xmlFreeDoc(doc);
}

/* ------------------------------------------------------ */
/*
 * save notes with last name (or get a new one)
 */

void
cb_save_file(GtkWidget *widget, gpointer p)
{
    if (GTK_CLIST(sp->note_tree)->rows < 1)
    {
        gnome_warning_dialog_parented(_("Nothing to save!"),
                                      yank_root_win(NULL));
        return; 
    }
    if (p == NULL)
    {
        quit_after_save = TRUE;
    }
    else
    {
        quit_after_save = FALSE;
    }
    
    filename ? note_save(filename, GTK_CTREE(sp->note_tree))
        : cb_file_save_dialog(NULL, NULL);
}

/* ------------------------------------------------------ */
/*
 * close the current note-file
 */

void
cb_close(GtkWidget *w, gpointer p)
{
/*     static GtkWidget *dialog = NULL; */
/*     gint             sel; */
/*  */
/*     ask_commit_changes(); */
/*  */
/*     if (sp->modified) */
/*     { */
/*         if (dialog == NULL) */
/*         { */
/*             dialog = gnome_question_dialog_modal_parented( */
/*                 _("Notes sp->modified. Do you want to save them?"), NULL, NULL, */
/*                 yank_root_win(NULL)); */
/*             gnome_dialog_close_hides(GNOME_DIALOG(dialog), TRUE); */
/*             gnome_dialog_set_default(GNOME_DIALOG(dialog), 0); */
/*         } */
/*         else */
/*         { */
/*             g_assert(GTK_WIDGET_REALIZED(dialog)); */
/*             gdk_window_show(dialog->window); */
/*             gdk_window_raise(dialog->window); */
/*         } */
/*         sel = gnome_dialog_run_and_close(GNOME_DIALOG(dialog)); */
/*         if (sel == 0) */
/*         { */
/*             cb_save_file(NULL, &cb_close); */
/*         } */
/*     } */

     edit_tree_node_clear(sp->edit_tree, NULL); 
    clear_tree_from_node(GTK_CTREE(sp->note_tree), NULL);
    sp->modified = FALSE;
/*     show_todo_tab(); */

/*     if (filename) */
/*     { */
        /* Add filename to recent files list */
/*         yank_recent_files_update((GtkWidget*) p, filename); */
/*         g_free(filename); */
/*         filename = NULL; */

        /* remove filename from window-title */
/*         gtk_window_set_title(yank_root_win(NULL), PACKAGE); */
/*     } */
}

/* ------------------------------------------------------ */
/*
 * append the data from file to the textwidget
 */

void
txt_append_file(GtkText *txt, gchar *fname)
{
    size_t buffz = 1024;
    size_t r_bytes;
    gchar  *buff;
    int    fp;

    buff = (gchar *) malloc(buffz);
    if (buff == NULL)
    {
        gnome_warning_dialog_parented(_("Txt-DND: Can't malloc!"),
                                      yank_root_win(NULL));
        return;
    }
    
    fp = open(fname, O_RDONLY);
    if (fp != -1)
    {
        gtk_text_freeze(txt);
        while ((r_bytes = read(fp, buff, buffz)))
        {
            gtk_text_set_point(txt, gtk_text_get_length(txt));
            gtk_text_insert(txt, NULL, NULL, NULL, buff, r_bytes);
        }
        close(fp);
        gtk_text_thaw(txt);
    }
    else
    {
        perror(fname);
    }
    
    free(buff);
}

/* ------------------------------------------------------ */
/*
 * load a filename into a notedata
 */

note_data*
make_note_from_file(gchar *fname)
{
    note_data *note;
    int       fp;
    gchar     *buff;
    size_t    buffz = 1024;
    size_t    alsize;
    size_t    olsize;
    size_t    r_bytes;
    gchar     *pcr;
    gint      tlen;
    
    buff = (gchar *) malloc(buffz);
    if (buff == NULL)
    {
        gnome_warning_dialog_parented(_("Create-Note-DND: Can't malloc!"),
                                      yank_root_win(NULL));
        return ((note_data *) NULL);
    }
    
    note = (note_data *) malloc(sizeof (note_data));
    if (note)
    {
        note->notetype = TodoNote;
        note->id = create_note_id();
        note->deadline = 0;
	note->expire = -1;
	note->created = time(NULL);
	note->changed = note->created;
        note->changes = 0;
        note->prio = 0;
        note->todo = 0;
        note->title = note->text = NULL;
        note->complete = 0;
        
        fp = open(fname, O_RDONLY);
        if (fp != -1)
        {
            /*
             * read the text
             */
            
            alsize = olsize = 0;
            while ((r_bytes = read(fp, buff, buffz)))
            {
                olsize = alsize;
                alsize += r_bytes;
                note->text = (gchar *) realloc(note->text, alsize);
                memcpy(note->text + olsize, buff, r_bytes);
            }
            close(fp);

            /*
             * create the headline
             */
            
            pcr = strchr(note->text, '\n');
            tlen = pcr - note->text;
            note->title = (gchar *) malloc(tlen + 1);
            strncpy(note->title, note->text, tlen);
            note->title[tlen] = '\0';
        }
        else
        {
            perror(fname);
        }
    }
    else
    {
        g_warning(_("Can't malloc!"));
    }
    free(buff);
    
    return (note);
}

/* ------------------------------------------------------ */
/*
 * return filename to other modules
 */

inline const gchar*
get_filename(void)
{
    return (filename);
}

/* ------------------------------------------------------ */
/*
 * if we're here someone sent us a SIGTERM - try to save the current
 * set of notes and terminate
 */

/* void */
/* handle_sigterm(int signum) */
/* { */
/*     yank_preferences *prefs; */
/*     gchar            *fname; */
    
    /* any notes to save? */
/*     if (GTK_CLIST(sp->note_tree)->rows < 1) */
/*     { */
/*         exit(0); */
/*     } */
/*      */
/*     prefs = get_preferences(); */
 /*    save_all = NULL;*/             /* make sure all notes are saved */ 
/*     if (prefs->yank_crash_file != NULL) */
/*     { */
/*         fname = g_strdup(prefs->yank_crash_file); */
/*     } */
/*     else */
/*     { */
/*         fname = g_strdup_printf("%s/%s", getenv("HOME"), */
/*                                                 default_yank_crash_file); */
/*     } */
/*     note_save(fname, GTK_CTREE(sp->note_tree)); */
/*     g_free(fname); */
/*      */
/*     exit(0); */
/* } */

/* ------------------------------------------------------ */
/*
 * the **-word feature
 */

gint
autosave_notes(gpointer p)
{
    yank_preferences *prefs;
    gchar            *buf;
    time_t           now;
    const struct tm   *local;
    static guint      handler = 0;
    
    prefs = get_preferences();
    if (filename != NULL && sp->modified == TRUE && prefs->auto_save_minutes > 0
        && handler > 0)
    {
        save_all = NULL;             /* make sure all notes are saved */
        note_save(filename, GTK_CTREE(sp->note_tree));
        now = time(NULL);
        local = localtime(&now);
        buf = g_strdup_printf(_("Autosaving %s (%02d:%02d:%02d)"), filename,
                              local->tm_hour, local->tm_min, local->tm_sec);
        set_status_text(buf);
        g_free(buf);
    }
    if (prefs->auto_save_minutes > 0)
    {
        handler = gtk_timeout_add((prefs->auto_save_minutes * 60000),
                                  autosave_notes, NULL);
    }
    else
    {
        if (handler > 0)
        {
            gtk_timeout_remove(handler);
        }
    }

    return (FALSE);
}

/* ------------------------------------------------------ */
/*
 * check for a crash file 
 */

/* gint */
/* get_crash_file(void) */
/* { */
/*     struct           stat buf; */
/*     gint             got_crash; */
/*     yank_preferences *prefs; */
/*     GtkWidget        *dialog; */
/*     gint             reply; */
/*     gint             res; */
/*     gchar            *fname; */
/*          */
/*     got_crash = FALSE; */
/*     prefs = get_preferences(); */
/*     if (prefs->yank_crash_file != NULL) */
/*     { */
/*         fname = g_strdup(prefs->yank_crash_file); */
/*     } */
/*     else */
/*     { */
/*         fname = g_strdup_printf("%s/%s", getenv("HOME"), */
/*                                                 default_yank_crash_file); */
/*     } */
/*      */
/*     if (stat(fname, &buf) == 0) */
/*     { */
/*  */
/*         dialog = gnome_ok_cancel_dialog_modal_parented( */
/*             "A crashfile has been found. Do you want to read it?", NULL, */
/*             NULL, yank_root_win(NULL)); */
/*         gnome_dialog_set_default(GNOME_DIALOG(dialog), 0); */
/*         reply = gnome_dialog_run_and_close(GNOME_DIALOG(dialog)); */
/*         if (reply == 0) */
/*         { */
/*             got_crash = TRUE; */
/*             res = load_notes_from_cli(fname, GTK_CTREE(sp->note_tree), NULL); */
/*             if (res == 0) */
/*             { */
/*                 remove(fname); */
/*                 sp->modified = TRUE; */
/*             } */
/*             if (filename != NULL) */
/*             { */
/*                 g_free(filename); */
/*                 filename = NULL; */
/*             } */
/*         } */
/*     } */
/*     else */
/*     { */
/*         got_crash = FALSE; */
/*     } */
/*     g_free(fname); */
/*          */
/*     return (got_crash); */
/* } */

/* ------------------------------------------------------ */
/*
 * default file avaliable?
 */

/* void */
/* get_def_file(void) */
/* { */
/*     yank_preferences *prefs; */
/*     struct           stat buf; */
/*     gchar            *fname; */
/*      */
/*     prefs = get_preferences(); */
/*     if (prefs->yank_def_file != NULL) */
/*     { */
/*         fname = g_strdup(prefs->yank_def_file); */
/*     } */
/*     else */
/*     { */
/*         fname = g_strdup_printf("%s/%s", getenv("HOME"), */
/*                                                 default_yank_def_file); */
/*     } */
/*     if (stat(fname, &buf) == 0) */
/*     { */
/*         load_notes_from_cli(fname, GTK_CTREE(sp->note_tree), NULL); */
/*     }     */
/*     g_free(fname); */
/* } */

/* ------------------------------------------------------ */
/* 
 * Command callback called on activation of a recent files menu item.
 */

void
cb_recent_files(GtkWidget * widget, GtkWidget * app)
{
    gchar *txt;
    gchar *name;
    gint stat;

    name = gtk_object_get_data(GTK_OBJECT(widget), UGLY_GNOME_UI_KEY);

    cb_close(NULL, app);
    stat = load_file(name);
    if (stat)
	txt = g_strdup_printf("Loaded: %s", filename);
    else
	txt = g_strdup_printf("Error while loading: %s", filename);
    set_status_text(txt);
    g_free(txt);
}

/* ------------------------------------------------------ */
