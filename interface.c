#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void action_bouton_ip(GtkWidget *pWidget, gpointer pData){
	cell *cellule_obj=(cell *) pData;
	
	if(cellule_obj->status_bouton_ip==1){
		cellule_obj->status_bouton_ip=0;
		
		if (cellule_obj->obj->nb_ligne_erreur==-1){
			gtk_widget_set_name(pWidget,"button_dark_mode");
		}else {
			gtk_widget_set_name(pWidget,"button_dark_mode_erreur");
		}
		gtk_revealer_set_reveal_child (GTK_REVEALER(cellule_obj->revealer),FALSE);
		gtk_widget_show(pWidget);
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

void agrandir_trame(GtkWidget *pWidget, gpointer pData){
	
	cell *liste=pData;	
	GtkWidget *	fenetre= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(fenetre,"fond_dark");
	gtk_window_set_position(GTK_WINDOW(fenetre),  GTK_WIN_POS_CENTER_ALWAYS );
	gtk_window_set_default_size(GTK_WINDOW(fenetre), 600,400);
	
	char tmp_titre[20];
	sprintf(tmp_titre,"trame n°%d",liste->obj->id);
	gtk_window_set_title(GTK_WINDOW(fenetre),tmp_titre);
	
	GtkWidget *box_fenetre=gtk_box_new (FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_fenetre),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(fenetre),box_fenetre);
	set_bouton_menu_trame(box_fenetre,liste,0);
	
	GtkWidget *panneau = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(panneau,"fond_panneau");
	gtk_widget_set_vexpand (panneau, TRUE);
	gtk_widget_set_hexpand (panneau, TRUE);
	
	
	gtk_box_pack_start(GTK_BOX(box_fenetre),panneau, TRUE, TRUE, 0);
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
	GtkWidget* box_trame=gtk_box_new(FALSE,0);
	gtk_box_set_homogeneous (GTK_BOX(box_trame),FALSE);
	gtk_widget_set_name(box_trame,"tree_dark_mode");
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_trame),GTK_ORIENTATION_VERTICAL);
	GtkWidget* box_octets=gtk_box_new(FALSE,0);
	gtk_widget_set_name(box_octets,"tree_dark_mode");
	gtk_box_set_homogeneous (GTK_BOX(box_octets),FALSE);
	gtk_widget_set_name(box_octets,"tree_dark_mode");
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_octets),GTK_ORIENTATION_VERTICAL);
	
	
	trame_agrandie *new_trame_agrandie=(trame_agrandie *) malloc(sizeof(trame_agrandie));
	new_trame_agrandie->b_box_actif=NULL;
	new_trame_agrandie->u_box_actif=NULL;
	int nb_l=( (int) ( (liste->obj->place)/16.0 ) ) + 1 ;
	new_trame_agrandie->tab_box_offset=(GtkWidget**) malloc(sizeof(GtkWidget*)*nb_l);
	new_trame_agrandie->expander_ethernet=NULL;
	new_trame_agrandie->expander_ip=NULL;
	new_trame_agrandie->expander_tcp=NULL;
	new_trame_agrandie->expander_http=NULL;
	new_trame_agrandie->box_trame=box_trame;
	new_trame_agrandie->box_offset=box_octets;
	new_trame_agrandie->ligne=0;
	new_trame_agrandie->obj=liste;
	
	remplir_arbre_agrandir_trame(box_trame,box_octets,new_trame_agrandie);
	
	
	gtk_container_add(GTK_CONTAINER(frame_haut),box_trame);
	gtk_container_add(GTK_CONTAINER(frame_bas),box_octets);

	
	gtk_widget_show_all(fenetre);
	g_signal_connect_swapped (fenetre, "destroy",G_CALLBACK (gtk_widget_destroy),fenetre);
	
	
}


