#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define numTh 4
#define gsize 2048
#define ite 5

// verifica todos os vizinhos adjacentes e soma
int sumVisinhos(int lin, int col, int** grid) {
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
void tredi(int **m1, int **m2) {
	int id;
	int start;
	int end;
	int **grid = m1;
	int **ngrid = m2;
	int i, sum, gridAtual = 1;

    omp_set_num_threads(numTh);
    #pragma omp parallel private(id, start, end, i, sum) 
    {
        // inicializando variávies
        id = omp_get_thread_num();
        start = gsize/numTh * id;
        end = start + gsize/numTh;
    
    	printf("thread %d criada\n", id);
        // laço principal da thread
      	for(i = 0; i < ite; i++) {
      		for(int lin = start; lin < end; lin++) {
      			for(int col = 0; col < gsize; col++) {
      				sum = sumVisinhos(lin, col, grid);
      				if(grid[lin][col] == 1 && (sum == 2 || sum == 3))ngrid[lin][col] = 1;
      				else if(grid[lin][col] == 0 && (sum == 3 || sum == 6))ngrid[lin][col] = 1;
      				else ngrid[lin][col] = 0;
      			}
      		}
      		#pragma omp barrier
			// faz a troca entre a matriz atual e a nova matriz
            #pragma omp single
            {
                if(gridAtual == 1) {
                    grid = m2;
                    ngrid = m1;
                    gridAtual = 2;
                } else {
                    grid = m1;
                    ngrid = m2;
                    gridAtual = 1;
                }
            }
      		#pragma omp barrier
        }
  	#pragma omp barrier
    }
}

// soma a quantidade de células vivas
int resultadoFinal(int** grid) {
	int lin, col, sum = 0;
	for(lin = 0; lin < gsize; lin++) {
		for(col = 0; col < gsize; col++)
			if(grid[lin][col] == 1)sum++;
	}
	return sum;
}

// print de matriz
void printa(int** grid) {
	int lin, col;
	for(lin = 0; lin < gsize; lin++) {
		for(col = 0; col < gsize; col++)
			printf("%d ",grid[lin][col]);
		printf("\n");
	}
}

int main(int argc, char** argv) {
	// declaração e inicialização da matriz
	int **m1 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++) {
		m1[i] = (int *)malloc(gsize * sizeof(int));
	}

	int **m2 = (int**)malloc(gsize * sizeof(int*));
	for(int i = 0; i < gsize; i++) {
		m2[i] = (int *)malloc(gsize * sizeof(int));
	}
	for(int i = 0; i < gsize; i++) {
		for(int j = 0; j < gsize; j++) {
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

    tredi(m1, m2);

    printf("resultado: %d\n", resultadoFinal(m1));

    return 0;
}
