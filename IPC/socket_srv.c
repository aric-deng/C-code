#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>

int main()
{
	int        rcd ;
	struct sockaddr_un  server_sockaddr ;
	int        backlog ;
	ushort        ci ;
	int        watch_fd_list[3] ;
	fd_set        catch_fd_set ;
	fd_set        watchset ;
	int    new_cli_fd ;
	int   maxfd;
	int    socklen ,server_len;
	struct sockaddr_un  cli_sockaddr ;
	struct {
		char  module_id ;  /* Module ID    */
		int  cli_sock_fd ;  /* Socket ID    */
	} cli_info_t[2] ;

	for (ci=0;ci<=1;ci++)
		cli_info_t[ci].cli_sock_fd=-1;

	for (ci=0;ci<=2;ci++)
		watch_fd_list[ci]=-1;  

	int server_sockfd,client_sockfd;

	server_sockfd = socket( AF_UNIX, SOCK_STREAM, 0 ) ;
	server_sockaddr.sun_family = AF_UNIX ;
	strcpy( server_sockaddr.sun_path, "server_socket" ) ;
	server_len=sizeof(server_sockaddr);
	rcd = bind( server_sockfd, ( struct sockaddr * )&server_sockaddr, server_len ) ;

	backlog = 5 ;
	rcd = listen( server_sockfd, backlog ) ;
	printf("SERVER::Server is  waitting on socket=%d \n",server_sockfd);
	
	watch_fd_list[0]=server_sockfd;
	FD_ZERO( &watchset ) ;
	FD_SET( server_sockfd, &watchset ) ;
	maxfd=watch_fd_list[0];
  
  	while (1){
		char ch;
		int fd;
		int nread;
		
		catch_fd_set=watchset;
		rcd = select( maxfd+1, &catch_fd_set, NULL, NULL, (struct timeval *)0 ) ;
		
		if ( rcd < 0 ) {
			printf("SERVER::Server 5 \n");
			exit(1);
		}
		
		if ( FD_ISSET( server_sockfd, &catch_fd_set ) ) {
			socklen = sizeof( cli_sockaddr ) ;
			new_cli_fd = accept( server_sockfd, ( struct sockaddr * )
				&( cli_sockaddr ), &socklen ) ;
			printf(" SERVER::open communication with  Client %s on socket %d\n", 
			   cli_sockaddr.sun_path,new_cli_fd);  

			for (ci=1;ci<=2;ci++){
				if(watch_fd_list[ci] != -1) 
					continue;
				else {  
				  	watch_fd_list[ci] = new_cli_fd;
				  	break;
				}  
			}  
			
			FD_SET(new_cli_fd , &watchset ) ;
			if ( maxfd < new_cli_fd ) {
				maxfd = new_cli_fd ;
			}

			for ( ci=0;ci<=1;ci++){
				if(cli_info_t[ci].cli_sock_fd == -1) {
					cli_info_t[ci].module_id=cli_sockaddr.sun_path[0];
					cli_info_t[ci].cli_sock_fd=new_cli_fd;
					break;
				}    
			}

			continue;  
		}
	
	
	    for ( ci = 1; ci<=2 ; ci++ ) {
			  int      dst_fd = -1 ;
			  char      dst_module_id;
			  char       src_module_id;
			  int    i;
			  if (watch_fd_list[ ci ]==-1) 
				continue;
				
			  if ( !FD_ISSET( watch_fd_list[ ci ], &catch_fd_set ) ) {
				continue ;
			  }
			  
			  ioctl(watch_fd_list[ ci ],FIONREAD,&nread);
			  if (nread==0){
				continue;
			  }  
			  
			  read( watch_fd_list[ ci ], &dst_module_id, 1 ) ;
			  for (i=0;i<=1;i++){
					if(cli_info_t[i].module_id == dst_module_id) 
						dst_fd =  cli_info_t[i].cli_sock_fd;  
					if(cli_info_t[i].cli_sock_fd==watch_fd_list[ ci ]) 
						src_module_id =  cli_info_t[i].module_id;    
			  }
			  
			  read( watch_fd_list[ ci ], &ch, 1 ) ;
			  printf("SERVER::char=%c to  Client %c on socket%d\n",ch, dst_module_id,dst_fd);  
			  write(dst_fd,&src_module_id, 1 ) ;
			  write(dst_fd,&ch, 1 ) ;
        }
    }  
}
