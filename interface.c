
#include "types.h"
//#include "actions_boutons_agrandir.h"
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
				int erreur=0;
				
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
					
					
					res=charge_trame(fichier_source,&ligne,pvbox->taille_liste,pvbox->liste,pvbox_haut, pvbox_bas,filename,&erreur);
					
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


void ajout_liste_b(cell_button **liste,cell_button *new_cell){
	
	
	if ((*liste)==NULL){
		(*liste)=new_cell;
	}else{
		new_cell->suiv=(*liste);
		(*liste)=new_cell;
	}
}

cell_button * creer_bouton(char *label,GtkWidget *box){
	
	
	GtkWidget *tmp_label=gtk_label_new(label);
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	
	GtkWidget *tmp_button=gtk_button_new ();
	gtk_container_add (GTK_CONTAINER(tmp_button),tmp_label);
	gtk_box_pack_start(GTK_BOX(box),tmp_button, FALSE, FALSE, 0);
	gtk_widget_set_name(tmp_button,"tree_dark_mode");
	cell_button *new_cell=(cell_button *) malloc(sizeof(cell_button));
	new_cell->suiv=NULL;
	new_cell->button=tmp_button;
	return new_cell;
}


static inline void offset_debut_ligne(GtkWidget *box, int n){
	char label[7];
	
	sprintf(label," %.4X ",n);
	
	GtkWidget *tmp_label=gtk_label_new(label);
	gtk_widget_set_name((tmp_label),"label_trame_offset");
	gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
	gtk_box_pack_start(GTK_BOX(box),tmp_label, FALSE, FALSE, 0);
	
}

GtkWidget* ajout_ligne_offset(trame_agrandie *tmp_trame_agrandie,int i){
	GtkWidget* box_trame=gtk_box_new(FALSE,0);
	gtk_box_set_homogeneous (GTK_BOX(box_trame),FALSE);
	gtk_widget_set_name(box_trame,"tree_dark_mode");
	gtk_box_pack_start(GTK_BOX(tmp_trame_agrandie->box_bas),box_trame, FALSE, FALSE, 0);
	
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_trame),GTK_ORIENTATION_HORIZONTAL);
	
	offset_debut_ligne(box_trame,i);
	return box_trame;
}

void init_trame_agrandie(trame_agrandie *new_trame_agrandie){
	
	new_trame_agrandie->box_haut_actif=NULL;
	new_trame_agrandie->box_bas_actif=NULL;	
	unsigned int* tab_trame=new_trame_agrandie->obj->obj->tab;
	int place=new_trame_agrandie->obj->obj->place;
	
	new_trame_agrandie->tab_offset=( cell_button** ) malloc(sizeof( cell_button* ) * place);
	
	
	
	
	new_trame_agrandie->option_tab=(cell_button_option **) malloc(sizeof(cell_button_option *)*6);
	
	(new_trame_agrandie->option_tab)[0]=NULL;
	(new_trame_agrandie->option_tab)[1]=NULL;
	(new_trame_agrandie->option_tab)[2]=NULL;
	(new_trame_agrandie->option_tab)[3]=NULL;
	(new_trame_agrandie->option_tab)[4]=NULL;
	(new_trame_agrandie->option_tab)[5]=NULL;
	
	GtkWidget* box_octets=NULL;
	
	
	cell_button *tmp_cell;
	char label[10];
	for (int i=0;i<place;i++){
		if ((i % 16) == 0){
			box_octets=ajout_ligne_offset(new_trame_agrandie,((int) i/16.0));
		}
		
		if ( ( ( i % 8 ) == 0 ) && ((i % 16) != 0) ) {
			sprintf(label,"  %.2X",tab_trame[i]);
			
			tmp_cell=creer_bouton(label,box_octets);
			gtk_widget_set_name(tmp_cell->button,"tree_dark_mode_offset");
			(new_trame_agrandie->tab_offset)[i]=tmp_cell;		
		}else{
			sprintf(label," %.2X",tab_trame[i]);
			
			tmp_cell=creer_bouton(label,box_octets);
			gtk_widget_set_name(tmp_cell->button,"tree_dark_mode_offset");
			(new_trame_agrandie->tab_offset)[i]=tmp_cell;
		}

		
		
		
	}
}