void fermer_selection_fichiers(GtkWidget *pWidget, gpointer pData){
	box *pvbox=(box*)pData;
	
	cell *liste=*(pvbox->liste);
	cell *a_supp=NULL;
	
	while (liste!=NULL) {
		a_supp=liste;
		liste=liste->suiv;
		
		int place=a_supp->obj->place;
		
	
		gtk_widget_destroy(a_supp->bouton);
		gtk_widget_destroy(a_supp->revealer);
		
		free(a_supp->obj->tab);
		free(a_supp->obj->nom_fichier);
		
		if (place>5){
			free(a_supp->obj->mac_dest);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>11){
			free(a_supp->obj->mac_source);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>13){
			free(a_supp->obj->ip_type);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>14){
			free(a_supp->obj->version);
	
			free(a_supp->obj->header_length);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>17){
			free(a_supp->obj->total_length);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>19){
			free(a_supp->obj->identification);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>20){
			
			free(a_supp->obj->flags_offset);
			
			free(a_supp->obj->reserved_bit);
			
			free(a_supp->obj->dont_fragment);
		
			free(a_supp->obj->more_fragment);
			
			
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>21){
			free(a_supp->obj->frag_offset);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>23){
			free(a_supp->obj->protocol);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>25){
			free(a_supp->obj->header_checksum);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>29){
			free(a_supp->obj->ip_source);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>33){
			free(a_supp->obj->ip_dest);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		int i=(a_supp->obj->header_length_)-20;
		
		if (place>(35+i)){
			free(a_supp->obj->source_port);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>(37+i)){
			free(a_supp->obj->destination_port);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>(41+i)){
			free(a_supp->obj->sequence_number_raw);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>(45+1)){
			free(a_supp->obj->acknowledgment_number_raw);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>(46+1)){
			free(a_supp->obj->tcp_header_length);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		
		if (place>(47+i)){
		
			free(a_supp->obj->urg);
		
			free(a_supp->obj->ack);
		
			free(a_supp->obj->push);
		
			free(a_supp->obj->reset);
		
			free(a_supp->obj->syn);
		
			free(a_supp->obj->fin);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		if (place>(49+i)){
			free(a_supp->obj->window);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}

		if (place>(51+i)){
			free(a_supp->obj->tcp_checksum);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}

		if (place>(53+i)){
			free(a_supp->obj->urgent);
		}else{
			free(a_supp->obj);
			free(a_supp);
			continue;
		}
		
		//free(a_supp->obj->no_option);
			
		free(a_supp->obj);
		free(a_supp);
		
	}
	pvbox->taille_liste=1;
	*(pvbox->liste)=NULL;
	
}


void sauvegarde_fichier(FILE *fichier,trame *tmp_trame){
	int place=tmp_trame->place;
	
	fprintf(fichier,"TRAME %d\t\t%s\n",tmp_trame->id,tmp_trame->nom_fichier);
	
	fprintf(fichier,"\tEthernet:\n");
	fprintf(fichier,"\t---------\n");
	
	if (place>5){
	fprintf(fichier,"\t\tSource\t:  %s\n",(tmp_trame->mac_source));
	}
	
	if (place>11){
	fprintf(fichier,"\t\tDestination:  %s\n",(tmp_trame->mac_dest));
	}
	if (place>13){
	fprintf(fichier,"\t\tType:  %s\n\n",(tmp_trame->ip_type));
	}
	if (place>14){
	fprintf(fichier,"\tInternet Protocol:\n");
	fprintf(fichier,"\t------------------\n");
	}
	if (place>14){
	fprintf(fichier,"\t\tVersion:  %s\n",(tmp_trame->version));
	fprintf(fichier,"\t\tHeader Length: %s\n",(tmp_trame->header_length));
	}
	
	if (place>17){
		fprintf(fichier,"\t\tTotal Length: %s\n",(tmp_trame->total_length));
	}
	
	if (place>19){
		fprintf(fichier,"\t\tIdentification: %s\n",(tmp_trame->identification));
	}
	
	if (place>20){
	
	fprintf(fichier,"\t\t\tValue: %s\n",(tmp_trame->flags_offset));
	fprintf(fichier,"\t\t\t %d... .... = Reserved bit: %s\n",tmp_trame->f0, (tmp_trame->reserved_bit));
	fprintf(fichier,"\t\t\t .%d.. .... = Don't Fragment: %s\n",tmp_trame->f1,(tmp_trame->dont_fragment));
	fprintf(fichier,"\t\t\t ..%d. .... = More Fragments: %s\n",tmp_trame->f2,(tmp_trame->more_fragment));
	}
	
	if (place>21){
	fprintf(fichier,"\t\tFragment Offset: %s\n",(tmp_trame->frag_offset));
	}
	
	if (place>22){
	fprintf(fichier,"\t\tTime to Live: %d\n",(tmp_trame->TTL));
	}
	
	if (place>23){
	fprintf(fichier,"\t\tProtocol: %s\n",(tmp_trame->protocol));
	}
	
	if (place>25){
	fprintf(fichier,"\t\tHeader Checksum: %s\n",(tmp_trame->header_checksum));
	}
	
	if (place>29){
	fprintf(fichier,"\t\tSource Address: %s\n",(tmp_trame->ip_source));
	}
	
	if (place>33){
	fprintf(fichier,"\t\tDestination Address: %s\n\n",(tmp_trame->ip_dest));
	}
	
	int i=(tmp_trame->header_length_)-20;
	
	if (place>35+i){
	fprintf(fichier,"\tTCP:\n");
	fprintf(fichier,"\t----\n");
	}
	
	if (place>35+i){
		fprintf(fichier,"\t\tSource Port: %s\n",(tmp_trame->source_port));
	}
	
	if (place>37+i){
	fprintf(fichier,"\t\tDestination Port: %s\n",(tmp_trame->destination_port));
	}
	
	if (place>41+i){
	fprintf(fichier,"\t\tSequence Number (raw): %s\n",(tmp_trame->sequence_number_raw));
	}
	
	if (place>45+i){
	fprintf(fichier,"\t\tAcknowledgment Number (raw): %s\n",(tmp_trame->acknowledgment_number_raw));
	}
	
	if (place>46+i){
	fprintf(fichier,"\t\tHeader Length: %s\n",(tmp_trame->tcp_header_length));
	}
	
	if (place>47+i){
	fprintf(fichier,"\t\tFlags");
	fprintf(fichier,"\t\t\t..%d. .... = Urgent: %s\n",tmp_trame->tcp_f0, (tmp_trame->urg));
	fprintf(fichier,"\t\t\t...%d .... = Acknowledgement: %s\n",tmp_trame->tcp_f1, (tmp_trame->ack));
	fprintf(fichier,"\t\t\t.... %d... = Push: %s\n",tmp_trame->tcp_f2, (tmp_trame->push));
	fprintf(fichier,"\t\t\t.... .%d.. = Reset: %s\n",tmp_trame->tcp_f3, (tmp_trame->reset));
	fprintf(fichier,"\t\t\t.... ..%d. = Syn: %s\n",tmp_trame->tcp_f4, (tmp_trame->syn));
	fprintf(fichier,"\t\t\t.... ...%d = Fin: %s\n",tmp_trame->tcp_f5, (tmp_trame->fin));
	}
	
	if (place>49+i){
	fprintf(fichier,"\t\tWindow: %s\n",(tmp_trame->window));
	}
	
	if (place>51+i){
	fprintf(fichier,"\t\tChecksum: %s\n",(tmp_trame->tcp_checksum));
	}
	
	if (place>53+i){
	fprintf(fichier,"\t\tUrgent Pointer: %s\n\n",(tmp_trame->urgent));
	}
	fprintf(fichier,"\n\n");
	return;
}

void sauvegarder_fichiers(GtkWidget *pWidget, gpointer pData){
	box *pvbox=(box*)pData;
	cell *liste=*(pvbox->liste);
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
				
				while (liste!=NULL) {
					sauvegarde_fichier(fichier, liste->obj);
					liste=liste->suiv;
				}
				
				fclose(fichier);
				break;
			}
		default:
			break;
	}
	
	gtk_widget_destroy (dialog);
}

void sauvegarder_fichier_unique(GtkWidget *pWidget, gpointer pData){
	
	cell *liste=pData;
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
				
				sauvegarde_fichier(fichier, liste->obj);
				
				fclose(fichier);
				break;
			}
		default:
			break;
	}
	
	gtk_widget_destroy (dialog);
}

