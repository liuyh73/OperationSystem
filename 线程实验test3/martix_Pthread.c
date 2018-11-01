#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
int A[10][10];
int B[10][10];
int C[10][10];
int M,K,N;
struct element{
	int row;
	int col;
}elem[100];

void init(){
	printf("Please input the M, K, N: \n");
	scanf("%d%d%d", &M, &K, &N);
	//matrix A
	printf("Please input the matrixA with row=%d and col=%d: \n", M, K);
	for(int i=1;i<=M;i++)
		for(int j=1;j<=K;j++)
			scanf("%d", &A[i][j]);

	//matrix B
	printf("Please input the matrixB with row=%d and col=%d: \n", K, N);
	for(int i=1;i<=K;i++)
		for(int j=1;j<=N;j++)
			scanf("%d", &B[i][j]);
}

void* matrix_multiply(void* _elem){
	struct element *elem=(struct element*)_elem;
	for(int i=1;i<=K;i++)
		C[elem->row][elem->col]+=A[elem->row][i]*B[i][elem->col];
	//pthread_exit(0);
}

void create_thread(){
	pthread_t tid[M*N];
	
	// obtain the parameter
	for(int i=1;i<=M;i++){
		for(int j=1;j<=N;j++){
			elem[(i-1)*N+j-1].row=i;
			elem[(i-1)*N+j-1].col=j;
		}
	}

	// create the thread and run the function 
	for(int i=1;i<=M;i++){
		for(int j=1;j<=N;j++){
			pthread_create(&tid[(i-1)*N+j-1], NULL, &matrix_multiply, (void*)&elem[(i-1)*N+j-1]);
		}
	}

	// join here 
	for(int i=1;i<=M;i++){
		for(int j=1;j<=N;j++){
			pthread_join(tid[(i-1)*N+j-1],NULL);
		}
	}
}

void print(){
	printf("The result matrix is as follows: \n");
	for(int i=1;i<=M;i++){
		for(int j=1;j<=N;j++)
			printf("%d ",C[i][j]);
		printf("\n");
	}
}

int main(){
	init();
	create_thread();
	print();
	return 0;
}