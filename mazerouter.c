#define MAX_ROUTE 150

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

int time();

typedef struct {
    int map[9][9];
    int x[MAX_ROUTE], y[MAX_ROUTE];
    int i;
    char d[MAX_ROUTE];
} Maze;

int fact(int n) {
    int nfact;

    for(nfact = 1; n > 0; n--) nfact *= n;

    return nfact;
}

int station(int s) {
    int c;

    switch(s) {
        case 1:  c = 20; break;
        case 2:  c = 40; break;
        case 3:  c = 60; break;
        case 4:  c = 82; break;
        case 5:  c = 84; break;
        case 6:  c = 86; break;
        case 7:  c = 68; break;
        case 8:  c = 48; break;
        case 9:  c = 28; break;
        case 10: c = 6;  break;
        case 11: c = 4;  break;
        case 12: c = 2;  break;
    }

    return c;
}

int notExceed(int x, int y) {
    return x >= 0 && x < 9 && y >= 0 && y < 9;
}

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

void freeDouble(int **array, int n) {
    int i;

    for(i = 0; i < n; i++)
        free(array[i]);
    free(array);
}

void setDir(Maze *maze, int s) {
    if(s < 4)           maze->d[maze->i] = 'u';
    if(s > 9)           maze->d[maze->i] = 'r';
    if(s > 3 && s < 7)  maze->d[maze->i] = 'l';
    if(s > 6 && s < 10) maze->d[maze->i] = 'd';
}

int stationDistance(Maze *maze, int xi, int yi, int xe, int ye) {
    int i, j, p = 1, map[9][9];

    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            map[i][j] = maze->map[i][j];

    for(map[xe][ye] = p; !map[xi][yi]; p++) {
        for(i = 0; i < 9; i++) {
            for(j = 0; j < 9; j++) {
                if(map[i][j]) continue;

                if(i != 8 && map[i + 1][j] == p) map[i][j] = p + 1;
                if(i != 0 && map[i - 1][j] == p) map[i][j] = p + 1;
                if(j != 8 && map[i][j + 1] == p) map[i][j] = p + 1;
                if(j != 0 && map[i][j - 1] == p) map[i][j] = p + 1;
            }
        }
    }

    return p;
}

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

void swapDir(Maze *maze) {
    switch(maze->d[maze->i - 1]) {
        case 'u': maze->d[maze->i] = 'd'; break;
        case 'd': maze->d[maze->i] = 'u'; break;
        case 'l': maze->d[maze->i] = 'r'; break;
        case 'r': maze->d[maze->i] = 'l'; break;
    }
}

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

void rearrangeChks(Maze *maze, int chks[], int n, int x, int y) {
    int i, j, xi, yi, xe, ye;
    int **routes = sequences(n), *sum, *tmp;

    sum = malloc(fact(n) * sizeof(int));
    tmp = malloc(n * sizeof(int));

    for(i = 0; i < fact(n); i++) {
        xi = x;
        yi = y;
        xe = station(chks[routes[i][0]]) / 10;
        ye = station(chks[routes[i][0]]) % 10;

        sum[i] = stationDistance(maze, xi, yi, xe, ye);

        for(j = 1; j < n; j++) {
            xi = station(chks[routes[i][j - 1]]) / 10;
            yi = station(chks[routes[i][j - 1]]) % 10;
            xe = station(chks[routes[i][j]]) / 10;
            ye = station(chks[routes[i][j]]) % 10;

            sum[i] += stationDistance(maze, xi, yi, xe, ye);
        }
    }

    i = indexSmallest(sum, fact(n));

    for(j = 0; j < n; j++)
        tmp[j] = chks[routes[i][j]];

    for(j = 0; j < n; j++)
        chks[j] = tmp[j];
}

Maze *createMaze(int si) {
    Maze *maze = malloc(sizeof(Maze));
    int i, j;

    maze->i = 0;

    /* initialize the maze at 0 */
    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            maze->map[i][j] = 0;

    /* make the empty spots - 1 */
    for(i = 1; i < 9; i += 2)
        for(j = 1; j < 9; j += 2)
            maze->map[i][j] = -1;

    maze->x[0] = station(si) / 10;
    maze->y[0] = station(si) % 10;

    for(i = 1; i < MAX_ROUTE; i++) maze->x[i] = 0, maze->y[i] = 0;

    setDir(maze, si); printf("%c\n", maze->d[0]);

    maze->i++;

    return maze;
}