void agrandir_trame(GtkWidget *pWidget, gpointer pData){
	
	cell *liste=pData;	
	GtkWidget *	fenetre= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_vexpand (	fenetre, FALSE);
	gtk_widget_set_hexpand (	fenetre, FALSE);
	
	gtk_widget_set_name(fenetre,"fond_dark");
	gtk_window_set_position(GTK_WINDOW(fenetre),  GTK_WIN_POS_MOUSE );
	gtk_window_set_default_size(GTK_WINDOW(fenetre), 600,400);
	
	char tmp_titre[20];
	sprintf(tmp_titre,"trame n°%d",liste->obj->id);
	gtk_window_set_title(GTK_WINDOW(fenetre),tmp_titre);
	
	GtkWidget *box_fenetre=gtk_box_new (FALSE,0);
	gtk_box_set_homogeneous (GTK_BOX(box_fenetre),FALSE);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_fenetre),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(fenetre),box_fenetre);
	set_bouton_menu_trame(box_fenetre,liste,0);
	
	GtkWidget *panneau = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_name(panneau,"fond_panneau");
	gtk_widget_set_vexpand (panneau, FALSE);
	gtk_widget_set_hexpand (panneau, FALSE);
	
	
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
	
	
	new_trame_agrandie->obj=liste;
	new_trame_agrandie->box_haut=box_trame;
	new_trame_agrandie->box_bas=box_octets;
	
	init_trame_agrandie(new_trame_agrandie);
	
	GtkWidget* label_bas=gtk_label_new ("    ");
	gtk_widget_set_vexpand (label_bas, FALSE);
	gtk_widget_set_hexpand (label_bas, FALSE);
	gtk_label_set_xalign (GTK_LABEL(label_bas),0);
	gtk_widget_set_name(label_bas,"fond_panneau_label_bas");
	
	new_trame_agrandie->label_bas=label_bas;
	gtk_box_pack_end(GTK_BOX(box_fenetre),label_bas, FALSE, FALSE, 0);
	
	remplir_arbre_agrandir_trame(box_trame,new_trame_agrandie);
	
	
	gtk_container_add(GTK_CONTAINER(frame_haut),box_trame);
	gtk_container_add(GTK_CONTAINER(frame_bas),box_octets);

	
	gtk_widget_show_all(fenetre);
	g_signal_connect_swapped (fenetre, "destroy",G_CALLBACK (gtk_widget_destroy),fenetre);
	
	
}

