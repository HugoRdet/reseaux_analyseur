#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void action_bouton_ip(GtkWidget *pWidget, gpointer pData){
	cell *cellule_obj=(cell *) pData;
	
	if(cellule_obj->status_bouton_ip==1){
		cellule_obj->status_bouton_ip=0;
		
		if (cellule_obj->obj->nb_ligne_erreur==-1){
			gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode");
		}else {
			gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode_erreur");
		}
		gtk_revealer_set_reveal_child (GTK_REVEALER(cellule_obj->revealer),FALSE);
		gtk_widget_show(GTK_WIDGET(cellule_obj->bouton));
		*(cellule_obj->bouton_actif)=NULL;
		return;
	}
	
	cell *tmp=*(cellule_obj->bouton_actif);
	
	if (tmp!=NULL){
		tmp->status_bouton_ip=0;
		
		if (tmp->obj->nb_ligne_erreur==-1){
			gtk_widget_set_name(GTK_WIDGET(tmp->bouton),"button_dark_mode");
		}else {
			gtk_widget_set_name(GTK_WIDGET(tmp->bouton),"button_dark_mode_erreur");
		}
		gtk_revealer_set_reveal_child (GTK_REVEALER(tmp->revealer),FALSE);
		gtk_widget_show(GTK_WIDGET(tmp->bouton));
	}
	
	*(cellule_obj->bouton_actif)=cellule_obj;
	
	
	if (cellule_obj->status_bouton_ip==0){
		cellule_obj->status_bouton_ip=1;
		gtk_widget_set_name(GTK_WIDGET(cellule_obj->bouton),"button_dark_mode_status_1");
		gtk_widget_show (GTK_WIDGET(cellule_obj->bouton));
		gtk_revealer_set_reveal_child (GTK_REVEALER(cellule_obj->revealer),TRUE);
		
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
				int taille_liste_prec=pvbox->taille_liste;

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
					
					res=charge_trame(fichier_source,&ligne,pvbox->taille_liste,pvbox->liste,pvbox_haut, pvbox_bas,filename);
					pvbox->taille_liste++;
				}
				
				if (taille_liste_prec==pvbox->taille_liste){
					
					
					GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
					GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(*(pvbox->window)),
						flags,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_NONE,
						"Aucune trame trouvée dans le fichier:\n “%s” ",
						filename);
					
					// Destroy the dialog when the user responds to it
					// (e.g. clicks a button)
					gtk_widget_set_name(dialog,"button_dark_mode_error_pop_up");
					gtk_widget_show(dialog);
					g_signal_connect_swapped (dialog, "destroy",
						G_CALLBACK (gtk_widget_destroy),
						dialog);
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
		gtk_widget_destroy(a_supp->revealer);
		
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
	box *pvbox=(box*)pData;
	GtkWidget *pvbox_haut=*(pvbox->pvbox_haut);
	GtkWidget *pvbox_bas=*(pvbox->pvbox_bas);
	
	GtkWidget *fenetre= gtk_window_new(GTK_WINDOW_POPUP);
	
	//positions de la fenetre lors de l ouverture
	gtk_window_set_position(GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER );
	
	//taille de la fenetre
	gtk_window_set_default_size(GTK_WINDOW(fenetre),600, 500);
	
	
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new ("Sauvegarder un fichier",
		GTK_WINDOW(fenetre),
		action,
		("_Annuler"),
		GTK_RESPONSE_CANCEL,
		("_Sauvegarder"),
		GTK_RESPONSE_ACCEPT,
		NULL);
	
	GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
	
	
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (res){
		case GTK_RESPONSE_ACCEPT :
			{
				char *filename;
				
				filename = gtk_file_chooser_get_filename (chooser);
				FILE *fichier=fopen(filename,"w");
				
				fprintf(fichier,"%s\n",filename);
				fclose(fichier);
				break;
			}
		default:
			break;
	}
	
	gtk_widget_destroy (dialog);
}

