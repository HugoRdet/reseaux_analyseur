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

//va jusqu'a la fin de la ligne actuelle d'un fichier
static inline int fin_ligne(FILE *fichier_source){
	char c;
	do {
			c=fgetc(fichier_source);
	} while ((c!='\n')&&(c!=EOF));
	
	return !(c==EOF);
}

//rentre les 16 octets d une ligne dans un tableau d entiers
static inline int charge_ligne(FILE *fichier_source,unsigned int tab[16]){
	unsigned int nbo_scan=0;
	char c1;
	char c0;
	
	unsigned int b1;
	unsigned int b0;
	
	unsigned int res;

	for (int i=0;i<16;i++){
		c1=fgetc(fichier_source);
		c0=fgetc(fichier_source);
	
		
		b1=val_exa_int(c1);
		b0=val_exa_int(c0);
		
		if ((c1==-1)||(c0==-1)){
			return nbo_scan;
		}
		
		res=16*b1+b0;
		
		tab[i]=res;
		++nbo_scan;
		c1=fgetc(fichier_source);
		if (c1!=' '){
			return nbo_scan;
		}
	
	}
	
	while ((c1!='\n')&&(c1!=EOF)) {
		c1=fgetc(fichier_source);
	}
	return nbo_scan;
}


void afficher_tableau(unsigned int tab[16]){
	for (int i=0;i<16;++i){
		printf("%.2x ",tab[i]);
	}
	printf("\n");
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




//une trame commence a la ligne 0;
//renvoie la prochaine n ieme ligne d une trame.
//les lignes avec un offset errone sont ignorees
static inline int cherche_prochaine_ligne(FILE *fichier_src,unsigned int n0_ligne,unsigned int *tab_ligne,unsigned int nb_trame){
	int offset=0;
	char c;
	//il y a 16 octets par ligne.
	unsigned int nombre_octets=n0_ligne*16;
	
	int off3=(int) (nombre_octets/4096.0);
	nombre_octets-=off3*4096;
	
	int off2=(int) (nombre_octets/256.0);
	
	nombre_octets-=off2*256;
	int off1=(int)(nombre_octets/16.0);
	
	nombre_octets-=off1*16;
	int off0=nombre_octets;
	
	
	
	do{
		
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		offset=val_exa_int( c );
		if (offset!=off3){
			if (fin_ligne(fichier_src)==0){
				return 0;
			}
			continue;
		}
		
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		offset=val_exa_int( c );
		if (offset!=off2){
			if (fin_ligne(fichier_src)==0){
				return 0;
			}
			continue;
		}
		
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		offset=val_exa_int( c );
		if (offset!=off1){
			if (fin_ligne(fichier_src)==0){
				return 0;
			}
			continue;
		}
		
		
		c=fgetc(fichier_src);
		if (c==EOF){
			return 0;
		}
		offset=val_exa_int( c );
		if (offset!=off0){
			if (fin_ligne(fichier_src)==0){
				return 0;
			}
			continue;
		}
		
		fgetc(fichier_src);
		fgetc(fichier_src);
		fgetc(fichier_src);
		int nb_octets=charge_ligne(fichier_src,tab_ligne);
			
		if (nb_octets!=16){
			printf("%d octets manquants a la ligne 2 de la trame %d\n",16-nb_octets,nb_trame);
			return nb_octets;
		}
		
		return 1;
		
		}while (c!=EOF);
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




int lecture_trame(FILE *fichier_source,int nb_trame,cell **liste,GtkWidget *box){
	/*variables*/
	unsigned int tab_ligne[16];
	int verif=0;
	
		
	
	verif=cherche_prochaine_ligne(fichier_source,0,tab_ligne,nb_trame);
	if (!verif){
		return 0;
	}
	
	trame *new_trame=(trame *) malloc(sizeof(trame));
	new_trame->id=nb_trame;
	
	
	
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
	//ligne 2;
	//chargement de la seconde ligne.
	verif=cherche_prochaine_ligne(fichier_source,1,tab_ligne,nb_trame);
	
	if (!verif){
		new_trame->nb_ligne_erreur=1;
		return 0;
	}
	
	new_trame->total_length=(int *) malloc(sizeof(int)*2);
	(new_trame->total_length)[0]=tab_ligne[0];
	(new_trame->total_length)[1]=tab_ligne[1];
	
	new_trame->identification=(int *) malloc(sizeof(int)*2);
	(new_trame->identification)[0]=tab_ligne[2];
	(new_trame->identification)[1]=tab_ligne[3];
	
	new_trame->flags_frag_offset=(int *) malloc(sizeof(int)*2);
	(new_trame->flags_frag_offset)[0]=tab_ligne[4];
	(new_trame->flags_frag_offset)[1]=tab_ligne[5];
	
	new_trame->TTL=tab_ligne[6];
	new_trame->protocol=tab_ligne[7];
	
	new_trame->header_checksum=(int *) malloc(sizeof(int)*2);
	(new_trame->header_checksum)[0]=tab_ligne[8];
	(new_trame->header_checksum)[1]=tab_ligne[9];
	
	new_trame->ip_source=(int *) malloc(sizeof(int)*4);
	(new_trame->ip_source)[0]=tab_ligne[10];
	(new_trame->ip_source)[1]=tab_ligne[11];
	(new_trame->ip_source)[2]=tab_ligne[12];
	(new_trame->ip_source)[3]=tab_ligne[13];
	
	new_trame->ip_dest=(int *) malloc(sizeof(int)*4);
	(new_trame->ip_dest)[0]=tab_ligne[14];
	(new_trame->ip_dest)[1]=tab_ligne[15];
	
	verif=cherche_prochaine_ligne(fichier_source,2,tab_ligne,nb_trame);
	if (!verif){
		new_trame->nb_ligne_erreur=2;
		return 0;
	}
	
	(new_trame->ip_dest)[2]=tab_ligne[0];
	(new_trame->ip_dest)[3]=tab_ligne[1];
	
	ajout_liste(liste,new_trame,box);
	return 1;
	
}

