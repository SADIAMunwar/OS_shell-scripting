//just shows how to parse command or terminal sa data ko recive kr ka furter code ma sa lana thats all it does
#include<iostream>
#include<cstring>
using namespace std;

int main()
{
char input[100];
char* data[11];
do{
cout<<"shell>";
cin.getline(input,100);

int count=0;
char* token=strtok(input," ");
while(token!=NULL && count<10){
data[count++]=token;
token=strtok(NULL," ");
}
data[count]=NULL;
cout<<"parsed command:"<<endl;
for(int i=0;i<count;i++){
cout<<" arg "<<i <<":"<<data[i]<<endl;
}} while (strcmp(input, "q") != 0);

return 0;
}
