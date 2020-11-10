#include <stdio.h>
#include "trame.h"
#include "interface.h"


int main(int    argc, char **argv) {
	
	FILE *fichier_source=fopen("test_res.txt","r");
	FILE *fichier_dest=fopen("fichier_dstination.txt","w");
	cell *liste=NULL;
	int res=1;
	int cpt=1;
	
	
	fclose(fichier_dest);
	
	
	
	
	gtk_init(&argc,&argv);
	
	
	//creation de la fenetre
	GtkWidget *fenetre=init_fenetre(400,600,"analyseur trame");
	
	//creation de la grille
	GtkWidget* grille=gtk_grid_new();
	
	//hauteur minimale des lignes fixée à 30 pixels
	gtk_grid_set_row_spacing (GTK_GRID(grille),30);
	//largeur minimale des colonnes fixée à 50 pixels
	gtk_grid_set_column_spacing (GTK_GRID(grille),50);

	//ajout de la grille a la fenetre
	gtk_container_add(GTK_CONTAINER(fenetre), GTK_WIDGET(grille));
	//barre d icones pour le menu
	GtkWidget* menu=gtk_toolbar_new ();
	//le menu ne vas pas s ellargir si on ellargit la fenetre
	gtk_widget_set_vexpand (menu, FALSE);
	gtk_widget_set_hexpand (menu, FALSE);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (menu),GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach(GTK_GRID(grille),menu,0,0,9,1);
	
	
	GtkWidget *panneau = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_vexpand (panneau, TRUE);
	gtk_widget_set_hexpand (panneau, TRUE);
	gtk_grid_attach(GTK_GRID(grille),panneau,0,1,9,9);
	//gtk_container_add(GTK_CONTAINER(fenetre),panneau);
	GtkWidget *frame_haut = gtk_scrolled_window_new (NULL,NULL);
	GtkWidget *frame_bas = gtk_scrolled_window_new (NULL,NULL);
	
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(frame_haut),GTK_SHADOW_IN);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(frame_bas),GTK_SHADOW_IN);
	
	
	gtk_paned_pack1 (GTK_PANED(panneau),frame_haut,TRUE,TRUE);
	gtk_paned_pack2 (GTK_PANED(panneau),frame_bas,TRUE,TRUE);
	
	GtkWidget* pvbox=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (pvbox),GTK_ORIENTATION_VERTICAL);

	gtk_container_add(GTK_CONTAINER(frame_haut),pvbox);
	
	int ligne=1;
	int offset=1;
	
	
	
	while (res!=0) {
			while (offset!=0) {
					cherche_prochaine_ligne(fichier_source,&offset,&ligne);
				}
			//afficher_ligne(fichier_source);
			res=charge_trame(fichier_source,&ligne,cpt,&liste,pvbox);
			cpt++;
	}
	
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
	gtk_widget_show(fenetre);
	
	//demarrage de la boucle evenementielle
	gtk_main();
	
	return EXIT_SUCCESS;
	
}