void change_label_bas_agrandir(trame_agrandie *tmp_trame_agrandie,char *label){
	gtk_label_set_text (GTK_LABEL(tmp_trame_agrandie->label_bas),label);
	gtk_widget_show_all(tmp_trame_agrandie->label_bas);
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
	}else{
		return ;
	}
	
	if (place>11){
	fprintf(fichier,"\t\tDestination:  %s\n",(tmp_trame->mac_dest));
	}else{
		return ;
	}
	
	if (place>13){
	fprintf(fichier,"\t\tType:  %s\n\n",(tmp_trame->ip_type));
	}else{
		return ;
	}
	
	if (place>14){
	fprintf(fichier,"\tInternet Protocol:\n");
	fprintf(fichier,"\t------------------\n");
	}else{
		return ;
	}
	
	if (place>14){
	fprintf(fichier,"\t\tVersion:  %s\n",(tmp_trame->version));
	fprintf(fichier,"\t\tHeader Length: %s\n",(tmp_trame->header_length));
	}else{
		return ;
	}
	
	
	if (place>17){
		fprintf(fichier,"\t\tTotal Length: %s\n",(tmp_trame->total_length));
	}else{
		return ;
	}
	
	
	if (place>19){
		fprintf(fichier,"\t\tIdentification: %s\n",(tmp_trame->identification));
	}else{
		return ;
	}
	
	
	if (place>20){
	
	fprintf(fichier,"\t\t\tValue: %s\n",(tmp_trame->flags_offset));
	fprintf(fichier,"\t\t\t %d... .... = Reserved bit: %s\n",tmp_trame->f0, (tmp_trame->reserved_bit));
	fprintf(fichier,"\t\t\t .%d.. .... = Don't Fragment: %s\n",tmp_trame->f1,(tmp_trame->dont_fragment));
	fprintf(fichier,"\t\t\t ..%d. .... = More Fragments: %s\n",tmp_trame->f2,(tmp_trame->more_fragment));
	}else{
		return ;
	}
	
	
	if (place>21){
	fprintf(fichier,"\t\tFragment Offset: %s\n",(tmp_trame->frag_offset));
	}else{
		return ;
	}
	
	
	if (place>22){
	fprintf(fichier,"\t\tTime to Live: %d\n",(tmp_trame->TTL));
	}else{
		return ;
	}
	
	
	if (place>23){
	fprintf(fichier,"\t\tProtocol: %s\n",(tmp_trame->protocol));
	}else{
		return ;
	}
	
	
	if (place>25){
	fprintf(fichier,"\t\tHeader Checksum: %s\n",(tmp_trame->header_checksum));
	}else{
		return ;
	}
	
	
	if (place>29){
	fprintf(fichier,"\t\tSource Address: %s\n",(tmp_trame->ip_source));
	}else{
		return ;
	}
	
	
	if (place>33){
	fprintf(fichier,"\t\tDestination Address: %s\n\n",(tmp_trame->ip_dest));
	}else{
		return ;
	}
	
	
	int i=(tmp_trame->header_length_)-20;
	
	if (place>35+i){
	fprintf(fichier,"\tTCP:\n");
	fprintf(fichier,"\t----\n");
	}else{
		return ;
	}
	
	
	if (place>35+i){
		fprintf(fichier,"\t\tSource Port: %s\n",(tmp_trame->source_port));
	}else{
		return ;
	}
	
	
	if (place>37+i){
	fprintf(fichier,"\t\tDestination Port: %s\n",(tmp_trame->destination_port));
	}else{
		return ;
	}
	
	
	if (place>41+i){
	fprintf(fichier,"\t\tSequence Number (raw): %s\n",(tmp_trame->sequence_number_raw));
	}else{
		return ;
	}
	
	
	if (place>45+i){
	fprintf(fichier,"\t\tAcknowledgment Number (raw): %s\n",(tmp_trame->acknowledgment_number_raw));
	}else{
		return ;
	}
	
	
	if (place>46+i){
	fprintf(fichier,"\t\tHeader Length: %s\n",(tmp_trame->tcp_header_length));
	}else{
		return ;
	}
	
	
	if (place>47+i){
	fprintf(fichier,"\t\tFlags");
	fprintf(fichier,"\t\t\t..%d. .... = Urgent: %s\n",tmp_trame->tcp_f0, (tmp_trame->urg));
	fprintf(fichier,"\t\t\t...%d .... = Acknowledgement: %s\n",tmp_trame->tcp_f1, (tmp_trame->ack));
	fprintf(fichier,"\t\t\t.... %d... = Push: %s\n",tmp_trame->tcp_f2, (tmp_trame->push));
	fprintf(fichier,"\t\t\t.... .%d.. = Reset: %s\n",tmp_trame->tcp_f3, (tmp_trame->reset));
	fprintf(fichier,"\t\t\t.... ..%d. = Syn: %s\n",tmp_trame->tcp_f4, (tmp_trame->syn));
	fprintf(fichier,"\t\t\t.... ...%d = Fin: %s\n",tmp_trame->tcp_f5, (tmp_trame->fin));
	}else{
		return ;
	}
	
	
	if (place>49+i){
	fprintf(fichier,"\t\tWindow: %s\n",(tmp_trame->window));
	}else{
		return ;
	}
	
	
	if (place>51+i){
	fprintf(fichier,"\t\tChecksum: %s\n",(tmp_trame->tcp_checksum));
	}else{
		return ;
	}
	
	
	if (place>53+i){
	fprintf(fichier,"\t\tUrgent Pointer: %s\n",(tmp_trame->urgent));
	}else{
		return ;
	}
	
		
	if(tmp_trame->no_option){
		fprintf(fichier,"\t\tNo Options: %s\n",(tmp_trame->no_option));
		return ;
	}
	
	
	
	
	int k = 0;
	while(tmp_trame->option_tab[k]){
		
		if(tmp_trame->option_tab[k] == 1){
			fprintf(fichier,"\t\tTCP Option - No-Operation\n");
			fprintf(fichier,"\t\t\tKind : %s\n","No Operation (NOP)");
		}
		
		if(tmp_trame->option_tab[k] == 2){
			fprintf(fichier,"\t\tTCP Option - Maximum Segment Size\n");
			fprintf(fichier,"\t\t\tKind : %s\n","Maximum Segment Size (MSS)");
			fprintf(fichier,"\t\t\tLength : %s\n","0x04 (4 bytes)");
			fprintf(fichier,"\t\t\tMSS value : %s\n",(tmp_trame->option_mss));
		}		
		
		if(tmp_trame->option_tab[k] == 3){
			fprintf(fichier,"\t\tTCP Option - Window Scale\n");
			fprintf(fichier,"\t\t\tKind : %s\n","Window Scale (Wscale)");
			fprintf(fichier,"\t\tShift count : %s\n",(tmp_trame->option_wscale));
			
		}
		
		if(tmp_trame->option_tab[k] == 4){
			fprintf(fichier,"\t\tTCP Option - Selective Acknowledgment\n");
			fprintf(fichier,"\t\t\tKind : %s\n","Selective Acknoledgment Permitted (SACK Permitted)");
			fprintf(fichier,"\t\t\tLength : %s\n","0x02 (2 bytes)");
		}
		
		if(tmp_trame->option_tab[k] == 8){
			fprintf(fichier,"\t\tTCP Option - Timestamps\n");
			fprintf(fichier,"\t\t\tKind : %s\n","Timestamps (TSOPT)");
			fprintf(fichier,"\t\t\tTimestamp value : %s\n",(tmp_trame->option_ts_val));
			fprintf(fichier,"\t\t\tTimestamp echo reply : %s\n",(tmp_trame->option_ts_ecr));
		}
		k++;
		
	}
	
	if (tmp_trame->methode==-1){
		fprintf(fichier,"\n\n");
		return;
	}
	
	char c;
	
	if (tmp_trame->methode==1){
		fprintf(fichier,"\n\tHypertext Transfer Protocol\n");
		fprintf(fichier,"\t----------------------------\n");
	}else{
		fprintf(fichier,"\n\tTCP SEGMENT DATA ( pas de méthode HTTP trouvée )\n");
		fprintf(fichier,"\t------------------------------------------------\n");
	}
	
	
	fprintf(fichier,"\t\t");
	for (int u=0;u<1518;u++){
		c=(tmp_trame->http)[u];
		fprintf(fichier,"%c",c);
		if (c=='\n'){
			fprintf(fichier,"\t\t");
		}
	
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
	
	GtkWidget *menu_label=gtk_label_new("\tId\tSource\t\t\tDestination\t\tProtocol");
	gtk_label_set_xalign (GTK_LABEL(menu_label),0);
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
			
			sprintf(label,"\n\t\tLength : %s\n","OxO2 (2 bytes)");
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
	if (tmp_cell->obj->methode==1){
		char label[80];
		sprintf(label,"\t\tméthode:  %s\n",(tmp_cell->obj->methode_));
		tmp_label=gtk_label_new(label);
		gtk_label_set_xalign (GTK_LABEL(tmp_label),0);
		gtk_box_pack_start(GTK_BOX(box_http),tmp_label, FALSE, FALSE, 0);
	}
	
	
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
	gtk_widget_set_vexpand (box_titre, FALSE);
	gtk_widget_set_hexpand (box_titre, FALSE);

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
	GtkWidget *http;
	if (tmp_cell->obj->methode==1){
		http=gtk_expander_new ("\nHypertext Transfer Protocol\n");
	}else{
		http=gtk_expander_new ("\nTCP SEGMENT DATA ( pas de méthode HTTP trouvée )\n");
	}
	
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
	if (tmp_cell->obj->methode==-1){
		return;
	}
	if ( creation_http(new_box,tmp_cell) == 0 )
		return;
	
	return;
}		







