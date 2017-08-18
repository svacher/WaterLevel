# WaterLevel
**Gestion du niveau d'eau dans une cuve**  
*Juin 2017*

## Besoin
* Mesurer le niveau de remplissage de la cuve du récupérateur d'eau de pluie
* Mesurer la température de l'eau pour éviter le gel (cassure de la cuve et des capteurs)
* Transmettre les données via I2C (soit 4 fils avec l'alimentation)
* Indiquer une alarme température sur une led visible près de la cuve

## Solution
Utilisation d'un ATMEGA328P (utilisé dans le Arduino UNO) de manière autonome (sans interface USB, sans horloge externe).  
RX et TX sont laissés libres en cas d'utilisation ultérieure (debug, wifi, …).  
Un connecteur ISCP est mis en place pour la programmation vu que l'ATMEGA est soudé sur le circuit.

Les informations sont demandées via I2C par un Raspberry Pi qui contrôle l'ensemble de la serre et indique un trop plein de la cuve (envoi SMS ?) ou un risque de gel.

La lecture des capteurs se fait à interval régulier, puis le microcontrôleur utilise le mode Sleep pour économiser l'énergie et augmenter la longévité du matériel.

Les capteurs de niveaux sont immersibles et sans partie électrique en contact avec l'eau (utilisation d'un aimant et d'un interrupteur reed) : https://www.amazon.fr/dp/B01EZGSVZK/ref=pe_3044141_189395771_TE_3p_dp_1.

Le capteur de température est de type analogique mesuré par le µC : AD22100. Autant utiliser un peu plus le µC au lieu d'utiliser des capteurs avec sortie I2C, ils seront plus utiles autre part.  
Des tests comparatifs ont été faits avec un TMP36, mais celui-ci est moins stable, moins précis (10mV/°C pour 22,5mV/°C pour le AD) et la température mesurée semble moins juste.  
Un deuxième capteur de température identique est ajouté sur le circuit, il faut bien utiliser le stock…

## Fonctionnement
La lecture des capteurs est faite toutes les 10 minutes puis le résultat est stocké en RAM.

Pour envoyer les informations, le contrôleur doit recevoir une demande de la part du maître I2C.  
4 commandes sont créés (uniquement en lecture) : lecture de toutes les informations, uniquement le niveau d'eau, uniquement température de l'eau et uniquement température du boîtier.

Commandes I2C :
- 0xFF renvoi toutes les informations à la suite (niveau d'eau, température eau, température boîtier)  
- 0xE0 renvoi le niveau d'eau dans la cuve (1 octet)  
- 0xD0 renvoi la température de l'eau (4 octets)  
- 0xD1 renvoi la température du boîtier (4 octets)  

Une led s'allume dès que la température de l'eau est inférieure à la consigne d'alarme, de manière indépendante, c'est-à-dire sans contrôle du maître I2C.

@bonus : voir plus tard, pour pouvoir modifier la consigne d'alarme de température par commande I2C avec stockage en EEPROM. Gérer aussi une notion de températures minis et maxis en EEPROM.
