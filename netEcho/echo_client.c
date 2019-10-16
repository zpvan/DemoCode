#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>

// 省去所有的错误检查, 而在真正健壮的时间中这是必需的
int main() {
    // echo服务器的主机地址和端口号
    char* echo_host = "192.168.1.20";
    int echo_port = 7777;
    int sockfd;
    struct sockaddr_in *server = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

    // 设置将要连接的服务器的地址
    server->sin_family = AF_INET;
    server->sin_port = htons(echo_port);  // 注意, 是网络字节序
    server->sin_addr.s_addr = inet_addr(echo_host);

    // 创建套接字(因特网地址族, 流套接字和默认协议)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 连接到服务器
    printf("Conneting to %s\n", echo_host);
    printf("Numeric: %u\n", server->sin_addr);
    connect(sockfd, (struct sockaddr*)server, sizeof(*server));

    // 发送消息
    char* msg = "Hello World";
    printf("\nSend: '%s'\n", msg);
    write(sockfd, msg, strlen(msg));

    // 接收返回结果
    char* buf = (char*)malloc(1000);  // 接收用的缓冲区, 最大为1000个ASCII字符
    int bytes = read(sockfd, (void*)buf, 1000);
    printf("\nBytes received: %u\n", bytes);
    printf("Text: '%s'\n", buf);

    // 结束通信, 即关闭套接字
    close(sockfd);
}