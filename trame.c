#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char protocol_code[5];
	char *http = "HTTP";
	
	if (elem->http[0]!=0){
		strncpy(protocol_code, http,4);
	}
	else{
		if ((elem->place)>33)
			strncpy(protocol_code, elem->protocol,3);
	}
	
	if ((elem->place)>33){
		sprintf(label,"%d\t %s\t %s\t %s",elem->id,elem->ip_source, elem->ip_dest,protocol_code);			
	}else{
		//29 source
		if ((elem->place)>29){
			printf("ok\n");
			sprintf(label,"%d\t %s\t ???:???:???:???\t ???",elem->id, elem->ip_dest);
		}else{
			sprintf(label,"%d\t ???:???:???:???\t ???:???:???:???\t ???",elem->id);
		}
		
	}
	
	GtkWidget* tmp_bouton=gtk_toggle_button_new_with_label(label);
	if (elem->nb_ligne_erreur==-1){
		gtk_widget_set_name(tmp_bouton,"button_dark_mode");
	}else {
		gtk_widget_set_name(tmp_bouton,"button_dark_mode_erreur");
	}
	cell *new_cell=(cell *) malloc(sizeof(cell));
	if (*liste==NULL){
		new_cell->bouton_actif=(cell **) malloc(sizeof(cell *));
		*(new_cell->bouton_actif)=NULL;
	}else {
		new_cell->bouton_actif=(*liste)->bouton_actif;
	}
	
	new_cell->obj=elem;
	new_cell->bouton=tmp_bouton;
	new_cell->suiv=(*liste);
	new_cell->status_bouton_ip=0;
	
	*liste=new_cell;
	
	
	
	GtkWidget *revealer = gtk_revealer_new();
	new_cell->revealer=revealer;
	gtk_revealer_set_transition_type (GTK_REVEALER(revealer),GTK_REVEALER_TRANSITION_TYPE_NONE);
	GtkWidget *new_box=gtk_box_new(FALSE,0);
	gtk_orientable_set_orientation (GTK_ORIENTABLE (new_box),GTK_ORIENTATION_VERTICAL);
	remplir_arbre(new_box, new_cell,1);
	gtk_widget_set_name(new_box,"tree_dark_mode");
	gtk_container_add(GTK_CONTAINER(revealer), new_box);
	gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
	gtk_box_pack_start(GTK_BOX(box_bas),revealer, FALSE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(box_haut),tmp_bouton, FALSE,TRUE, 0);
	gtk_widget_show_all(box_bas);
	
	gtk_widget_show (tmp_bouton);
	
	g_signal_connect(G_OBJECT(tmp_bouton), "clicked", G_CALLBACK(action_bouton_ip),new_cell);

}

int remplir_ethernet_(trame *new_trame){
	
	int place=new_trame->place;
	
	unsigned int *tab_ligne=new_trame->tab;
	
		
	if (place>5){
		
		new_trame->mac_dest=(char *) malloc(sizeof(char)*80);
		sprintf(new_trame->mac_dest, "%X%X:%X%X:%X%X:%X%X:%X%X:%X%X", tab_ligne[0]/16,tab_ligne[0]%16,tab_ligne[1]/16,tab_ligne[1]%16, tab_ligne[2]/16,tab_ligne[2]%16,tab_ligne[3]/16,tab_ligne[3]%16,tab_ligne[4]/16,tab_ligne[4]%16,tab_ligne[5]/16,tab_ligne[5]%16);	
		
	}else{ return 0; }
	
	if (place>11){
		
		new_trame->mac_source=(char *) malloc(sizeof(char)*80);	
		sprintf(new_trame->mac_source, "%X%X:%X%X:%X%X:%X%X:%X%X:%X%X", tab_ligne[6]/16,tab_ligne[6]%16,tab_ligne[7]/16,tab_ligne[7]%16, tab_ligne[8]/16,tab_ligne[8]%16,tab_ligne[9]/16,tab_ligne[9]%16,tab_ligne[10]/16,tab_ligne[10]%16,tab_ligne[11]/16,tab_ligne[11]%16);	
		
	}else{ return 0; }
	
	if (place>13){
		new_trame->ip_type=(char *) malloc(sizeof(char)*10);	
		char *ether_type = "";
		int ether_type_value = tab_ligne[12]+tab_ligne[13];
		if(ether_type_value == 8){
			ether_type = "IPv4";
		}
		if(ether_type_value == 14){
			ether_type = "ARP";
		}
		if(ether_type_value == 154){
			ether_type = "IPv6";
		}
		sprintf(new_trame->ip_type, "(0x%X%X%X%X)   %s", tab_ligne[12]/16,tab_ligne[12]%16,tab_ligne[13]/16,tab_ligne[13]%16,ether_type);
	}else{ return 0; }
	
	return 1;
}

