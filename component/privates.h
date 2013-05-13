#include "e-yank-type.h"
#include <gtk/gtk.h>
#include <glib.h>

#ifndef _PRIVATES_H
#define _PRIVATES_H

typedef struct {
	GtkWidget        *title_entry ;
	GtkWidget *todo_check ;
	GtkWidget *prio_entry ;
	GtkWidget *de_deadline ;
	GtkObject *ad_complete ;
	GtkWidget *tlabel_created ;
	GtkWidget *tlabel_changed ;
	GtkWidget *label_changes ;
	GtkWidget *tlabel_expire ;
	GtkWidget *note_id_entry ;
	GtkWidget        *text_entry ;
	GtkWidget        *note_tree ;      /* tree with notes on the left */
	GtkWidget        *b_ok ;
	GtkWidget        *b_apply ;
	GtkWidget        *b_cancel ;
	GtkWidget        *todolist ;       /* todolist on the right */
	GtkWidget *notebook ;
	GtkWidget *status ;         /* statusbar */
	GtkWidget *prio_date_box ;  /* hbox used for showing/ hiding */
	gint modified;
	GNode *edit_tree;
} SuperPrivate ;

#endif
