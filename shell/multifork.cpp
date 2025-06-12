#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;

#define max 10
void execute(char* arg[]){
pid_t pid=fork();
if(pid<0){
cerr<<"error: fork failed"<<endl;
return;
}
else if(pid==0){
execvp(arg[0],arg);
cerr<<"error : "<<arg[0]<<" not found"<<endl;
}else{
wait(NULL);
}}
int main(){
char input[100];
char * args[max+1];
int arg_count = 0;
 cout << "Shell> ";
 cin.getline(input, 100);
    char* token = strtok(input, " ");
    while (token != NULL && arg_count < max) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    if (arg_count == 0) {
        cerr << "Error: No command entered" << endl;
        cout << "Shell> ";
        return 0;
    }
    
    execute(args);
    cout << "Shell> ";
    return 0;
}



