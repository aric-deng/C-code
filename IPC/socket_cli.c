#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	
	int client_sockfd;
	int len;
	struct sockaddr_un server_sockaddr, cli_sockaddr;
	int result;
	char dst_module_id = 'B';
	char ch = '1';
	char src_module_id;
	
	client_sockfd = socket( AF_UNIX, SOCK_STREAM, 0 );
	
	cli_sockaddr.sun_family = AF_UNIX ;
	strcpy( cli_sockaddr.sun_path, "A" ) ;
	bind( client_sockfd, (struct sockaddr * )&cli_sockaddr, sizeof( cli_sockaddr ) ) ;
	server_sockaddr.sun_family = AF_UNIX;
	strcpy( server_sockaddr.sun_path, "server_socket" ) ;
	len = sizeof( server_sockaddr );
	
	result = connect( client_sockfd, ( struct sockaddr * )&server_sockaddr, len );
	if (result < 0){
		printf("ClientA::error on connecting \n");	
		exit(1);
	}
	
	printf("ClientA::succeed in connecting with server\n");
	sleep(10);
	write( client_sockfd, &dst_module_id, 1 );
	write( client_sockfd, &ch, 1 ); 
	read ( client_sockfd, &src_module_id, 1 ); 
	read ( client_sockfd, &ch, 1 ); 
	printf("ClientA::char from  Client %c =%c\n", src_module_id, ch);	
	close (client_sockfd);
	
}
