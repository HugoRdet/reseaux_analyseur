#include <stdio.h>
#include <stdlib.h>
#include "trame.h"
#include "interface.h"

#define val_byte_1(x) ( (int) (x/16.0) )
#define val_byte_0(x) ( x%16 )


//renvoie la valeur décimale d'un caractère 
int val_exa_int(char c){
	
	unsigned int val=(int) c;
		
	switch (val) 
	{
	case 48:
		return 0;
		break;	
	case 49:
		return 1;
		break;
	case 50:
		return 2;
		break;
	case 51:
		return 3;
		break;
	case 52:
		return 4;
		break;
	case 53:
		return 5;
		break;
	case 54:
		return 6;
		break;
	case 55:
		return 7;
		break;
	case 56:
		return 8;
		break;
	case 57:
		return 9;
		break;
	case 65:
		return 10;
		break;
	case 66:
		return 11;
		break;
	case 67:
		return 12;
		break;
	case 68:
		return 13;
		break;
	case 69:
		return 14;
		break;
	case 70:
		return 15;
		break;
	case 97:
		return 10;
		break;
	case 98:
		return 11;
		break;
	case 99:
		return 12;
		break;
	case 100:
		return 13;
		break;
	case 101:
		return 14;
		break;
	case 102:
		return 15;
		break;
	default :
		return -1;
	}
	
}

void afficher_ligne(FILE *fichier){
	char c=' ';
	while ((c!=EOF)&&(c!='\n')) {
		c=fgetc(fichier);
		printf("%c",c);
	}
	printf("\n");
}

//va jusqu'a la fin de la ligne actuelle d'un fichier
static inline int fin_ligne(FILE *fichier_source,int *ligne,char c){
	while ((c!='\n')&&(c!=EOF)){
			c=fgetc(fichier_source);
	};
	(*ligne)++;
	return !(c==EOF);
}


//renvoie x a la puissance n
static inline double pow(double x,int n){
	double acc=1.0;
	for (int i=0;i<n;i++){
		acc*=x;
	}
	return acc;
} 


unsigned int valeur_n_eme_bit(unsigned int x, int n){
	unsigned int reste;
	unsigned int quotient;
	double puissance2;
	for (int i=7;i>=n;i--){
		puissance2=pow(2,i);
		quotient=(int) (x/puissance2);
		reste=x-(quotient*puissance2);
		x=reste;
	}
	return quotient;
}

static inline void charge_ligne(FILE *fichier_src,unsigned int *tab,int *place){
	char c;
	int i=*place;
	int val_exa;
	do{	
		//saut de l espace;
		c=fgetc(fichier_src);
		if (c=='\n'){
			*place=i;
			return;
		}
		
		c=fgetc(fichier_src);
		val_exa=val_exa_int(c);
		
		if (c=='\n'){
			*place=i;
			return;
		}
		
		
		if (val_exa==-1){
			*place=i;
			fin_ligne(fichier_src,place,c);
			return;
		}
		
		c=fgetc(fichier_src);
		val_exa=(val_exa*16)+val_exa_int(c);
		if (c=='\n'){
			*place=i;
			return;
		}
		tab[i]=val_exa;
		i++;
	}while ((c!='\n')&&(c!=EOF));
	(*place)=i;
}


//cherche le prochain offset
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne){
	
	char c;
	int offset;
	int tmp;
	
	do{
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			continue;
		}else{
			offset=tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			continue;
		}else{
			offset=offset*16+tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==' '){
			(*pt_offset)=offset;
			return 1;
		}
		
		if (c==EOF){
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			continue;
		}else{
			offset=offset*16+tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			continue;
		}else{
			offset=offset*16+tmp;
		}
		(*pt_offset)=offset;
		
		return 1;
		
			
	}while(c!=EOF);
	
	return 0;

}


