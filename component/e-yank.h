/*
 * e-yank.h: Header file for the EYank object.
 *
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Authors: Iain Holmes  <iain@ximian.com>
 */

#ifndef _E_YANK_H__
#define _E_YANK_H__

#include <gtk/gtkvbox.h>
#include <Evolution.h>
#include "e-yank-type.h"
#include "privates.h"


#define E_YANK_TYPE (e_yank_get_type ())
#define E_YANK(obj) (GTK_CHECK_CAST ((obj), E_YANK_TYPE, EYank))
#define E_YANK_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass), E_YANK_TYPE, EYankClass))
#define IS_E_YANK(obj) (GTK_CHECK_TYPE ((obj), E_YANK_TYPE))
#define IS_E_YANK_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((obj), E_YANK_TYPE))

typedef struct _EYankPrivate EYankPrivate;
typedef struct _EYankClass EYankClass;

typedef void (* EYankProtocolListener) (EYank *yank,
					   const char *uri,
					   void *closure);
struct _EYank {
	GtkVBox parent;
	EYankPrivate *priv;
	GNOME_Evolution_ShellView shell_view_interface;
};

struct _EYankClass {
	GtkVBoxClass parent_class;
};

GtkType e_yank_get_type (void);
GtkWidget *e_yank_new (const GNOME_Evolution_Shell shell);
void e_yank_print (GtkWidget *widget,
		      EYank *yank);
void e_yank_draw (EYank *yank);
void e_yank_change_current_view (EYank *yank,
				    const char *uri);
void e_yank_set_message (EYank *yank,
			    const char *message,
			    gboolean busy);
void e_yank_unset_message (EYank *yank);
void e_yank_add_protocol_listener (EYank *yank,
				      const char *protocol,
				      EYankProtocolListener listener,
				      void *closure);
void e_yank_reconfigure (EYank *yank);
int e_yank_count_connections (EYank *yank);
#endif

typedef struct _ProtocolListener {
	EYankProtocolListener listener;
	void *closure;
} ProtocolListener;

struct _EYankPrivate {
  GNOME_Evolution_Shell shell;
  GNOME_Evolution_ShellView shell_view_interface;
	GtkWidget *hpaned;
	gchar *uri;
  GHashTable *protocol_hash;
  GList *connections;
	gpointer alarm;
	SuperPrivate sp;
};

#define YANK_FILENAME "yanktodo.xml"
#define YANK_TYPE "Yank"

extern SuperPrivate *sp;
