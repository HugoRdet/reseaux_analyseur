#include "interface.h"
#include <stdio.h>
#include <stdlib.h>


void action_bouton_ip(GtkWidget *pWidget, gpointer pData){
	cell *cellule_obj=(cell *) pData;
	
	if (cellule_obj->status_bouton_ip==0){
		cellule_obj->status_bouton_ip=1;
		gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode_status_1");
		gtk_widget_show (GTK_WIDGET(cellule_obj->bouton));
		gtk_revealer_set_reveal_child (GTK_REVEALER(cellule_obj->revealer),TRUE);
		//gtk_widget_show_all(GTK_WIDGET(cellule_obj->arbre));
	}else{
		cellule_obj->status_bouton_ip=0;
		
		if (cellule_obj->obj->nb_ligne_erreur==-1){
			gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode");
		}else {
			gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode_erreur");
		}
		gtk_revealer_set_reveal_child (GTK_REVEALER(cellule_obj->revealer),FALSE);
		gtk_widget_show(GTK_WIDGET(cellule_obj->bouton));
	}
}

void affiche_selection_fichiers(GtkWidget *pWidget, gpointer pData){
	
	box *pvbox=(box*)pData;
	GtkWidget *pvbox_haut=*(pvbox->pvbox_haut);
	GtkWidget *pvbox_bas=*(pvbox->pvbox_bas);
	
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
				FILE *fichier_source=fopen(filename,"r");
				printf("%s\n",filename);
				int verif=1;
				int res=1;
				int ligne=1;
				int offset=1;
				

				while (res!=0) {
					while ((offset!=0)&&(verif!=0)) {
						verif=cherche_prochaine_ligne(fichier_source,&offset,&ligne);
					}
					if (verif==0){
						break;
						gtk_widget_destroy (dialog);
						fclose(fichier_source);
						return;
					}
					
					res=charge_trame(fichier_source,&ligne,pvbox->taille_liste,pvbox->liste,pvbox_haut, pvbox_bas);
					
					pvbox->taille_liste++;
				}
				
				fclose(fichier_source);
				break;
			}
		default:
			break;
	}
	
	gtk_widget_destroy (dialog);
	
	
}

void fermer_selection_fichiers(GtkWidget *pWidget, gpointer pData){
	box *pvbox=(box*)pData;
	
	cell *liste=*(pvbox->liste);
	cell *a_supp=NULL;
	
	while (liste!=NULL) {
		a_supp=liste;
		liste=liste->suiv;
		gtk_widget_destroy(a_supp->bouton);
		gtk_widget_destroy(a_supp->arbre);
		free(a_supp->obj->tab);
		free(a_supp->obj->mac_dest);
		free(a_supp->obj->mac_source);
		free(a_supp->obj);
		free(a_supp);
		
	}
	pvbox->taille_liste=1;
	*(pvbox->liste)=NULL;
}

void sauvegarder_fichiers(GtkWidget *pWidget, gpointer pData){
	printf("prout alors\n");
}

GtkWidget* init_menu(GtkWidget* grille,box *pvbox){
	GtkWidget* menu=gtk_toolbar_new ();
	gtk_widget_set_name(menu,"fond_menu");
		//le menu ne vas pas s ellargir si on ellargit la fenetre
	gtk_toolbar_set_icon_size (GTK_TOOLBAR(menu),32);
	gtk_widget_set_vexpand (menu, FALSE);
	gtk_widget_set_hexpand (menu, FALSE);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (menu),GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach(GTK_GRID(grille),menu,0,0,9,1);
	
	
	
	GtkToolItem *ouvrir_fichier=gtk_tool_button_new (NULL,"ouvrir..");
	GtkToolItem *fermer_fichier=gtk_tool_button_new (NULL,"fermer");
	GtkToolItem *sauvegarder_fichier=gtk_tool_button_new (NULL,"sauvegarder");
	
	
	
	GtkWidget * ouvrir_fichier_icone=gtk_image_new_from_file ("icones/ajouter_fichier_32px.png");
	GtkWidget * fermer_fichier_icone=gtk_image_new_from_file ("icones/supprimer_fichier_1_32px.png");
	GtkWidget * sauvegarder_fichier_icone=gtk_image_new_from_file ("icones/save_fichier_32px.png");
	
	/*
	gtk_widget_set_name(ouvrir_fichier_icone,"fond_menu");
	gtk_widget_set_name(fermer_fichier_icone,"fond_menu");
	gtk_widget_set_name(sauvegarder_fichier_icone,"fond_menu");
	*/

	
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON(ouvrir_fichier),ouvrir_fichier_icone);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON(fermer_fichier),fermer_fichier_icone);
	gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON(sauvegarder_fichier),sauvegarder_fichier_icone);
	
	
	gtk_tool_item_set_homogeneous (ouvrir_fichier,TRUE);
	gtk_tool_item_set_homogeneous (fermer_fichier,TRUE);
	gtk_tool_item_set_homogeneous (sauvegarder_fichier,TRUE);
	gtk_tool_item_set_expand (ouvrir_fichier,FALSE);
	gtk_tool_item_set_expand (fermer_fichier,FALSE);
	gtk_tool_item_set_expand (sauvegarder_fichier,FALSE);
	gtk_tool_item_set_visible_vertical (ouvrir_fichier,TRUE);
	gtk_tool_item_set_visible_vertical (fermer_fichier,TRUE);
	gtk_tool_item_set_visible_vertical (sauvegarder_fichier,TRUE);
	gtk_tool_item_set_tooltip_text (ouvrir_fichier,"ouvrir un fichier");
	gtk_tool_item_set_tooltip_text (fermer_fichier,"fermer un fichier");
	gtk_tool_item_set_tooltip_text (sauvegarder_fichier,"sauvegarder un fichier");
	
	gtk_toolbar_insert (GTK_TOOLBAR(menu),sauvegarder_fichier,0);
	gtk_toolbar_insert (GTK_TOOLBAR(menu),fermer_fichier,0);
	gtk_toolbar_insert (GTK_TOOLBAR(menu),ouvrir_fichier,0);
	
	
	
	
	g_signal_connect(G_OBJECT(ouvrir_fichier),"clicked",G_CALLBACK(affiche_selection_fichiers),pvbox);
	g_signal_connect(G_OBJECT(fermer_fichier),"clicked",G_CALLBACK(fermer_selection_fichiers),pvbox);
	g_signal_connect(G_OBJECT(sauvegarder_fichier),"clicked",G_CALLBACK(sauvegarder_fichiers),pvbox);
	return menu;

}


