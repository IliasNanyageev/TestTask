//============================================================================
// Name        : HelloWorld.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <regex>

#define SERVER_PORT 1235

void parseMessage(const std::string& msg);
int makeTCPServer(struct sockaddr_in& client_addr)
{
	int server_descriptor_TCP = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_descriptor_TCP == -1) {
		perror("Server TCP Socket Error");
		exit(EXIT_FAILURE);
	}
	int bind_status=bind(server_descriptor_TCP,(struct sockaddr*)&client_addr,sizeof(client_addr));
	if(bind_status==-1)
	{
		perror("Binding TCP Socket Error");
		close(server_descriptor_TCP);
		exit(EXIT_FAILURE);
	}
	int listen_status=listen(server_descriptor_TCP,10);
	if(listen_status==-1){
		perror("Listen TCP Socket Error");
		close(server_descriptor_TCP);
		exit(EXIT_FAILURE);
	}
	return server_descriptor_TCP;
}
int makeUDPServer(struct sockaddr_in& client_addr)
{
	int server_descriptor_UDP=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(server_descriptor_UDP == -1) {
		perror("Server UDP Socket Error");
		exit(EXIT_FAILURE);
	}
	int bind_status=bind(server_descriptor_UDP,(struct sockaddr*)&client_addr,sizeof(client_addr));
	if(bind_status==-1){
		perror("Binding UDP Socket Error");
		close(server_descriptor_UDP);
		exit(EXIT_FAILURE);
	}
	return server_descriptor_UDP;
}
int main(int argc,char* argv[]) {

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	int server_descriptor_TCP=makeTCPServer(client_addr);
	int server_descriptor_UDP=makeUDPServer(client_addr);

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(server_descriptor_TCP,&readfds);
	FD_SET(server_descriptor_UDP,&readfds);
	select(server_descriptor_UDP+1,&readfds,NULL,NULL,NULL);
	if(FD_ISSET(server_descriptor_TCP,&readfds)){
		int client_decriptor=accept(server_descriptor_TCP,0,0);
		char msg_size[4];
		recv(client_decriptor,&msg_size,4,0);
		uint32_t msg_len=ntohl(*((uint32_t*)msg_size));
		uint32_t recieved_bytes=0;
		char current_byte;
		std::string msg;
		while(recieved_bytes<msg_len){
			recieved_bytes+=recv(client_decriptor,&current_byte,1,0);
			msg+=current_byte;
		}
		parseMessage(msg);
		uint32_t sended_bytes=0;
		while(sended_bytes<msg.size())
			sended_bytes+=send(client_decriptor,&msg[sended_bytes],1,0);
		close(client_decriptor);
	}

	if(FD_ISSET(server_descriptor_UDP,&readfds)){
		char msg_size[4];
		socklen_t clientAddrLen=sizeof(client_addr);
		recvfrom(server_descriptor_UDP,&msg_size,4,0,((struct sockaddr*)&client_addr),&clientAddrLen);
		uint32_t msg_len=ntohl(*((uint32_t*)msg_size));
		uint32_t recieved_bytes=0;
		char current_byte;
		std::string msg;
		while(recieved_bytes<msg_len){
			recieved_bytes+=recvfrom(server_descriptor_UDP,&current_byte,1,0,((struct sockaddr*)&client_addr),&clientAddrLen);
			msg+=current_byte;
		}
		parseMessage(msg);
		uint32_t sended_bytes=0;
		while(sended_bytes<msg.size())
			sended_bytes+=sendto(server_descriptor_UDP,&msg[sended_bytes],1,0,((struct sockaddr*)&client_addr),clientAddrLen);
	}

	close(server_descriptor_TCP);
	close(server_descriptor_UDP);
	return 0;
}
void parseMessage(const std::string& msg)
{
	std::string result="";
	std::regex regular("[0-9]");
	for(char ch:msg)
		if(std::regex_search(&ch,regular))
			result+=ch;
	std::sort(result.begin(),result.end());
	int sum=0;
	for(auto ch:result)
		sum+=atoi(&ch);
	std::cout<<"Sum: "<<sum<<std::endl;
	std::cout<<"Min Value: "<<*result.begin()<<std::endl;
	std::cout<<"Max Value: "<<*(result.end()-1)<<std::endl;
	for(auto ch=result.rbegin();ch<result.rend();ch++)
		std::cout<<*ch<<' ';
	std::cout<<std::endl;
}