static inline void reset_bouton_actif(trame_agrandie *tmp_trame_agrandie){
	
	cell_button *tmp=tmp_trame_agrandie->box_haut_actif;
	cell_button *tmp_change_dest=NULL;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp_change_dest=tmp;
		tmp=tmp->suiv;
		tmp_change_dest->suiv=NULL;
	}
	
	
	
	tmp=tmp_trame_agrandie->box_bas_actif;
	
	while (tmp!=NULL) {
		
		gtk_widget_set_name(tmp->button,"tree_dark_mode_offset");
		
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp_change_dest=tmp;
		tmp=tmp->suiv;
		
		tmp_change_dest->suiv=NULL;
	
	}
	
	tmp_trame_agrandie->box_bas_actif=NULL;
	tmp_trame_agrandie->box_haut_actif=NULL;
}

static inline void mise_a_jour_theme_actif(trame_agrandie *tmp_trame_agrandie){
	cell_button *tmp=tmp_trame_agrandie->box_haut_actif;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode_status_1");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp=tmp->suiv;
	}
	
	
	tmp=tmp_trame_agrandie->box_bas_actif;
	
	while (tmp!=NULL) {
		gtk_widget_set_name(tmp->button,"tree_dark_mode_offset_status_1");
		gtk_widget_show(GTK_WIDGET(tmp->button));
		tmp=tmp->suiv;
	}
	
}


