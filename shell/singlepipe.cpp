#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;

int main(){
int pipefd[2];

if(pipe(pipefd)<0){
cout<<"creation failed"<<endl;
return 1;
}

pid_t pid1=fork();
if(pid1<0){
cerr<<"fork failed" <<endl;
close(pipefd[0]);
close(pipefd[1]);
return 2;
}
else if(pid1==0){
close(pipefd[0]);
dup2(pipefd[1],1);
close(pipefd[1]);
execlp("ls","ls",NULL);
cerr<<"ls failed"<<endl;
exit(1);
}

pid_t pid2=fork();
if(pid2<0){
cerr<<"fork failed" <<endl;
close(pipefd[0]);
close(pipefd[1]);
return 3;
}
else if(pid2==0){
close(pipefd[1]);
dup2(pipefd[0],0);
close(pipefd[0]);
execlp("wc","wc",NULL);
cerr<<"wc failed"<<endl;
exit(1);
}
close(pipefd[0]);
close(pipefd[1]);
wait(NULL);
wait(NULL);
return 0;
}