void ajout_liste(cell **liste,trame *elem,GtkWidget* box_haut, GtkWidget* box_bas){
	char label[80];
	sprintf(label,"%d\t%d:%d:%d:%d\t%d:%d:%d:%d",elem->id,(elem->ip_source)[0],(elem->ip_source)[1],(elem->ip_source)[2],(elem->ip_source)[3],(elem->ip_dest)[0],(elem->ip_dest)[1],(elem->ip_dest)[2],(elem->ip_dest)[3]);					
	
	
	GtkWidget* tmp_bouton=gtk_toggle_button_new_with_label(label);
	if (elem->nb_ligne_erreur==-1){
		gtk_widget_set_name(tmp_bouton,"button_dark_mode");
	}else {
		gtk_widget_set_name(tmp_bouton,"button_dark_mode_erreur");
	}
	cell *new_cell=(cell *) malloc(sizeof(cell));
	
	new_cell->obj=elem;
	new_cell->arbre=NULL;
	new_cell->bouton=tmp_bouton;
	new_cell->suiv=(*liste);
	new_cell->status_bouton_ip=0;
	*liste=new_cell;
	
	GtkWidget *revealer = gtk_revealer_new();
	new_cell->revealer=revealer;
	GtkWidget *new_box=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (new_box),GTK_ORIENTATION_VERTICAL);
	
	gtk_revealer_set_transition_type (GTK_REVEALER(revealer),GTK_REVEALER_TRANSITION_TYPE_NONE);
	remplir_arbre(new_box, new_cell);
	gtk_widget_set_name(new_box,"tree_dark_mode");
	gtk_container_add(GTK_CONTAINER(revealer), new_box);
	gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
	gtk_box_pack_start(GTK_BOX(box_haut),tmp_bouton, FALSE,TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box_bas),revealer, FALSE, TRUE, 0);
	gtk_widget_show (tmp_bouton);
	gtk_widget_show_all(box_bas);
	g_signal_connect(G_OBJECT(tmp_bouton), "clicked", G_CALLBACK(action_bouton_ip),new_cell);

}

//cette fonction va charger le contenu entier d une trame dans tab_ligne
//tab_ligne doit faire 1518octets taille maximale d une trame ethernet

//on suppose que quand on lance cette fonction fichier_src pointe deja vers le premier octet (on viens de trouver le premier offset;

void affiche_tab(unsigned int *tab){
	for (int i=0;i<16;i++){
		printf("%d ",tab[i]);
	}
	printf("\n");
}

