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
	int cpt_lignes=0;
	do{
		c=fgetc(fichier_src);
		if (c==EOF){
			(*ligne)+=cpt_lignes;
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			cpt_lignes++;
			continue;
		}else{
			offset=tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==EOF){
			(*ligne)+=cpt_lignes;
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			cpt_lignes++;
			continue;
		}else{
			offset=offset*16+tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==' '){
			(*ligne)+=cpt_lignes;
			(*pt_offset)=offset;
			return 1;
		}
		
		if (c==EOF){
			(*ligne)+=cpt_lignes;
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			cpt_lignes++;
			continue;
		}else{
			offset=offset*16+tmp;
		}
		
		c=fgetc(fichier_src);
		if (c==EOF){
			(*ligne)+=cpt_lignes;
			return 0;
		}
		tmp=val_exa_int(c);
		if (tmp==-1){
			fin_ligne(fichier_src,ligne,c);
			cpt_lignes++;
			continue;
		}else{
			offset=offset*16+tmp;
		}
		(*ligne)+=cpt_lignes;
		(*pt_offset)=offset;
		
		return 1;
		
			
	}while(c!=EOF);
	
	return 0;

}

void ajout_liste(cell **liste,trame *elem,GtkWidget* box){
	char label[80];
	sprintf(label,"%d\t%d:%d:%d:%d\t%d:%d:%d:%d",elem->id
												,(elem->ip_source)[0],(elem->ip_source)[1],(elem->ip_source)[2],(elem->ip_source)[3]
												,(elem->ip_dest)[0],(elem->ip_dest)[1],(elem->ip_dest)[2],(elem->ip_dest)[3]
												);					
	GtkWidget* tmp_bouton=gtk_button_new_with_label(label);
	gtk_box_pack_start(GTK_BOX(box),tmp_bouton, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(tmp_bouton),"clicked",G_CALLBACK(affiche_trame_terminal),elem);
	//creation du nouveau bouton correspondant a la trame
	cell *new_cell=(cell *) malloc(sizeof(cell));
	new_cell->obj=elem;
	new_cell->bouton=tmp_bouton;
	new_cell->suiv=(*liste);
	*liste=new_cell;
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
	new_trame->mac_dest=(int *) malloc(sizeof(int)*6);
	new_trame->mac_source=(int *) malloc(sizeof(int)*6);
	
	(new_trame->mac_source)[0]=tab_ligne[6];
	(new_trame->mac_source)[1]=tab_ligne[7];
	(new_trame->mac_source)[2]=tab_ligne[8];
	(new_trame->mac_source)[3]=tab_ligne[9];
	(new_trame->mac_source)[4]=tab_ligne[10];
	(new_trame->mac_source)[5]=tab_ligne[11];
	
	(new_trame->mac_dest)[0]=tab_ligne[0];
	(new_trame->mac_dest)[1]=tab_ligne[1];
	(new_trame->mac_dest)[2]=tab_ligne[2];
	(new_trame->mac_dest)[3]=tab_ligne[3];
	(new_trame->mac_dest)[4]=tab_ligne[4];
	(new_trame->mac_dest)[5]=tab_ligne[5];
	
	new_trame->ip_type=(int *) malloc(sizeof(int)*2);
	(new_trame->ip_type)[0]=tab_ligne[12];
	(new_trame->ip_type)[1]=tab_ligne[13];
	
	new_trame->version_HL=tab_ligne[14];
	new_trame->DSF=tab_ligne[15];
	
	new_trame->total_length=(int *) malloc(sizeof(int)*2);
	(new_trame->total_length)[0]=tab_ligne[16];
	(new_trame->total_length)[1]=tab_ligne[17];
	
	new_trame->identification=(int *) malloc(sizeof(int)*2);
	(new_trame->identification)[0]=tab_ligne[18];
	(new_trame->identification)[1]=tab_ligne[19];
	
	new_trame->flags_frag_offset=(int *) malloc(sizeof(int)*2);
	(new_trame->flags_frag_offset)[0]=tab_ligne[20];
	(new_trame->flags_frag_offset)[1]=tab_ligne[21];
	
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
	
	
	return 1;
	
}



int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box){
	
	int offset=0;
	int offset_prec=0;
	int offset_prec_prec=0;
	int tmp_ligne=0;
	int verif=1;
	
	trame *new_trame=(trame *) malloc(sizeof(trame));
	
	new_trame->id=nb_trame;
	
	unsigned int *tab=(unsigned int *) calloc(1518,sizeof(unsigned int));
	new_trame->tab=tab;
	
	do{
		offset_prec_prec=offset_prec;
		charge_ligne(fichier_src,tab,&offset);
		offset_prec=offset;
		
		verif=cherche_prochaine_ligne(fichier_src,&offset,&tmp_ligne);
		
		//fin de fichier
		if (verif==0){
			lecture_trame(new_trame);
			ajout_liste(liste,new_trame,box);
			return 0;
		}
		
		if (offset-offset_prec>0){
			printf("%d octets manquants a la ligne %d\n",offset-offset_prec,*ligne);
			lecture_trame(new_trame);
			ajout_liste(liste,new_trame,box);
			return 1;
		}
	
	}while (offset!=0);
	
		
	lecture_trame(new_trame);
	ajout_liste(liste,new_trame,box);
	return 1;
}





