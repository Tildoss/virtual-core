# virtual-core

## Structure

Le core (écrit en C) prend en argument un binaire ainsi qu'un fichier avec l'état initial des registres. Le binaire est obtenu en compilant (avec notre propre compilateur) un fichier d'instructions en assembleur-like. Le compileur va lire ligne par ligne le fichier d'instructions et les transformer en ligne de 32-bits (4 octets) suivant les valeurs données dans les tableaux du sujet

BIN_NAME \<CODE\> \<STATE\> (VERBOSE)\
-> Le programme qu'on exécute, prend 2 arguments obligatoires et un optionnel\
-> \<CODE\> Binary file contenant les instructions qui seront executées par le core, larges de 32 bits (compilés par notre compilateur)\
-> \<STATE\> l'état initial des registres\
-> (VERBOSE) ajoute ou non l'état verbose

## Instructions

### Encoding

0000        (Branch Condition Code 28-31)\
000         (always 0 25-27)\
0           (Immediate Value flag 24)\
0000        (operation code 20-23)\
0000        (operande 1 16-19)\
0000        (operande 2 12-15)\
0000        (destination register 8-11)\
00000000    (Immediate Value 0-7)\
