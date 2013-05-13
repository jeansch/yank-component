/*
 * e-yank-factory.c: Executive Summary Factory.
 *
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Authors:  Iain Holmes <iain@ximian.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <bonobo/bonobo-control.h>
#include <bonobo/bonobo-exception.h>
#include <bonobo/bonobo-ui-util.h>

#include "widgets/e-gui-utils.h"

#include "e-yank.h"
 #include "notedefs.h" 
#include "e-yank-factory.h"
#include "evolution-shell-component-utils.h" /* For E_PIXMAP */

#include "callbacks.h"
#include "fileio.h"
#include "print.h"

void cb_page_setup(GtkWidget *, gpointer);
void cb_print_notes(GtkWidget *, gpointer);

void verb_new_text(void) {
	prepare_add(TextNote);
}
void verb_new_check(void) {
	prepare_add(CheckNote);
}
void verb_new_todo(void) {
	prepare_add(TodoNote);	
}

BonoboUIVerb verbs[] = {
 	BONOBO_UI_VERB ("v_addtext", verb_new_text), 
 	BONOBO_UI_VERB ("v_addcheck", verb_new_check), 
 	BONOBO_UI_VERB ("v_addtodo", verb_new_todo), 

	BONOBO_UI_VERB ("v_cut", cb_edit_cut),
	BONOBO_UI_VERB ("v_copy", cb_edit_copy),
	BONOBO_UI_VERB ("v_paste", cb_edit_paste),

	BONOBO_UI_VERB ("v_selectall", cb_edit_selall),

	BONOBO_UI_VERB ("v_listdate", cb_sort_todo_date),
	BONOBO_UI_VERB ("v_listpriority", cb_sort_todo_prio),
	BONOBO_UI_VERB ("v_listcompletion", cb_sort_todo_complete),
	BONOBO_UI_VERB ("v_listtitle", cb_sort_todo_title),
	BONOBO_UI_VERB ("v_listdateprio", cb_sort_todo_date_then_prio),
	BONOBO_UI_VERB ("v_listpriodate", cb_sort_todo_prio_then_date),

	BONOBO_UI_VERB ("v_sorttree", cb_sort_subtree),

	BONOBO_UI_VERB ("v_intotext", cb_modify_text),
	BONOBO_UI_VERB ("v_intocheck", cb_modify_check),
	BONOBO_UI_VERB ("v_intotodo",	cb_modify_todo),

	BONOBO_UI_VERB ("v_openbrowser", cb_open_in_browser),
	BONOBO_UI_VERB ("v_viewmime", cb_view_as_mime),
	BONOBO_UI_VERB ("v_runcmd", cb_run_command),
	BONOBO_UI_VERB ("v_runpipe", cb_run_pipe),

	BONOBO_UI_VERB ("v_delete", cb_delete_dialog),
	BONOBO_UI_VERB ("v_find", cb_search_dialog),

	BONOBO_UI_VERB ("v_prefs", cb_pref_dialog),
/* 	BONOBO_UI_VERB ("v_print", cb_print_notes), */
/* 	BONOBO_UI_VERB ("v_setpage", cb_page_setup), */

	BONOBO_UI_VERB_END
};


static EPixmap pixmaps [] = {
	E_PIXMAP ("/Toolbar/tb_textnote", "text.png"),
	E_PIXMAP ("/Toolbar/tb_checknote", "circle.png"),
	E_PIXMAP ("/Toolbar/tb_todonote", "box2.png"),
	E_PIXMAP ("/Toolbar/tb_cut", "buttons/cut.png"),
	E_PIXMAP ("/Toolbar/tb_copy", "buttons/copy.png"),
	E_PIXMAP ("/Toolbar/tb_paste", "buttons/paste.png"),
	E_PIXMAP ("/Toolbar/tb_delete", "buttons/delete-message.png"),
	E_PIXMAP ("/Toolbar/tb_find", "find_message.xpm"),
	E_PIXMAP ("/Toolbar/tb_print", "buttons/print.png"),
	E_PIXMAP ("/Toolbar/tb_printpv", "buttons/print-preview-24.png"),
	E_PIXMAP_END
};

