#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define numTh 4
#define gsize 2048
#define ite 2000

pthread_barrier_t barrier;

// estrutura de dados da thread
typedef struct{
	int **grid1, **grid2, *ind;
}pthread_data;

// verifica todos os vizinhos adjacentes e soma
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

// thread principal
void* tredi(void* arg){
    // inicializando variávies
	pthread_data data = *(pthread_data*) arg;
	int id = *data.ind;
	printf("thread %d criada\n",id);
	int start = gsize/numTh * id;
	int end = start + gsize/numTh;
	int **grid = data.grid1;
	int **ngrid = data.grid2;
	int i, sum, gridAtual = 1;

    // loop principal
	for(i = 0; i < ite; i++){
		for(int lin = start; lin < end; lin++){
			for(int col = 0; col < gsize; col++){
				sum = sumVisinhos(lin, col, grid);
				if(grid[lin][col] == 1 && (sum == 2 || sum == 3))ngrid[lin][col] = 1;
				else if(grid[lin][col] == 0 && (sum == 3 || sum == 6))ngrid[lin][col] = 1;
				else ngrid[lin][col] = 0;
			}
		}
		pthread_barrier_wait(&barrier);
        // faz a troca entre a matriz atual e a nova matriz
		if(gridAtual == 1){
			grid = data.grid2;
			ngrid = data.grid1;
			gridAtual = 2;
		} else {
			grid = data.grid1;
			ngrid = data.grid2;
			gridAtual = 1;
		}
		pthread_barrier_wait(&barrier);
	}
	pthread_exit(NULL);
}

// soma a quantidade de células vivas
int resultadoFinal(int** grid){
	int lin, col, sum = 0;
	for(lin = 0; lin < gsize; lin++){
		for(col = 0; col < gsize; col++)
			if(grid[lin][col] == 1)sum++;
	}
	return sum;
}

// print de matriz
void printa(int** grid){
	int lin, col;
	for(lin = 0; lin < gsize; lin++){
		for(col = 0; col < gsize; col++)
			printf("%d ",grid[lin][col]);
		printf("\n");
	}
}

int main(int argc, char** argv){
    // declaração e inicialização da matriz
	int **m1 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++){
		m1[i] = (int *)malloc(gsize * sizeof(int));
	}

	int **m2 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++){
		m2[i] = (int *)malloc(gsize * sizeof(int));
	}
	for(int i = 0; i < gsize; i++){
		for(int j = 0; j < gsize; j++){
			m1[i][j] = 0; m2[i][j] = 0;
		}
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

    // declaração de threads e barreira
	pthread_t t[numTh];
	pthread_barrier_init(&barrier, NULL, numTh);

    // inicialização da estrutura de dados e da thread
	for(int i = 0; i < numTh; i++){
		pthread_data *arg = (pthread_data*)malloc(sizeof(pthread_data));
		arg->grid1 = m1;
		arg->grid2 = m2;
		arg->ind = (int*)malloc(sizeof(int));
		*arg->ind = i;
		pthread_create(&t[i], NULL, tredi,(void*) arg);
	}	
	for(int i = 0; i < numTh; i++) // join das threads
		pthread_join(t[i], NULL);
	printf("resultado: %d\n", resultadoFinal(m1));

	pthread_exit(0);
}
