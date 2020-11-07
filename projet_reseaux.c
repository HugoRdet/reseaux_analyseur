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
	
	
	
	
	gtk_init(&argc,&argv);
	
	//creation de la fenetre
	GtkWidget *fenetre=init_fenetre(400,600,"analyseur trame");
	

	//signal fermeture de la fenetre
	g_signal_connect(G_OBJECT(fenetre), "destroy", G_CALLBACK(fermeture_fenetre), NULL);
	
	
	//ouverture de la fenetre a l ecran
	gtk_widget_show(fenetre);
	
	//demarrage de la boucle evenementielle
	gtk_main();
	
	return EXIT_SUCCESS;
	
}