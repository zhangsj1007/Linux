#include<iostream>
#include<sys/epoll.h>
#include<cstring>
#include<cstdlib>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
using namespace std;


#define HOST "127.0.0.1"
#define PORT 6000

int main(int argc, char ** argv){
	int serverFd = 0;
	struct sockaddr_in serveraddr;
	char buffer[50];
	int n = 0;

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = inet_addr(HOST);
	
	bind(serverFd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

	if (connect(serverFd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))){
		cout << "Connect Failed" << endl;
		exit(1);
	}

	for(int i = 0; i < 5 ; i++){
		sleep(2);
		bzero(buffer, 50);
		strcpy(buffer, "Hello, Server!");
		write(serverFd, buffer, 50);
		n = read(serverFd, buffer, 50);
		if (n < 0){
			cout << "Read from server failed" << endl;
			exit(1);
		}
		buffer[n] = 0;
		cout << "Message From server : " << buffer << endl;
	}
	close(serverFd);
}