void action_bouton_mac_dest(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Adresse mac de destination");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->mac_dest;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[0];
	
	((tmp_trame_agrandie->tab_offset)[0])->suiv=((tmp_trame_agrandie->tab_offset)[1]);
	((tmp_trame_agrandie->tab_offset)[1])->suiv=((tmp_trame_agrandie->tab_offset)[2]);
	((tmp_trame_agrandie->tab_offset)[2])->suiv=((tmp_trame_agrandie->tab_offset)[3]);
	((tmp_trame_agrandie->tab_offset)[3])->suiv=((tmp_trame_agrandie->tab_offset)[4]);
	((tmp_trame_agrandie->tab_offset)[4])->suiv=((tmp_trame_agrandie->tab_offset)[5]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_mac_source(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Adresse mac source");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->mac_source;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[6];
	
	((tmp_trame_agrandie->tab_offset)[6])->suiv=((tmp_trame_agrandie->tab_offset)[7]);
	((tmp_trame_agrandie->tab_offset)[7])->suiv=((tmp_trame_agrandie->tab_offset)[8]);
	((tmp_trame_agrandie->tab_offset)[8])->suiv=((tmp_trame_agrandie->tab_offset)[9]);
	((tmp_trame_agrandie->tab_offset)[9])->suiv=((tmp_trame_agrandie->tab_offset)[10]);
	((tmp_trame_agrandie->tab_offset)[10])->suiv=((tmp_trame_agrandie->tab_offset)[11]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_differentiated_services(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Differentiated Services");
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=NULL;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[15];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ethernet_version(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"IP type");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ethernet),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->ip_type;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[12];
	
	((tmp_trame_agrandie->tab_offset)[12])->suiv=((tmp_trame_agrandie->tab_offset)[13]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}


void action_bouton_ip_version(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"IP Version");
	

	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->version;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[14];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_HL(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Header length");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->header_length;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[14];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_version_HL_bas(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Header Length / version IP");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->header_length;
	tmp_trame_agrandie->header_length->suiv=tmp_trame_agrandie->version;
	
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[14];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_total_length(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Total length");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->total_length;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[16];
	((tmp_trame_agrandie->tab_offset)[16])->suiv=((tmp_trame_agrandie->tab_offset)[17]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_identification(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Identification");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->identification;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[18];
	((tmp_trame_agrandie->tab_offset)[18])->suiv=((tmp_trame_agrandie->tab_offset)[19]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}



void action_bouton_ip_flags(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie," ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->flags_value;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[20];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_flags_f0(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"    ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->f0;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[20];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_flags_f1(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"    ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->f1;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[20];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_flags_f2(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"    ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->f2;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[20];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_flags_box_bas(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"IP Flags    ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags),TRUE);
	
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->flags_value;
	
	(tmp_trame_agrandie->flags_value)->suiv=(tmp_trame_agrandie->f0);
	(tmp_trame_agrandie->f0)->suiv=(tmp_trame_agrandie->f1);
	(tmp_trame_agrandie->f1)->suiv=(tmp_trame_agrandie->f2);
	
	
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[20];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
	
	
	
}

void action_bouton_ip_frag_offset(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Fragment offset   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->frag_offset;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[21];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_TTL(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Time to Live");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->TTL;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[22];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_protocol(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Protocol IP");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->protocol;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[23];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_checksum(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"IP Checksum   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->header_checksum;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[24];
	((tmp_trame_agrandie->tab_offset)[24])->suiv=((tmp_trame_agrandie->tab_offset)[25]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_source(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Adresse IP source   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->ip_source;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[26];
	((tmp_trame_agrandie->tab_offset)[26])->suiv=((tmp_trame_agrandie->tab_offset)[27]);
	((tmp_trame_agrandie->tab_offset)[27])->suiv=((tmp_trame_agrandie->tab_offset)[28]);
	((tmp_trame_agrandie->tab_offset)[28])->suiv=((tmp_trame_agrandie->tab_offset)[29]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_ip_dest(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"Adresse IP de Destination   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_ip),TRUE);
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->ip_dest;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[30];
	((tmp_trame_agrandie->tab_offset)[30])->suiv=((tmp_trame_agrandie->tab_offset)[31]);
	((tmp_trame_agrandie->tab_offset)[31])->suiv=((tmp_trame_agrandie->tab_offset)[32]);
	((tmp_trame_agrandie->tab_offset)[32])->suiv=((tmp_trame_agrandie->tab_offset)[33]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}


void action_bouton_tcp_sourceport(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Source Port   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->source_port;
	
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[34+i];
	((tmp_trame_agrandie->tab_offset)[34+i])->suiv=((tmp_trame_agrandie->tab_offset)[35+i]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_destport(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Destination Port   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->destination_port;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[36+i];
	
	((tmp_trame_agrandie->tab_offset)[36+i])->suiv=((tmp_trame_agrandie->tab_offset)[37+i]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_seq_number(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Sequence Number   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->sequence_number_raw;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[38+i];
	
	((tmp_trame_agrandie->tab_offset)[38+i])->suiv=((tmp_trame_agrandie->tab_offset)[39+i]);
	((tmp_trame_agrandie->tab_offset)[39+i])->suiv=((tmp_trame_agrandie->tab_offset)[40+i]);
	((tmp_trame_agrandie->tab_offset)[40+i])->suiv=((tmp_trame_agrandie->tab_offset)[41+i]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_ack_number(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Acknowledgment Number   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);
	
	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->acknowledgment_number_raw;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[42+i];
	
	((tmp_trame_agrandie->tab_offset)[42+i])->suiv=((tmp_trame_agrandie->tab_offset)[43+i]);
	((tmp_trame_agrandie->tab_offset)[43+i])->suiv=((tmp_trame_agrandie->tab_offset)[44+i]);
	((tmp_trame_agrandie->tab_offset)[44+i])->suiv=((tmp_trame_agrandie->tab_offset)[45+i]);
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_header_length(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Header Length   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->tcp_header_length;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[46+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_urg(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->urg;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_ack(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->ack;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_push(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->push;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_reset(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->reset;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_syn(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->syn;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_fin(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->fin;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_flags_box_bas(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP flags   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_flags_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->urg;
	(tmp_trame_agrandie->urg)->suiv=(tmp_trame_agrandie->ack);
	(tmp_trame_agrandie->ack)->suiv=(tmp_trame_agrandie->push);
	(tmp_trame_agrandie->push)->suiv=(tmp_trame_agrandie->reset);
	(tmp_trame_agrandie->reset)->suiv=(tmp_trame_agrandie->syn);
	(tmp_trame_agrandie->syn)->suiv=(tmp_trame_agrandie->fin);
	
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[47+i];
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_window(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Window  ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->window;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[48+i];
	((tmp_trame_agrandie->tab_offset)[48+i])->suiv=((tmp_trame_agrandie->tab_offset)[49+i]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_checksum(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Checksum   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->tcp_checksum;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[50+i];
	((tmp_trame_agrandie->tab_offset)[50+i])->suiv=((tmp_trame_agrandie->tab_offset)[51+i]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_urgent(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Urgent   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	
	reset_bouton_actif(tmp_trame_agrandie);
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->urgent;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[52+i];
	((tmp_trame_agrandie->tab_offset)[52+i])->suiv=((tmp_trame_agrandie->tab_offset)[53+i]);
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_tcp_options(GtkWidget *pWidget, gpointer pData){
	;
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	change_label_bas_agrandir(tmp_trame_agrandie,"TCP Options   ");
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_tcp),TRUE);	
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_options_tcp),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	int hl=tmp_trame_agrandie->obj->obj->tcp_header_length_;
	int place=tmp_trame_agrandie->obj->obj->place;
	
	reset_bouton_actif(tmp_trame_agrandie);
	
	int u=54+i;
	cell_button *tmp_button=(tmp_trame_agrandie->tab_offset)[u];
	cell_button *tmp_gtk_button=NULL;
	
	tmp_trame_agrandie->box_haut_actif=NULL;
	
	tmp_trame_agrandie->box_bas_actif=tmp_button;
	u++;
	while ((u<place)&&(u<34+hl+i)){
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[u] ) ;
		tmp_button->suiv=tmp_gtk_button;
		tmp_button=tmp_button->suiv;
		u++;
	}
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}

void action_bouton_http(GtkWidget *pWidget, gpointer pData){
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *) pData;
	if (tmp_trame_agrandie->obj->obj->methode==1){
		change_label_bas_agrandir(tmp_trame_agrandie,"Données HTTP   ");
	}else {
		change_label_bas_agrandir(tmp_trame_agrandie,"TCP SEGMENT DATA ( pas de méthode HTTP trouvée )");
	}
	gtk_expander_set_expanded (GTK_EXPANDER(tmp_trame_agrandie->expander_http),TRUE);	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	int hl=tmp_trame_agrandie->obj->obj->tcp_header_length_;
	
	
	reset_bouton_actif(tmp_trame_agrandie);
	
	tmp_trame_agrandie->box_haut_actif=tmp_trame_agrandie->http;
	tmp_trame_agrandie->box_bas_actif=(tmp_trame_agrandie->tab_offset)[34+hl+i];
	int place=tmp_trame_agrandie->obj->obj->place;
	
	cell_button *tmp_button=(tmp_trame_agrandie->tab_offset)[34+hl+i];
	cell_button *tmp_gtk_button=NULL;
	for (int u=i+35+hl;u<place;u++){
		tmp_gtk_button= (tmp_trame_agrandie->tab_offset)[u]  ;
		tmp_button->suiv=tmp_gtk_button;
		tmp_button=tmp_button->suiv;
	}
	
	
	mise_a_jour_theme_actif(tmp_trame_agrandie);
}







	
int remplir_ethernet_agrandir(GtkWidget *box_ethernet,trame_agrandie *tmp_trame_agrandie){
	cell *tmp_cell=tmp_trame_agrandie->obj;
	char label[80];
	int place=tmp_trame_agrandie->obj->obj->place;
	cell_button *tmp_button;
	GtkWidget *tmp_gtk_button;
	
	
	if (place>5){
		sprintf(label,"\t\tDestination:  %s\n",(tmp_cell->obj->mac_dest));
		
		tmp_button=creer_bouton(label,box_ethernet);
		tmp_trame_agrandie->mac_dest=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_mac_dest),tmp_trame_agrandie);
		
		for (int i=0;i<6;i++){
			tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[i] ) -> button ;
			g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_mac_dest),tmp_trame_agrandie);
		}
		
		
	}else{ return 0; }
	
	if (place>11){
		sprintf(label,"\t\tSource\t:  %s\n",(tmp_cell->obj->mac_source));
		
		tmp_button=creer_bouton(label,box_ethernet);
		tmp_trame_agrandie->mac_source=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_mac_source),tmp_trame_agrandie);
		
		for (int i=0;i<6;i++){
			tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[6+i] ) -> button ;
			g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_mac_source),tmp_trame_agrandie);
		}

	}else{ return 0; }
	
	
	
	
	if (place>13){
		sprintf(label,"\t\tType:  %s\n",(tmp_cell->obj->ip_type));
		
		tmp_button=creer_bouton(label,box_ethernet);
		tmp_trame_agrandie->ip_type=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ethernet_version),tmp_trame_agrandie);
		
		for (int i=0;i<2;i++){
			tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[12+i] ) -> button ;
			g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ethernet_version),tmp_trame_agrandie);
		}

	}else{ return 0; }
	
	
	
	return 1;
}


int remplir_ip_agrandir(GtkWidget *box_ip, trame_agrandie *tmp_trame_agrandie){
	
	char label[80];
	cell_button *tmp_button;
	GtkWidget *tmp_gtk_button;

	cell *tmp_cell=tmp_trame_agrandie->obj;
	int place=tmp_cell->obj->place;
	

	if (place>14){
		sprintf(label,"\t\tVersion:  %s\n",(tmp_cell->obj->version));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->version=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_version),tmp_trame_agrandie);
		
		sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->header_length));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->header_length=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_HL),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[14] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_version_HL_bas),tmp_trame_agrandie);
		
		

	}else{ return 0; }
	
	
	if (place>15){
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[15] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_differentiated_services),tmp_trame_agrandie);
	}else{ return 0; }
	
	if (place>17){
		sprintf(label,"\t\tTotal Length: %s\n",(tmp_cell->obj->total_length));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->total_length=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_total_length),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[16] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_total_length),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[17] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_total_length),tmp_trame_agrandie);
		
		
	}else{ return 0; }
	
	if (place>19){
		sprintf(label,"\t\tIdentification: %s\n",(tmp_cell->obj->identification));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->identification=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_identification),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[18] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_identification),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[19] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_identification),tmp_trame_agrandie);

		
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
		tmp_button=creer_bouton(label,box_flags);
		tmp_trame_agrandie->flags_value=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_flags),tmp_trame_agrandie);

		
		sprintf(label,"\t\t %d... .... = Reserved bit: %s\n",tmp_cell->obj->f0, (tmp_cell->obj->reserved_bit));
		tmp_button=creer_bouton(label,box_flags);
		tmp_trame_agrandie->f0=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_flags_f0),tmp_trame_agrandie);
		
		sprintf(label,"\t\t .%d.. .... = Don't Fragment: %s\n",tmp_cell->obj->f1,(tmp_cell->obj->dont_fragment));
		tmp_button=creer_bouton(label,box_flags);
		tmp_trame_agrandie->f1=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_flags_f1),tmp_trame_agrandie);
		
		sprintf(label,"\t\t ..%d. .... = More Fragments: %s\n",tmp_cell->obj->f2,(tmp_cell->obj->more_fragment));
		tmp_button=creer_bouton(label,box_flags);
		tmp_trame_agrandie->f2=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_flags_f2),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[20] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_flags_box_bas),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place>21){
		sprintf(label,"\t\tFragment Offset: %s\n",(tmp_cell->obj->frag_offset));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->frag_offset=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_frag_offset),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[21] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK( action_bouton_ip_frag_offset),tmp_trame_agrandie);
		
	}else{ return 0; }
	
	if (place>22){
		sprintf(label,"\t\tTime to Live: %d\n",(tmp_cell->obj->TTL));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->TTL=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_TTL),tmp_trame_agrandie);	
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[22] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_TTL),tmp_trame_agrandie);
		
	}else { return 0; }
	
	if (place>23){
		sprintf(label,"\t\tProtocol: %s\n",(tmp_cell->obj->protocol));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->protocol=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_protocol),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[23] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_protocol),tmp_trame_agrandie);
		
	}else{ return 0; }
	
	if (place > 25){
		sprintf(label,"\t\tHeader Checksum: %s\n",(tmp_cell->obj->header_checksum));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->header_checksum=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_checksum),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[24] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_checksum),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[25] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_checksum),tmp_trame_agrandie);


	}else{ return 0; }
	
	if (place > 29){
		sprintf(label,"\t\tSource Address: %s\n",(tmp_cell->obj->ip_source));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->ip_source=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[26] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);

		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[27] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[28] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);

		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[29] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_source),tmp_trame_agrandie);
		
		
	}else{ return 0; }
	
	if (place> 33){
		sprintf(label,"\t\tDestination Address: %s\n",(tmp_cell->obj->ip_dest));
		tmp_button=creer_bouton(label,box_ip);
		tmp_trame_agrandie->ip_dest=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[30] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[31] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[32] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[33] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_ip_dest),tmp_trame_agrandie);
	}else{ return 0; }
	
	return 1;
	
}


