/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <config.h>
#include <stdio.h>

#include <liboaf/liboaf.h>
#include <bonobo/bonobo-context.h>
#include <bonobo/bonobo-generic-factory.h>
#include <bonobo/bonobo-main.h>

#include <importer/evolution-importer.h>
#include <importer/GNOME_Evolution_Importer.h>


#define COMPONENT_FACTORY_IID "OAFIID:GNOME_Evolution_Yank_ImporterFactory"

static BonoboGenericFactory *factory = NULL;

typedef struct {
	char *filename;
	char *folderpath;
} YankImporter;

static gboolean
check_file_is_yank (const char *filename)
{
	FILE *handle;
	char line[4096];
	gboolean result;

	handle = fopen (filename, "r");
	if (handle == NULL) {
		return FALSE;
	}
		
	fgets (line, 4096, handle); /* première ligne */
	fgets (line, 4096, handle); /* seconde ligne */
	if (line == NULL) {
		fclose (handle);
		return FALSE;
	}

	if ((strncmp (line, "<yank>", 6) == 0) || (strncmp (line, "<evolution-yank>", 6) == 0)) {
		puts("c'est un yank");
		result = TRUE;
	} else {
		puts("pas un yank");
		result = FALSE;
	}

	fclose (handle);
	return result;
}

void copy(char *pfi, char *pfo) {
	char *buf;

	FILE *fi, *fo;

	buf = (char *)malloc(4096 * sizeof(char));

	fi = fopen(pfi, "rb");
	fo = fopen(pfo, "wb");

	while(fgets(buf, sizeof(buf), fi) != 0) {
		fputs(buf, fo);
	}

	fclose(fo);
	fclose(fi);

	free(buf);

}

/* EvolutionImporter methods */
static void
process_item_fn (EvolutionImporter *importer,
		 CORBA_Object listener,
		 void *closure,
		 CORBA_Environment *ev)
{
	char *destpath;
	YankImporter *gci = (YankImporter *) closure;


	if (check_file_is_yank(gci->filename)) {
		destpath = g_strdup_printf("%s/evolution/local%s/yanktodo.xml", g_get_home_dir(), gci->folderpath);
		printf("process : %s %s\n", gci->filename, destpath);
		copy(gci->filename, destpath);
		GNOME_Evolution_ImporterListener_notifyResult (listener,
						       GNOME_Evolution_ImporterListener_OK,
						       FALSE, ev); 

		g_free(destpath);
		


	} else {
			GNOME_Evolution_ImporterListener_notifyResult (listener,
						       GNOME_Evolution_ImporterListener_UNSUPPORTED_OPERATION,
						       FALSE, ev); 

	}
	if (ev->_major != CORBA_NO_EXCEPTION) {
		g_warning ("Error notifying listeners.");
	}
	
	return;
}

static gboolean
support_format_fn (EvolutionImporter *importer,
		   const char *filename,
		   void *closure)
{
	char *ext;
	int i;
	puts("support format");
			return check_file_is_yank (filename);
}

static void
importer_destroy_cb (GtkObject *object,
		     YankImporter *gci)
{
	gtk_main_quit ();
}

static gboolean
load_file_fn (EvolutionImporter *importer,
	      const char *filename,
	      const char *folderpath,
	      void *closure)
{
	YankImporter *gci;

	gci = (YankImporter *) closure;
	gci->filename = g_strdup (filename);
	gci->folderpath = g_strdup (folderpath);

	return check_file_is_yank(filename);
}
					   
static BonoboObject *
factory_fn (BonoboGenericFactory *_factory,
	    void *closure)
{
	EvolutionImporter *importer;
	YankImporter *gci;

	gci = g_new (YankImporter, 1);
	importer = evolution_importer_new (support_format_fn, load_file_fn, 
					   process_item_fn, NULL, gci);
	
	gtk_signal_connect (GTK_OBJECT (importer), "destroy",
			    GTK_SIGNAL_FUNC (importer_destroy_cb), gci);
	
	return BONOBO_OBJECT (importer);
}

static void
importer_init (void)
{
	if (factory != NULL)
		return;

	factory = bonobo_generic_factory_new (COMPONENT_FACTORY_IID, 
					      factory_fn, NULL);

	if (factory == NULL) {
		g_error ("Unable to create factory");
	}

	bonobo_running_context_auto_exit_unref (BONOBO_OBJECT (factory));
}

int
main (int argc,
      char **argv)
{
	CORBA_ORB orb;
	
	gnome_init_with_popt_table ("Evolution-Yank-Importer",
				    PACKAGE, argc, argv, oaf_popt_options, 0,
				    NULL);
	orb = oaf_init (argc, argv);
	if (bonobo_init (orb, CORBA_OBJECT_NIL, CORBA_OBJECT_NIL) == FALSE) {
		g_error ("Could not initialize Bonobo.");
	}

	importer_init ();
	bonobo_main ();

	return 0;
}


