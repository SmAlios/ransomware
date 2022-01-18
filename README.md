# ransomcraft (v 1.1.3)
### ⚠️ Projet à but éducatif ⚠️

Ce logiciel est réalisé dans le cadre du cours de développement pour nos études de sécurité informatique. Il a pour objectif d'amener les étudiants de cette section à désacraliser les virus informatiques en comprenant leurs fonctionnement. La meilleure solution pour celà étant d'en réaliser un sois-même, notre corps enseignant à pris la décision d'en faire notre examen. Ce travail nous étant imposé, nous ne sommes en rien responsable de l'utilisation que vous pourriez en faire.

Caractéristiques du logiciel :
 - language de programmation : C

Fonctionnalités du logiciel :
 - parcours d'une arborescence à partir d'un répertoir donné
 - génération d'une cléf de cryptage et son vecteur d'initialisation
 - chiffrement en AES-256-CBC
 - chiffrement des fichiers détectés dans l'arborescence
 - envois de la cléf à un serveur
 - déchiffrement des données si la cléf et le vecteur d'initialisation sont donnés
 - déchiffrement des données automatique si le fichier "clients.txt" est présent

## Exécution
Il est fortement conseillé d'utiliser une machine virtuelle pour l'utilisation du logiciel. Veuillez également preter attention au fait que si il est lancé sans être relié au serveur, vous ne pourrez pas récuppérer la cléf de cryptage et le vecteur d'initialisation. Vos fichiers resteront alors chiffrés à jamais.

Nous vous conseillons la mise en place de deux machine virtuelles de type Debian connectées entre elles en réseau interne via votre gestionnaire de machines virtuelles.

### 🔐 Exécution du ransomware (v1.1.3)

Pour exécuter le logiciel de chiffrement, il est nécessaire d'entrer certains paramètres :
 - le mode d'exécution (chiffrement/déchiffrement)
 - en cas de chiffrement, le dossier d'où le parcours de l'arborescence démarrera
 - en cas de déchiffrement manuel, la cléf de chiffrement ainsi que le vecteur d'initialisation

Ainsi, il est nécessaire d'entrer,
Au chiffrement :

```
./ransom -c -f {dossier racine de l'arborescence}
```

Au déchiffrement (en mode manuel) :

```
./ransom -d -f {dossier racine de l'arborescence} -k {cléf de chiffrement} -v {vecteur d'initialisation}
```

Au déchiffrement (en mode automatique) :

```
./ransom -d -f {dossier racine de l'arborescence} -a
```

En cas de souci, vous pouvez faire appel à l'aide avec la commande suivante:

```
./ransom -h
```

### 🚀 Exécution du server (v1.1.s)

Pour lancer l'écoute, et ainsi récolter une potentiel cléf de chiffrement et son vecteur il faut entrer la commande d'écoute.

```
./server -l
```

En cas de souci, vous pouvez faire appel à l'aide avec la commande suivante:

```
./server -h
```

### 💻 Systèmes d'exploitation compatible

 | Système d'exploitation                      |   Shortcut
 | ------------------------------------------- |:-----------------------------
 | Windows                                     | <kbd>Non compatible</kbd>
 | Linux                                       | <kbd>Compatible (Debian type)</kbd>
 | Mac OS                                      | <kbd>Non compatible</kbd>