static void
control_activate (BonoboControl *control,
		  BonoboUIComponent *ui_component,
		  EYank *yank)
{
	Bonobo_UIContainer container;
	gchar *filename;

	container = bonobo_control_get_remote_ui_container (control);
	bonobo_ui_component_set_container (ui_component, container);
	bonobo_object_release_unref (container, NULL);

	bonobo_ui_component_add_verb_list_with_data (ui_component, verbs, yank);
	bonobo_ui_component_freeze (ui_component, NULL);

	bonobo_ui_util_set_ui (ui_component, EVOLUTION_DATADIR,
			       "evolution-yank.xml", "evolution-yank");
   	e_pixmaps_update (ui_component, pixmaps);  

	bonobo_ui_component_thaw (ui_component, NULL);
  filename = g_concat_dir_and_file (yank->priv->uri + 7, YANK_FILENAME);
	sp = &(yank->priv->sp);
	printf("control_activate : sp %x\n", sp);
	load_file(filename);
	g_free(filename);

}

static void
control_deactivate (BonoboControl *control,
		    BonoboUIComponent *ui_component,
		    EYank *yank)
{
	gchar *filename;

  filename = g_concat_dir_and_file (yank->priv->uri + 7, YANK_FILENAME);
   	note_save(filename, GTK_CTREE(yank->priv->sp.note_tree));   
/*   edit_tree_node_clear(yank->priv->sp.edit_tree, NULL);   */
	sp = &(yank->priv->sp);
	yank->priv->sp.modified = FALSE;
	printf("control_desactivate : sp %x\n", sp);
  clear_tree_from_node(GTK_CTREE(yank->priv->sp.note_tree), NULL);
	g_free(filename);

/* 	bonobo_ui_component_rm (ui_component, "/", NULL); */
	bonobo_ui_component_unset_container (ui_component);


}

static void
control_activate_cb (BonoboControl *control,
		     gboolean activate,
		     EYank *yank)
{
	BonoboUIComponent *ui_component;
	
	ui_component = bonobo_control_get_ui_component (control);
	
	if (yank->shell_view_interface == NULL) {
		Bonobo_ControlFrame control_frame;
		CORBA_Environment ev;

		control_frame = bonobo_control_get_control_frame (control);
		if (control_frame == NULL) {
			goto out;
		}

		CORBA_exception_init (&ev);
		yank->shell_view_interface = Bonobo_Unknown_queryInterface (control_frame, "IDL:GNOME/Evolution/ShellView:1.0", &ev);

		if (BONOBO_EX (&ev)) {
			g_warning ("Error getting ShellView. %s", CORBA_exception_id (&ev));
			yank->shell_view_interface = CORBA_OBJECT_NIL;
		}
		CORBA_exception_free (&ev);
	}
 out:

	if (activate)
		control_activate (control, ui_component, yank);
	else
		control_deactivate (control, ui_component, yank);
}

static void
control_destroy_cb (BonoboControl *control,
		    EYank *yank)
{
	gtk_object_destroy (GTK_OBJECT (yank));
}

BonoboControl *
e_yank_factory_new_control (const char *uri,
			       const GNOME_Evolution_Shell shell)
{
	BonoboControl *control;
	GtkWidget *yank;
	EYank *e_yank;

	yank = e_yank_new (shell);
	if (yank == NULL) {
		return NULL;
	}


	gtk_widget_show (yank);
	
	e_yank = (EYank *)yank;
	e_yank->priv->uri = g_strdup(uri);

	control = bonobo_control_new (yank);

	if (control == NULL) {
		gtk_object_destroy (GTK_OBJECT (yank));
		return NULL;
	}

	

	gtk_signal_connect (GTK_OBJECT (control), "activate",
			    control_activate_cb, yank);
	gtk_signal_connect (GTK_OBJECT (control), "destroy",
			    control_destroy_cb, yank);

	return control;
}