int remplir_ip_(trame *new_trame){
	
	int place=new_trame->place;
	unsigned int *tab_ligne=new_trame->tab;
	
	
	
	if (place>14){
		
		new_trame->version=(char *)malloc(sizeof(char)*4);
		sprintf(new_trame->version, "%X", tab_ligne[14]/16),
		new_trame->header_length=(char *)malloc(sizeof(char)*10);
		int header_length=tab_ligne[14]%16*4;
		new_trame->header_length_=header_length;
		sprintf(new_trame->header_length, "(%X)\t%d bytes",tab_ligne[14]%16, header_length);
	
	}else{ return 0; }
	
	if (place>17){
		
		new_trame->total_length=(char *) malloc(sizeof(char)*10);
		int total_length = tab_ligne[16]*256 + tab_ligne[17];
		sprintf(new_trame->total_length, "%d", total_length);
		
	}else{
		return 0; }
		
	if (place>19){
		new_trame->identification=(char *) malloc(sizeof(char)*20);
		int identifier=tab_ligne[18]*256+tab_ligne[19];
		sprintf(new_trame->identification, "0x%X%X\t(%d)", tab_ligne[18], tab_ligne[19], identifier);
	}else{ return 0; }
	
	if (place>20){
		
		new_trame->flags_offset=(char *) malloc(sizeof(char)*10);
		sprintf(new_trame->flags_offset, "0x%X%X",tab_ligne[20]/16,tab_ligne[20]%16);
	
		int bit ;
		bit= valeur_n_eme_bit(tab_ligne[20], 7);
		new_trame->f0 = bit;
		new_trame->reserved_bit=(char*)malloc(sizeof(char)*20);
		if(bit){
			strcpy(new_trame->reserved_bit,"Set");
		}else{
			strcpy(new_trame->reserved_bit,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[20], 6);
		new_trame->f1 = bit;
		new_trame->dont_fragment=(char*)malloc(sizeof(char)*20);
		if(bit){
			strcpy(new_trame->dont_fragment,"Set");
		}
		else{
			strcpy(new_trame->dont_fragment,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[20], 5);
		new_trame->f2 = bit;
		new_trame->more_fragment=(char*)malloc(sizeof(char)*20);
		if(bit){
			strcpy(new_trame->more_fragment,"Set");
		}
		else{
			strcpy(new_trame->more_fragment,"Not Set");
		}
	
	}else{ return 0; }
	
	if (place>21){
		
		new_trame->frag_offset = (char *)malloc(sizeof(char)*10);
		int frag = (tab_ligne[20]%64)*256+tab_ligne[21];
		sprintf(new_trame->frag_offset, "%d",frag);
		
	}else{ return 0; }
	
	if (place>22){
		
		new_trame->TTL=tab_ligne[22];
		
	}else { return 0; }
	
	if (place>23){
		
		new_trame->protocol=(char*)malloc(sizeof(char)*10);
		if(tab_ligne[23] == 1){
			sprintf(new_trame->protocol,"ICMP\t(%d)", tab_ligne[23]/16+tab_ligne[23]%16);
		}

		if(tab_ligne[23] == 6){
			sprintf(new_trame->protocol,"TCP\t(%d)", tab_ligne[23]/16+tab_ligne[23]%16);
		}
		
		if(tab_ligne[23] == 23){
			sprintf(new_trame->protocol,"UDP\t(%d%d)", tab_ligne[23]/16,tab_ligne[23]%16);
		}
	}else{ return 0; }
	
	if (place > 25){
		
		new_trame->header_checksum=(char *) malloc(sizeof(char)*10);
		sprintf(new_trame->header_checksum,"0x%X%X%X%X", tab_ligne[24]/16,tab_ligne[24]%16, tab_ligne[25]/16, tab_ligne[25]%16);
		
	}else{ return 0; }
	
	if (place > 29){
		
		new_trame->ip_source=(char *) malloc(sizeof(char)*15);
		sprintf(new_trame->ip_source,"%d.%d.%d.%d", tab_ligne[26],tab_ligne[27],tab_ligne[28],tab_ligne[29]);
		
	}else{ return 0; }
	
	if (place> 33){
		
		new_trame->ip_dest=(char *) malloc(sizeof(char)*15);
		sprintf(new_trame->ip_dest,"%d.%d.%d.%d", tab_ligne[30],tab_ligne[31],tab_ligne[32],tab_ligne[33]);
		
	}else{ return 0; }
	
	
	return 1;
}



int remplir_tcp_2(trame *new_trame, unsigned int *tab_ligne){
	
	int place=new_trame->place;
	int i=(new_trame->header_length_)-20;
	
	if (place>(35+i)){
		new_trame->source_port =(char *)malloc(sizeof(char)*5);
		sprintf(new_trame->source_port, "%d", tab_ligne[34+i]*256 +tab_ligne[35+i]);
	}else{ return 0; }
	
	if (place>(37+i)){
		new_trame->destination_port =(char *)malloc(sizeof(char)*5);
		sprintf(new_trame->destination_port, "%d", tab_ligne[36+i]*256 + tab_ligne[37+i]);
	}else{ return 0; }
		
	if (place>(41+i)){
		new_trame->sequence_number_raw =(char *)malloc(sizeof(char)*5);
		long raw = (long)tab_ligne[38+i]*16777216 + tab_ligne[39+i]*65536 + tab_ligne[40+i]*256 + tab_ligne[41+i];
		sprintf(new_trame->sequence_number_raw, "%ld",raw);
	}else{ return 0; }
		
	if (place>(45+i)){
		new_trame->acknowledgment_number_raw =(char *)malloc(sizeof(char)*5);
		long int raw = (long)tab_ligne[42+i]*16777216 + tab_ligne[43+i]*65536 + tab_ligne[44+i]*256 + tab_ligne[45+i];
		sprintf(new_trame->acknowledgment_number_raw, "%ld",raw);
	}else{ return 0; }
		
	int header_length=tab_ligne[46+i]/16;
	if (place>(46+i)){
		new_trame->tcp_header_length =(char *)malloc(sizeof(char)*10);
		sprintf(new_trame->tcp_header_length, "(%X)\t%d bytes",tab_ligne[46+i]/10, header_length*4);
	}else{ return 0; }
	
	if (place>(47+i)){
		new_trame->urg =(char*)malloc(sizeof(char)*7);
		new_trame->ack =(char*)malloc(sizeof(char)*7);
		new_trame->push =(char*)malloc(sizeof(char)*7);
		new_trame->reset =(char*)malloc(sizeof(char)*7);
		new_trame->syn =(char*)malloc(sizeof(char)*7);
		new_trame->fin =(char*)malloc(sizeof(char)*7);
	
		int bit = valeur_n_eme_bit(tab_ligne[47+i], 5);
		new_trame->tcp_f0=bit;
		

		if(bit){
			strcpy(new_trame->urg,"Set");
		}
		else{
			strcpy(new_trame->urg,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[47+i], 4);
		new_trame->tcp_f1=bit;
	
		if(bit){
			strcpy(new_trame->ack,"Set");
		}else{
			strcpy(new_trame->ack,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[47+i], 3);
		new_trame->tcp_f2=bit;
	
		if(bit){
			strcpy(new_trame->push,"Set");
		}else{
			strcpy(new_trame->push,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[47+i], 2);
		new_trame->tcp_f3=bit;
		
		if(bit){
			strcpy(new_trame->reset,"Set");
		}else{
			strcpy(new_trame->reset,"Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[47+i], 1);
		new_trame->tcp_f4=bit;
	
		if(bit){
			strcpy(new_trame->syn,"Set");
		}else{
			strcpy(new_trame->syn,"Not Set");
		}
		
		bit = valeur_n_eme_bit(tab_ligne[47+i], 0);
		new_trame->tcp_f5=bit;
		
		if(bit){
			strcpy(new_trame->fin,"Set");
		}else{
			strcpy(new_trame->fin,"Not Set");
		}
	
	}else{ return 0; }
		
	if (place>(49+i)){
		new_trame->window =(char *)malloc(sizeof(char)*5);
		sprintf(new_trame->window, "%d",tab_ligne[48+i]*256+tab_ligne[49]);
	}else{ return 0; }
	
	if (place>(51+i)){
		new_trame->tcp_checksum =(char *)malloc(sizeof(char)*6);
		sprintf(new_trame->tcp_checksum, "0x%X%X%X%X",tab_ligne[50+i]/16,tab_ligne[50+i]%16,tab_ligne[51+i]/16,tab_ligne[51+i]%16);
	}else{ return 0; }
		
	if (place>(53+i)){
		new_trame->urgent = (char*)malloc(sizeof(char)*10);
		sprintf(new_trame->urgent, "0x%X%X%X%X", tab_ligne[52+i]/16,tab_ligne[52]%16,tab_ligne[53]/16,tab_ligne[53]%16);
		new_trame->no_option = NULL;
	}else{ return 0; }
	
	if((new_trame->header_length_) <= 5){
		new_trame->no_option = (char *)malloc(sizeof(char)*10);
		sprintf(new_trame->no_option, "0x00  (EOL)");
		return 1;
	}
	
	int taille_option = 0;
	int indice_option = 0;
	int is_option = 1;	
	if(place > 54 + i){
		int option_length = (header_length - 5)*4;
		new_trame->option_length =(char*)malloc(sizeof(char)*30);
		sprintf(new_trame->option_length, "Options:\t(%d bytes)", option_length);
		new_trame->option_tab = (int *)malloc(sizeof(int)*6); 
		
		while((place > 54 + i + taille_option) &&  is_option != 0 && indice_option < 6){
			is_option = 0;
			if(tab_ligne[54+i+taille_option]==1){ 
				is_option = 1;
				new_trame->option_tab[indice_option] = 1;
				taille_option ++;
				indice_option ++;
				continue;
			}	
			if(tab_ligne[54+i+taille_option]==2){
				is_option = 1;
				new_trame->option_tab[indice_option] = 2;
				taille_option += 2;
				if(place > 54 + i + taille_option){
					new_trame->option_mss = (char *)malloc(sizeof(char)*20);
					int option_mss_value = tab_ligne[54+i+taille_option]*256 + tab_ligne[54+i+taille_option+1];
					sprintf(new_trame->option_mss,"%d bytes", option_mss_value);
				}else{return 0;}
				taille_option += 2;
				indice_option ++;
				continue;
			}
			if(tab_ligne[54+i+taille_option]==3){
				is_option = 1;
				new_trame->option_tab[indice_option] = 3;
				taille_option += 2;
				if(place > 54 + i + taille_option){
					new_trame->option_wscale = (char *)malloc(sizeof(char)*10);
					sprintf(new_trame->option_wscale, "%d", tab_ligne[54+i+taille_option]);
				}else{return 0;}
				taille_option ++;
				indice_option ++;
				continue;
			}
			if(tab_ligne[54+i+taille_option]==4){
				is_option = 1;
				new_trame->option_tab[indice_option] = 4;
				taille_option += 2;
				indice_option++;
				continue;
			}
			if(tab_ligne[54+i+taille_option]==8){
				is_option = 1;
				new_trame->option_tab[indice_option] = 8;
				taille_option += 2;
				if(tab_ligne[54+i+taille_option+7]){
					new_trame->option_ts_val = (char *)malloc(sizeof(char)*30);
					new_trame->option_ts_ecr = (char *)malloc(sizeof(char)*30);
					long val = (long)tab_ligne[54+i+taille_option]*16777216 + tab_ligne[54+i+taille_option+1]*65536 + tab_ligne[54+i+taille_option+2]*256 + tab_ligne[54+i+taille_option+3];
					taille_option += 4;		
					long ecr = (long)tab_ligne[54+i+taille_option]*16777216 + tab_ligne[54+i+taille_option+1]*65536 + tab_ligne[54+i+taille_option+2]*256 + tab_ligne[54+i+taille_option+3];
					sprintf(new_trame->option_ts_val, "%ld", val);
					sprintf(new_trame->option_ts_ecr, "%ld", ecr);
				}else{return 0;}
				taille_option += 4;
				indice_option++;
				continue;
			}
		}
		int pos = 54+i+taille_option;
		return pos;

	}else{return 0;}
	return 1; 
}

void remplir_http_2(trame *new_trame, unsigned int *tab_ligne, int pos){
	int place = new_trame->place;
	
	int i = 0;
	while(place > pos + 4){
		if(tab_ligne[pos]==13 && tab_ligne[pos+1]==10 && tab_ligne[pos+3]==10 && tab_ligne[pos+2]==13)	
			return;
		new_trame->http[i]=tab_ligne[pos];
		i++;
		pos++;
	}
	return;	
}

static inline int lecture_trame(trame *new_trame){
	
	
	unsigned int *tab_ligne=new_trame->tab;

	new_trame->http = (char *)malloc(sizeof(char)*1518);
	for(int k=0; k<1518; k++)
	{
		new_trame->http[k]=0;
	}
	
	if (remplir_ethernet_(new_trame)==0){
		return 1;
	}
	
	
	
	if (remplir_ip_(new_trame)==0){
		return 1;
	}
	
	
	
	int pos = remplir_tcp_2(new_trame, tab_ligne);
	if (pos == 0){
		return 1;
	}
	
	remplir_http_2(new_trame, tab_ligne, pos);
	
	
	return 1;
	
	
}



int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut, GtkWidget *box_bas,char *filename){
	
	int offset=0;
	int offset_prec=0;
	int offset_prec_prec=0;
	int verif=1;
	
	trame *new_trame=(trame *) malloc(sizeof(trame));
	
	new_trame->id=nb_trame;
	new_trame->nb_octet_erreur=-1;
	new_trame->nb_ligne_erreur=-1;
	new_trame->place=1518;
	unsigned int *tab=(unsigned int *) malloc(sizeof(unsigned int)*1518);
	new_trame->tab=tab;
	
	new_trame->nom_fichier=(char *) malloc(sizeof(char)*80);
	strcpy(new_trame->nom_fichier,filename);
	
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
			new_trame->place=offset_prec;
			lecture_trame(new_trame);
			
			ajout_liste(liste,new_trame,box_haut, box_bas);
			
			
			
			return 0;
		}
		
		if (offset-offset_prec>0){
			printf("%d octets manquants a la ligne %d\n",offset-offset_prec,(*ligne)-2);
			new_trame->nb_ligne_erreur=(*ligne)-2;
			new_trame->nb_octet_erreur=offset-offset_prec;
			while ((offset!=0)&&(verif!=0)) {
				verif=cherche_prochaine_ligne(fichier_src,&offset,ligne);
			}
			
			new_trame->place=offset_prec;
			lecture_trame(new_trame);
			ajout_liste(liste,new_trame,box_haut, box_bas);
			
			
			return verif;
		}
		
	}while (offset!=0);
	
	new_trame->place=offset_prec;
	lecture_trame(new_trame);
	
	ajout_liste(liste,new_trame,box_haut, box_bas);
	
	return 1;
}





