#include <stdio.h>
#include "trame.h"
#include "interface.h"


int main(int    argc, char **argv) {
	/*
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
	

	return 0;
	*/
	
	GtkApplication *app;
	int status;
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

		return status;
	
	
	
}