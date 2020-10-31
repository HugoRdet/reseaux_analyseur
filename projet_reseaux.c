#include <stdio.h>
#include <stdlib.h>

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
static inline double pow(double x,unsigned int n){
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
	unsigned int offset=0;

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
		
		
		
		offset=val_exa_int( fgetc(fichier_src) );
		if (offset!=off3){
			if ((offset==EOF)||(fin_ligne(fichier_src)==0)){
				return 0;
			}
			continue;
		}
		offset=val_exa_int( fgetc(fichier_src) );
		if (offset!=off2){
			if ((offset==EOF)||(fin_ligne(fichier_src)==0)){
				return 0;
			}
			continue;
		}
		offset=val_exa_int( fgetc(fichier_src) );
		if (offset!=off1){
			if ((offset==EOF)||(fin_ligne(fichier_src)==0)){
				return 0;
			}
			continue;
		}
		offset=val_exa_int( fgetc(fichier_src) );
		if (offset!=off0){
			if ((offset==EOF)||(fin_ligne(fichier_src)==0)){
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
			return 0;
		}
		
		return 1;
		
		}while (offset!=EOF);
	return 0;
}
int lecture_trame(FILE *fichier_source,FILE *fichier_dest,int nb_trame){
	/*variables*/
	unsigned int tab_ligne[16];
	int verif=0;
	
		
	
	verif=cherche_prochaine_ligne(fichier_source,0,tab_ligne,nb_trame);
	if (!verif){
		return 0;
	}
	
	fprintf(fichier_dest," TRAME :%d\n",nb_trame);
	fprintf(fichier_dest,"---------\n");
	fprintf(fichier_dest,"\n");
		
	
	fprintf(fichier_dest,"\tcouche ethernet:\n");
	fprintf(fichier_dest,"\t----------------\n");
	fprintf(fichier_dest,"\t\tsource :");
	fprintf(fichier_dest," %.2x:",tab_ligne[6]);
	fprintf(fichier_dest," %.2x:",tab_ligne[7]);
	fprintf(fichier_dest," %.2x:",tab_ligne[8]);
	fprintf(fichier_dest," %.2x:",tab_ligne[9]);
	fprintf(fichier_dest," %.2x:",tab_ligne[10]);
	fprintf(fichier_dest," %.2x\n",tab_ligne[11]);
	
	fprintf(fichier_dest,"\t\tdestination :");
	fprintf(fichier_dest," %.2x:",tab_ligne[0]);
	fprintf(fichier_dest," %.2x:",tab_ligne[1]);
	fprintf(fichier_dest," %.2x:",tab_ligne[2]);
	fprintf(fichier_dest," %.2x:",tab_ligne[3]);
	fprintf(fichier_dest," %.2x:",tab_ligne[4]);
	fprintf(fichier_dest," %.2x\n",tab_ligne[5]);
	
	fprintf(fichier_dest,"\t\ttype: 0x%.2x%.2x",tab_ligne[12],tab_ligne[13]);
	if ((tab_ligne[12]==8)&&(tab_ligne[13]==0)){
		fprintf(fichier_dest," (IPv4)\n");
	}
	fprintf(fichier_dest,"\n");
	//fin de la couche ethernet , j'ai pas fait une fonction 
	//special couche ethernet car on est pas arrivés à la fin de la ligne.		
	fprintf(fichier_dest,"\tcouche IP :\n");
	fprintf(fichier_dest,"\t-----------\n");
	fprintf(fichier_dest,"\t\tVersion : %d\n",val_byte_1(tab_ligne[14]));
	int IHL=val_byte_0(tab_ligne[14]);
	//le IHL est en quad words ici.
	unsigned int len_IHL=IHL*4;
	fprintf(fichier_dest,"\t\tLongueur header (IHL) : %d (%d)\n",len_IHL,IHL);
	fprintf(fichier_dest,"\t\tDifferentiated Services Codepoint: %d\n",val_byte_1(tab_ligne[15]));
	fprintf(fichier_dest,"\t\tExplicit Congestion Notification: %d\n",val_byte_0(tab_ligne[15]));
	//ligne 2;
	
	//chargement de la seconde ligne.
	verif=cherche_prochaine_ligne(fichier_source,1,tab_ligne,nb_trame);
	if (!verif){
		printf("\ttrame %d incomplète \n",nb_trame);
		return 0;
	}
	
	unsigned int longueur_totale=tab_ligne[0]*256+tab_ligne[1];
	fprintf(fichier_dest,"\t\tlongueur totale: %d\n",longueur_totale );
	fprintf(fichier_dest,"\t\tIdentification; 0x%.2x%.2x (%d)\n",tab_ligne[2],tab_ligne[3],tab_ligne[2]*256+tab_ligne[3]);
	fprintf(fichier_dest,"\t\tflags :0x%.2x%.2x\n",tab_ligne[4],tab_ligne[5]);
		
	fprintf(fichier_dest,"\t\t\t(R)  Reserved bit  : \t%d\n",valeur_n_eme_bit(tab_ligne[4],7));
	fprintf(fichier_dest,"\t\t\t(DF) Don't Fragment: \t%d\n",valeur_n_eme_bit(tab_ligne[4],6));
	fprintf(fichier_dest,"\t\t\t(MF) More Fragments: \t%d\n",valeur_n_eme_bit(tab_ligne[4],5));
	//j'ai préféré ne pas faire de boucle ici car le nombre d'iterations est fixe
	int Frag_offset=valeur_n_eme_bit(tab_ligne[4],4)*((int) pow(16,12))+
	valeur_n_eme_bit(tab_ligne[4],3)*((int) pow(16,11))+
	valeur_n_eme_bit(tab_ligne[4],2)*((int) pow(16,10))+
	valeur_n_eme_bit(tab_ligne[4],1)*((int) pow(16,9))+
	valeur_n_eme_bit(tab_ligne[4],0)*((int) pow(16,8))+tab_ligne[5];
	fprintf(fichier_dest,"\t\tFragment offset:\t%d\n",Frag_offset);
	
	fprintf(fichier_dest,"\t\tTime To Live:\t%d\n",tab_ligne[6]);
	fprintf(fichier_dest,"\t\tProtocol:\t%d ",tab_ligne[7]);
	fprintf(fichier_dest,"\t\tHeader Checksum :0x%.2x%.2x\n",tab_ligne[8],tab_ligne[9]);
	fprintf(fichier_dest,"\t\tsource :");
		fprintf(fichier_dest," %.d.",tab_ligne[10]);
		fprintf(fichier_dest," %.d.",tab_ligne[11]);
		fprintf(fichier_dest," %.d.",tab_ligne[12]);
		fprintf(fichier_dest," %.d\n",tab_ligne[13]);
	
	
	fprintf(fichier_dest,"\t\tsource :");
	fprintf(fichier_dest," %.d:",tab_ligne[14]);
	fprintf(fichier_dest," %.d:",tab_ligne[15]);
	
	verif=cherche_prochaine_ligne(fichier_source,2,tab_ligne,nb_trame);
	if (!verif){
		printf("trame %d incomplète \n",nb_trame);
		return 0;
	}	
	fprintf(fichier_dest," %.d:",tab_ligne[0]);
	fprintf(fichier_dest," %.d\n",tab_ligne[1]);
	//fin de la couche IP
		
	//LES OPTIONS NE SONT PAS IMPLÉMENTÉES
	fprintf(fichier_dest,"\n\tCOUCHE TCP :\n");
	fprintf(fichier_dest,"\t------------\n");
	fprintf(fichier_dest,"\t\tSource Port: %d\n",(tab_ligne[2])*256+tab_ligne[3]);
	fprintf(fichier_dest,"\t\tDestination Port: %d\n",(tab_ligne[4])*256+tab_ligne[5]);
	
	
	long int sequence_number=(tab_ligne[6])*((long int)pow(16,6))
						+(tab_ligne[7])*((int)pow(16,4))
						+(tab_ligne[8])*((int)pow(16,2))
						+(tab_ligne[9]);	
	fprintf(fichier_dest,"\t\tSequence number (raw) :%li \n",sequence_number);
	long int acknowledgment_number=(tab_ligne[10])*((long int)pow(16,6))
							+(tab_ligne[11])*((int)pow(16,4))
							+(tab_ligne[12])*((int)pow(16,2))
							+(tab_ligne[13]);
	fprintf(fichier_dest,"\t\tacknowledgement number (raw) :%li \n",acknowledgment_number);
	return 1;
	
}
int main(int argc, char *argv[]) {
	
	FILE *fichier_source=fopen("test_res.txt","r");
	FILE *fichier_dest=fopen("fichier_dstination.txt","w");
	
	int res=1;
	int cpt=0;
	while (res!=0) {
	
	res=lecture_trame(fichier_source,fichier_dest,cpt);
	cpt+=res;
	
	}
	
	printf("trames scanees : %d\n",cpt);
	fclose(fichier_dest);
	return 0;
}
