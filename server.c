#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024

int main(void){
    int sockid;
	int server_port = 8888;
	char *server_ip = "127.0.0.1";

	sockid = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in server_addr, client_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

    char *buffer[SIZE];
    int n;
    int len;

	int bind_result = bind(sockid, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    
    if(bind_result < 0){
        printf("Error during binding\n");
    }
    else{
        printf("Server listening on %s:%d\n", server_ip, server_port);
        int len = sizeof(client_addr);

        int n = recvfrom(sockid, (char *)buffer, SIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);

        printf("Got msg from %s:%d : %s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, buffer);
    }

	close(sockid);
}