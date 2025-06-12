#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;
int main(){
char input[100];
char* data[11];
while(true){
cout<<"shell>";
cin.getline(input,100);
if(strcmp(input,"q")==0){
cout<<"existing shell "<<endl;
break;
}
if(strlen(input)==0)
continue;
int count=0;
char*token=strtok(input," ");
while(token!=NULL && count<10){
data[count++]=token;
token=strtok(NULL," ");
}
data[count]=NULL;
pid_t pid=fork();
if(pid<0){
cerr<<"fork failed"<<endl;
continue;
}
else if(pid==0){
execvp(data[0],data);
cerr<<"unknown command"<<data[0]<<endl;
exit(1);
}
else{
wait(NULL);
}}return 0;}

