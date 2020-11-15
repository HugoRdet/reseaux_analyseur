#include <stdio.h>
#include <stdlib.h>
#include "trame.h"
#include "interface.h"


int main(int    argc, char **argv) {
	
	
	
	

	gtk_init(&argc,&argv);
	
	
	GtkWidget* grille=NULL;
	//creation de la fenetre
	GtkWidget *fenetre=init_fenetre(400,600,"analyseur trame",&grille);
	
	box *pvbox=(box *) malloc(sizeof(box));
	cell *liste=NULL;
	GtkWidget *pvbox_haut=NULL;
	GtkWidget *pvbox_bas=NULL;
	pvbox->taille_liste=1;
	pvbox->liste=&liste;
	pvbox->pvbox_haut=&pvbox_haut;
	pvbox->pvbox_bas=&pvbox_bas;
	
	init_panneau(grille,(pvbox->pvbox_haut),(pvbox->pvbox_bas));
	GtkWidget* menu=init_menu(grille,pvbox);
	gtk_widget_show_all(fenetre);
	//signal fermeture de la fenetre
	g_signal_connect(G_OBJECT(fenetre), "destroy", G_CALLBACK(fermeture_fenetre), liste);
	
	//ouverture de la fenetre a l ecran
	gtk_widget_show_all(fenetre);
	
	//demarrage de la boucle evenementielle
	gtk_main();
	
	return EXIT_SUCCESS;
	
}