#include "declarations.h"

/* returns the factorial of n */
int fact(int n) {
    int nfact;

    for(nfact = 1; n > 0; n--) nfact *= n;

    return nfact;
}

/* creates a double array of dimensions nxm and makes all the members 0 */
int **createDouble(int n, int m) {
    int i, j, **array;

    array = (int**) malloc(n * sizeof(int*));
    for(i = 0; i < n; i++) {
        array[i] = (int*) malloc(m * sizeof(int));

        for(j = 0; j < m; j++)
            array[i][j] = 0;
    }

    return array;
}

/* frees a double array */
void freeDouble(int **array, int n) {
    int i;

    for(i = 0; i < n; i++)
        free(array[i]);
    free(array);
}

/* returns the index of the smallest member of an array */
int indexSmallest(int a[], int n) {
    int i, z = 0, s = a[0];

    for(i = 1; i < n; i++) {
        if(a[i] <= s) {
            s = a[i];
            z = i;
        }
    }

    return z;
}

/* creates an array of all possible index sequences for arrays with lenght n */
int **sequences(int n) {
    int i, j, k, m, **seq = createDouble(1, 1), **tmp;

    for(i = 2; i <= n; i++) {
        tmp = createDouble(fact(i), i);

        for(j = 0; j < i; j++) {
            for(k = 0; k < fact(i - 1); k++) {
                tmp[j * fact(i - 1) + k][0] = j;

                for(m = 0; m < i - 1; m++)
                    tmp[j * fact(i - 1) + k][m + 1] = seq[k][m] + 1 + j;
            }
        }

        seq = createDouble(fact(i), i);

        for(j = 0; j < fact(i); j++)
            for(k = 0; k < i; k++)
                seq[j][k] = tmp[j][k] % i;

        freeDouble(tmp, i);
    }

    return seq;
}

/* prints the maze->map */
void testPrint(Maze *maze) {
    int i, j, k = 0;
    char test[MAP_SIZE][MAP_SIZE];
    char str[MAP_SIZE * (2 * MAP_SIZE) + 1];

    for(i = 0; i < MAP_SIZE; i++) {
        for(j = 0; j < MAP_SIZE; j++) {
            switch(maze->map[i][j]) {
                case  0: test[i][j] = '.'; break;
                case -1: test[i][j] = ' '; break;
                case -2: test[i][j] = 'X'; break;
                case -3: test[i][j] = 'O'; break;
            }
        }
    }

    for(i = 0; i < maze->i; i++)
        if(test[maze->x[i]][maze->y[i]] != 'X')
            test[maze->x[i]][maze->y[i]] = '+';

    switch(maze->d[i - 1]) {                    /* the robot is advised to go to the arrow */
        case 'l': test[maze->x[i - 1]][maze->y[i - 1]] = '<'; break;
        case 'r': test[maze->x[i - 1]][maze->y[i - 1]] = '>'; break;
        case 'u': test[maze->x[i - 1]][maze->y[i - 1]] = '^'; break;
        case 'd': test[maze->x[i - 1]][maze->y[i - 1]] = 'v'; break;
    }

    test[maze->x[i - 2]][maze->y[i - 2]] = 'o'; /* the robot has just passed the o and is now communicating */

    for(i = MAP_SIZE - 1; i >= 0; i--) {
        for(j = 0; j < MAP_SIZE; j++) {
            str[k] = test[j][i]; k++;
            str[k] = ' '; k++;
        }
        str[k] = '\n'; k++;
    }

    str[k] = '\0';

    printf("\n%s\n", str);
}

/* checks whether time due is over since the robot started driving */
int isTime(Maze *maze, int due) {
    int diff = 0;

    diff = (int) difftime(time(NULL), maze->start);

    return diff > due;
}

/* prints a status bar for the time */
void printTime(Maze *maze, int due) {
    int diff = (int) difftime(time(NULL), maze->start);
    
    int chars = 2 * MAP_SIZE - 3, i;
    int gone = chars;
    
    if(diff < due) {
        gone = (int) ((double) diff) / ((double) due) * ((double) chars);
    }

    printf("[");
    for(i = 0; i < gone; i++)
        printf("#");
    for(i = gone; i < chars; i++)
        printf("-");
    printf("]\n\n");
}