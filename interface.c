#include "interface.h"
#include <stdio.h>



void affiche_selection_fichiers(GtkWidget *pWidget, gpointer pData){
	cell **liste=(cell **) pData;
	
	
		
	GtkWidget *fenetre= gtk_window_new(GTK_WINDOW_POPUP);
	
	//positions de la fenetre lors de l ouverture
	gtk_window_set_position(GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER );
	
	//taille de la fenetre
	gtk_window_set_default_size(GTK_WINDOW(fenetre),600, 500);
	
	
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new ("Ouvrir un fichier",
		GTK_WINDOW(fenetre),
		action,
		("_Annuler"),
		GTK_RESPONSE_CANCEL,
		("_Ouvrir"),
		GTK_RESPONSE_ACCEPT,
		NULL);
	
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	
	switch (res){
		case GTK_RESPONSE_ACCEPT :
			{
				
				char *filename;
				GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
				filename = gtk_file_chooser_get_filename (chooser);
				//FILE *fichier_source=fopen(filename,"r");
				/*printf("\n");
				for (int i=0;i<50;i++){
					printf("%c",fgetc(fichier_source));
				}
				printf("\n");
				fclose(fichier_source);
				*/
	
				
				break;
			}
		default:
			break;
	}
	
	gtk_widget_destroy (dialog);
	
}



GtkWidget* init_menu(GtkWidget* grille,cell **liste){
	GtkWidget* menu=gtk_toolbar_new ();
		//le menu ne vas pas s ellargir si on ellargit la fenetre
	gtk_toolbar_set_icon_size (GTK_TOOLBAR(menu),30);
	gtk_widget_set_vexpand (menu, FALSE);
	gtk_widget_set_hexpand (menu, FALSE);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (menu),GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach(GTK_GRID(grille),menu,0,0,9,1);
	
	
	
	
	
	
	GtkToolItem *ouvrir_fichier=gtk_tool_button_new (NULL,"ouvrir..");
	
	gtk_tool_item_set_homogeneous (ouvrir_fichier,TRUE);
	gtk_tool_item_set_expand (ouvrir_fichier,FALSE);
	gtk_tool_item_set_visible_vertical (ouvrir_fichier,TRUE);
	gtk_tool_item_set_tooltip_text (ouvrir_fichier,"ouvrir un fichier");
	
	gtk_toolbar_insert (GTK_TOOLBAR(menu),ouvrir_fichier,0);
	
	
	
	g_signal_connect(G_OBJECT(ouvrir_fichier),"clicked",G_CALLBACK(affiche_selection_fichiers),liste);
		
	return menu;

}


void init_panneau(GtkWidget *grille,GtkWidget **pvbox_haut_,GtkWidget **pvbox_bas_){
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
	
	
	/* TOP BOX */	
	GtkWidget* pvbox_haut=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (pvbox_haut),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(frame_haut),pvbox_haut);
	
	/* BOT BOX */
	GtkWidget* pvbox_bas=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (pvbox_bas),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(frame_bas),pvbox_bas);
	
	*pvbox_haut_=pvbox_haut;
	*pvbox_bas_=pvbox_bas;
	
}

//initialisation de la fenetre
GtkWidget *init_fenetre(int largeur,int hauteur,char *titre,GtkWidget **grid){
	//declaration pointeur fenetre
	GtkWidget *fenetre;
	
	
	//initialisation de la fenetre
	fenetre= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	//positions de la fenetre lors de l ouverture
	gtk_window_set_position(GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER );
	
	//taille de la fenetre
	gtk_window_set_default_size(GTK_WINDOW(fenetre), hauteur, largeur);
	
	//titre de la fenetre
	gtk_window_set_title(GTK_WINDOW(fenetre), titre);
	
	
	//creation de la grille
	GtkWidget* grille=gtk_grid_new();
		
	//hauteur minimale des lignes fixée à 30 pixels
	gtk_grid_set_row_spacing (GTK_GRID(grille),0);
	//largeur minimale des colonnes fixée à 50 pixels
	gtk_grid_set_column_spacing (GTK_GRID(grille),50);

	//ajout de la grille a la fenetre
	gtk_container_add(GTK_CONTAINER(fenetre), GTK_WIDGET(grille));
	(*grid)=grille;
	return fenetre;
}


//fermeture de la fenêtre et arret de la boucle evenementielle
void fermeture_fenetre(GtkWidget *pWidget, gpointer pData){
	//arret de la boucle evenementielle
	gtk_main_quit();
}

void affiche_trame_terminal(GtkWidget *pWidget, gpointer pData){
	trame *tmp_trame=(trame *)pData;
	printf("Mac Dest: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",(tmp_trame->mac_dest)[0],(tmp_trame->mac_dest)[1],(tmp_trame->mac_dest)[2],
										(tmp_trame->mac_dest)[3],(tmp_trame->mac_dest)[4],(tmp_trame->mac_dest)[5]);
	
	
}


void creer_model_arbre(GtkTreeStore *model, trame *trame){
	
}

void remplir_arbre(GtkWidget *pWidget, gpointer pData){
	
	cell *tmp_cell=(cell *)pData;	
	GtkTreeStore *model;
	
	enum{
		INFOS_COLUMN,
		N_COLUMNS
	};
	
	GtkTreeIter iter1;
	GtkTreeIter iter2;
	
	model = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING);
	
	gtk_tree_store_append(model, &iter1, NULL);
	gtk_tree_store_set(model, &iter1, INFOS_COLUMN, "Ethernet", -1);
	gtk_tree_store_append(model, &iter2, &iter1);
	gtk_tree_store_set(model, &iter2, INFOS_COLUMN, tmp_cell->obj->mac_source, -1);
	gtk_tree_store_append(model, &iter2, &iter1);
	gtk_tree_store_set(model, &iter2, INFOS_COLUMN, tmp_cell->obj->mac_dest, -1);
	
	tmp_cell->arbre = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Infos", renderer, "text", INFOS_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tmp_cell->arbre), column);
	gtk_widget_grab_focus(tmp_cell->arbre);
	
	return;
}

/*
void assigne_ui_liste(cell **liste,GtkWidget* box_haut, GtkWidget* box_bas){
	cell *tmp=*liste;
	
	while (tmp!=NULL) {
		trame *elem=tmp->obj;
		printf("ok\n");
		char label[80];
		sprintf(label,"%d\t%d:%d:%d:%d\t%d:%d:%d:%d",elem->id,(elem->ip_source)[0],(elem->ip_source)[1],(elem->ip_source)[2],(elem->ip_source)[3],(elem->ip_dest)[0],(elem->ip_dest)[1],(elem->ip_dest)[2],(elem->ip_dest)[3]);					
		GtkWidget* tmp_bouton=gtk_button_new_with_label(label);
		
		tmp->arbre=NULL;
		tmp->bouton=tmp_bouton;
		remplir_arbre(NULL, tmp);
		
		//ATTENTION J AI MIS END 
		gtk_box_pack_start(GTK_BOX(box_haut),tmp_bouton, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box_bas),tmp->arbre, FALSE, FALSE, 0);
		//g_signal_connect(G_OBJECT(tmp_bouton),"clicked",G_CALLBACK(remplir_arbre),new_cell);
		
		
		tmp=tmp->suiv;
		
	}
	
}
	*/