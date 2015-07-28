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
#include <unistd.h>

#define PORT	"10086"
#define IP		"0.0.0.0"
#define SIZE    1024

typedef struct __file_info 
{
#define FILENAMELEN		64
	int fsize;
	char fname[FILENAMELEN];
}finfo_t;
typedef struct __file_all_t
{
	int fnum;
	finfo_t finfo[0];
}file_t;

typedef struct __send_data_
{
	int fdno;
	char buf[SIZE];
}sdata_t;


int main()
{
	return 0;
	int ret, opt, count;
	char ipstr[SIZE];
	socklen_t socklen;
	int fd, sockfd, newfd;
	struct sockaddr_in laddr, raddr;
	file_t file;
	sdata_t sdata;

	laddr.sin_family = AF_INET;
	laddr.sin_port  = htons(atoi(PORT));
	inet_pton(AF_INET, IP, &laddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		fprintf(stdout, "socket() error: %s\r\n", strerror(errno));
		exit(-1);
	}

	if(bind(sockfd, (void*)&laddr, sizeof(laddr)) < 0) {
		fprintf(stdout, "bind() error: %s\r\n", strerror(errno));
		exit(-1);
	}

	if(listen(sockfd, 10)) {
		fprintf(stdout, "listen() error: %s\r\n", strerror(errno));
		exit(-1);
	}

	socklen = sizeof(raddr);
	while(1)	{
		newfd = accept(sockfd, (void*)&raddr, &socklen);		
		if(newfd < 0) {
			perror("accept");	
			exit(-1);
		}
		if(newfd == EAGAIN)
			continue;

		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, sizeof(ipstr));
		printf("%s\r\n", ipstr);
		break;
	}

	int fnamelen;
	if(recv(newfd, &fnamelen, sizeof(int), 0) <= 0) {
		fprintf(stdout, "get file size failed\r\n");
		exit(-1);
	}
	memset(&file, '\0', sizeof(file_t));
	if(recv(newfd, &file, fnamelen, 0) <= 0) {
		fprintf(stdout, "get file info failed\r\n");
		exit(-1);
	}

	int flag = 1;
	opt = file.fnum;
	while(opt--)
	{
		fprintf(stdout, "file: %s\r\n", file.finfo[opt].fname);

		if((fd = open(file.finfo[opt].fname, O_CREAT | O_TRUNC| O_RDWR, 444)) < 0) {
			fprintf(stdout, "open %s error: %s\r\n", file.finfo[opt].fname, strerror(errno));
			continue;
		}

		while(1)
		{
			if(flag)
				ret = recv(newfd, &sdata, sizeof(sdata_t), 0);		

			if(sdata.fdno != opt) {
				flag = 0;
				break;
			}
			flag = 1;

			write(fd, sdata.buf, ret - sizeof(int));
			count += ret;
			memset(sdata.buf, '\0', SIZE);

			if(ret == EAGAIN) 
				continue;
			else if(ret <= 0) 
				break;
		}
	}

	//fprintf(stdout, "file size: %d\r\n", count);
	printf("\r\nRECEIVE OK\r\n\r\n");

	close(fd);
	close(newfd);
	close(sockfd);
	return 0;
}
