// #include <bits/stdc++.h>
// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// using namespace std;
 
// int main(int argc, char *argv[])
// {	
// 	printf("argc = %d\n", argc);
// 	int port = (argc == 1 ? 8000 : atoi(argv[1]));
// 	printf("port = %d\n", port);
//     int client_sockfd;
//     int len;
//     struct sockaddr_in remote_addr; //服务器端网络地址结构体
//     char buf[BUFSIZ];  //数据传送的缓冲区
//     memset(&remote_addr,0,sizeof(remote_addr)); //数据初始化--清零
//     remote_addr.sin_family=AF_INET; //设置为IP通信
//     remote_addr.sin_addr.s_addr=inet_addr("9.134.237.210");//服务器IP地址
//     remote_addr.sin_port=htons(8000); //服务器端口号
    
//     /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
//     if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
//     {
//         perror("socket error");
//         return 1;
//     }
    


// /*

// 	sockaddr_in client;
// 	client.sin_family = AF_INET;
// 	client.sin_addr.s_addr = htonl(INADDR_ANY);
// 	client.sin_port = htons(port);
// 	if (bind(client_sockfd, (struct sockaddr *)&client, sizeof(client)) < 0) {
// 		printf("bind() failed.\n");
// 		close(client_sockfd);
// 		return 1;
// 	}
// */


//     /*将套接字绑定到服务器的网络地址上*/
//     if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
//     {
//         perror("connect error");
//         return 1;
//     }
//     printf("connected to server\n");
//     //len=recv(client_sockfd,buf,BUFSIZ,0);//接收服务器端信息
//     //    buf[len]='\0';
//     //printf("%s",buf); //打印服务器端信息
    
//     /*循环的发送接收信息并打印接收信息（可以按需发送）--recv返回接收到的字节数，send返回发送的字节数*/
//     for (int i = 1; i <= 10000; ++i)
//     {
//         //printf("Enter string to send:");
//         //scanf("%s",buf);
// 		buf[0] = 'a' + i % 26;
// 		buf[1] = '\0';
//         len=send(client_sockfd,buf,strlen(buf),0);
//         //len=recv(client_sockfd,buf,BUFSIZ,0);
//         //buf[len]='\0';
//         //printf("received:%s\n",buf);
// 	//fflush(stdout);
//     }
//     cout << "end" << endl;
//     /*关闭套接字*/
    
//     return 0;
// }