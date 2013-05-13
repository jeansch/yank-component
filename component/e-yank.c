/*
 * e-yank.c: EYank object.
 *
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Authors: Iain Holmes  <iain@ximian.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <libgnome/gnome-defs.h>
#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <libgnomevfs/gnome-vfs.h>

#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtml-stream.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlselection.h>

#include <gal/util/e-util.h>
#include <gal/widgets/e-gui-utils.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include <gal/e-table/e-tree.h>
#include <gal/e-table/e-tree-scrolled.h>
#include <gal/e-table/e-tree-memory.h>
#include <gal/e-table/e-tree-memory-callbacks.h>
#include "gal/e-table/e-table-simple.h"
#include "gal/e-table/e-table.h"

#include <bonobo/bonobo-listener.h>
#include <libgnome/gnome-paper.h>
#include <libgnome/gnome-url.h>

#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include <glade/glade.h>

#include <time.h>
#include <Evolution.h>

#include "e-yank.h"

#include "notedefs.h"

#define PARENT_TYPE (gtk_vbox_get_type ())

extern char *evolution_dir;

static GtkObjectClass *e_yank_parent_class;



static void
destroy (GtkObject *object)
{
	EYank *yank;
	EYankPrivate *priv;

	yank = E_YANK (object);
	priv = yank->priv;

	if (priv == NULL) {
		return;
	}

	g_free (priv);
	yank->priv = NULL;

	e_yank_parent_class->destroy (object);
}

static char *
e_pixmap_file (const char *filename)
{
	char *ret;
	char *edir;

	if (g_file_exists (filename)) {
		ret = g_strdup (filename);

		return ret;
	}

	/* Try the evolution images dir */
	edir = g_concat_dir_and_file (EVOLUTION_DATADIR "/images/evolution",
				      filename);

	if (g_file_exists (edir)) {
		ret = g_strdup (edir);
		g_free (edir);

		return ret;
	}
	g_free (edir);

	/* Try the evolution button images dir */
	edir = g_concat_dir_and_file (EVOLUTION_DATADIR "/images/evolution/buttons",
				      filename);

	if (g_file_exists (edir)) {
		ret = g_strdup (edir);
		g_free (edir);
		
		return ret;
	}
	g_free (edir);

	/* Fall back to the gnome_pixmap_file */
	return gnome_pixmap_file (filename);
}

static void
e_yank_evolution_protocol_listener (EYank *yank,
				       const char *uri,
				       void *closure)
{
	e_yank_change_current_view (yank, uri);
}

static void
e_yank_class_init (GtkObjectClass *object_class)
{
	object_class->destroy = destroy;

	e_yank_parent_class = gtk_type_class (PARENT_TYPE);
}

static void
e_yank_init (EYank *yank)
{
  EYankPrivate *priv;

	GtkWidget *hpaned;
	GtkWidget *scrolled;
	ETreeModel *e_tree_model = NULL;
	GtkWidget *e_tree;
 	ETreeMemory *etmm;
  ETreePath *root_node;
 	GtkWidget *e_table; 
	ETableModel *e_table_model = NULL; 

  yank->priv = g_new (EYankPrivate, 1);

  priv = yank->priv;
	sp = &priv->sp;

	priv->hpaned = e_hpaned_new();

 	gtk_box_pack_start (GTK_BOX (yank), priv->hpaned, 
 			    TRUE, TRUE, 0); 


 	yank_app_new(yank);  
	gtk_widget_show_all(priv->hpaned);
}

E_MAKE_TYPE (e_yank, "EYank", EYank, e_yank_class_init,
	     e_yank_init, PARENT_TYPE);

GtkWidget *
e_yank_new (const GNOME_Evolution_Shell shell)
{
	EYank *yank;

	yank = gtk_type_new (e_yank_get_type ());
	printf("e_yank_new yank : %x\n yank->priv %x\n", yank, yank->priv);
	yank->priv->shell = shell;

	e_yank_add_protocol_listener (yank, "evolution", e_yank_evolution_protocol_listener, yank);

	return GTK_WIDGET (yank);
}

void
e_yank_add_protocol_listener (EYank *yank,
				 const char *protocol,
				 EYankProtocolListener listener,
				 void *closure)
{
	ProtocolListener *old;

	g_return_if_fail (yank != NULL);
	g_return_if_fail (IS_E_YANK (yank));
	g_return_if_fail (protocol != NULL);
	g_return_if_fail (listener != NULL);

	if (yank->priv->protocol_hash == NULL) {
		g_print ("Creating\n");
		yank->priv->protocol_hash = g_hash_table_new (g_str_hash,
								 g_str_equal);
		old = NULL;
	}
/* 	} else { */
/* 		old = g_hash_table_lookup (yank->priv->protocol_hash, protocol); */
/* 	} */
	if (old != NULL) {
		return;
	}

	old = g_new (ProtocolListener, 1);
	old->listener = listener;
	old->closure = closure;

	g_hash_table_insert (yank->priv->protocol_hash, g_strdup (protocol), old);
}

void
e_yank_change_current_view (EYank *yank,
			       const char *uri)
{
	GNOME_Evolution_ShellView svi;
	CORBA_Environment ev;

	g_return_if_fail (yank != NULL);
	g_return_if_fail (IS_E_YANK (yank));

	svi = yank->shell_view_interface;
	if (svi == NULL) {
		return;
	}

	CORBA_exception_init (&ev);
	GNOME_Evolution_ShellView_changeCurrentView (svi, uri, &ev);
	CORBA_exception_free (&ev);
}

void
e_yank_set_message (EYank *yank,
		       const char *message,
		       gboolean busy)
{
	GNOME_Evolution_ShellView svi;
	CORBA_Environment ev;

	g_return_if_fail (yank != NULL);
	g_return_if_fail (IS_E_YANK (yank));

	svi = yank->shell_view_interface;
	if (svi == NULL) {
		return;
	}

	CORBA_exception_init (&ev);
	GNOME_Evolution_ShellView_setMessage (svi, message ? message : "", busy, &ev);
	CORBA_exception_free (&ev);
}

void
e_yank_unset_message (EYank *yank)
{
	GNOME_Evolution_ShellView svi;
	CORBA_Environment ev;

	g_return_if_fail (yank != NULL);
	g_return_if_fail (IS_E_YANK (yank));

	svi = yank->shell_view_interface;
	if (svi == NULL) {
		return;
	}

	CORBA_exception_init (&ev);
	GNOME_Evolution_ShellView_unsetMessage (svi, &ev);
	CORBA_exception_free (&ev);
}


