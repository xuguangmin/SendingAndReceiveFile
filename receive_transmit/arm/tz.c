#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>


#define PORT	"10086"
#define SIZE	1024	

typedef struct __file_info 
{
#define FILENAMELEN		128
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

static unsigned long file_size(int fd);


int main(int argc, char **argv)
{
	if(argc <= 2) {
		fprintf(stdout, "Usage: tz IP filename\r\n");
		return -1;
	}
	if(argv[1] == NULL || argv[2] == NULL) {
		fprintf(stdout, "Usage: tz IP filename\r\n");
		return -1;
	}

	int fd, sockfd;
	struct sockaddr_in raddr;
	int opt, ret, len, pos, count;
	file_t file;
	sdata_t sdata;

	file.fnum = opt = argc - 2;			//file number

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		fprintf(stdout, "socket() error: %s\r\n", strerror(errno));
		exit(-1);
	}

	raddr.sin_family = AF_INET;
	raddr.sin_port  = htons(atoi(PORT));
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);

	ret = connect(sockfd, (void*)&raddr, sizeof(raddr));		
	if(ret < 0) {
		fprintf(stdout, "connect error: %s\r\n", strerror(errno));
		exit(-1);
	}

	//get file infomatation
	while(opt--)
	{
		strncpy(file.finfo[opt].fname, argv[2 + opt], strlen(argv[2 + opt]));	
		if( (fd = open(argv[2 + opt], O_RDONLY)) < 0) {
			fprintf(stdout, "open %s error: %s\r\n", argv[2 + opt], strerror(errno));
			opt++;
			file.fnum--;
			continue;
		}
		if( (file.finfo[opt].fsize = file_size(fd)) < 0) {
			fprintf(stdout, "get file \"%s\" size failed\r\n", file.finfo[opt].fname);
			return -1;	
		}
		close(fd);
		//printf("%s\t%d\r\n", file.finfo[opt].fname, file.finfo[opt].fsize);
	}
		
	//send file infomatation
	int filelen = sizeof(file);
	if(send(sockfd, &filelen, sizeof(int), 0) <= 0) {
		fprintf(stdout, "send file info error: %s\r\n", strerror(errno));
		return -1;
	}		
	if(send(sockfd, &file, filelen, 0) <= 0) {
		fprintf(stdout, "send file info error: %s\r\n", strerror(errno));
		return -1;
	}		

	opt = file.fnum;
	while(opt--) 
	{
		if( (fd = open(argv[2 + opt], O_RDONLY)) < 0) {
			fprintf(stdout, "open %s error: %s\r\n", argv[2 + opt], strerror(errno));
			continue;
		}

		sdata.fdno = opt;
		while(1)
		{
			len = read(fd, sdata.buf, SIZE);
			if(len < 0) {
				perror("read()");	
				break;
			}		
			if(len == 0)
				break;

			pos = 0;
			while(len > 0) {
				ret = write(fd, sdata.buf+pos, len);
				if(ret < 0) {
					printf("write() error: %s  %s\r\n", file.finfo[opt].fname, strerror(errno));
					break;
				}
				len -= ret;
				pos += ret;
			}

			count += len;
		}

		close(fd);
		printf("file : %s\tsize : %d\tTRANSMISSION OK !\r\n", file.finfo[opt].fname, count);
	}

	close(sockfd);
	exit(0);
}

static unsigned long file_size(int fd)
{
	struct stat statbuff;
	int filesize = -1;

	if(fstat(fd, &statbuff)) {
		fprintf(stdout, "fstat error: %s\r\n", strerror(errno));
		return filesize;
	} 

	filesize = statbuff.st_size;

	return filesize;
}

