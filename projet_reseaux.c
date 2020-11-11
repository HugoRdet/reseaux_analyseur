#include <stdio.h>
#include <stdlib.h>
#include "trame.h"
#include "interface.h"


int main(int    argc, char **argv) {
	
	
	
	

	gtk_init(&argc,&argv);
	
	
	GtkWidget* grille=NULL;
	//creation de la fenetre
	GtkWidget *fenetre=init_fenetre(400,600,"analyseur trame",&grille);
	
	GtkWidget *pvbox_haut=NULL;
	GtkWidget *pvbox_bas=NULL;
	init_panneau(grille,&pvbox_haut,&pvbox_bas);
	
	
	cell *liste=NULL;
	GtkWidget* menu=init_menu(grille,&pvbox_haut);
	
	
	//assigne_ui_liste(&liste,pvbox_haut,pvbox_bas);
	FILE *fichier_source=fopen("test_res.txt","r");
	
	int res=1;
	int cpt=1;
	int ligne=1;
	int offset=1;
	
	
	while (res!=0) {
		while (offset!=0) {
			cherche_prochaine_ligne(fichier_source,&offset,&ligne);
		}
		res=charge_trame(fichier_source,&ligne,cpt,&liste,pvbox_haut, pvbox_bas);
		cpt++;
	}

	
	
	
	
	GtkWidget *pButton = gtk_button_new_with_label("quitter - test 2");
	gtk_box_pack_start(GTK_BOX(pvbox_haut), pButton, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(pButton),"clicked",G_CALLBACK(fermeture_fenetre),liste);
	
	
		
	
	
	
	
	
	
	/*
	while (tmp!=NULL) {
		tmp_bouton=gtk_button_new_with_label("ouin ouin");
		gtk_box_pack_start(GTK_BOX(pvbox),tmp_bouton, FALSE, FALSE, 0);
	
		tmp=tmp->suiv;
	}
	*/

	
	
	gtk_widget_show_all(fenetre);
	//signal fermeture de la fenetre
	g_signal_connect(G_OBJECT(fenetre), "destroy", G_CALLBACK(fermeture_fenetre), liste);
	
	//ouverture de la fenetre a l ecran
	gtk_widget_show_all(fenetre);
	
	//demarrage de la boucle evenementielle
	gtk_main();
	
	return EXIT_SUCCESS;
	
}