static inline int lecture_trame(trame *new_trame){
	
	unsigned int *tab_ligne=new_trame->tab;
	new_trame->mac_dest=(char *) malloc(sizeof(char)*80);
	new_trame->mac_source=(char *) malloc(sizeof(char)*80);	
	
	sprintf(new_trame->mac_source, "%X:%X:%X:%X%X:%X", tab_ligne[6],tab_ligne[7],tab_ligne[8],tab_ligne[9],tab_ligne[10],tab_ligne[11]);
	sprintf(new_trame->mac_dest, "%X:%X:%X:%X:%X:%X", tab_ligne[0],tab_ligne[1],tab_ligne[2],tab_ligne[3],tab_ligne[4],tab_ligne[5]);

	
	new_trame->ip_type=(char *) malloc(sizeof(char)*10);	
	sprintf(new_trame->ip_type, "(0x0%X0%X)", tab_ligne[12],tab_ligne[13]);
	

	new_trame->version=(char *)malloc(sizeof(char)*4);
	sprintf(new_trame->version, "%X", tab_ligne[14]/16),
	new_trame->header_length=(char *)malloc(sizeof(char)*10);
	int header_length=tab_ligne[14]%16*4;
	sprintf(new_trame->header_length, "(%X) - %d bytes",tab_ligne[14]%16, header_length);
	
	
	new_trame->total_length=(char *) malloc(sizeof(char)*10);
	int total_length = tab_ligne[16]*256 + tab_ligne[17];
	sprintf(new_trame->total_length, "%d", total_length);

	new_trame->identification=(char *) malloc(sizeof(char)*20);
	int identifier=tab_ligne[18]*256+tab_ligne[19];
	sprintf(new_trame->identification, "0x%X%X - (%d)", tab_ligne[18], tab_ligne[19], identifier);
	
	new_trame->flags_offset=(char *) malloc(sizeof(char)*10);
	sprintf(new_trame->flags_offset, "0x%X%X",tab_ligne[20]/16,tab_ligne[20]%16);
	int bit; 

	bit = valeur_n_eme_bit(tab_ligne[20], 7);
	new_trame->reserved_bit=(char*)malloc(sizeof(char)*20);
	if(bit){new_trame->reserved_bit = "Set";}
	else{new_trame->reserved_bit = "Not Set";}
	
	bit = valeur_n_eme_bit(tab_ligne[20], 6);
	new_trame->dont_fragment=(char*)malloc(sizeof(char)*20);
	if(bit){new_trame->dont_fragment = "Set";}
	else{new_trame->dont_fragment = "Not Set";}

	bit = valeur_n_eme_bit(tab_ligne[20], 5);
	new_trame->more_fragment=(char*)malloc(sizeof(char)*20);
	if(bit){new_trame->more_fragment = "Set";}
	else{new_trame->more_fragment = "Not Set";}
		
	new_trame->TTL=tab_ligne[22];
	new_trame->protocol=tab_ligne[23];
	
	new_trame->header_checksum=(int *) malloc(sizeof(int)*2);
	(new_trame->header_checksum)[0]=tab_ligne[24];
	(new_trame->header_checksum)[1]=tab_ligne[25];
	
	new_trame->ip_source=(int *) malloc(sizeof(int)*4);
	(new_trame->ip_source)[0]=tab_ligne[26];
	(new_trame->ip_source)[1]=tab_ligne[27];
	(new_trame->ip_source)[2]=tab_ligne[28];
	(new_trame->ip_source)[3]=tab_ligne[29];
	
	new_trame->ip_dest=(int *) malloc(sizeof(int)*4);
	(new_trame->ip_dest)[0]=tab_ligne[30];
	(new_trame->ip_dest)[1]=tab_ligne[31];
	
	
	(new_trame->ip_dest)[2]=tab_ligne[32];
	(new_trame->ip_dest)[3]=tab_ligne[33];
	
	int i=header_length-20;
	
	
	
	
	return 1;
	
}



int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut, GtkWidget *box_bas){
	
	int offset=0;
	int offset_prec=0;
	int offset_prec_prec=0;
	int verif=1;
	
	trame *new_trame=(trame *) malloc(sizeof(trame));
	
	new_trame->id=nb_trame;
	new_trame->nb_octet_erreur=-1;
	new_trame->nb_ligne_erreur=-1;
	unsigned int *tab=(unsigned int *) calloc(1518,sizeof(unsigned int));
	new_trame->tab=tab;
	
	do{
		offset_prec_prec=offset_prec;
		charge_ligne(fichier_src,tab,&offset);
		offset_prec=offset;
		
		do{
			offset=offset_prec;
			verif=cherche_prochaine_ligne(fichier_src,&offset,ligne);
			(*ligne)++;
		}while(((offset!=0)&&(offset<=offset_prec_prec))&&(verif!=0));
		
		
		//fin de fichier
		if (verif==0){
			lecture_trame(new_trame);
			ajout_liste(liste,new_trame,box_haut, box_bas);
			new_trame->place=offset;
			return 0;
		}
		
		if (offset-offset_prec>0){
			printf("%d octets manquants a la ligne %d\n",offset-offset_prec,(*ligne)-2);
			
			while ((offset!=0)&&(verif!=0)) {
					verif=cherche_prochaine_ligne(fichier_src,&offset,ligne);
			}
			
			
			
			new_trame->nb_octet_erreur=offset_prec;
			new_trame->nb_ligne_erreur=(*ligne)-2;
			lecture_trame(new_trame);
			ajout_liste(liste,new_trame,box_haut, box_bas);
			new_trame->place=offset;
			return verif;
		}
	
	}while (offset!=0);
	
	lecture_trame(new_trame);
	ajout_liste(liste,new_trame,box_haut, box_bas);
	new_trame->place=offset;
	return 1;
}