void init_panneau(GtkWidget *grille,GtkWidget **pvbox_haut_,GtkWidget **pvbox_bas_){
	GtkWidget *panneau = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(panneau,"fond_panneau");
	gtk_widget_set_vexpand (panneau, TRUE);
	gtk_widget_set_hexpand (panneau, TRUE);
	gtk_grid_attach(GTK_GRID(grille),panneau,0,2,9,9);
	//gtk_container_add(GTK_CONTAINER(fenetre),panneau);
	GtkWidget *frame_haut = gtk_scrolled_window_new (NULL,NULL);
	GtkWidget *frame_bas = gtk_scrolled_window_new (NULL,NULL);
	
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(frame_haut),GTK_SHADOW_IN);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(frame_bas),GTK_SHADOW_IN);
	
	GtkWidget *scroll_bar_1=gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW(frame_haut));
	GtkWidget *scroll_bar_2=gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW(frame_bas));
	gtk_widget_set_name(scroll_bar_1,"scroll_bar_perso");
	gtk_widget_set_name(scroll_bar_2,"scroll_bar_perso");
	
	gtk_paned_pack1 (GTK_PANED(panneau),frame_haut,TRUE,TRUE);
	gtk_paned_pack2 (GTK_PANED(panneau),frame_bas,TRUE,TRUE);
	
	
	/* TOP BOX */	
	GtkWidget* pvbox_haut=gtk_box_new(FALSE,0);
	gtk_widget_set_name(pvbox_haut,"fond_panneau");
	gtk_orientable_set_orientation (GTK_ORIENTABLE (pvbox_haut),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(frame_haut),pvbox_haut);
	
	GtkWidget *menu_label=gtk_toggle_button_new_with_label("Id\t     IP source       \tIP destination");
	gtk_widget_set_name(menu_label,"label_menu_dark_mode");
	gtk_widget_set_vexpand (menu_label, FALSE);
	gtk_widget_set_hexpand (menu_label, FALSE);
	gtk_box_pack_start(GTK_BOX(pvbox_haut),menu_label, FALSE, FALSE, 0);
	gtk_widget_show (menu_label);


	/* BOT BOX */
	GtkWidget* pvbox_bas=gtk_box_new(FALSE,0);
	gtk_widget_set_name(pvbox_bas,"fond_panneau");
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
	gtk_widget_set_name(fenetre,"fond_dark");
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

void remplir_ethernet(GtkWidget *box_ethernet,cell *tmp_cell){
	char label[80];
	GtkWidget *tmp_label=NULL;
	
	sprintf(label,"\t\tSource\t\t:  %s\n",(tmp_cell->obj->mac_source));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tDestination\t:  %s\n",(tmp_cell->obj->mac_dest));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\ttype\t\t:  %s\n",(tmp_cell->obj->ip_type));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
}

void remplir_ip(GtkWidget *box_ip, cell *tmp_cell){
	char label[80];
	GtkWidget *tmp_label=NULL;
	
	sprintf(label,"\t\tVersion\t\t:  %s\n",(tmp_cell->obj->version));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tHeader Length : %s\n",(tmp_cell->obj->header_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tTotal Length : %s\n",(tmp_cell->obj->total_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tIdentification : %s\n",(tmp_cell->obj->identification));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);


	GtkWidget *flags=gtk_expander_new ("Flags");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(flags),FALSE);
	gtk_box_pack_start (GTK_BOX(box_ip),flags,FALSE,FALSE,0);
	GtkWidget *box_flags=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(flags),box_flags);	
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_flags),GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(GTK_WIDGET(flags),"expander-tabbed");

	sprintf(label,"\t\tValue : %s\n",(tmp_cell->obj->flags_offset));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tReserved bit : %s\n",(tmp_cell->obj->reserved_bit));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tDon't Fragments : %s\n",(tmp_cell->obj->dont_fragment));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tMore Fragments : %s\n",(tmp_cell->obj->more_fragment));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);


}

