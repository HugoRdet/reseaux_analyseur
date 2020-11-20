# -*- coding: utf-8 -*-
"""
Created on Thu Nov 12 21:31:49 2020

"""

def list_to_str(liste):
    """ Permet de transformer un liste de trame
        en sa forme chaine de caractere nous permettant 
        de l'inserer dans un fichier text"""
    
    res = "[("
    a = ""
    b = []
    
    cpt1 = 1
    cpt2 = 1
    print(len(liste))
    
    for e in liste:
        if cpt1 < len(liste):
            a, b = e
            res += str(a) + ",\t[('"
            for i in b:
                print(len(b))
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
    """liste(str) -> list(tuple(str, list(str))
    
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
    
    
    
    prems_espace = True
    
    
    for e in liste:
        if len(e) >= 4:
            if (e[caractere] == "0") and (e[caractere + 1] == "0") and (e[caractere + 2] == "0") and (e[caractere + 3] == "0"):
                if cpt == cpt2:
                    trame = (numero, list_offtrame)
                    list_trame.append(trame)
                    numero += 1
                    list_offtrame = []
                    cpt2 += 1
                cpt += 1
                    
        for i in e:
            if (i == " ") and (prems_espace == True):
                prems_espace = False
            elif (i != " ") and (prems_espace == True):
                offset += i
            else:
                ligne += i
        offtrame = (offset, ligne) 
        list_offtrame.append(offtrame)
        offset = ""
        ligne = ""
        prems_espace = True
        
    
    return list_trame
             
            
        
            
        
            
            
            

                    
                
""" MAIN """

liste_lignes = []
i = 0 
cpt = 0
ligne = ""

f = open("test_res.txt", "r")
tab = f.read()
while i < len(tab):
    if tab[i] != "\n":
        ligne += tab[i]
    elif (tab[i] == "\n") and (ligne != "") :
        liste_lignes.append(ligne)
        ligne = ""
        cpt += 1
    i += 1
f.close()
liste_trame = trouve_list_tram(liste_lignes)
f2 = open("Fvide.txt", "w")
trame_str = list_to_str(liste_trame)
f2.write(trame_str)
f2.close()







