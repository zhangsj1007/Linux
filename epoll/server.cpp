#include<iostream>
#include<sys/epoll.h>
#include<cstring>
#include<cstdlib>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
using namespace std;

#define HOST "127.0.0.1"
#define PORT 6000

int main(int argc, char ** argv){
	int epfd = 0;
	int listenFd = 0;
	int clientFd = 0;
	int n;
	struct epoll_event ev, events[20];
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	char buffer[50];
	socklen_t len;

	//epoll
	epfd = epoll_create(20);
	//IPV4, TCP
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	ev.data.fd = listenFd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &ev);

	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(HOST);
	serveraddr.sin_port = htons(PORT);
	bind(listenFd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(listenFd, 5);
	cout << "Begin to listen clients..." << endl;


	while(1){
		n = epoll_wait(epfd, events, 20, 10);
		for (int i = 0; i < n; i++){
			if (events[i].data.fd == listenFd){
				bzero(&clientaddr, sizeof(clientaddr));
				clientFd = accept(listenFd, (struct sockaddr*)&clientaddr, &len);
				if (clientFd <= 0){
					cout << "Bad Connect Fd, exit" << endl;
					exit(1);
				}

				char * p = inet_ntoa(clientaddr.sin_addr);
				cout << "Accept a connection : "<< p << endl;
				ev.data.fd = clientFd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clientFd, &ev);	
			}else if(events[i].events & EPOLLIN){
				clientFd = events[i].data.fd;
				int nr = 0;
				bzero(&buffer, 50);
				nr = read(clientFd, buffer, 50);
				if (nr < 0){
					cout << "Bad Read From client" << endl;
					exit(1);
				}
				if (nr == 0) {
					cout << "Connection breakup !" << endl; 
					epoll_ctl(epfd, EPOLL_CTL_DEL, clientFd, &ev);
					close(clientFd);
					continue;
				}
 
				buffer[nr] = 0;
				cout << "Data from client : ";
				cout << buffer << endl;
				bzero(&ev, sizeof(ev));
				ev.data.fd = clientFd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, clientFd, &ev);
			}else if(events[i].events & EPOLLOUT){
				clientFd = events[i].data.fd;
				strcpy(buffer, "Thank you for visiting");
				int nw = write(clientFd, buffer, 50);
				if (nw < 0){
					cout << "write error " << endl;
					exit(1); 
				}
				bzero(&ev, sizeof(ev));
				ev.data.fd = clientFd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, clientFd, &ev);
			}
		}
			
	}
}