GtkWidget* init_menu(GtkWidget* grille,box *pvbox){
	
	GtkWidget* menu_box=gtk_box_new(FALSE,0);
	gtk_widget_set_vexpand (menu_box, FALSE);
	gtk_widget_set_hexpand (menu_box, FALSE);
	gtk_widget_set_name(menu_box,"fond_menu");
	gtk_orientable_set_orientation (GTK_ORIENTABLE (menu_box),GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach(GTK_GRID(grille),menu_box,0,0,9,1);

	
	GtkWidget *ouvrir_fichier=gtk_button_new ();
	GtkWidget *fermer_fichier=gtk_button_new ();
	GtkWidget *sauvegarder_fichier=gtk_button_new ();
	
	gtk_widget_set_name(ouvrir_fichier,"fond_menu");
	gtk_widget_set_name(fermer_fichier,"fond_menu");
	gtk_widget_set_name(sauvegarder_fichier,"fond_menu");

	
	
	GtkWidget * ouvrir_fichier_icone=gtk_image_new_from_file ("icones/ajouter_fichier_32px.png");
	GtkWidget * fermer_fichier_icone=gtk_image_new_from_file ("icones/supprimer_fichier_1_32px.png");
	GtkWidget * sauvegarder_fichier_icone=gtk_image_new_from_file ("icones/save_fichier_32px.png");
	

	
	gtk_button_set_image(GTK_BUTTON(ouvrir_fichier),ouvrir_fichier_icone);
	gtk_button_set_image(GTK_BUTTON(fermer_fichier),fermer_fichier_icone);
	gtk_button_set_image(GTK_BUTTON(sauvegarder_fichier),sauvegarder_fichier_icone);
	
	
	gtk_widget_set_name(ouvrir_fichier_icone,"fond_menu");
	gtk_widget_set_name(fermer_fichier_icone,"fond_menu");
	gtk_widget_set_name(sauvegarder_fichier_icone,"fond_menu");
	
	gtk_widget_set_tooltip_text (ouvrir_fichier,"ouvrir un fichier");
	gtk_widget_set_tooltip_text (fermer_fichier,"fermer un fichier");
	gtk_widget_set_tooltip_text (sauvegarder_fichier,"sauvegarder un fichier");
	
	
	gtk_box_pack_start (GTK_BOX(menu_box),ouvrir_fichier,FALSE,FALSE,0);
	gtk_box_pack_start (GTK_BOX(menu_box),fermer_fichier,FALSE,FALSE,0);
	gtk_box_pack_start (GTK_BOX(menu_box),sauvegarder_fichier,FALSE,FALSE,0);
	
	g_signal_connect(G_OBJECT(ouvrir_fichier),"clicked",G_CALLBACK(affiche_selection_fichiers),pvbox);
	g_signal_connect(G_OBJECT(fermer_fichier),"clicked",G_CALLBACK(fermer_selection_fichiers),pvbox);
	g_signal_connect(G_OBJECT(sauvegarder_fichier),"clicked",G_CALLBACK(sauvegarder_fichiers),pvbox);
	gtk_widget_show_all (menu_box);
	return menu_box;

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
	
	GtkWidget *menu_label=gtk_toggle_button_new_with_label("Id\t     Source       \tDestination       \tProtocol");
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
	
	sprintf(label,"\t\tSource\t:  %s\n",(tmp_cell->obj->mac_source));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tDestination:  %s\n",(tmp_cell->obj->mac_dest));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tType:  %s\n",(tmp_cell->obj->ip_type));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	
}

void remplir_ip(GtkWidget *box_ip, cell *tmp_cell){
	char label[80];
	GtkWidget *tmp_label=NULL;
	
	sprintf(label,"\t\tVersion:  %s\n",(tmp_cell->obj->version));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->header_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tTotal Length: %s\n",(tmp_cell->obj->total_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tIdentification: %s\n",(tmp_cell->obj->identification));
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

	sprintf(label,"\t\tValue: %s\n",(tmp_cell->obj->flags_offset));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t %d... .... = Reserved bit: %s\n",tmp_cell->obj->f0, (tmp_cell->obj->reserved_bit));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t .%d.. .... = Don't Fragment: %s\n",tmp_cell->obj->f1,(tmp_cell->obj->dont_fragment));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\t ..%d. .... = More Fragments: %s\n",tmp_cell->obj->f2,(tmp_cell->obj->more_fragment));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tFragment Offset: %s\n",(tmp_cell->obj->frag_offset));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
		
	sprintf(label,"\t\tTime to Live: %d\n",(tmp_cell->obj->TTL));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tProtocol: %s\n",(tmp_cell->obj->protocol));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tHeader Checksum: %s\n",(tmp_cell->obj->header_checksum));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tSource Address: %s\n",(tmp_cell->obj->ip_source));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tDestination Address: %s\n",(tmp_cell->obj->ip_dest));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
}

