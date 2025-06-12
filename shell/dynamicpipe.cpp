#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;
int main() {
int i=0,j=0;
   char input[100];
   char* cmd1[10],*cmd2[10];
   bool found=false;
 while (true) {  
   cout<<"shell> ";
   cin.getline(input,100);
    if (strcmp(input, "q") == 0) break;
   char* token=strtok(input," ");
   while(token!=NULL){
   if(strcmp(token, "|")==0){
   found=true;
   cmd1[i]=NULL;
   token=strtok(NULL," ");
   continue;
   }if(!found){
   cmd1[i++]=token;
   }else{
   cmd2[j++]=token;
   }
   token=strtok(NULL," ");
}   cmd2[j]=NULL;
if(!found){
pid_t pid = fork();
if(pid==0){
execvp(cmd1[0],cmd1);
cerr<<"command failed"<<endl;
}else if(pid>0){
wait(NULL);}
else{
cerr<<"fork failed"<<endl;
}}
else{
int pipefd[2];
pipe(pipefd);

pid_t pid1=fork();
if(pid1==0){
dup2(pipefd[1],1);
close(pipefd[0]);
close(pipefd[1]);
execvp(cmd1[0],cmd1);
cerr << "Command 1 failed" << endl;
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            dup2(pipefd[0], 0);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(cmd2[0], cmd2);
            cerr << "Command 2 failed" << endl;
        }
close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL);
        wait(NULL);
    
}}
    return 0;
}