void init_menu(GtkWidget* grille,box *pvbox){
	
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

	char open[100];
	char close[100];
	char save[100];
	getcwd(open, 100);
	getcwd(close, 100);
	getcwd(save, 100);
	printf("%s\n", open);
	strcat(open, "/icones/ajouter_fichier_32px.png");
	strcat(close, "/icones/supprimer_fichier_1_32px.png");
	strcat(save, "/icones/save_fichier_32px.png");

	GtkWidget * ouvrir_fichier_icone=gtk_image_new_from_file (open);
	GtkWidget * fermer_fichier_icone=gtk_image_new_from_file (close);
	GtkWidget * sauvegarder_fichier_icone=gtk_image_new_from_file (save);
	

	
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
	
	GtkWidget *menu_label=gtk_toggle_button_new_with_label("Id\t   Source      \t \tDestination       \t\tProtocol\t\t");
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

int remplir_ethernet(GtkWidget *box_ethernet,cell *tmp_cell){
	char label[80];
	GtkWidget *tmp_label=NULL;
	int place=tmp_cell->obj->place;
	
	if (place>5){
		sprintf(label,"\t\tDestination:  %s\n",(tmp_cell->obj->mac_dest));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>11){
		sprintf(label,"\t\tSource\t:  %s\n",(tmp_cell->obj->mac_source));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	
	
	
	if (place>13){
	sprintf(label,"\t\tType:  %s\n",(tmp_cell->obj->ip_type));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	return 1;
}

int remplir_ip(GtkWidget *box_ip, cell *tmp_cell){
	char label[80];
	GtkWidget *tmp_label=NULL;
	int place=tmp_cell->obj->place;
	
	if (place>14){
	sprintf(label,"\t\tVersion:  %s\n",(tmp_cell->obj->version));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	
	

	sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->header_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>17){
	sprintf(label,"\t\tTotal Length: %s\n",(tmp_cell->obj->total_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>19){
	sprintf(label,"\t\tIdentification: %s\n",(tmp_cell->obj->identification));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>20){
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
	}else{ return 0; }
	
	if (place>21){
	sprintf(label,"\t\tFragment Offset: %s\n",(tmp_cell->obj->frag_offset));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>22){
	sprintf(label,"\t\tTime to Live: %d\n",(tmp_cell->obj->TTL));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else { return 0; }
	
	if (place>23){
	sprintf(label,"\t\tProtocol: %s\n",(tmp_cell->obj->protocol));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place > 25){
	sprintf(label,"\t\tHeader Checksum: %s\n",(tmp_cell->obj->header_checksum));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place > 29){
	sprintf(label,"\t\tSource Address: %s\n",(tmp_cell->obj->ip_source));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place> 33){
	sprintf(label,"\t\tDestination Address: %s\n",(tmp_cell->obj->ip_dest));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_ip),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	return 1;
	
}

int remplir_tcp(GtkWidget *box_tcp, cell *tmp_cell){
	
	char label[80];
	GtkWidget *tmp_label=NULL;
	int place=tmp_cell->obj->place;
	int i=(tmp_cell->obj->header_length_)-20;
	
	if (place>(35+i)){
	sprintf(label,"\t\tSource Port: %s\n",(tmp_cell->obj->source_port));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(37+i)){
	sprintf(label,"\t\tDestination Port: %s\n",(tmp_cell->obj->destination_port));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(41+i)){
	sprintf(label,"\t\tSequence Number (raw): %s\n",(tmp_cell->obj->sequence_number_raw));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(45+i)){
	sprintf(label,"\t\tAcknowledgment Number (raw): %s\n",(tmp_cell->obj->acknowledgment_number_raw));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(46+i)){
	sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->tcp_header_length));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(47+i)){
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
	}else{ return 0; }
	
	if (place>(49+i)){
	sprintf(label,"\t\tWindow: %s\n",(tmp_cell->obj->window));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(51+i)){
	sprintf(label,"\t\tChecksum: %s\n",(tmp_cell->obj->tcp_checksum));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	if (place>(53+i)){
	sprintf(label,"\t\tUrgent Pointer: %s\n",(tmp_cell->obj->urgent));
	tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
	}else{ return 0; }
	
	
	if(tmp_cell->obj->no_option){
		sprintf(label,"\t\tNo Options: %s\n",(tmp_cell->obj->no_option));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
		return 1;
	}
	
	GtkWidget *tcp_options=gtk_expander_new (tmp_cell->obj->option_length);
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options),FALSE);
	gtk_box_pack_start (GTK_BOX(box_tcp),tcp_options,FALSE,FALSE,0);
	GtkWidget *box_options=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(tcp_options),box_options);	
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options),GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(GTK_WIDGET(tcp_options),"expander-tabbed");
	
	int k = 0;
	while(tmp_cell->obj->option_tab[k]){
		
		if(tmp_cell->obj->option_tab[k] == 1){
			GtkWidget *tcp_options_1 =gtk_expander_new ("TCP Option - No-Operation");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_1),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_1,FALSE,FALSE,0);
			GtkWidget *box_options_1=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_1),box_options_1);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_1),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_1),"expander-tabbed-2");

			sprintf(label,"\n\t\tKind : %s\n","No Operation (NOP)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_1),tmp_label, FALSE, FALSE, 0);
		}	
												
		if(tmp_cell->obj->option_tab[k] == 2){
			GtkWidget *tcp_options_2 =gtk_expander_new ("TCP Option - Maximum Segment Size");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_2),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_2,FALSE,FALSE,0);
			GtkWidget *box_options_2=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_2),box_options_2);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_2),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_2),"expander-tabbed-2");

			sprintf(label,"\n\t\tKind : %s\n","Maximum Segment Size (MSS)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\n\t\tLength : %s\n","0x04 (4 bytes)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\n\t\tMSS value : %s\n",(tmp_cell->obj->option_mss));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_2),tmp_label, FALSE, FALSE, 0);
				
		}		
		
		if(tmp_cell->obj->option_tab[k] == 3){
			GtkWidget *tcp_options_3 =gtk_expander_new ("TCP Option - Window Scale");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_3),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_3,FALSE,FALSE,0);
			GtkWidget *box_options_3=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_3),box_options_3);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_3),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_3),"expander-tabbed-2");

			sprintf(label,"\n\t\tKind : %s\n","Window Scale (Wscale)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_3),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\n\t\tShift count : %s\n",(tmp_cell->obj->option_wscale));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_3),tmp_label, FALSE, FALSE, 0);
		}
			
		if(tmp_cell->obj->option_tab[k] == 4){
			GtkWidget *tcp_options_4 =gtk_expander_new ("TCP Option - Selective Acknowledgment");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_4),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_4,FALSE,FALSE,0);
			GtkWidget *box_options_4=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_4),box_options_4);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_4),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_4),"expander-tabbed-2");

			sprintf(label,"\n\t\tKind : %s\n","Selective Acknoledgment Permitted (SACK Permitted)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_4),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\n\t\tLength : %s\n","0x02 (2 bytes)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_4),tmp_label, FALSE, FALSE, 0);
		}

		if(tmp_cell->obj->option_tab[k] == 8){
			GtkWidget *tcp_options_5 =gtk_expander_new ("TCP Option - Timestamps");
			gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_options_5),FALSE);
			gtk_box_pack_start (GTK_BOX(box_options),tcp_options_5,FALSE,FALSE,0);
			GtkWidget *box_options_5=gtk_box_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(tcp_options_5),box_options_5);	
			gtk_orientable_set_orientation (GTK_ORIENTABLE (box_options_5),GTK_ORIENTATION_VERTICAL);
			gtk_widget_set_name(GTK_WIDGET(tcp_options_5),"expander-tabbed-2");

			sprintf(label,"\n\t\tKind : %s\n","Timestamps (TSOPT)");
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);
		
			sprintf(label,"\n\t\tTimestamp value : %s\n",(tmp_cell->obj->option_ts_val));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);

			sprintf(label,"\n\t\tTimestamp echo reply : %s\n",(tmp_cell->obj->option_ts_ecr));
			tmp_label=gtk_label_new(label);
			gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
			gtk_box_pack_start(GTK_BOX(box_options_5),tmp_label, FALSE, FALSE, 0);
		}
		k++;
		
	}
	return 1;
}


