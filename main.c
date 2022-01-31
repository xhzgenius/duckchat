#include "header.h"

/* Global variables */
int namenum = 0;
const int helpnum = 5;
char names[MAXNAME][MAXNAME];
char help[MAXNAME][200] = {"Hints:\n",">>>help: show this table\n",">>>names: show all the names of the users\n",
">>>send <\"...\"> (to <name>...): Send message(...) (to user(s)) if the command includes \"to\", otherwise it has the same effect as echo does.\n",
">>>refresh or >>>re: Updating messages from your message queue\n"};
char *uc = "Undefined command\n";
char *nonews = "No news feed\n";
sem_t mutex; // 用于在线程间锁定全局变量

/* queue */
char messages[MAXNAME][MAXINF][MAXINF] = {0};
int messagenum[MAXNAME][2] = {0}; //0: front, 1: rear

extern void *thread_user(void *vargp); // 用于服务客户端的线程函数
extern void *thread_admin(void *vargp); // 用于查看服务器信息的线程函数
extern void *thread_shaker(void *vargp); // 用于和所有客户端握手的线程函数

int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    int listenfd, i = 0;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    pthread_t tid[MAXLINE];
    struct information *info;
    Sem_init(&mutex, 0, 1);
    
    // 开机
    listenfd = Open_listenfd(argv[1]); // 包装过的Open_listenfd可以保证返回值不小于0，否则引发unix_error

    // 创建一个线程，用于应答服务端的输入（服务端也需要看用户数量之类的信息）
    pthread_create(NULL, NULL, thread_admin, NULL);
    // 创建一个线程，专门用于和所有客户端握手
    pthread_create(NULL, NULL, thread_shaker, NULL);

    while(1)
    {
        // Accept客户端的连接请求
        clientlen = sizeof(struct sockaddr_storage);
        info = Malloc(sizeof(struct information));
        info->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        P(&mutex);
        info->pos = i;
        i++;
        V(&mutex);

        Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        // 创建一个新线程，用来服务客户端
        pthread_create(tid+info->pos, NULL, thread_user, info);

        // 打印到服务器日志
        char timestr[MAXINF];
        gettime(timestr);
        printf("Connected to (%s, %s) %s with tid %ld\n", 
            client_hostname, client_port, timestr, tid[i-1]);
    }
    exit(0); // 不必运行到此处
}
