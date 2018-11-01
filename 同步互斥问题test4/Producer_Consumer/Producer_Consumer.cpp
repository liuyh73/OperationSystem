#include <sys/types.h>  
#include <iostream>
#include <pthread.h>
#include <semaphore.h> 
#include <cstring>
#include <cstdlib>
#include <queue>
#include <unistd.h>
using namespace std;
#define NBUFF 5
struct sto {
    queue<int>items;
    sem_t empty;
    sem_t full;
    sem_t mutex;
}shared;

struct person{
	int tid;
	char role;
	int startTime;
	int duration;
	int itemId;
}P_C[100];
pthread_t pth[100];
void *produce(void *), *consume(void *);

int main(){
	sem_init(&shared.empty, 0, NBUFF);
	sem_init(&shared.full, 0, 0);
	sem_init(&shared.mutex, 0, 1);
	int index=0;
	while(cin>>P_C[index].tid>>P_C[index].role>>P_C[index].startTime>>P_C[index].duration){
		if(P_C[index].role=='P'){
			cin>>P_C[index].itemId;
			cout<<"Create producer thread: "<<P_C[index].tid<<endl;
			pthread_create(&pth[P_C[index].tid], NULL, produce, (void*)&P_C[index]);
		}
		else{
			cout<<"Create consumer thread: "<<P_C[index].tid<<endl;
			pthread_create(&pth[P_C[index].tid], NULL, consume, (void*)&P_C[index]);
		}
		index++;
	}
	for(int i=0;i<index;i++)
		pthread_join(pth[P_C[i].tid], NULL);
}

void *produce(void *_producer){
	person *producer=(person*)_producer;
	sleep(producer->startTime);
	cout<<"The producer thread "<<producer->tid<<" produces an item "<<producer->itemId<<"."<<endl;

	sem_wait(&shared.empty);
	sem_wait(&shared.mutex);
	cout<<"The producer thread "<<producer->tid<<" adds the item "<<producer->itemId<<" to the buffer."<<endl;

	shared.items.push(producer->itemId);
	sleep(producer->duration);

	sem_post(&shared.mutex);
	sem_post(&shared.full);
	cout<<"The producer thread "<<producer->tid<<" ends producing."<<endl;
}

void *consume(void *_consumer){
	person *consumer=(person*)_consumer;
	sleep(consumer->startTime);

	sem_wait(&shared.full);
	sem_wait(&shared.mutex);
	int itemId=shared.items.front();
	cout<<"The consumer thread "<<consumer->tid<<" removes an item "<<itemId<<" from the buffer."<<endl;

	shared.items.pop();
	sleep(consumer->duration);

	sem_post(&shared.mutex);
	sem_post(&shared.empty);
	cout<<"The consumer thread "<<consumer->tid<<" consume the item "<<itemId<<"."<<endl;
}