void bloc_erreur(GtkWidget *new_box,cell *tmp_cell){
	if (tmp_cell->obj->nb_ligne_erreur!=-1){
		
		char tmp_label[255];
		
		GtkWidget *erreur_expander=gtk_expander_new ("\nErreur\n");
		gtk_box_pack_start (GTK_BOX(new_box),erreur_expander,FALSE,FALSE,0);
		gtk_widget_set_name(GTK_WIDGET(erreur_expander),"expander");
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(erreur_expander),FALSE);
		GtkWidget *box_erreur=gtk_box_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(erreur_expander),box_erreur);
		sprintf(tmp_label,"\t\t%d octets manquants à la ligne %d du fichier source.",tmp_cell->obj->nb_octet_erreur,tmp_cell->obj->nb_ligne_erreur);
		GtkWidget *tmp_label_erreur=gtk_label_new(tmp_label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label_erreur),0);
		gtk_box_pack_start(GTK_BOX(box_erreur),tmp_label_erreur, FALSE, FALSE, 0);
	}
}

int remplir_http(GtkWidget *box_http, cell *tmp_cell){
	
	GtkWidget *tmp_label=NULL;
	tmp_label=gtk_label_new(tmp_cell->obj->http);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box_http),tmp_label, FALSE, FALSE, 0);
	return 1;

}
void set_bouton_menu_trame(GtkWidget *new_box,cell *tmp_cell,int statut){
	
	char tmp_label[256];
	sprintf(tmp_label," trame n°%d\n fichier source:%s",tmp_cell->obj->id,tmp_cell->obj->nom_fichier);
	GtkWidget *label_nom_trame=gtk_label_new(tmp_label);
	gtk_label_set_xalign (GTK_LABEL(label_nom_trame),0);
	
	GtkWidget *box_titre=gtk_box_new (FALSE,0);
	gtk_box_set_homogeneous (GTK_BOX(box_titre),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),box_titre,FALSE,FALSE,0);
	gtk_box_pack_start (GTK_BOX(box_titre),label_nom_trame,FALSE,FALSE,0);
	
	GtkWidget *bouton_menu_trame=gtk_menu_button_new ();
	GtkWidget *menu_bouton_menu_trame =gtk_menu_new ();
	gtk_widget_set_name(menu_bouton_menu_trame,"label_menu_trame_dark_mode");
	gtk_menu_button_set_popup (GTK_MENU_BUTTON(bouton_menu_trame),menu_bouton_menu_trame);
	
	GtkWidget *agrandir=gtk_menu_item_new_with_label ("agrandir");
	GtkWidget *sauvegarder=gtk_menu_item_new_with_label ("Sauvegarder la trame");
	
	if (statut){
		g_signal_connect(G_OBJECT(agrandir), "activate", G_CALLBACK(agrandir_trame),tmp_cell);
		gtk_menu_attach (GTK_MENU(menu_bouton_menu_trame),agrandir,0,2,0,1);
		bloc_erreur(new_box,tmp_cell);
	}
	g_signal_connect(G_OBJECT(sauvegarder), "activate", G_CALLBACK(sauvegarder_fichier_unique),tmp_cell);
	
	
	
	gtk_menu_attach (GTK_MENU(menu_bouton_menu_trame),sauvegarder,0,2,1,2);
	
	char option[100];
	getcwd(option, 100);
	strcat(option, "/icones/options_32px.png");

	GtkWidget *bouton_menu_trame_icone=gtk_image_new_from_file (option);
	gtk_button_set_image(GTK_BUTTON(bouton_menu_trame),bouton_menu_trame_icone);
	gtk_box_pack_end (GTK_BOX(box_titre),bouton_menu_trame,FALSE,FALSE,0);
	
	gtk_widget_show_all(menu_bouton_menu_trame);
	
	if (tmp_cell->obj->nb_ligne_erreur==-1){
		gtk_widget_set_name(label_nom_trame,"label_trame_dark_mode");
		gtk_widget_set_name(box_titre,"label_trame_dark_mode");
		gtk_widget_set_name(bouton_menu_trame,"label_trame_dark_mode");
	}else{
		gtk_widget_set_name(label_nom_trame,"label_trame_dark_mode_erreur");
		gtk_widget_set_name(box_titre,"label_trame_dark_mode_erreur");
		gtk_widget_set_name(bouton_menu_trame,"label_trame_dark_mode_erreur");
	}
}

int creation_ethernet(GtkWidget *new_box,cell *tmp_cell){
	
	
	GtkWidget *ethernet=gtk_expander_new ("\nEthernet II\n");
	gtk_widget_set_name(GTK_WIDGET(ethernet),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	return remplir_ethernet(box_ethernet,tmp_cell);	
	
	
	
}


int creation_ip(GtkWidget *new_box,cell *tmp_cell){
	
	int place=tmp_cell->obj->place;
	
	if (place>14){
	GtkWidget *IP=gtk_expander_new ("\nInternet Protocol\n");
	gtk_widget_set_name(GTK_WIDGET(IP),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(IP),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),IP,FALSE,FALSE,0);
	GtkWidget *box_ip=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(IP),box_ip);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ip),GTK_ORIENTATION_VERTICAL);
	return remplir_ip(box_ip, tmp_cell);
	}else{
		return 0;
	}
	return 1;
	
}


int creation_tcp(GtkWidget *new_box,cell *tmp_cell){
	
	int place=tmp_cell->obj->place;
	int i=(tmp_cell->obj->header_length_)-20;
	
	if (place>(35+i)){
		
	GtkWidget *tcp=gtk_expander_new ("\nTransmission Control Protocol\n");
	gtk_widget_set_name(GTK_WIDGET(tcp),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),tcp,FALSE,FALSE,0);
	GtkWidget *box_tcp=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(tcp),box_tcp);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_tcp),GTK_ORIENTATION_VERTICAL);
	return remplir_tcp(box_tcp, tmp_cell);	
	
	}else {
		return 0;
	}
	return 1;
}