void goLeft(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'l';
            x--;
        case 'd':
            maze->d[this] = 'r';
            x++;
        case 'r':
            maze->d[this] = 'u';
            y++;
        case 'l':
            maze->d[this] = 'd';
            y--;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

void goRight(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'r';
            x++;
        case 'd':
            maze->d[this] = 'l';
            x--;
        case 'r':
            maze->d[this] = 'd';
            y--;
        case 'l':
            maze->d[this] = 'u';
            y++;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

void goForward(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'u';
            y++;
        case 'd':
            maze->d[this] = 'd';
            y--;
        case 'r':
            maze->d[this] = 'r';
            x++;
        case 'l':
            maze->d[this] = 'l';
            x--;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    (maze->i)++;
}

void goBack(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'd';
            y--;
        case 'd':
            maze->d[this] = 'u';
            y++;
        case 'r':
            maze->d[this] = 'l';
            x--;
        case 'l':
            maze->d[this] = 'r';
            x++;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

void testPrint(Maze *maze) {
    int i, j;
    char test[9][9];

    for(i = 0; i < 9; i++) {
        for(j = 0; j < 9; j++) {
            switch(maze->map[i][j]) {
                case 0: test[i][j] = '.'; break;
                case -1: test[i][j] = 'X'; break;
            }
        }
    }

    for(i = 1; i < 9; i += 2)
        for(j = 1; j < 9; j += 2)
            test[i][j] = ' ';

    for(i = 0; i < maze->i; i++)
        test[maze->x[i]][maze->y[i]] = '+';

    for(i = 8; i >= 0; i--) {
        for(j = 0; j < 9; j++) {
            printf("%c ", test[j][i]);
        }
        printf("\n");
    }

    for(i = 0; i < maze->i; i++)
        printf("%c ", maze->d[i]);
}

void calculateNext(Maze *maze, int xe, int ye) {
    int i, j, p = 1, map[9][9], ind = maze->i;

    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            map[i][j] = maze->map[i][j];

    for(map[xe][ye] = p; !map[maze->x[ind - 1]][maze->y[ind - 1]]; p++) {
        for(i = 0; i < 9; i++) {
            for(j = 0; j < 9; j++) {
                if(map[i][j]) continue;

                if(i != 8 && map[i + 1][j] == p) map[i][j] = p + 1;
                if(i != 0 && map[i - 1][j] == p) map[i][j] = p + 1;
                if(j != 8 && map[i][j + 1] == p) map[i][j] = p + 1;
                if(j != 0 && map[i][j - 1] == p) map[i][j] = p + 1;
            }
        }
    }

    for(i = 8; i >= 0; i--) {
        for(j = 0; j < 9; j++) {
            printf("%d\t", map[j][i]);
        }
        printf("\n");
    }printf("%d\n", maze->i);

    i = maze->x[ind - 1]; j = maze->y[ind - 1]; p--;

    if       (maze->d[maze->i - 1] == 'r') {
        if     (i != 8 && map[i + 1][j] == p) goForward(maze);
        else if(j != 0 && map[i][j - 1] == p) goRight(maze);
        else if(j != 8 && map[i][j + 1] == p) goLeft(maze);
        else if(j != 0 && map[i][j - 1] == p) goBack(maze);
    } else if(maze->d[maze->i - 1] == 'd') {
        if     (j != 0 && map[i][j - 1] == p) goForward(maze);
        else if(i != 0 && map[i - 1][j] == p) goRight(maze);
        else if(i != 8 && map[i + 1][j] == p) goLeft(maze);
        else if(i != 0 && map[i - 1][j] == p) goBack(maze);
    } else if(maze->d[maze->i - 1] == 'u') {
        if     (j != 8 && map[i][j + 1] == p) goForward(maze);
        else if(i != 8 && map[i + 1][j] == p) goRight(maze);
        else if(i != 0 && map[i - 1][j] == p) goLeft(maze);
        else if(i != 8 && map[i + 1][j] == p) goBack(maze);
    } else if(maze->d[maze->i - 1] == 'l') {
        if     (i != 0 && map[i - 1][j] == p) goForward(maze);
        else if(j != 8 && map[i][j + 1] == p) goRight(maze);
        else if(j != 0 && map[i][j - 1] == p) goLeft(maze);
        else if(j != 8 && map[i][j + 1] == p) goBack(maze);
    }

    testPrint(maze); getch();
}

void createMines(int map[9][9], int n) {
    int i, j, p, *x, *y;

    x = malloc(n * sizeof(int));
    y = malloc(n * sizeof(int));

    for(i = 0; i < n; i++) {
        p = 1;
        while(p) {
            x[i] = rand() % 9;
            y[i] = rand() % 9;
            p = 0;

            if(!((x[i] + y[i]) % 2)) p = 1;

            for(j = 0; j < i; j++) {
                if(x[i] == x[j] && y[i] == y[j]) p = 1;
            }
        }

        map[x[i]][y[i]] = -1;
    }
}

int isExplored(Maze *maze) {
    int i, j, e = 0, map[9][9];

    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            map[i][j] = maze->map[i][j];

    for(i = 0; i < maze->i; i++)
        map[maze->x[i]][maze->y[i]] = 1;

    for(i = 0; i < 9; i++) 
        for(j = 0; j < 9; j++)
            e += !map[i][j];

    return !e;
}

int closestNonExplored(Maze *maze) {
    int d = 20, dCurrent, i, j, x, y, map[9][9];

    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            map[i][j] = maze->map[i][j];

    for(i = 0; i < maze->i; i++)
        map[maze->x[i]][maze->y[i]] = 1;

    /*for(i = 8; i >= 0; i--) {
        for(j = 0; j < 9; j++) {
            printf("%d\t", map[j][i]);
        }
        printf("\n");
    }*/

    x = maze->x[maze->i - 1];
    y = maze->y[maze->i - 1];
    switch(maze->d[maze->i - 1]) {
        case 'u': y++;
        case 'd': y--;
        case 'r': x++;
        case 'l': x--;
    }

    if(!(notExceed(x, y) && map[x][y] == 0)) {
        for(i = 0; i < 9; i++) {
            for(j = 0; j < 9; j++) {
                dCurrent = stationDistance(maze, maze->x[maze->i - 1], maze->y[maze->i - 1], i, j);
                if(dCurrent < d && map[i][j] == 0) {
                    d = dCurrent;
                    x = i;
                    y = j;
                }
            }
        }
    }

    return x * 10 + y;
}

void askChks(int chks[], int n) {
    int i;

    for(i = 0; i < n; i++) {
        scanf("%d", chks + i);

        while(chks[i] < 1 || chks[i] > 12) {
            printf("\n%d is no valid station!\n", chks[i]);
            scanf("%d", chks + i);
        }
    }
}

int askMine(Maze *maze) {
    int a = 0;

    if((maze->x[maze->i-1] + maze->y[maze->i-1]) % 2) {
        testPrint(maze);
        printf("Mine (1/0)? ");
        scanf("%d", &a);
        printf("\n");
    }

    return a;
}

void a() {
    int i, si = 1, chks[3] = {5, 7, 11}, xe, ye;
    Maze *maze;

    askChks(chks, 3);

    maze = createMaze(si);

    rearrangeChks(maze, chks, 3, station(si) / 10, station(si) % 10);

    for(i = 0; i < 3; i++) {
        xe = station(chks[i]) / 10;
        ye = station(chks[i]) % 10;
        
        while((maze->x[maze->i-1] != xe) || (maze->y[maze->i-1] != ye)) {
            calculateNext(maze, xe, ye);
            printf("%d %d %d %d\n", xe, ye, maze->x[maze->i-1], maze->y[maze->i-1]);
        }

        goBack(maze);
    }

    testPrint(maze);
}

void b() {
    int c, si = 1, chks[3] = {11, 7, 5}, xe, ye;
    Maze *maze = createMaze(si);

    askChks(chks, 3);

    rearrangeChks(maze, chks, 3, station(si) / 10, station(si) % 10);

    for(c = 0; c < 3; c++) {
        xe = station(chks[c]) / 10;
        ye = station(chks[c]) % 10;

        while((maze->x[maze->i-1] != xe) || (maze->y[maze->i-1] != ye)) {
            calculateNext(maze, xe, ye);

            if(askMine(maze)) {
                maze->i--;
                swapDir(maze);
                maze->map[maze->x[maze->i]][maze->y[maze->i]] = -1;
                rearrangeChks(maze, chks + c, 3 - c, maze->x[maze->i - 1], maze->y[maze->i - 1]);

                xe = station(chks[c]) / 10;
                ye = station(chks[c]) % 10;
            }
        }

        printf("Checkpoint %d reached!\n", chks[c]);

        maze->i--;
        setDir(maze, chks[c]);
        maze->i++;
    }

    testPrint(maze);
}

void c() {
    int i, j, xy, mines[9][9];
    Maze *maze = createMaze(1);

    for(i = 0; i < 9; i++)
        for(j = 0; j < 9; j++)
            mines[i][j] = maze->map[i][j];

    createMines(mines, 13);

    /*for(i = 8; i >= 0; i--) {
        for(j = 0; j < 9; j++) {
            printf("%d\t", mines[j][i]);
        }
        printf("\n");
    }*/

    /*for(i = 0; i < 9; i++) for(j = 0; j < 9; j++) if(maze->map[i][j] == 0) maze->map[i][j] = 1;*/

    while(!isExplored(maze)) {
        xy = closestNonExplored(maze);

        calculateNext(maze, xy / 10, xy % 10);

        if(mines[maze->x[maze->i - 1]][maze->y[maze->i - 1]] == -1) {
            maze->i -= 2;
            swapDir(maze);
            maze->i++;
            maze->map[maze->x[maze->i]][maze->y[maze->i]] = -1;
        }

        /*if(maze->x[maze->i] == 0 && maze->y[maze->i] == 0)*/ getchar();

        /*testPrint(maze);*/
    }

    testPrint(maze);
}

int main() {
    int t;
    char w = 'c';

    srand((unsigned) time(&t));

    printf("Challenge? ");
    scanf("%c", &w);
    printf("\n");

    if(w == 'a') {
        a();
    } else if(w == 'b') {
        b();
    } else if(w == 'c') {
        c();
    }

    getch();

    return 0;
}