int remplir_tcp_agrandir(GtkWidget *box_tcp,trame_agrandie *tmp_trame_agrandie){
	char label[80];
	GtkWidget *tmp_label=NULL;
	
	cell_button *tmp_button;
	GtkWidget *tmp_gtk_button;
	
	cell *tmp_cell=tmp_trame_agrandie->obj;
	int place=tmp_cell->obj->place;
	int i=(tmp_cell->obj->header_length_)-20;
	

	if (place>(35+i)){
		sprintf(label,"\t\tSource Port: %s\n",(tmp_cell->obj->source_port));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->source_port=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_sourceport),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[34+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_sourceport),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[35+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_sourceport),tmp_trame_agrandie);

		
	}else{ return 0; }
	
	if (place>(37+i)){
		sprintf(label,"\t\tDestination Port: %s\n",(tmp_cell->obj->destination_port));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->destination_port=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_destport),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[36+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_destport),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[37+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_destport),tmp_trame_agrandie);
		
	}else{ return 0; }
	
	if (place>(41+i)){
		sprintf(label,"\t\tSequence Number (raw): %s\n",(tmp_cell->obj->sequence_number_raw));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->sequence_number_raw=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_seq_number),tmp_trame_agrandie);
		
			
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[38+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_seq_number),tmp_trame_agrandie);

		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[39+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_seq_number),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[40+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_seq_number),tmp_trame_agrandie);

		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[41+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_seq_number),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place>(45+i)){
		sprintf(label,"\t\tAcknowledgment Number (raw): %s\n",(tmp_cell->obj->acknowledgment_number_raw));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->acknowledgment_number_raw=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_ack_number),tmp_trame_agrandie);
		
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[42+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_ack_number),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[43+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_ack_number),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[44+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_ack_number),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[45+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_ack_number),tmp_trame_agrandie);	
	}else{ return 0; }
	
	if (place>(46+i)){
		sprintf(label,"\t\tHeader Length: %s\n",(tmp_cell->obj->tcp_header_length));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->tcp_header_length=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_header_length),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[46+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_header_length),tmp_trame_agrandie);


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
		
		
		sprintf(label,"\t\t..%d. .... = Urgent: %s\n",tmp_cell->obj->tcp_f0, (tmp_cell->obj->urg));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->urg=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_urg),tmp_trame_agrandie);
		
		sprintf(label,"\t\t...%d .... = Acknowledgement: %s\n",tmp_cell->obj->tcp_f1, (tmp_cell->obj->ack));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->ack=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_ack),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... %d... = Push: %s\n",tmp_cell->obj->tcp_f2, (tmp_cell->obj->push));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->push=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_push),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... .%d.. = Reset: %s\n",tmp_cell->obj->tcp_f3, (tmp_cell->obj->reset));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->reset=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_reset),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... ..%d. = Syn: %s\n",tmp_cell->obj->tcp_f4, (tmp_cell->obj->syn));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->syn=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_syn),tmp_trame_agrandie);
		
		sprintf(label,"\t\t.... ...%d = Fin: %s\n",tmp_cell->obj->tcp_f5, (tmp_cell->obj->fin));
		tmp_button=creer_bouton(label,box_flags_2);
		tmp_trame_agrandie->fin=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_flags_fin),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[47+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_flags_box_bas),tmp_trame_agrandie);
		
	}else{ return 0; }
	
	if (place>(49+i)){
		sprintf(label,"\t\tWindow: %s\n",(tmp_cell->obj->window));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->window=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_window),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[48+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_window),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[49+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_window),tmp_trame_agrandie);

	}else{ return 0; }
	
	if (place>(51+i)){
		sprintf(label,"\t\tChecksum: %s\n",(tmp_cell->obj->tcp_checksum));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->tcp_checksum=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_checksum),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[50+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_checksum),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[51+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_checksum),tmp_trame_agrandie);	}else{ return 0; }
	
	if (place>(53+i)){
		sprintf(label,"\t\tUrgent Pointer: %s\n",(tmp_cell->obj->urgent));
		tmp_button=creer_bouton(label,box_tcp);
		tmp_trame_agrandie->urgent=tmp_button;
		g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_tcp_urgent),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[52+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_urgent),tmp_trame_agrandie);
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[53+i] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_urgent),tmp_trame_agrandie);
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
	tmp_trame_agrandie->expander_options_tcp=tcp_options;
	
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
	
	int u=i+54;
	int hl=tmp_trame_agrandie->obj->obj->tcp_header_length_;

	while ((u<place)&&(u<34+hl+i)){
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[u] ) -> button ;
		
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_tcp_options),tmp_trame_agrandie);
		u++;
	}
	
		
	return 1;

}




