/*
 * Yank composant
 *
 * Authors: Jean Schurger <jk24@schurger.org> & Alcôve <http://www.alcove.fr>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <bonobo/bonobo-object.h>
#include <bonobo/bonobo-generic-factory.h>
#include <bonobo/bonobo-context.h>

#include  <libgnomevfs/gnome-vfs-types.h>
#include  <libgnomevfs/gnome-vfs-uri.h>
#include  <libgnomevfs/gnome-vfs-ops.h>


#include <shell/evolution-shell-component.h>
#include <shell/Evolution.h>

#include "e-yank-factory.h"
#include "e-yank.h"
#include "component-factory.h"
#include <gal/widgets/e-gui-utils.h>
#include <errno.h>

#define COMPONENT_FACTORY_IID "OAFIID:GNOME_Evolution_Yank_ShellComponentFactory"

static gint running_objects = 0;

static const EvolutionShellComponentFolderType folder_types[] = {
  { YANK_TYPE, "evolution-yank.png", "Yank", "Folder containing yank", TRUE, NULL, NULL},
 	{ NULL, NULL, NULL, NULL, FALSE, NULL, NULL } 
	
};


static char *evolution_dir = NULL;

/* EvolutionShellComponent methods and signals */


static void
create_folder (EvolutionShellComponent *shell_component,
  const char *physical_uri,
  const char *type,
  const GNOME_Evolution_ShellComponentListener listener,
  void *closure) {
  CORBA_Environment ev;

  CORBA_exception_init(&ev);
 if (!strcmp(type, YANK_TYPE) ) {
    GNOME_Evolution_ShellComponentListener_notifyResult(listener, GNOME_Evolution_ShellComponentListener_OK, &ev);
  } else {
    GNOME_Evolution_ShellComponentListener_notifyResult(listener, GNOME_Evolution_ShellComponentListener_UNSUPPORTED_TYPE, &ev);
  }

  CORBA_exception_free(&ev);
}

static void
remove_folder (EvolutionShellComponent *shell_component,
         const char *physical_uri,
         const char *type,
         const GNOME_Evolution_ShellComponentListener listener,
         void *closure) {

  CORBA_Environment ev;
  const char *file_name;
  gchar *path;
  int rv;

  CORBA_exception_init(&ev);

	printf("type : %s\n", type);


  /* check URI */
  if (strncmp (physical_uri, "file://", 7)) {
    GNOME_Evolution_ShellComponentListener_notifyResult (
      listener,
      GNOME_Evolution_ShellComponentListener_INVALID_URI,
      &ev);
    CORBA_exception_free (&ev);
    return;
  }

  /* FIXME: check if there are subfolders? */

  file_name = YANK_FILENAME;
  if (file_name == NULL) {
    GNOME_Evolution_ShellComponentListener_notifyResult (listener,
                     GNOME_Evolution_ShellComponentListener_UNSUPPORTED_TYPE,
                     &ev);
    CORBA_exception_free (&ev);
    return;
  }

  /* remove the .ics file */
  path = g_concat_dir_and_file (physical_uri + 7, file_name);
  rv = unlink (path);
  g_free (path);
  if (rv == 0) {
    /* everything OK; notify the listener */
    GNOME_Evolution_ShellComponentListener_notifyResult (
      listener,
      GNOME_Evolution_ShellComponentListener_OK,
      &ev);
  }
  else {
    if (errno == EACCES || errno == EPERM)
      GNOME_Evolution_ShellComponentListener_notifyResult (
        listener,
        GNOME_Evolution_ShellComponentListener_PERMISSION_DENIED,
        &ev);
    else
      GNOME_Evolution_ShellComponentListener_notifyResult (
        listener,
        GNOME_Evolution_ShellComponentListener_INVALID_URI, /*XXX*/
        &ev);
  }


  CORBA_exception_free(&ev);
}

 static void
