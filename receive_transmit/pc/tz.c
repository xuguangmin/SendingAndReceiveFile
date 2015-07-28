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
#define size	1024	

int main(int argc, char **argv)
{
	if(argc != 3) {
		fprintf(stdout, "Usage: tz IP filename\r\n");	
		return -1;
	}
	if(argv[1] == NULL || argv[2] == NULL) {
		fprintf(stdout, "Usage: tz IP filename\r\n");
		return -1;
	}

	int fd;
	int sockfd;
	int ret;
	socklen_t len;
	char buf[size];
	struct sockaddr_in raddr;

	int i;
	char ch;

	fd = open(argv[2], O_RDONLY);
	if(fd < 0) {
		perror("open():");	
		exit(-1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("socket");	
		exit(-1);
	}

	raddr.sin_family = AF_INET;
	raddr.sin_port  = htons(atoi(port));
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);

	ret = connect(sockfd, (void*)&raddr, sizeof(raddr));		
	if(ret < 0) {
		perror("connect():");	
		exit(-1);
	}

	int fnamelen = strlen(argv[2]);
	send(sockfd, &fnamelen, sizeof(int), 0);		
	send(sockfd, argv[2], fnamelen, 0);		
	printf("%s\r\n", argv[2]);
	int j = 0, n = 0;
	int count = 0;
	while(1) {
		memset(buf, '\0', size);
		n = read(fd, buf, size);
		ret = send(sockfd, buf, n, 0);		
		count += ret;
		if(ret <= 0)
			break;
	}
	printf("count = %d\r\n", count);
	printf("\r\nTRANSMISSION OK !\r\n\r\n");
	close(sockfd);
	
	exit(0);
}

