# -*- coding: utf-8 -*-
"""
Created on Thu Nov 12 21:31:49 2020

"""
def DecToHex(dec):
    x = (dec % 16)
    digits = "0123456789ABCDEF"
    rest = dec // 16
    if (rest == 0):
        return digits[x]
    return DecToHex(rest) + digits[x]


def HexToDec(hex):
    return int((str(hex)), 16)



def list_to_str(liste):
    """ Permet de transformer un liste de trame
        en sa forme "chaine de caractere" nous permettant 
        de l'inserer dans un fichier text"""
    
    res = "[("
    a = ""
    b = []
    
    cpt1 = 1
    cpt2 = 1
    
    for e in liste:
        if cpt1 < len(liste):
            a, b = e
            res += str(a) + ",\t[('"
            for i in b:
                if cpt2 < len(b):
                     x, y = i
                     res += str(x) + "', '" + str(y) + "'),\n\t('"
                     cpt2 += 1
                else:
                    x, y = i
                    res += str(x) + "', '" + str(y) + "')]),\n\n("
                    cpt2 = 1
            cpt1 += 1
        else:
             a, b = e
             res += str(a) + ",\t[('"
             for i in b:
                 if cpt2 < len(b):
                     x, y = i
                     res += str(x) + "', '" + str(y) + "'),\n\t('"
                     cpt2 += 1
                 else:
                     x, y = i
                     res += str(x) + "', '" + str(y) + "')])]"
                     cpt2 = 1
    return res

def list_to_str2(liste):
    res = ""
    
    for e in liste:
        res += str(e) + "\n"
    return res
    
      

def offset_valides(tupl1, tupl2):
    a, b = tupl1
    x, y = tupl2  
    try:
        return HexToDec(x) >= (int((len(b) + 1) / 3)) + HexToDec(a)
    except ValueError:
        return 1 == 2
    
def octetManquant(tupl1, tupl2)  :
    a, b = tupl1
    x, y = tupl2 

    if offset_valides(tupl1, tupl2):
        if HexToDec(x) > (int((len(b) + 1) / 3)) + HexToDec(a):
            
            return HexToDec(x) - (int((len(b) + 1) / 3)) + HexToDec(a) 
    return 0  
        
    
def verif(text): 
    """Verifie si l'octet est bien un nombre en hexadecimal"""
    i = 0
    j = 0
    while i < len(text):
          if j == 0 or j == 1:
                if not((ord(tab[i]) <= ord('f') and ord(tab[i]) >= ord('a')) 
                       or (ord(tab[i]) >= ord('0') and ord(tab[i]) <= ord('9'))):
                      return 0
          if j == 2:
                if tab[i] != ' ':
                      return 0
          j += 1
          i += 1
          if j == 3:
                j = 0
    return 1
  
    
  

def trouve_deb_tram(liste):
    """ list(str) -> int
    
    trouve la position d'un offset 0000 permettant de savoir où 
    pourrait commencer la trame"""
    cpt = -1 
    for e in liste:
        if (e[0] == "0") and (e[1] == "0") and (e[2] == "0") and (e[3] == "0"):
            return cpt
        cpt += 1
        
        
    
def trouve_list_tram(liste):
    """liste(str) -> list(tuple(str, list(tuple(str, str))
    
    Separe en 'sous-trame' chaque partie de la liste
    séparé par un offset 0000"""
    
    list_trame = []
    
    trame = (0, [])
    numero = 1
    list_offtrame = []
    
    offtrame = ("","" )
    offset = ""
    ligne = ""
    caractere = 0
    
    cpt = 0 
    cpt2 = 1
    nligne = 0
    
    lignex = ("0000", "0")
    lignexp1 = ("", "")
    
    
    prems_espace = True
    
    
    for e in liste:
        if len(e) >= 4:
            if (e[caractere] == "0") and (e[caractere + 1] == "0") and (e[caractere + 2] == "0") and (e[caractere + 3] == "0"):
                if (len(e) > 5):
                    if cpt == cpt2:
                        trame = (numero, list_offtrame)
                        list_trame.append(trame)
                        numero += 1
                        list_offtrame = []
                        cpt2 += 1
                    cpt += 1
                    lignex = ("0000", "0")
                    
        for i in e:
            if (i == " ") and (prems_espace == True):
                prems_espace = False
            elif (i != " ") and (prems_espace == True):
                offset += i
            else:
                ligne += i
        offtrame = (offset, ligne)
        
        lignexp1 = offtrame
        if (offset_valides(lignex, lignexp1) and (octetManquant(lignex, lignexp1) == 0)):
            list_offtrame.append(offtrame) 
            lignex = lignexp1
        elif (offset_valides(lignex, lignexp1) and (octetManquant(lignex, lignexp1) != 0)):
            print("Trame n°" + str(numero) + ", ligne n°" + str(nligne - 1) +": " + str(octetManquant(lignex, lignexp1)) + " octets manquant \n" )
            lignex = lignexp1
        nligne += 1
        offset = ""
        ligne = ""
        prems_espace = True
        lignexp1 = ("", "")

    trame = (numero, list_offtrame)
    list_trame.append(trame)
    numero += 1
    list_offtrame = []
    cpt2 += 1
        
    return list_trame

def trameXpur(liste):
    """ list(tuple(str, list(tuple(str, str)))) -> liste(str)
        retourne la liste de ligne d'une trame demandé sans ses 
        offset"""
    cpt = 0
    res = []
    
    for t in liste:
        cpt += 1
    print(str(cpt) + " trame(s) a(ont) été capturé(es)")
    print("laquelle choisissez vous?")
    numero = input()
    numero = int(numero)
    
    for t in liste:
        a, b = t
        if a == numero:
            for l in b:
                x, y = l
                print(y)
                res.append(y)
            break
        
    return res

            
            

                    
                
""" MAIN """

liste_lignes = []
i = 0 
cpt = 0
ligne = ""


f = open("test_res.txt" , "r")
tab = f.read()
while i < (len(tab)):
    if tab[i] != "\n":
        ligne += tab[i]
    else:
        liste_lignes.append(ligne)
        ligne = ""
        cpt += 1  
    i += 1
liste_lignes.append(ligne)
ligne = ""
cpt += 1  
f.close()

liste_trame = trouve_list_tram(liste_lignes)
f2 = open("Fvide.txt", "x")
trame_str = list_to_str(liste_trame)
f2.write(trame_str)
f2.close()

trameX = trameXpur(liste_trame)
f3 = open("Trame_pur.txt", "x")
trameStr = list_to_str2(trameX)
f3.write(trameStr)
f3.close()






"""tmp = ""
txtpars = []
j = 0
while i < len(tab):
      if j < 2: 
            tmp += tab[i]
      i += 1
      j += 1
      if j == 3:
            txtpars.append(tmp)
            tmp = ""
            j = 0
print(txtpars) # de la D
liste_des_trames = trouve_list_tram(txtpars)
print(liste_des_trames) #regler ca imperativement

f = open("trame.txt", "r")
ligne_initiale = 0
ligne = f.readline() 
print(ligne)

liste_des_trames = trouve_list_tram(lignes)
print(liste_des_trames)

  
ligne_initiale = trouve_deb_tram(lignes)
#print(ligne_initiale)
f.close()
for ligne in lignes:
  print(ligne)"""