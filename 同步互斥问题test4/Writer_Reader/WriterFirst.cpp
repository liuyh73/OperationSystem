#include <sys/types.h>  
#include <iostream>
#include <pthread.h>
#include <semaphore.h> 
#include <cstring>
#include <cstdlib>
#include <queue>
#include <unistd.h>
using namespace std;
static pthread_mutex_t reader_count_mutex = PTHREAD_MUTEX_INITIALIZER; 
static pthread_mutex_t writer_count_mutex = PTHREAD_MUTEX_INITIALIZER; 
struct sto {
  int reader_count=0;
  int writer_count=0;
  sem_t write_region, read_permit;
}shared;

struct person{
	int tid;
	char role;
	int startTime;
	int duration;
}R_W[100];
pthread_t pth[100];

void *read(void *_reader){
	person* reader=(person*)_reader;
	sleep(reader->startTime);

	cout<<"The reader thread "<<reader->tid<<" trys to read until there are no writers waiting"<<endl;
	sem_wait(&shared.read_permit);
	pthread_mutex_lock(&reader_count_mutex);
	shared.reader_count++;
	if(shared.reader_count==1)
		sem_wait(&shared.write_region);
	pthread_mutex_unlock(&reader_count_mutex);
	sem_post(&shared.read_permit);

	cout<<"The reader thread "<<reader->tid<<" is reading"<<endl;
	sleep(reader->duration);

	pthread_mutex_lock(&reader_count_mutex);
	shared.reader_count--;
	if(shared.reader_count==0)
		sem_post(&shared.write_region);
	pthread_mutex_unlock(&reader_count_mutex);
	cout<<"The reader thread "<<reader->tid<<" ends reading"<<endl;
}

void *write(void *_writer){
	person* writer=(person*)_writer;
	sleep(writer->startTime);

	pthread_mutex_lock(&writer_count_mutex);
	shared.writer_count++;		
	if(shared.writer_count==1){
		cout<<"The writer thread "<<writer->tid<<" blocks the reader"<<endl;
		sem_wait(&shared.read_permit);
	}
	pthread_mutex_unlock(&writer_count_mutex);

	cout<<"The writer thread "<<writer->tid<<" trys to write"<<endl;
	sem_wait(&shared.write_region);
	cout<<"The writer thread "<<writer->tid<<" is writing"<<endl;
	sleep(writer->duration);
	cout<<"The writer thread "<<writer->tid<<" ends writing"<<endl;
	sem_post(&shared.write_region);

	pthread_mutex_lock(&writer_count_mutex);
	shared.writer_count--;
	if(shared.writer_count==0){
		cout<<"The writer thread "<<writer->tid<<" resumes the reader"<<endl;
		sem_post(&shared.read_permit);
	}
	pthread_mutex_unlock(&writer_count_mutex);
}

int main(){
	int index=0;
	sem_init(&shared.write_region, 0, 1);
	sem_init(&shared.read_permit, 0, 1);
	while(cin>>R_W[index].tid>>R_W[index].role>>R_W[index].startTime>>R_W[index].duration){
		if(R_W[index].role=='R'){
			cout<<"Create reader thread: "<<R_W[index].tid<<endl;
			pthread_create(&pth[R_W[index].tid], NULL, read, (void*)&R_W[index]);
		}
		else{
			cout<<"Create writer thread: "<<R_W[index].tid<<endl;
			pthread_create(&pth[R_W[index].tid], NULL, write, (void*)&R_W[index]);
		}
		index++;
	}

	for(int i=0;i<index;i++)
		pthread_join(pth[R_W[i].tid], NULL);
}