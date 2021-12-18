#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024

void usage(void){
    printf("Working ...");
}

void send_msg(char *client_ip, char *client_key, char *client_iv){
    printf("working ...");
}

void get_msg(void){
    int sockid;
	int server_port = 8888;
	char *server_ip = "192.168.1.4";

	sockid = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in server_addr, client_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

    char *buffer[SIZE];
    int len;

	int bind_result = bind(sockid, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    
    if(bind_result < 0){
        printf("Error during binding\n");
    }
    else{
        printf("Server listening on %s:%d\n", server_ip, server_port);
        len = sizeof(client_addr);

        recvfrom(sockid, (char *)buffer, SIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        printf("Got msg from %s:%d : %s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, buffer);
    }

	close(sockid);
}

int main(int argc, char *argv[]){
    if(argc > 1){
        if(strcmp(argv[1], "-l") == 0){
            get_msg();
        }
        else if(strcmp(argv[1], "-a") == 0){
            if(argc < 7){
                printf("argument(s) missing !\n");
                return 0;
            }

            char *client_ip;
            char *client_key;
            char *client_iv;

            for(int i = 0; i < 7; i++){
                if(strcmp(argv[i], "-a") == 0){
                    client_ip = argv[i + 1];
                }
                else if(strcmp(argv[i], "-k") == 0){
                    client_key = argv[i, + 1];
                }
                else if(strcmp(argv[i], "-v") == 0){
                    client_iv = argv[i, + 1];
                }
            }

            if(strcmp(client_ip, NULL) == 0 || strcmp(client_key, NULL) == 0 || strcmp(client_iv, NULL) == 0){
                printf("argument(s) missing !\n");
                return 0;
            }

            send_msg(client_ip, client_key, client_iv);
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