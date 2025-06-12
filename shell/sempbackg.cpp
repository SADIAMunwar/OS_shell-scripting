#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<semaphore.h>
#include<cstring>
using namespace std;
void execute(char * args[],bool bckg,sem_t* sem){
pid_t pid=fork();
if(pid<0){
cerr<<"error:fork failed"<<endl;
return;
}
else if(pid==0){
if(sem!=NULL)
sem_wait(sem);
execvp(args[0],args);
if(sem!=NULL)
sem_post(sem);
cerr<<"error:command "<<args[0]<<endl;
exit(1);
}
else{
if(!bckg && sem !=NULL){
sem_wait(sem);
int status;
waitpid(pid,&status,0);
sem_post(sem);
if(WIFEXITED(status)){
cout<<"command completed with status: "<<WEXITSTATUS(status)<<endl;
}
}
else if(bckg){
cout<<"running in background (pid "<<pid<<")"<<endl;
}}}
int main(){
sem_t sem;
sem_init(&sem,0,1);
char input[100];
char* args[10+1];
    while (true) {
int arg_count=0;
bool background=false;
cout<<"shell> ";
cin.getline(input,100);
 if (strcmp(input, "q") == 0) break;
char*token=strtok(input," ");
while(token!=NULL && arg_count<10){
if(strcmp(token,"&")==0){
background=true;
break;
}args[arg_count++]=token;
token=strtok(NULL," ");
}
args[arg_count]=NULL;
execute(args,background,&sem);
if(!background){
cout<<"shell> ";
}}
sem_destroy(&sem);
return 0;}