int creation_http(GtkWidget *new_box, cell *tmp_cell){

	int place=tmp_cell->obj->place;
	int i=(tmp_cell->obj->header_length_)-20;
	
		
	GtkWidget *http=gtk_expander_new ("\nHypertext Transfer Protocol\n");
	gtk_widget_set_name(GTK_WIDGET(http),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(http),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),http,FALSE,FALSE,0);
	GtkWidget *box_http=gtk_box_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(http),box_http);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_http),GTK_ORIENTATION_VERTICAL);
	return remplir_http(box_http, tmp_cell);	
	
	return 1;
}

void remplir_arbre(GtkWidget *new_box, gpointer pData,int statut){
	
	cell *tmp_cell=(cell *)pData;
	
	set_bouton_menu_trame(new_box,tmp_cell,statut);
	
	if ( creation_ethernet(new_box,tmp_cell) == 0){
		return ;
	}
	
	if ( creation_ip(new_box,tmp_cell) == 0 ){
		return ;
	}
	
	if ( creation_tcp(new_box,tmp_cell) == 0 ){
		return ;
	}	
	if(tmp_cell->obj->http[0] == 0){
		return;
	}
	if ( creation_http(new_box,tmp_cell) == 0 )
		return;
	return;
}		

cell_button *cree_cell_button(GtkWidget* button){
	cell_button *new=(cell_button *) malloc(sizeof(cell_button));
	new->suiv=NULL;
	new->button=button;
	return new;
}

static inline void reset_bouton_actif(trame_agrandie *tmp_trame_agrandie){
	
	cell_button *tmp=tmp_trame_agrandie->b_box_actif;
	cell_button *tmp_change_dest=NULL;
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp_change_dest=tmp;
		tmp=tmp->suiv;
		tmp_change_dest->suiv=NULL;
		
	}
	
	tmp=tmp_trame_agrandie->u_box_actif;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp_change_dest=tmp;
		tmp=tmp->suiv;
		tmp_change_dest->suiv=NULL;
	}
	
	tmp_trame_agrandie->u_box_actif=NULL;
	tmp_trame_agrandie->b_box_actif=NULL;
}

static inline void mise_a_jour_theme_actif(trame_agrandie *tmp_trame_agrandie){
	cell_button *tmp=tmp_trame_agrandie->b_box_actif;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode_status_1");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp=tmp->suiv;
	}
	
	tmp=tmp_trame_agrandie->u_box_actif;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode_status_1");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp=tmp->suiv;
	}
	
}



static inline void offset_debut_ligne(GtkWidget *box, int n){
	char label[7];
	
	sprintf(label," %.4X ",n);
	
	GtkWidget *tmp_label=gtk_label_new(label);
	gtk_widget_set_name((tmp_label),"label_trame_offset");
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box),tmp_label, FALSE, FALSE, 0);
	
}

static inline void ajout_ligne_offset(trame_agrandie *tmp_trame_agrandie){
	GtkWidget* box_trame=gtk_box_new(FALSE,0);
	(tmp_trame_agrandie->tab_box_offset)[(tmp_trame_agrandie->ligne)]=box_trame;
	gtk_box_pack_start(GTK_BOX(tmp_trame_agrandie->box_offset),box_trame, FALSE, FALSE, 0);
	
	gtk_box_set_homogeneous (GTK_BOX(box_trame),FALSE);
	gtk_widget_set_name(box_trame,"tree_dark_mode");
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_trame),GTK_ORIENTATION_HORIZONTAL);
	
	
	offset_debut_ligne(box_trame,tmp_trame_agrandie->ligne);
	(tmp_trame_agrandie->ligne)++;
}

void action_bouton_mac_dest(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_mac_dest;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_mac_dest;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}


void action_bouton_mac_source(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_mac_source;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_mac_source;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_type(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_ip_type;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_ip_type;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}



int remplir_ethernet_agrandir(GtkWidget *box_ethernet,GtkWidget *box_bas_agrandir,trame_agrandie *tmp_trame_agrandie){
	cell *tmp_cell=tmp_trame_agrandie->obj;
	
	ajout_ligne_offset(tmp_trame_agrandie);
	GtkWidget *box_trame=(tmp_trame_agrandie->tab_box_offset)[0];
	char label[80];
	GtkWidget *tmp_label=NULL;
	GtkWidget *tmp_button=NULL;;
	int place=tmp_trame_agrandie->obj->obj->place;
	char label_ip[20];
	
	if (place>5){
		sprintf(label,"\t\tDestination:  %s\n",(tmp_cell->obj->mac_dest));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_mac_dest=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_button, FALSE, FALSE, 0);
		sprintf(label_ip," %.2X %.2X %.2X %.2X %.2X %.2X",(tmp_cell->obj->tab)[0]
			,(tmp_cell->obj->tab)[1]
			,(tmp_cell->obj->tab)[2]
			,(tmp_cell->obj->tab)[3]
			,(tmp_cell->obj->tab)[4]
			,(tmp_cell->obj->tab)[5]);
		tmp_label=gtk_label_new(label_ip);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_mac_dest=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_mac_dest),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_mac_dest),tmp_trame_agrandie);
		
		
	}else{ return 0; }
	
	if (place>11){
		sprintf(label,"\t\tSource\t:  %s\n",(tmp_cell->obj->mac_source));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_mac_source=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_button, FALSE, FALSE, 0);
		sprintf(label_ip," %.2X %.2X %.2X %.2X %.2X %.2X",(tmp_cell->obj->tab)[6]
													,(tmp_cell->obj->tab)[7]
													,(tmp_cell->obj->tab)[8]
													,(tmp_cell->obj->tab)[9]
													,(tmp_cell->obj->tab)[10]
													,(tmp_cell->obj->tab)[11]);
		tmp_label=gtk_label_new(label_ip);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_mac_source=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_mac_source),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_mac_source),tmp_trame_agrandie);
	
	}else{ return 0; }
	
	
	
	
	if (place>13){
		sprintf(label,"\t\tType:  %s\n",(tmp_cell->obj->ip_type));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_ip_type=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ethernet),tmp_button, FALSE, FALSE, 0);
		char label_type[10];
		
		sprintf(label_type," %.2X %.2X",(tmp_cell->obj->tab)[12],(tmp_cell->obj->tab)[13]);
		tmp_label=gtk_label_new(label_type);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_ip_type=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_type),tmp_trame_agrandie);
		
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_type),tmp_trame_agrandie);
		
		
	}else{ return 0; }
	gtk_widget_show_all(box_bas_agrandir);
	return 1;
}


