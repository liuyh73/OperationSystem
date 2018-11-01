#include <pthread.h>
#include <stdio.h>
struct Fibonacci{
	int fib_num;
	long long fib_array[100];
}fib;

void *fib_generate(void *_fib){
	struct Fibonacci *fib=(struct Fibonacci*)_fib;
	fib->fib_array[0]=0;
	fib->fib_array[1]=1;
	for(int i=2;i<=fib->fib_num;i++)
		fib->fib_array[i]=fib->fib_array[i-1]+fib->fib_array[i-2];
}

int main(){
	printf("Please input a num that less than 50: \n");
	scanf("%d", &fib.fib_num);

	pthread_t tid;
	pthread_create(&tid, NULL, fib_generate, (void*)&fib);
	pthread_join(tid, NULL);

	for(int i=1;i<=fib.fib_num;i++)
		printf("%lld ", fib.fib_array[i]);
	printf("\n");
	return 0;
}