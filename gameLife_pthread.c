#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define numTh 4
#define gsize 2048
#define ite 5

pthread_barrier_t barrier;

typedef struct{
	int **grid1, **grid2, *ind;
}pthread_data;

int sumVisinhos(int lin, int col, int** grid){
	int sum=0;
	if(grid[((lin-1)+gsize)%gsize][((col-1)+gsize)%gsize] == 1)sum++;
	if(grid[((lin-1)+gsize)%gsize][col] == 1)sum++;
	if(grid[((lin-1)+gsize)%gsize][(col+1)%gsize] == 1)sum++;
	if(grid[lin][(col+1)%gsize] == 1)sum++;
	if(grid[lin][((col-1)+gsize)%gsize] == 1)sum++;
	if(grid[(lin+1)%gsize][((col-1)+gsize)%gsize] == 1)sum++;
	if(grid[(lin+1)%gsize][col] == 1)sum++;
	if(grid[(lin+1)%gsize][(col+1)%gsize] == 1)sum++;
	return sum;


}

void printa(int** grid){
	int lin, col, sum = 0; 
	for(lin = 0; lin < 50; lin++){
		for(col = 0; col < 50; col++){
			printf("%d ",grid[lin][col]);
			if(grid[lin][col] == 1) sum++;
		}
		printf("\n");
	}
	printf("resultado dessa iteracao: %d\n\n",sum);
}

void* tredi(void* arg){
	pthread_data data = *(pthread_data*) arg;
	int id = *data.ind;
	//printf("thread %d criada\n",id);
	int start = gsize/numTh * id;
	int end = start + gsize/numTh;
	int **grid = data.grid1;
	int **ngrid = data.grid2;
	int i, sum, **swap, gridAtual = 1;


	for(i = 0; i < ite; i++){
		for(int lin = start; lin < end; lin++){
			for(int col = 0; col < gsize; col++){
				sum = sumVisinhos(lin, col, grid);
				if(grid[lin][col] == 1 && (sum == 2 || sum == 3))ngrid[lin][col] = 1;
				else if(grid[lin][col] == 0 && sum == 3)ngrid[lin][col] = 1;
				else ngrid[lin][col] = 0;
			}
		}
		swap = grid;
		grid = ngrid;
		ngrid = swap;
		if(id == 0) printa(grid);
		pthread_barrier_wait(&barrier);
	}
	pthread_exit(NULL);
}

int resultadoFinal(int** grid){
	int lin, col, sum = 0;
	for(lin = 0; lin < gsize; lin++){
		for(col = 0; col < gsize; col++)
			if(grid[lin][col] == 1)sum++;
	}
	return sum;
}

int main(int argc, char** argv){
	time_t time = clock();
	int **m1 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++){
		m1[i] = (int *)malloc(gsize * sizeof(int));
	}

	int **m2 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++){
		m2[i] = (int *)malloc(gsize * sizeof(int));
	}
	
		//GLIDER
	int lin = 1, col = 1;
	m1[lin  ][col+1] = 1;
	m1[lin+1][col+2] = 1;
	m1[lin+2][col  ] = 1;
	m1[lin+2][col+1] = 1;
	m1[lin+2][col+2] = 1;
	 
	//R-pentomino
	lin =10; col = 30;
	m1[lin  ][col+1] = 1;
	m1[lin  ][col+2] = 1;
	m1[lin+1][col  ] = 1;
	m1[lin+1][col+1] = 1;
	m1[lin+2][col+1] = 1;

	pthread_t t[numTh];
	pthread_barrier_init(&barrier, NULL, numTh);

	for(int i = 0; i < numTh; i++){
		pthread_data *arg = (pthread_data*)malloc(sizeof(pthread_data));
		arg->grid1 = m1;
		arg->grid2 = m2;
		arg->ind = (int*)malloc(sizeof(int));
		*arg->ind = i;
		pthread_create(&t[i], NULL, tredi,(void*) arg);
	}	
	for(int i = 0; i < numTh; i++) //se colovar uma barreira acho que fica melhor
		pthread_join(t[i], NULL);
	time = clock() - time;
	double time_f = ((double)time)/CLOCKS_PER_SEC;
	printf("resultado: %d\n", resultadoFinal(m1));
	printf("tempo: %f",time_f);

	pthread_exit(0);
}
