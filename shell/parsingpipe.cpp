#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;

int main(){
int pipefd[2];
pipe(pipefd);

pid_t pid1=fork();
if(pid1==0){
dup2(pipefd[1],1);
close(pipefd[0]);
execlp("ls","ls",NULL);
}
pid_t pid2=fork();
if(pid2==0){
dup2(pipefd[0],0);
close(pipefd[1]);
execlp("ls","-l",NULL);
}
close(pipefd[0]);
close(pipefd[1]);
wait(NULL);
wait(NULL);
return 0;
}

