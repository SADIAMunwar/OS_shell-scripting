#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;
void run(const char* cmd,char* const args[]){
pid_t pid=fork();
if(pid==0){
execvp(cmd,args);
cerr<<"excelp failed"<<endl;
exit(1);
}
else if(pid>0){
wait(NULL);
cout<<"ls command executed"<<endl;
}
else{
cerr<<"fork failed"<<endl;
}}
int main(){
char* args1[]={(char*)"ls",NULL};
char* args2[]={(char*)"ls",(char*)"-l",NULL};
run("ls",args1);
cout<<"executed :ls"<<endl;
run("ls",args2);
cout<<"executed :ls -l"<<endl;
return 0;
}

