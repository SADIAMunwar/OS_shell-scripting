#include<iostream>
#include<unistd.h>
#include<fcntl.h>
using namespace std;

//int main(){
//int fd=open("output.txt",O_WRONLY | O_CREAT | O_TRUNC , 0644);
//if(fd<0){
//err<<"failed to open file "<<endl;
//return 1;
//}

//int new_fd=dup(fd);
//write(new_fd,"hellow using dup \n",16);
//close(fd);
//close(new_fd);
//return 0;
//}

int main(){
int fd=open("output.txt",O_WRONLY | O_CREAT | O_TRUNC , 0644);
if(fd<0){
cerr<<"failed to open file "<<endl;
return 1;
}


dup2(fd,1);
close(fd);
cout<<"hii cutiee";
return 0;
}

