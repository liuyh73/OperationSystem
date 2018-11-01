#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
int main(){
    pid_t pid1=fork();
    if(pid1==0)
        printf("Child1: b\n");
    else{
        pid_t pid2=fork();
        if(pid2==0)
            printf("Child2: c\n");
        else
            printf("Parent: a\n");
    }
    return 0;
}