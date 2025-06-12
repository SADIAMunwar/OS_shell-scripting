#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;
int main(){
pid_t pid=fork();
if(pid==0){
execlp("ls","ls",NULL);
cerr<<"excelp failed"<<endl;
exit(1);
}
else if(pid>0){
wait(NULL);
cout<<"ls command executed"<<endl;
}
else{
cerr<<"fork failed"<<endl;
}
return 0;
}

