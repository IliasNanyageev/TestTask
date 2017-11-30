
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <stdint.h>
#define SERVER_PORT 1235
#define SERVER_IP "127.0.0.1"
int main(int argc,char* argv[]) {
	if(argc<2){
		perror("Input protocol type \"udp\" or \"tcp\"");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET,SERVER_IP,&(serverAddress.sin_addr));
	std::string msg;
	std::getline(std::cin,msg);
	char* msg_size=NULL;
	uint32_t size=htonl(msg.size());
	msg_size=(char*)&size;
	if(strcmp(argv[1],"udp")==0)
	{
		int client_descriptor = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (client_descriptor == -1) {
			perror("Socket error");
			exit(EXIT_FAILURE);
		}
		uint32_t sended_bytes=0;
		while(sended_bytes<4)
			sended_bytes+=sendto(client_descriptor,msg_size,4,0,((struct sockaddr*)&serverAddress),sizeof(serverAddress));
		sended_bytes=0;
		while(sended_bytes<msg.size())
			sended_bytes+=sendto(client_descriptor,&msg[sended_bytes],1,0,((struct sockaddr*)&serverAddress),sizeof(serverAddress));

		std::string server_answer;
		uint32_t bytes_received=0;
		char current_byte;
		socklen_t serverAddrLen=sizeof(serverAddress);
		while(bytes_received<msg.size()){
			int bytes=recvfrom(client_descriptor,&current_byte,1,0,((struct sockaddr*)&serverAddress),&serverAddrLen);
			bytes_received+=bytes;
			server_answer+=current_byte;
		}
		std::cout<<"Received message: "<<server_answer<<std::endl;
	}
	else if(strcmp(argv[1],"tcp")==0){
		int client_descriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (client_descriptor == -1) {
			perror("Socket error");
			exit(EXIT_FAILURE);
		}
		int server_descriptor=connect(client_descriptor,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
		if(server_descriptor==-1){
			perror("Connection error");
			close(client_descriptor);
			exit(EXIT_FAILURE);
		}

		uint32_t sended_bytes=0;
		while(sended_bytes<4)
			sended_bytes+=send(client_descriptor,msg_size,4,0);
		sended_bytes=0;
		while(sended_bytes<msg.size())
			sended_bytes+=send(client_descriptor,&msg[sended_bytes],1,0);
		std::string server_answer;
		uint32_t bytes_received=0;
		char current_byte;
		while(bytes_received<msg.size()){
			bytes_received+=recv(client_descriptor,&current_byte,1,0);
			server_answer+=current_byte;
		}
		std::cout<<"Received message: "<<server_answer;
	}
	else{
		perror("Unknown protocol");
		exit(EXIT_FAILURE);
	}

	return 0;
}