void action_bouton_version(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_version_hl;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_version;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_hl(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_version_hl;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_header_length;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_version_hl(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_version_hl;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_header_length;
	tmp_trame_agrandie->u_header_length->suiv=tmp_trame_agrandie->u_version;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_total_length(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_total_length;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_total_length;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_identification(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_identification;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_identification;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_flags(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_flags_offset;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_flags_offset;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_flag_offset(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_frag_offset;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_frag_offset;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ttl(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_ttl;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_ttl;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_protocol(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_protocol;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_protocol;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_hchecksum(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_header_checksum;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_header_checksum;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_source(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_ip_source;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_ip_source;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_dest(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_ip_dest;
	tmp_trame_agrandie->b_ip_dest->suiv=tmp_trame_agrandie->b_ip_dest_1;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_ip_dest;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

int remplir_ip_agrandir(GtkWidget *box_ip,GtkWidget *box_bas_agrandir, trame_agrandie *tmp_trame_agrandie){
	char label[80];
	GtkWidget *tmp_label=NULL;
	GtkWidget *tmp_button=NULL;
	cell *tmp_cell=tmp_trame_agrandie->obj;
	int place=tmp_cell->obj->place;
	
	ajout_ligne_offset(tmp_trame_agrandie);
	GtkWidget *box_trame=(tmp_trame_agrandie->tab_box_offset)[1];
	
	if (place>14){
		char label_version[5];
		
		sprintf(label_version," %.2X",(tmp_cell->obj->tab)[14]);
		tmp_label=gtk_label_new(label_version);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_version_hl=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX((tmp_trame_agrandie->tab_box_offset)[0]),tmp_button_bas, FALSE, FALSE, 0);
		
		sprintf(label,"\t\tVersion:  %s\n",(tmp_cell->obj->version));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_version=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_version),tmp_trame_agrandie);
		
		
		
		sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->header_length));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_header_length=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_hl),tmp_trame_agrandie);
		
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_version_hl),tmp_trame_agrandie);
		
		
		char label_octet_fin[4];
		sprintf(label_octet_fin," 00");
		tmp_label=gtk_label_new(label_octet_fin);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_widget_set_name(tmp_label,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(((tmp_trame_agrandie->tab_box_offset)[0])),tmp_label, FALSE, FALSE, 0);
		

	}else{ return 0; }
	
	if (place>17){
		sprintf(label,"\t\tTotal Length: %s\n",(tmp_cell->obj->total_length));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_total_length=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_total_l[10];
		sprintf(label_total_l," %.2X %.2X",(tmp_cell->obj->tab)[16],(tmp_cell->obj->tab)[17]);
		tmp_label=gtk_label_new(label_total_l);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_total_length=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_total_length),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_total_length),tmp_trame_agrandie);
	}else{ return 0; }
	
	if (place>19){
		sprintf(label,"\t\tIdentification: %s\n",(tmp_cell->obj->identification));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_identification=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		char label_identification[10];
		sprintf(label_identification," %.2X %.2X",(tmp_cell->obj->tab)[18],(tmp_cell->obj->tab)[19]);
		tmp_label=gtk_label_new(label_identification);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_identification=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_identification),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_identification),tmp_trame_agrandie);

		
		
	}else{ return 0; }
	
	if (place>20){
		
		GtkWidget *flags=gtk_expander_new ("Flags");
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(flags),FALSE);
		gtk_box_pack_start (GTK_BOX(box_ip),flags,FALSE,FALSE,0);
		GtkWidget *box_flags=gtk_box_new(FALSE,0);
		tmp_trame_agrandie->expander_flags=flags;
		gtk_container_add(GTK_CONTAINER(flags),box_flags);	
		gtk_orientable_set_orientation (GTK_ORIENTABLE (box_flags),GTK_ORIENTATION_VERTICAL);
		gtk_widget_set_name(GTK_WIDGET(flags),"expander-tabbed");
		
		sprintf(label,"\t\tValue: %s\n",(tmp_cell->obj->flags_offset));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_flags_offset=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_flags),tmp_button, FALSE, FALSE, 0);
		char label_flags[10];
		sprintf(label_flags," %.2X",(tmp_cell->obj->tab)[20]);
		tmp_label=gtk_label_new(label_flags);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_flags_offset=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_flags),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_flags),tmp_trame_agrandie);

		
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
		
		
		
	}else{ return 0; }
	
	if (place>21){
		sprintf(label,"\t\tFragment Offset: %s\n",(tmp_cell->obj->frag_offset));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_frag_offset=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_total_l[10];
		sprintf(label_total_l," %.2X",(tmp_cell->obj->tab)[21]);
		tmp_label=gtk_label_new(label_total_l);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_frag_offset=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_flag_offset),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_flag_offset),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place>22){
		sprintf(label,"\t\tTime to Live: %d\n",(tmp_cell->obj->TTL));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_ttl=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_ttl[10];
		sprintf(label_ttl," %.2X",(tmp_cell->obj->tab)[22]);
		tmp_label=gtk_label_new(label_ttl);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_ttl=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_ttl),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_ttl),tmp_trame_agrandie);
	}else { return 0; }
	
	if (place>23){
		sprintf(label,"\t\tProtocol: %s\n",(tmp_cell->obj->protocol));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_protocol=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_protocol[10];
		sprintf(label_protocol," %.2X",(tmp_cell->obj->tab)[23]);
		tmp_label=gtk_label_new(label_protocol);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_protocol=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_protocol),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_protocol),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place > 25){
		sprintf(label,"\t\tHeader Checksum: %s\n",(tmp_cell->obj->header_checksum));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_header_checksum=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_hchecksum[10];
		sprintf(label_hchecksum," %.2X %.2X",(tmp_cell->obj->tab)[24],(tmp_cell->obj->tab)[25]);
		tmp_label=gtk_label_new(label_hchecksum);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_header_checksum=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_hchecksum),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_hchecksum),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place > 29){
		sprintf(label,"\t\tSource Address: %s\n",(tmp_cell->obj->ip_source));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_ip_source=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_ip_source[20];
		sprintf(label_ip_source," %.2X %.2X %.2X %.2X",(tmp_cell->obj->tab)[26],(tmp_cell->obj->tab)[27],(tmp_cell->obj->tab)[28],(tmp_cell->obj->tab)[29]);
		tmp_label=gtk_label_new(label_ip_source);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_ip_source=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);
	}else{ return 0; }
	
	if (place> 33){
		sprintf(label,"\t\tDestination Address: %s\n",(tmp_cell->obj->ip_dest));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_ip_dest=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_ip),tmp_button, FALSE, FALSE, 0);
		
		char label_ip_dest[15];
		sprintf(label_ip_dest," %.2X %.2X",(tmp_cell->obj->tab)[30],(tmp_cell->obj->tab)[31]);
		tmp_label=gtk_label_new(label_ip_dest);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_ip_dest=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		
		ajout_ligne_offset(tmp_trame_agrandie);
		sprintf(label_ip_dest," %.2X %.2X",(tmp_cell->obj->tab)[32],(tmp_cell->obj->tab)[33]);
		tmp_label=gtk_label_new(label_ip_dest);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_ip_dest_1=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX((tmp_trame_agrandie->tab_box_offset)[2]),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
	}else{ return 0; }
	
	gtk_widget_show_all(box_bas_agrandir);
	
	return 1;
	
}