void remplir_http(){

}
	
void remplir_arbre(GtkWidget *new_box, gpointer pData){
	
	cell *tmp_cell=(cell *)pData;
	
	GtkWidget *ethernet=gtk_expander_new ("Ethernet II");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	remplir_ethernet(box_ethernet,tmp_cell);	
	
	GtkWidget *IP=gtk_expander_new ("Internet Protocol");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(IP),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),IP,FALSE,FALSE,0);
	GtkWidget *box_ip=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(IP),box_ip);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ip),GTK_ORIENTATION_VERTICAL);

	remplir_ip(box_ip, tmp_cell);	

	
	/*
	GtkTreeStore *arbre=gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter header_ethernet;
	GtkTreeIter contenu_ethernet;
	GtkTreeIter header_IP;
	GtkTreeIter contenu_IP;
	
	gtk_tree_store_insert (arbre,&header_ethernet,NULL,-1);
	gtk_tree_store_set(arbre, &header_ethernet,0,"Ethernet II",1,NULL, -1);
	
	gtk_tree_store_insert (arbre,&contenu_ethernet,&header_ethernet,-1);
	gtk_tree_store_set(arbre,&contenu_ethernet, 0, "Source:",1,tmp_cell->obj->mac_source, -1);
	gtk_tree_store_insert (arbre,&contenu_ethernet,&header_ethernet,-1);
}

void remplir_http(){

}
	
void remplir_arbre(GtkWidget *new_box, gpointer pData){
	
	cell *tmp_cell=(cell *)pData;
	
	GtkWidget *ethernet=gtk_expander_new ("Ethernet II");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	remplir_ethernet(box_ethernet,tmp_cell);	
	
	GtkWidget *IP=gtk_expander_new ("Internet Protocol");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(IP),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),IP,FALSE,FALSE,0);
	
	GtkWidget *box_ip=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ip),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(IP),box_ip);
	remplir_ip(box_ip,tmp_cell);	
	/*
	GtkTreeStore *arbre=gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter header_ethernet;
	GtkTreeIter contenu_ethernet;
	GtkTreeIter header_IP;
	GtkTreeIter contenu_IP;
	
	gtk_tree_store_insert (arbre,&header_ethernet,NULL,-1);
	gtk_tree_store_set(arbre, &header_ethernet,0,"Ethernet II",1,NULL, -1);
	
	gtk_tree_store_insert (arbre,&contenu_ethernet,&header_ethernet,-1);
	gtk_tree_store_set(arbre,&contenu_ethernet, 0, "Source:",1,tmp_cell->obj->mac_source, -1);
	gtk_tree_store_insert (arbre,&contenu_ethernet,&header_ethernet,-1);
	gtk_tree_store_set(arbre,&contenu_ethernet, 0, "Destination:",1,tmp_cell->obj->mac_dest, -1);
	gtk_tree_store_insert (arbre,&contenu_ethernet,&header_ethernet,-1);
	gtk_tree_store_set(arbre,&contenu_ethernet, 0, "type:",1,tmp_cell->obj->ip_type, -1);
	
	gtk_tree_store_insert(arbre,&header_IP,NULL,-1);
	gtk_tree_store_set(arbre,&header_IP,0,"Internet Protocol",1,"", -1);
	
	gtk_tree_store_insert(arbre,&contenu_IP,&header_IP,-1);
	gtk_tree_store_set(arbre,&contenu_IP,0,"IP Version:",1,tmp_cell->obj->version, -1);
	gtk_tree_store_insert(arbre,&contenu_IP,&header_IP,-1);
	gtk_tree_store_set(arbre,&contenu_IP,0,"IP Version:",1,tmp_cell->obj->version, -1);
	gtk_tree_store_insert(arbre,&contenu_IP,&header_IP,-1);
	gtk_tree_store_set(arbre,&contenu_IP,0,"IP Version:",1,tmp_cell->obj->version, -1);
	

	tmp_cell->arbre = gtk_tree_view_new_with_model(GTK_TREE_MODEL(arbre));
	
	GtkCellRenderer *renderer_col_0;
	GtkCellRenderer *renderer_col_1;
	GtkTreeViewColumn *column_0;
	GtkTreeViewColumn *column_1;
	
	
	char label[20];
	sprintf(label,"trame n°%d",tmp_cell->obj->id);

	renderer_col_0 = gtk_cell_renderer_text_new();
	renderer_col_1 = gtk_cell_renderer_text_new();
	
	column_0 = gtk_tree_view_column_new_with_attributes(label, renderer_col_0, "text", 0, NULL);
	column_1 = gtk_tree_view_column_new_with_attributes(NULL, renderer_col_1, "text",1, NULL);
	
	gtk_tree_view_append_column(GTK_TREE_VIEW(tmp_cell->arbre), column_0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tmp_cell->arbre), column_1);
	
	*/
	
	return;
}
	
