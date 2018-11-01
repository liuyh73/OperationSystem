#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
extern int errno;
#define MAX_SEQUENCE 10

typedef struct{
    long fib_sequence[MAX_SEQUENCE];
    int sequence_size;
}shared_data;

int main(){
    int shmid;
    char *shmptr;
    key_t key;
    pid_t pid;
    shared_data fib;

    printf("Please input a number less than 10: \n");
    scanf("%d", &fib.sequence_size);

    //get the key
    if((key = ftok("./share", 1)) < 0) {
        printf("ftok error:%s\n", strerror(errno));
        return -1;
    }
    //create the share memory
    if((shmid = shmget(key, sizeof(shared_data), 0600|IPC_CREAT)) < 0){
        printf("shmget error:%s\n", strerror(errno));
        exit(-1);
    }
    //link with the share memory
    if((shmptr = (char*)shmat(shmid, 0, 0)) == (void*)-1) {
        printf("shmat error:%s\n", strerror(errno));
        exit(-1);
    }

    //create a child process
    pid=fork();
    if(pid==0){
        //in the child process, we will obtain the fibonacci list
        memset(fib.fib_sequence, 0, sizeof(fib.fib_sequence));
        long num1=0, num2=1;
        for(int i=0;i<fib.sequence_size; i++){
            fib.fib_sequence[i]=num2;
            num2=num1+num2;
            num1=num2-num1;
        }
        //store the result to the share memory
        memcpy(shmptr, &fib, sizeof(fib));
        printf("child:pid is %d,share memory from %lx to %lx, content:%s\n",
            getpid(), (unsigned long)shmptr, (unsigned long)(shmptr + sizeof(fib)), shmptr);
        printf("child process has store the first %d fibonacci numbers in the share memory\n", fib.sequence_size);
        printf("child process exit successfully!\n");
    }
    else{
        wait(NULL);
        printf("parent:pid is %d,share memory from %lx to %lx, content:%s\n",
            getpid(),(unsigned long)shmptr, (unsigned long)(shmptr + sizeof(shared_data)), shmptr);
        printf("parent process read the fibonacci sequence from the share memory\n");
        
        //get the fibonacci list
        shared_data *fib=(shared_data*)shmptr;
        for(int i=0;i<fib->sequence_size;i++){
            printf("%ld ", fib->fib_sequence[i]);
        }
        printf("\n");
        
        //delete the share memory
        if((shmctl(shmid, IPC_RMID, 0) < 0)){
            printf("shmctl error:%s\n", strerror(errno));
            exit(-1);
        }
        printf("delete the share memory and exit successfully!\n");
    }
    return 0;
}
