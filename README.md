# ransomcraft (v 1.0)
/!\ Ce projet est à but éducatif /!\

Ce logiciel est réalisé dans le cadre du cours de développement pour nos études de sécurité informatique. Il a pour objectif d'amener les étudiants de cette section à désacraliser les virus informatiques en comprenant leurs fonctionnement. La meilleure solution pour celà étant d'en réaliser un sois-même, notre corps enseignant à pris la décision d'en faire notre examen. Ce travail nous étant imposé, nous ne sommes en rien responsable de l'utilisation que vous pourriez en faire.

Caractéristiques du logiciel :
 - language de programmation : C

Fonctionnalités du logiciel :
 - parcours d'une arborescence à partir d'un répertoir donné
 - génération d'une cléf de cryptage et son vecteur d'initialisation
 - chiffrement en AES-256-CBC
 - chiffrement des fichiers détectés dans l'arborescence
 - envois de la cléf à un serveur
 - déchiffrement des données si le cléf et le vecteur d'initialisation sont donnés

# Exécution
Il est fortement conseillé d'utiliser une machine virtuelle pour l'utilisation du logiciel. Veuillez également preter attention au fait que si il est lancé sans être relié au serveur, vous ne pourrez pas récuppérer la cléf de cryptage et le vecteur d'initialisation. Vos fichiers resteront alors chiffrés à jamais.

Nous vous conseillons la mise en place de deux machine virtuelles de type Debian connectées entre elles en réseau interne via votre gestionnaire de machines virtuelles.

-- A rédiger une fois le logiciel fini --