void action_bouton_source_port(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_source_port;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_source_port;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}


void action_bouton_destination_port(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_destination_port;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_destination_port;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_sequence_number(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_sequence_number_raw;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_sequence_number_raw;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ack_number(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_acknowledgment_number_raw;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_acknowledgment_number_raw;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_hl(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_header_length;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_header_length;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_u(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f0;
	tmp_trame_agrandie->b_tcp_f0->suiv=tmp_trame_agrandie->b_tcp_f1;
	tmp_trame_agrandie->b_tcp_f1->suiv=tmp_trame_agrandie->b_tcp_f2;
	tmp_trame_agrandie->b_tcp_f2->suiv=tmp_trame_agrandie->b_tcp_f3;
	tmp_trame_agrandie->b_tcp_f3->suiv=tmp_trame_agrandie->b_tcp_f4;
	tmp_trame_agrandie->b_tcp_f4->suiv=tmp_trame_agrandie->b_tcp_f5;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}


void action_bouton_tcp_flags_f0(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f0;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_f1(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f1;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_f2(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f2;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_f3(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f3;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_f4(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f4;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_f5(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_f5;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_f;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_window(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_window;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_window;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_checksum(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_tcp_checksum;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_tcp_checksum;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_urgent_pointer(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->b_box_actif=tmp_trame_agrandie->b_urgent;
	tmp_trame_agrandie->u_box_actif=tmp_trame_agrandie->u_urgent;
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

int remplir_tcp_agrandir(GtkWidget *box_tcp,GtkWidget *box_bas_agrandir,trame_agrandie *tmp_trame_agrandie){
	
	char label[80];
	GtkWidget *tmp_label=NULL;
	
	
	GtkWidget *tmp_button=NULL;
	cell *tmp_cell=tmp_trame_agrandie->obj;
	int place=tmp_cell->obj->place;
	GtkWidget *box_trame=(tmp_trame_agrandie->tab_box_offset)[2];
	
	int i=(tmp_cell->obj->header_length_)-20;
	
	
	
	if (place>(35+i)){
		sprintf(label,"\t\tSource Port: %s\n",(tmp_cell->obj->source_port));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_source_port=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		char label_src_port[10];
		sprintf(label_src_port," %.2X %.2X",(tmp_cell->obj->tab)[34+i],(tmp_cell->obj->tab)[35+i]);
		tmp_label=gtk_label_new(label_src_port);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_source_port=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_source_port),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_source_port),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place>(37+i)){
		sprintf(label,"\t\tDestination Port: %s\n",(tmp_cell->obj->destination_port));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_destination_port=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		char label_dst_port[10];
		sprintf(label_dst_port," %.2X %.2X",(tmp_cell->obj->tab)[36+i],(tmp_cell->obj->tab)[37+i]);
		tmp_label=gtk_label_new(label_dst_port);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_destination_port=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_destination_port),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_destination_port),tmp_trame_agrandie);	
	}else{ return 0; }
	
	if (place>(41+i)){
		sprintf(label,"\t\tSequence Number (raw): %s\n",(tmp_cell->obj->sequence_number_raw));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_sequence_number_raw=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		char label_sequence_number[25];
		sprintf(label_sequence_number," %.2X %.2X %.2X %.2X",(tmp_cell->obj->tab)[38+i],(tmp_cell->obj->tab)[39+i],(tmp_cell->obj->tab)[40+i],(tmp_cell->obj->tab)[41+i]);
		tmp_label=gtk_label_new(label_sequence_number);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_sequence_number_raw=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_sequence_number),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_sequence_number),tmp_trame_agrandie);		
	}else{ return 0; }
	
	if (place>(45+i)){
		sprintf(label,"\t\tAcknowledgment Number (raw): %s\n",(tmp_cell->obj->acknowledgment_number_raw));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_acknowledgment_number_raw=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		char label_ack_number[25];
		sprintf(label_ack_number," %.2X %.2X %.2X %.2X",(tmp_cell->obj->tab)[42+i],(tmp_cell->obj->tab)[43+i],(tmp_cell->obj->tab)[44+i],(tmp_cell->obj->tab)[45+i]);
		tmp_label=gtk_label_new(label_ack_number);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_acknowledgment_number_raw=cree_cell_button(tmp_button_bas);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_ack_number),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_ack_number),tmp_trame_agrandie);		}else{ return 0; }
	
	if (place>(46+i)){
		sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->tcp_header_length));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		
		
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_header_length=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		
		char label_hl[10];
		sprintf(label_hl," %.2X",(tmp_cell->obj->tab)[46+i]);
		tmp_label=gtk_label_new(label_hl);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->u_tcp_header_length=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_hl),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_tcp_hl),tmp_trame_agrandie);

		
		
	}else{ return 0; }
	
	if (place>(47+i)){
		GtkWidget *tcp_flags=gtk_expander_new ("Flags");
		tmp_trame_agrandie->expander_flags_tcp=tcp_flags;
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp_flags),FALSE);
		gtk_box_pack_start (GTK_BOX(box_tcp),tcp_flags,FALSE,FALSE,0);
		GtkWidget *box_flags_2=gtk_box_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(tcp_flags),box_flags_2);	
		gtk_orientable_set_orientation (GTK_ORIENTABLE (box_flags_2),GTK_ORIENTATION_VERTICAL);
		gtk_widget_set_name(GTK_WIDGET(tcp_flags),"expander-tabbed");
		
		
		char label_flags_tcp[10];
		sprintf(label_flags_tcp," %.2X",(tmp_cell->obj->tab)[47+i]);
		tmp_label=gtk_label_new(label_flags_tcp);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->u_tcp_f=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_tcp_flags_u),tmp_trame_agrandie);
		
		
		sprintf(label,"\t\t..%d. .... = Urgent: %s\n",tmp_cell->obj->tcp_f0, (tmp_cell->obj->urg));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f0=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f0),tmp_trame_agrandie);
		
		
		
		
		sprintf(label,"\t\t...%d .... = Acknowledgement: %s\n",tmp_cell->obj->tcp_f1, (tmp_cell->obj->ack));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f1=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f1),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... %d... = Push: %s\n",tmp_cell->obj->tcp_f2, (tmp_cell->obj->push));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f2=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f2),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... .%d.. = Reset: %s\n",tmp_cell->obj->tcp_f3, (tmp_cell->obj->reset));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f3=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f3),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... ..%d. = Syn: %s\n",tmp_cell->obj->tcp_f4, (tmp_cell->obj->syn));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f4=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f4),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... ...%d = Fin: %s\n",tmp_cell->obj->tcp_f5, (tmp_cell->obj->fin));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_f5=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_f5),tmp_trame_agrandie);	
	}else{ return 0; }
	
	if (place>(49+i)){
		
		
		ajout_ligne_offset(tmp_trame_agrandie);
		box_trame=(tmp_trame_agrandie->tab_box_offset)[3];
		
		sprintf(label,"\t\tWindow: %s\n",(tmp_cell->obj->window));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_window=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		
		char label_window[10];
		sprintf(label_window," %.2X",(tmp_cell->obj->tab)[49+i]);
		tmp_label=gtk_label_new(label_window);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_window=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_window),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_window),tmp_trame_agrandie);
	}else{ return 0; }
	
	if (place>(51+i)){
		sprintf(label,"\t\tChecksum: %s\n",(tmp_cell->obj->tcp_checksum));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_tcp_checksum=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		
		char label_tcp[10];
		sprintf(label_tcp," %.2X %.2X",(tmp_cell->obj->tab)[50+i],(tmp_cell->obj->tab)[51+i]);
		tmp_label=gtk_label_new(label_tcp);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_tcp_checksum=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_checksum),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_tcp_checksum),tmp_trame_agrandie);
	}else{ return 0; }
	
	if (place>(53+i)){
		sprintf(label,"\t\tUrgent Pointer: %s\n",(tmp_cell->obj->urgent));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		tmp_button=gtk_button_new ();
		tmp_trame_agrandie->u_urgent=cree_cell_button(tmp_button);
		gtk_widget_set_name(tmp_button,"tree_dark_mode");
		gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_button, FALSE, FALSE, 0);
		
		char label_urg_ptr[10];
		sprintf(label_urg_ptr," %.2X %.2X",(tmp_cell->obj->tab)[52+i],(tmp_cell->obj->tab)[53+i]);
		tmp_label=gtk_label_new(label_urg_ptr);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		GtkWidget *tmp_button_bas=gtk_button_new ();
		tmp_trame_agrandie->b_urgent=cree_cell_button(tmp_button_bas);
		gtk_container_add (GTK_CONTAINER(tmp_button_bas),tmp_label);
		gtk_widget_set_name(tmp_button_bas,"tree_dark_mode");
		gtk_box_pack_start(GTK_BOX(box_trame),tmp_button_bas, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(tmp_button), "clicked", G_CALLBACK(action_bouton_tcp_urgent_pointer),tmp_trame_agrandie);
		g_signal_connect(G_OBJECT(tmp_button_bas), "clicked", G_CALLBACK(action_bouton_tcp_urgent_pointer),tmp_trame_agrandie);
		/*
		
		
		
		
		
		*/
	}else{ return 0; }
	
	
	if(tmp_cell->obj->no_option){
		sprintf(label,"\t\tNo Options: %s\n",(tmp_cell->obj->no_option));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_tcp),tmp_label, FALSE, FALSE, 0);
		return 1;
	}
	
	return 1;
}








