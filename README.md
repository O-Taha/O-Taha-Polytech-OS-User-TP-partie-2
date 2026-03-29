Ce projet implémente un système de communication basé sur les Sockets UNIX avec un protocole simple nommé BEUIP.\

Le serveur écoute sur un port fixe, reçoit des messages et maintient une table des utilisateurs actifs (pseudo + IP).\
Chaque message est structuré avec un type, une signature "BEUIP" et un champ expediteur.

Au lancement, le serveur envoie un broadcast pour annoncer sa présence sur le réseau.\
Le type '1' permet de gérer les broadcasts et d’ajouter les utilisateurs dans la table.

Le code client permet l'envoi de commandes au serveur :
* Le type '3' affiche la liste des utilisateurs connus côté serveur : ```3BEUIP```
* Le type '4' est censé permettre l’envoi de messages privés (pseudo + message).\

Cependant, je n’ai pas réussi à faire fonctionner correctement ce système de MP (header '9'), de plus, pour parser le message, j'utilise un '_' (underscore) pour séparer le pseudo du message.
