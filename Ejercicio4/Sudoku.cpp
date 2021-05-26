/* Tomado de https://gist.github.com/sowmyagowri/f4bde25b3ebba1e6e9930afbdf31a25b */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <chrono>

#define num_threads 27

using namespace std;
using namespace std::chrono; // tiempo

typedef struct {
    int row; 
    int col;
    int (* board)[9];//tablero puntero de 9 espacios
} parameters;
int result[num_threads] = {0};
void *check_grid(void *params);
void *check_rows(void *params);
void *check_cols(void *params);
int sudoku_checker(int sudoku[9][9]);

int main(void) {
    int sudoku[9][9] =
    {
            {6, 2, 4, 5, 3, 9, 1, 8, 7},
            {5, 1, 9, 7, 2, 8, 6, 3, 4},
            {8, 3, 7, 6, 1, 4, 2, 9, 5},
            {1, 4, 3, 8, 6, 5, 7, 2, 9},
            {9, 5, 8, 2, 4, 7, 3, 6, 1},
            {7, 6, 2, 3, 9, 1, 4, 5, 8},
            {3, 7, 1, 9, 5, 6, 8, 4, 2},
            {4, 9, 6, 1, 8, 2, 5, 7, 3},
            {2, 8, 5, 4, 7, 3, 9, 1, 6}
     };
      //:: llamar elementos de una estructura de datos 

    steady_clock::time_point start_time_single_thread = steady_clock::now();//crea un reloj monotono con el tiempo actual

      // 0 si bien 1 si mal
    if(sudoku_checker(sudoku))
        printf("Con un solo hilo: Solución INCORRECTA del sudoku\n");
    else
        printf("Con un solo hilo: Solución CORRECTA del sudoku\n");
    steady_clock::time_point end_time_single_thread = steady_clock::now();// crea otro reloj cuando ya termino
    duration<double> elapsed_time_single_thread = duration_cast<duration<double>>(end_time_single_thread - start_time_single_thread);// hace un aresta para saber cuanto demoro

    cout << endl << "Tiempo total usando un solo hilo: " << elapsed_time_single_thread.count() << " segundos" << endl << endl;
    
    // aqui es para 27 hilos
    
    steady_clock::time_point start_time_threads = steady_clock::now(); //crea un reloj monotono con el tiempo actual
    pthread_t threads[num_threads];// crea un arreglo de hilos 
    int threadIndex = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {//revisa una sola vez la fila
            if (i%3 == 0 && j%3 == 0) {// 9 grillas
                parameters *gridData = (parameters *) malloc(sizeof(parameters));
                gridData->row = i;
                gridData->col = j;
                gridData->board = sudoku;
                pthread_create(&threads[threadIndex++], NULL, check_grid, gridData);
            }
            if (j == 0) {// revisa las filas solo en la primera iteracion (9 hilos por filas)
                parameters *rowData = (parameters *) malloc(sizeof(parameters));
                rowData->row = i;
                rowData->col = j;
                rowData->board = sudoku;
                pthread_create(&threads[threadIndex++], NULL, check_rows, rowData);
            }
            if (i == 0) {// revisa las columans solo en la primera iteracion (9 hilos por columnas)
                parameters *columnData = (parameters *) malloc(sizeof(parameters));
                columnData->row = i;
                columnData->col = j;
                columnData->board = sudoku;
                pthread_create(&threads[threadIndex++], NULL, check_cols, columnData);
            }
        }
    }
    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);// hace que el hilo padre espere a todos los hijos
    for (int i = 0; i < num_threads; i++) {
        if (result[i] == 0) {
            cout << "Con varios hilos: Solución INCORRECTA del sudoku" << endl;
            steady_clock::time_point end_time_threads = steady_clock::now();
            duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);
            cout << endl << "Tiempo total utilizando 27 hilos: " << elapsed_time_threads.count() << " segundos" << endl;
            return 1;
        }
    }
    cout << "Con varios hilos: Solución CORRECTA del sudoku" << endl;
    steady_clock::time_point end_time_threads = steady_clock::now();
    duration<double> elapsed_time_threads = duration_cast<duration<double>>(end_time_threads - start_time_threads);
    cout << endl << "Tiempo total utilizando 27 hilos: " << elapsed_time_threads.count() << " segundos" << endl;
}

//revisa cada bloque de 3x3 llena result con 1 si funciona y 0 si hay alguno repetido
void *check_grid(void * params) {
    parameters *data = (parameters *) params;
    int startRow = data->row;
    int startCol = data->col;
    int validarray[10] = {0};
    for (int i = startRow; i < startRow + 3; ++i) {
        for (int j = startCol; j < startCol + 3; ++j) {
            int val = data->board[i][j];
            if (validarray[val] != 0) {
				// printf("[*check_grid] Error en la cuadricula que inicia en la fila %d, columna %d, \n",startRow,startCol);
                pthread_exit(NULL);
                // si encuentra uno repetido rtermina el hilo
            } else {
                validarray[val] = 1;
			}
        }
    }
    // 1 _ _ _ _ _ _ 
    result[startRow + startCol/3] = 1; 
    pthread_exit(NULL);
}
/*
123 123
456 123
789 123
*/
//llena result con 1 si funciona y 0 si hay alguno repetido
void *check_rows(void *params) {
    parameters *data = (parameters *) params;
    int row = data->row;
    int validarray[10] = {0};
    for (int j = 0; j < 9; j++) {
        int val = data->board[row][j];
        if (validarray[val] != 0) {
			// printf("[*check_rows] Error en la fila %d\n",row);
            pthread_exit(NULL);
		} else {
            validarray[val] = 1;
		}
    }
    result[9 + row] = 1; 
    pthread_exit(NULL);
}
//llena result con 1 si funciona y 0 si hay alguno repetido
void *check_cols(void *params) {
    parameters *data = (parameters *) params;
    int col = data->col;
    int validarray[10] = {0};
    for (int i = 0; i < 9; i++) {
        int val = data->board[i][col];
        if (validarray[val] != 0) {
			// printf("[*check_cols] Error en la columna %d, \n",col);
            pthread_exit(NULL);
		} else {
            validarray[val] = 1;
		}
    }
    result[18 + col] = 1; 
    pthread_exit(NULL);
 }

//valida si en la linea no hay numeros repetido 1 si se repite 0 si no
int check_line(int input[9]) {
    int validarray[10] = {0}; // 0,0,0,0,0,
    for (int i = 0; i < 9; i++) {
        int val = input[i];
        if (validarray[val] != 0)
            return 1;
        else
            validarray[val] = 1;
    }
    return 0;
}

//mira si no estan repetidos los digitos 
int check_grid(int sudoku[9][9]) {
    int temp_row, temp_col;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            temp_row = 3 * i;
            temp_col = 3 * j;
            int validarray[10] = {0};
            for(int p=temp_row; p < temp_row+3; p++) {
                for(int q=temp_col; q < temp_col+3; q++) {
                    int val = sudoku[p][q];
                    if (validarray[val] != 0)
                        return 1;
                    else
                        validarray[val] = 1;
                }
            }
        }
    }
    return 0;
}

int sudoku_checker(int sudoku[9][9]) {
    for (int i=0; i<9; i++) {
        if(check_line(sudoku[i]))// revisa si hay repetidos
            return 1;
        int check_col[9];
        for (int j=0; j<9; j++)// vuelve a copiar la fila en checkcol
            check_col[j] = sudoku[i][j];
        if(check_line(check_col))// revisa si hay repeditos
            return 1;
        if(check_grid(sudoku))// revisa si hay repeditos por grid
            return 1;
    }
    return 0;
}