int remplir_http_agrandir(GtkWidget *box_http,trame_agrandie *tmp_trame_agrandie){
	cell *tmp_cell=tmp_trame_agrandie->obj;
	
	
	if (tmp_cell->obj->methode==1){
		char label[80];
		sprintf(label,"\t\tméthode:  %s\n",(tmp_cell->obj->methode_));
		cell_button *tmp_button=creer_bouton(label,box_http);
		tmp_trame_agrandie->http=tmp_button;
	}
	
	
	cell_button *tmp_button=creer_bouton(tmp_cell->obj->http,box_http);
	tmp_trame_agrandie->http=tmp_button;
	
	int i=(tmp_trame_agrandie->obj->obj->header_length_)-20;
	int hl=tmp_cell->obj->tcp_header_length_;
	int place=tmp_cell->obj->place;
	GtkWidget *tmp_gtk_button;
	

	for (int u=i+hl+34;u<place;u++){
		
		tmp_gtk_button=( (tmp_trame_agrandie->tab_offset)[u] ) -> button ;
		g_signal_connect(G_OBJECT(tmp_gtk_button), "clicked", G_CALLBACK(action_bouton_http),tmp_trame_agrandie);
	}
	
	
	g_signal_connect(G_OBJECT(tmp_button->button), "clicked", G_CALLBACK(action_bouton_http),tmp_trame_agrandie);	
	return 1;
	
}