xfer_folder (EvolutionShellComponent *shell_component,
       const char *source_physical_uri,
       const char *destination_physical_uri,
       const char *type,
       gboolean remove_source,
       const GNOME_Evolution_ShellComponentListener listener,
       void *closure)
{
 CORBA_Environment ev;
  gchar *source_path;
  gchar *dest_path;
  GnomeVFSHandle *handle;
  GnomeVFSURI *uri;
  GnomeVFSFileSize out;
  char *buf;
  const char *file_name;

  CORBA_exception_init (&ev);
	printf("type : %s\n", type);

  /* check URI */
  if (strncmp (source_physical_uri, "file://", 7)
      || strncmp (destination_physical_uri, "file://", 7)) {
    GNOME_Evolution_ShellComponentListener_notifyResult (
      listener,
      GNOME_Evolution_ShellComponentListener_INVALID_URI,
      &ev);   
    CORBA_exception_free (&ev);
    return; 
  }

  file_name = YANK_FILENAME; 
/*   if (file_name == NULL) {  */
/*     GNOME_Evolution_ShellComponentListener_notifyResult (listener, */
/*                      GNOME_Evolution_ShellComponentListener_UNSUPPORTED_TYPE, */
/*                      &ev); */
/*     CORBA_exception_free (&ev); */
/*     return;  */
/*   } */

  /* open source and destination files */
  source_path = g_concat_dir_and_file (source_physical_uri + 7, YANK_FILENAME);
  dest_path = g_concat_dir_and_file (destination_physical_uri + 7, YANK_FILENAME);

	if (gnome_vfs_move (source_path, dest_path, TRUE)
      != GNOME_VFS_OK) {
    GNOME_Evolution_ShellComponentListener_notifyResult (
      listener,
      GNOME_Evolution_ShellComponentListener_INVALID_URI,
      &ev);
    g_free (source_path);
    g_free (dest_path);
		puts("move bad");

    CORBA_exception_free (&ev);
    return;
  }

 g_free (source_path);
    g_free (dest_path);

	puts("move ok");


 GNOME_Evolution_ShellComponentListener_notifyResult(listener, GNOME_Evolution_ShellComponentListener_OK, &ev);

  CORBA_exception_free (&ev);
}

static GList *shells = NULL;


static EvolutionShellComponentResult
create_view (EvolutionShellComponent *shell,
	     const char *physical_uri,
	     const char *folder_type,
	     BonoboControl **control_return,
	     void *closure)
{
	EvolutionShellClient *shell_client;
	GNOME_Evolution_Shell corba_shell;
	BonoboControl *control;

	puts("create view");

	if (g_strcasecmp (folder_type, YANK_TYPE) != 0) {
		return EVOLUTION_SHELL_COMPONENT_UNSUPPORTEDTYPE;
	}

	shell_client = evolution_shell_component_get_owner (shell);
	corba_shell = bonobo_object_corba_objref (BONOBO_OBJECT (shell_client));
	control = e_yank_factory_new_control (physical_uri, corba_shell);
	if (!control)
		return EVOLUTION_SHELL_COMPONENT_NOTFOUND;

	*control_return = control;


	return EVOLUTION_SHELL_COMPONENT_OK;
}

static void
owner_set_cb (EvolutionShellComponent *shell_component,
	      EvolutionShellClient *shell_client,
	      const char *evolution_homedir,
	      gpointer user_data)
{
	if (evolution_dir != NULL) {
		evolution_dir = g_strdup (evolution_homedir);
	}
}

static void
owner_unset_cb (EvolutionShellComponent *shell_component,
		gpointer user_data)
{
	gtk_main_quit ();
}

static void
component_destroy (BonoboObject *factory,
		 gpointer user_data)
{
	running_objects--;

	if (running_objects > 0) {
		return;
	}

	gtk_main_quit ();
}

static void user_create_new_item_cb (EvolutionShellComponent *shell_component,
           const char *id,
           const char *parent_folder_physical_uri,
           const char *parent_folder_type)
 {
   /* FIXME */
 }

static BonoboObject *
factory_fn (BonoboGenericFactory *factory,
	    void *closure)
{
	EvolutionShellComponent *shell_component;

	running_objects++;

	shell_component = evolution_shell_component_new (folder_types,
							 NULL,
							 create_view,
							 create_folder,
							 remove_folder,
							 xfer_folder, NULL,
							 NULL, NULL);

/* New Yank in the File-New menu */
/*  	evolution_shell_component_add_user_creatable_item (shell_component, "Yank", _("New Yank"), _("New _Yank"), 'y');  */
/* Connect a cb to this menu item */
/* 	gtk_signal_connect (GTK_OBJECT(shell_component), "user_create_new_item", */
/* 		GTK_SIGNAL_FUNC (user_create_new_item_cb), NULL); */

	gtk_signal_connect (GTK_OBJECT (shell_component), "destroy",
			    GTK_SIGNAL_FUNC (component_destroy), NULL);
	gtk_signal_connect (GTK_OBJECT (shell_component), "owner_set",
			    GTK_SIGNAL_FUNC (owner_set_cb), NULL);
	gtk_signal_connect (GTK_OBJECT (shell_component), "owner_unset",
			    GTK_SIGNAL_FUNC (owner_unset_cb), NULL);


	return BONOBO_OBJECT (shell_component);
}

void
component_factory_init (void)
{
	BonoboGenericFactory *object;

	object = bonobo_generic_factory_new (COMPONENT_FACTORY_IID,
					     factory_fn, NULL);

	if (object == NULL) {
		e_notice (NULL, GNOME_MESSAGE_BOX_ERROR,
			  _("Cannot initialize Evolution's Yank component."));
		exit (1);
	}

	bonobo_running_context_auto_exit_unref (BONOBO_OBJECT (object));
}
