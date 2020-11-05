#include <stdio.h>
#include "trame.h"
#include "interface.h"


int main(int    argc, char **argv) {
	
	FILE *fichier_source=fopen("test_res.txt","r");
	FILE *fichier_dest=fopen("fichier_dstination.txt","w");
	
	int res=1;
	int cpt=0;
	while (res!=0) {
	
	res=lecture_trame(fichier_source,fichier_dest,cpt);
	cpt+=res;
	
	}
	
	printf("trames scanees : %d\n",cpt);
	fclose(fichier_dest);
	
	
	
	
	
	GtkBuilder *builder;
	GObject *window;
	GObject *button;
	GError *error = NULL;

	gtk_init (&argc, &argv);

	/* Construct a GtkBuilder instance and load our UI description */
	builder = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, "builder.ui", &error) == 0)
		{
			g_printerr ("Error loading file: %s\n", error->message);
			g_clear_error (&error);
			return 1;
		}

	/* Connect signal handlers to the constructed widgets. */
	window = gtk_builder_get_object (builder, "window");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	button = gtk_builder_get_object (builder, "button1");
	g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

	button = gtk_builder_get_object (builder, "button2");
	g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

	button = gtk_builder_get_object (builder, "quit");
	g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

	gtk_main ();

	return 0;
	
	
	
}