void remplir_tcp(GtkWidget *box_tcp, cell *tmp_cell){

	char label[80];
	GtkWidget *tmp_label=NULL;

	sprintf(label,"\t\tSource Port: %s\n",(tmp_cell->obj->source_port));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tDestination Port: %s\n",(tmp_cell->obj->destination_port));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tSequence Number (raw): %s\n",(tmp_cell->obj->sequence_number_raw));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tAcknowledgment Number (raw): %s\n",(tmp_cell->obj->acknowledgment_number_raw));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->tcp_header_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);

	GtkWidget *tcp_flags=gtk_expander_new ("Flags");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_flags),FALSE);
	gtk_box_pack_start (GTK_BOX(box_tcp),tcp_flags,FALSE,FALSE,0);
	GtkWidget *box_flags_2=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(tcp_flags),box_flags_2);	
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_flags_2),GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(GTK_WIDGET(tcp_flags),"expander-tabbed");


	sprintf(label,"\t\t..%d. .... = Urgent: %s\n",tmp_cell->obj->tcp_f0, (tmp_cell->obj->urg));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t...%d .... = Acknowledgement: %s\n",tmp_cell->obj->tcp_f1, (tmp_cell->obj->ack));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t.... %d... = Push: %s\n",tmp_cell->obj->tcp_f2, (tmp_cell->obj->push));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t.... .%d.. = Reset: %s\n",tmp_cell->obj->tcp_f3, (tmp_cell->obj->reset));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\t.... ..%d. = Syn: %s\n",tmp_cell->obj->tcp_f4, (tmp_cell->obj->syn));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\t.... ...%d = Fin: %s\n",tmp_cell->obj->tcp_f5, (tmp_cell->obj->fin));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_flags_2),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tWindow: %s\n",(tmp_cell->obj->window));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);

	sprintf(label,"\t\tChecksum: %s\n",(tmp_cell->obj->tcp_checksum));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	
	sprintf(label,"\t\tUrgent Pointer: %s\n",(tmp_cell->obj->urgent));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	
	if(tmp_cell->obj->no_option){
		sprintf(label,"\t\tNo Options: %s\n",(tmp_cell->obj->no_option));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
		return;
	}
	GtkWidget *tcp_options=gtk_expander_new (tmp_cell->obj->option_length);
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options),FALSE);
	gtk_box_pack_start (GTK_BOX(box_tcp),tcp_options,FALSE,FALSE,0);
	GtkWidget *box_options=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(tcp_options),box_options);	
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options),GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(GTK_WIDGET(tcp_options),"expander-tabbed");
	
	int i = 0;
	while(tmp_cell->obj->option_tab[i]){
		if(strcmp(tmp_cell->obj->option_tab[i], "No Operation (NOP)") == 0){
			GtkWidget *tcp_options_1 =gtk_expander_new ("TCP Option - No-Operation");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_1),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_1,FALSE,FALSE,0);
			GtkWidget *box_options_1=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_1),box_options_1);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_1),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_1),"expander-tabbed");

			sprintf(label,"\t\tKind : %s\n",(tmp_cell->obj->option_tab[i]));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_1),tmp_label, FALSE, FALSE, 0);
		}	
												
		if(strcmp(tmp_cell->obj->option_tab[i], "Maximum Segment Size (MSS)") == 0){
			GtkWidget *tcp_options_2 =gtk_expander_new ("TCP Option - Maximum segment size");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_2),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_2,FALSE,FALSE,0);
			GtkWidget *box_options_2=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_2),box_options_2);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_2),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_2),"expander-tabbed");

			sprintf(label,"\t\tKind : %s\n",(tmp_cell->obj->option_tab[i]));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\t\tLength : %s\n",(tmp_cell->obj->option_mss_length));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\t\tMSS value : %s\n",(tmp_cell->obj->option_mss));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);
				
		}		
		if(strcmp(tmp_cell->obj->option_tab[i], "Window Scale (WScale)") == 0){
			GtkWidget *tcp_options_3 =gtk_expander_new ("TCP Option - Window scale");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_3),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_3,FALSE,FALSE,0);
			GtkWidget *box_options_3=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_3),box_options_3);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_3),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_3),"expander-tabbed");

			sprintf(label,"\t\tKind : %s\n",(tmp_cell->obj->option_tab[i]));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_3),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\t\tShift count : %s\n",(tmp_cell->obj->option_wscale));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_3),tmp_label, FALSE, FALSE, 0);
		}
			
		if(strcmp(tmp_cell->obj->option_tab[i], "Selective Acknowledgment (SACK)") == 0){
			GtkWidget *tcp_options_4 =gtk_expander_new ("TCP Option - Selective Acknowledgment");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_4),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_4,FALSE,FALSE,0);
			GtkWidget *box_options_4=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_4),box_options_4);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_4),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_4),"expander-tabbed");

			sprintf(label,"\t\tKind : %s\n",(tmp_cell->obj->option_sack));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_4),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\t\tLength : %s\n",(tmp_cell->obj->option_sack_length));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_4),tmp_label, FALSE, FALSE, 0);
		}

		if(strcmp(tmp_cell->obj->option_tab[i], "Timestamps (TS)") == 0){
			GtkWidget *tcp_options_5 =gtk_expander_new ("TCP Option - Timestamps");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_5),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_5,FALSE,FALSE,0);
			GtkWidget *box_options_5=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_5),box_options_5);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_5),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_5),"expander-tabbed");

			sprintf(label,"\t\tKind : %s\n",(tmp_cell->obj->option_tab[i]));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\t\tTimestamp value : %s\n",(tmp_cell->obj->option_ts_val));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\t\tTimestamp echo reply : %s\n",(tmp_cell->obj->option_ts_ecr));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);
		}
		i++;
	}
	return;
}
void remplir_arbre(GtkWidget *new_box, gpointer pData){
	
	cell *tmp_cell=(cell *)pData;
	char tmp_label[256];
	sprintf(tmp_label," trame n°%d\n fichier source:%s",tmp_cell->obj->id,tmp_cell->obj->nom_fichier);
	GtkWidget *label_nom_trame=gtk_label_new(tmp_label);
	if (tmp_cell->obj->nb_ligne_erreur==-1){
		gtk_widget_set_name(label_nom_trame,"label_trame_dark_mode");
	}else{
		gtk_widget_set_name(label_nom_trame,"label_trame_dark_mode_erreur");
	}
	gtk_label_set_xalign (GTK_LABEL(label_nom_trame),0);
	gtk_box_pack_start (GTK_BOX(new_box),label_nom_trame,FALSE,FALSE,0);
	
	GtkWidget *ethernet=gtk_expander_new ("\nEthernet II\n");
	gtk_widget_set_name(GTK_WIDGET(ethernet),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	remplir_ethernet(box_ethernet,tmp_cell);	
	
	GtkWidget *IP=gtk_expander_new ("\nInternet Protocol\n");
	gtk_widget_set_name(GTK_WIDGET(IP),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(IP),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),IP,FALSE,FALSE,0);
	GtkWidget *box_ip=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(IP),box_ip);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ip),GTK_ORIENTATION_VERTICAL);
	remplir_ip(box_ip, tmp_cell);	

	GtkWidget *tcp=gtk_expander_new ("\nTransmission Control Protocol\n");
	gtk_widget_set_name(GTK_WIDGET(tcp),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),tcp,FALSE,FALSE,0);
	GtkWidget *box_tcp=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(tcp),box_tcp);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_tcp),GTK_ORIENTATION_VERTICAL);
	remplir_tcp(box_tcp, tmp_cell);	

}		
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
		
	return;*/
	
