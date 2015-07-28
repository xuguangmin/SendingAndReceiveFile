#include <stdio.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define port	"10086"
#define ip	"0.0.0.0"
#define size     1024

int main()
{
	int fd;
	int sockfd;
	int newfd;
	int ret;
	socklen_t len;
	char buf[size];
	char fname[size];
	struct sockaddr_in laddr, raddr;

	laddr.sin_family = AF_INET;
	laddr.sin_port  = htons(atoi(port));
	inet_pton(AF_INET, ip, &laddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("socket");	
		exit(-1);
	}

	ret = bind(sockfd, (void*)&laddr, sizeof(laddr));
	if(ret < 0) {
		perror("bind");	
		exit(-1);
	}
	listen(sockfd, 10);
	len = sizeof(raddr);
	while(1)	{
		newfd = accept(sockfd, (void*)&raddr, &len);		
		if(newfd < 0) {
			perror("accept");	
			exit(-1);
		}
		if(newfd == EAGAIN)
			continue;
		char ipstr[1024];
		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, sizeof(ipstr));
		printf("%s\r\n", ipstr);
		break;
	}

	int fnamelen;
	if(recv(newfd, &fnamelen, sizeof(int), 0) <= 0) {
		fprintf(stdout, "get filename failed\r\n");
		exit(-1);
	}		
	memset(fname, '\0',size);
	if(recv(newfd, fname, fnamelen, 0) <= 0) {
		fprintf(stdout, "get filename failed\r\n");
		exit(-1);
	}		
	strcat(&fname[fnamelen], "\0");
	printf("file: %s\r\n", fname);

	fd = open(fname, O_CREAT | O_TRUNC| O_RDWR, 444);
	if(fd < 0) {
		perror("open():");	
		exit(-1);
	}

	int j = 0;
	int count = 0;
	while(1) {
		ret = recv(newfd, buf, size, 0);		
		write(fd, buf, ret);
		count += ret;
		memset(buf, '\0', size);
		if(ret == EAGAIN) 
			continue;
		if(ret <= 0) {
			break;
		}
	}
	fprintf(stdout, "file size: %d\r\n", count);
	printf("\r\nRECEIVE OK\r\n\r\n");

	close(fd);
	close(newfd);
	close(sockfd);
	return 0;
}