int creation_ethernet_agrandir(GtkWidget *new_box,GtkWidget *box_bas_agrandir,trame_agrandie *tmp_trame_agrandie){
	
	GtkWidget *ethernet=gtk_expander_new ("\nEthernet II\n");
	tmp_trame_agrandie->expander_ethernet=ethernet;
	gtk_widget_set_name(GTK_WIDGET(ethernet),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	return remplir_ethernet_agrandir(box_ethernet,box_bas_agrandir,tmp_trame_agrandie);	
}


int creation_ip_agrandir(GtkWidget *new_box,GtkWidget *box_bas_agrandir,trame_agrandie *tmp_trame_agrandie){
	
	int place=tmp_trame_agrandie->obj->obj->place;
	
	if (place>14){
		GtkWidget *IP=gtk_expander_new ("\nInternet Protocol\n");
		tmp_trame_agrandie->expander_ip=IP;
		gtk_widget_set_name(GTK_WIDGET(IP),"expander");
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(IP),FALSE);
		gtk_box_pack_start (GTK_BOX(new_box),IP,FALSE,FALSE,0);
		GtkWidget *box_ip=gtk_box_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(IP),box_ip);
		gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ip),GTK_ORIENTATION_VERTICAL);
		return remplir_ip_agrandir(box_ip,box_bas_agrandir, tmp_trame_agrandie);
	}else{
		return 0;
	}
	return 1;
	
}


int creation_tcp_agrandir(GtkWidget *new_box,GtkWidget *box_bas_agrandir,trame_agrandie *tmp_trame_agrandie){
	
	int place=tmp_trame_agrandie->obj->obj->place;
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	if (place>(35+i)){
		
		GtkWidget *tcp=gtk_expander_new ("\nTransmission Control Protocol\n");
		tmp_trame_agrandie->expander_tcp=tcp;
		gtk_widget_set_name(GTK_WIDGET(tcp),"expander");
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(tcp),FALSE);
		gtk_box_pack_start (GTK_BOX(new_box),tcp,FALSE,FALSE,0);
		GtkWidget *box_tcp=gtk_box_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(tcp),box_tcp);
		gtk_orientable_set_orientation (GTK_ORIENTABLE (box_tcp),GTK_ORIENTATION_VERTICAL);
		return remplir_tcp_agrandir(box_tcp,box_bas_agrandir, tmp_trame_agrandie);	
		
	}else {
		return 0;
	}
	return 1;
}



void remplir_arbre_agrandir_trame(GtkWidget *new_box,GtkWidget *box_bas_agrandir, gpointer pData){
	
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *)pData;
	
	bloc_erreur(new_box,tmp_trame_agrandie->obj);
	
	if ( creation_ethernet_agrandir(new_box,box_bas_agrandir,tmp_trame_agrandie) == 0){
		return ;
	}
	
	if ( creation_ip_agrandir(new_box,box_bas_agrandir,tmp_trame_agrandie) == 0 ){
		return ;
	}
	
	if ( creation_tcp_agrandir(new_box,box_bas_agrandir,tmp_trame_agrandie) ==0 ){
		return ;
	}
	
}