int creation_ethernet_agrandir(GtkWidget *new_box,trame_agrandie *tmp_trame_agrandie){
	
	GtkWidget *ethernet=gtk_expander_new ("\nEthernet II\n");
	tmp_trame_agrandie->expander_ethernet=ethernet;
	gtk_widget_set_name(GTK_WIDGET(ethernet),"expander");
	gtk_expander_set_resize_toplevel (GTK_EXPANDER(ethernet),FALSE);
	gtk_box_pack_start (GTK_BOX(new_box),ethernet,FALSE,FALSE,0);
	GtkWidget *box_ethernet=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (box_ethernet),GTK_ORIENTATION_VERTICAL);
	gtk_container_add(GTK_CONTAINER(ethernet),box_ethernet);
	return remplir_ethernet_agrandir(box_ethernet,tmp_trame_agrandie);	
}


int creation_ip_agrandir(GtkWidget *new_box,trame_agrandie *tmp_trame_agrandie){
	
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
		return remplir_ip_agrandir(box_ip, tmp_trame_agrandie);
	}else{
		return 0;
	}
	return 1;
	
}



int creation_http_agrandir(GtkWidget *new_box,trame_agrandie *tmp_trame_agrandie){
	
	int place=tmp_trame_agrandie->obj->obj->place;
	
	if (place>14){
		
		
		GtkWidget *HTTP;
		if (tmp_trame_agrandie->obj->obj->methode==1){
			HTTP=gtk_expander_new ("\nHypertext Transfer Protocol\n");
		}else{
			HTTP=gtk_expander_new ("\nTCP SEGMENT DATA ( pas de méthode HTTP trouvée )\n");
		}
		
		
		
		tmp_trame_agrandie->expander_http=HTTP;
		gtk_widget_set_name(GTK_WIDGET(HTTP),"expander");
		gtk_expander_set_resize_toplevel (GTK_EXPANDER(HTTP),FALSE);
		gtk_box_pack_start (GTK_BOX(new_box),HTTP,FALSE,FALSE,0);
		GtkWidget *box_http=gtk_box_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(HTTP),box_http);
		gtk_orientable_set_orientation (GTK_ORIENTABLE (box_http),GTK_ORIENTATION_VERTICAL);
		return remplir_http_agrandir(box_http,tmp_trame_agrandie);
	}else{
		return 0;
	}
	return 1;
	
}

int creation_tcp_agrandir(GtkWidget *new_box,trame_agrandie *tmp_trame_agrandie){
	
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
		return remplir_tcp_agrandir(box_tcp, tmp_trame_agrandie);	
		
	}else {
		return 0;
	}
	return 1;
}



void remplir_arbre_agrandir_trame(GtkWidget *new_box, gpointer pData){
	
	trame_agrandie *tmp_trame_agrandie=(trame_agrandie *)pData;
	
		
	bloc_erreur(new_box,tmp_trame_agrandie->obj);
	

	
	if ( creation_ethernet_agrandir(new_box,tmp_trame_agrandie) == 0){
		return ;
	}
	
	if ( creation_ip_agrandir(new_box,tmp_trame_agrandie) == 0 ){
		return ;
	}
	
	if ( creation_tcp_agrandir(new_box,tmp_trame_agrandie) ==0 ){
		return ;
	}
	if (tmp_trame_agrandie->obj->obj->methode==-1){
		return;
	}
	creation_http_agrandir(new_box,tmp_trame_agrandie);
}

						