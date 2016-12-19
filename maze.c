#include "declarations.h"

/* this function returns the coordinates of a given station in the form of xy */
/* this way the x can be read via xy / MAP_SIZE and y via xy % MAP_SIZE       */
int station(int s) {
    int c;

    switch(s) {
        case 1:  c = 3  * MAP_SIZE +  0; break;
        case 2:  c = 5  * MAP_SIZE +  0; break;
        case 3:  c = 7  * MAP_SIZE +  0; break;
        case 4:  c = 10 * MAP_SIZE +  3; break;
        case 5:  c = 10 * MAP_SIZE +  5; break;
        case 6:  c = 10 * MAP_SIZE +  7; break;
        case 7:  c = 7  * MAP_SIZE + 10; break;
        case 8:  c = 5  * MAP_SIZE + 10; break;
        case 9:  c = 3  * MAP_SIZE + 10; break;
        case 10: c = 0  * MAP_SIZE +  7; break;
        case 11: c = 0  * MAP_SIZE +  5; break;
        case 12: c = 0  * MAP_SIZE +  3; break;
    }

    return c;
}

/* this function checks whether the given coordinates are within the field without the stations */
int notExceed(int x, int y) {
    return x >= 1 && x < MAP_SIZE - 1 && y >= 1 && y < MAP_SIZE - 1;
}

/* this function returns whether a given pair of coordinates an edge is */
int isEdge(int x, int y) {
    return ((x + y) % 2 && notExceed(x, y));
}

/* this function returns whether a given pair of coordinates a crossingpoint is */
int isCross(int x, int y) {
    return (x % 2 && y % 2 && !((x + y) % 2) && notExceed(x, y));
}

/* this function returns whether a given pair of coordinates a station is */
int isStation(int x, int y) {
    int s, i = 0;

    for(s = 1; s < 13; s++)
        if(station(s) == x * MAP_SIZE + y)
            i = 1;

    return i;
}

/* this function gives the amount of spots it takes for the robot to drive      */
/* without hitting the mines from a given pair to a given pair of coordinates   */
int stationDistance(Maze *maze, int xi, int yi, int xe, int ye) {
    int i, j, p = 1, map[MAP_SIZE][MAP_SIZE];

    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            map[i][j] = maze->map[i][j];

    for(map[xe][ye] = p; !map[xi][yi]; p++) {
        for(i = 0; i < MAP_SIZE; i++) {
            for(j = 0; j < MAP_SIZE; j++) {
                if(map[i][j]) continue;

                if(i != MAP_SIZE - 1 && map[i + 1][j] == p) map[i][j] = p + 1;
                if(i != 0            && map[i - 1][j] == p) map[i][j] = p + 1;
                if(j != MAP_SIZE - 1 && map[i][j + 1] == p) map[i][j] = p + 1;
                if(j != 0            && map[i][j - 1] == p) map[i][j] = p + 1;
            }
        }

        if(p > 50) break;
    }

    return p;
}

/* this function rearranges a given array of n checkpoints, so it will be the fastest route to x, y */
void rearrangeChks(Maze *maze, int chks[], int n, int x, int y) {
    int i, j, xi, yi, xe, ye;
    int **routes = sequences(n), *sumOfRoutes, *tmp;

    sumOfRoutes = malloc(fact(n) * sizeof(int));
    tmp = malloc(n * sizeof(int));

    for(i = 0; i < fact(n); i++) {
        xi = x;
        yi = y;
        xe = station(chks[routes[i][0]]) / MAP_SIZE;
        ye = station(chks[routes[i][0]]) % MAP_SIZE;

        sumOfRoutes[i] = stationDistance(maze, xi, yi, xe, ye);

        for(j = 1; j < n; j++) {
            xi = station(chks[routes[i][j - 1]]) / MAP_SIZE;
            yi = station(chks[routes[i][j - 1]]) % MAP_SIZE;
            xe = station(chks[routes[i][j]]) / MAP_SIZE;
            ye = station(chks[routes[i][j]]) % MAP_SIZE;

            sumOfRoutes[i] += stationDistance(maze, xi, yi, xe, ye);
        }
    }

    i = indexSmallest(sumOfRoutes, fact(n));

    for(j = 0; j < n; j++)
        tmp[j] = chks[routes[i][j]];

    for(j = 0; j < n; j++)
        chks[j] = tmp[j];
}

/* this function returns a pointer to a Maze, based on the begin station
    maze->map  : a twodimensional array filled with 0, -1 and -2 so the program knows where it can ride safely
    maze->x    : an array of the x coordinates of the route so far
    maze->y    : an array of the y coordinates of the route so far
    maze->d    : an array of the directions in which the robot has driven so far
    maze->i    : the index of the new xyd-combination
    maze->start: the time the robot started driving
    maze->blow : this decides whether the robot should blow up the mine or turn around when seeing one */
Maze *createMaze(int si) {
    Maze *maze = malloc(sizeof(Maze));
    int i, j;

    maze->start = time(NULL);
    maze->i = 0;
    maze->blow = 0;

    /* make the whole maze -1 */
    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            maze->map[i][j] = -1;

    /* all but the outer border is set to 0 */
    for(i = 1; i < MAP_SIZE - 1; i++)
        for(j = 1; j < MAP_SIZE - 1; j++)
            maze->map[i][j] = 0;

    /* the spots in between edges is set to -1 */
    for(i = 2; i < MAP_SIZE - 2; i += 2)
        for(j = 2; j < MAP_SIZE - 2; j += 2)
            maze->map[i][j] = -1;

    /* the stations are set to 0 */
    for(i = 1; i <= 12; i++)
        maze->map[station(i) / MAP_SIZE][station(i) % MAP_SIZE] = 0;

    /* the starting point is set to station si */
    maze->x[0] = station(si) / MAP_SIZE;
    maze->y[0] = station(si) % MAP_SIZE;

    /* all members of arrays x and y are set to 0 */
    for(i = 1; i < MAX_ROUTE; i++) maze->x[i] = 0, maze->y[i] = 0;

    /* the first direction is set to the direction of station si */
    if(si < 4)            maze->d[maze->i] = 'u';
    if(si > 9)            maze->d[maze->i] = 'r';
    if(si > 3 && si < 7)  maze->d[maze->i] = 'l';
    if(si > 6 && si < 10) maze->d[maze->i] = 'd';

    maze->i++;

    return maze;
}

/* reset the time of a maze */
void resetTime(Maze *maze) {
    maze->start = time(NULL);
}

/* this function makes the Maze file go left */
void goLeft(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'l';
            if(x != 0) x--;
            break;
        case 'd':
            maze->d[this] = 'r';
            if(x != MAP_SIZE - 1) x++;
            break;
        case 'r':
            maze->d[this] = 'u';
            if(y != MAP_SIZE - 1) y++;
            break;
        case 'l':
            maze->d[this] = 'd';
            if(y != 0) y--;
            break;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

/* this function makes the Maze file go right */
void goRight(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'r';
            if(x != MAP_SIZE - 1) x++;
            break;
        case 'd':
            maze->d[this] = 'l';
            if(x != 0) x--;
            break;
        case 'r':
            maze->d[this] = 'd';
            if(y != 0) y--;
            break;
        case 'l':
            maze->d[this] = 'u';
            if(y != MAP_SIZE - 1) y++;
            break;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

/* this function makes the Maze file go forward */
void goForward(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'u';
            if(y != MAP_SIZE - 1) y++;
            break;
        case 'd':
            maze->d[this] = 'd';
            if(y != 0) y--;
            break;
        case 'r':
            maze->d[this] = 'r';
            if(x != MAP_SIZE - 1) x++;
            break;
        case 'l':
            maze->d[this] = 'l';
            if(x != 0) x--;
            break;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    (maze->i)++;
}

/* this function makes the Maze file turn */
void goTurn(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'd';
            if(y != 0) y--;
            break;
        case 'd':
            maze->d[this] = 'u';
            if(y != MAP_SIZE - 1) y++;
            break;
        case 'r':
            maze->d[this] = 'l';
            if(x != 0) x--;
            break;
        case 'l':
            maze->d[this] = 'r';
            if(x != MAP_SIZE - 1) x++;
            break;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

/* this function makes the Maze file go backward */
void goBackward(Maze *maze) {
    int last = maze->i - 1, this = maze->i;
    int x = maze->x[last], y = maze->y[last];

    switch(maze->d[last]) {
        case 'u':
            maze->d[this] = 'u';
            if(y != 0) y--;
            break;
        case 'd':
            maze->d[this] = 'd';
            if(y != MAP_SIZE - 1) y++;
            break;
        case 'r':
            maze->d[this] = 'r';
            if(x != 0) x--;
            break;
        case 'l':
            maze->d[this] = 'l';
            if(x != MAP_SIZE - 1) x++;
            break;
    }

    maze->x[this] = x;
    maze->y[this] = y;

    maze->i++;
}

/* this function makes the next move towards xe, ye using the Lee algorithm */
void calculateNext(Maze *maze, int xe, int ye) {
    int i, j, p = 1, map[MAP_SIZE][MAP_SIZE], ind = maze->i, blowup = BLOW * maze->blow;

    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            map[i][j] = maze->map[i][j];

    for(map[xe][ye] = p; !map[maze->x[ind - 1]][maze->y[ind - 1]]; p++) {
        for(i = 0; i < MAP_SIZE; i++) {
            for(j = 0; j < MAP_SIZE; j++) {
                if(map[i][j]) continue;

                if(i != MAP_SIZE - 1 && map[i + 1][j] == p) map[i][j] = p + 1;
                if(i != 0            && map[i - 1][j] == p) map[i][j] = p + 1;
                if(j != MAP_SIZE - 1 && map[i][j + 1] == p) map[i][j] = p + 1;
                if(j != 0            && map[i][j - 1] == p) map[i][j] = p + 1;
            }
        }

        if(p > 50) printf("Something goes wrong in the maze struct...\n");
    }

    i = maze->x[ind - 1]; j = maze->y[ind - 1]; p--;

    if       (maze->d[maze->i - 1] == 'r') {
        if     (i != MAP_SIZE - 1 && map[i + 1][j] == p) goForward(maze),   sendByte(FORW + blowup);
        else if(j != 0            && map[i][j - 1] == p) goRight(maze),     sendByte(RGHT + blowup);
        else if(j != MAP_SIZE - 1 && map[i][j + 1] == p) goLeft(maze),      sendByte(LEFT + blowup);
        else if(i != 0            && map[i - 1][j] == p) goTurn(maze),      sendByte(TURN + blowup);
    } else if(maze->d[maze->i - 1] == 'd') {
        if     (j != 0            && map[i][j - 1] == p) goForward(maze),   sendByte(FORW + blowup);
        else if(i != 0            && map[i - 1][j] == p) goRight(maze),     sendByte(RGHT + blowup);
        else if(i != MAP_SIZE - 1 && map[i + 1][j] == p) goLeft(maze),      sendByte(LEFT + blowup);
        else if(j != MAP_SIZE - 1 && map[i][j + 1] == p) goTurn(maze),      sendByte(TURN + blowup);
    } else if(maze->d[maze->i - 1] == 'u') {
        if     (j != MAP_SIZE - 1 && map[i][j + 1] == p) goForward(maze),   sendByte(FORW + blowup);
        else if(i != MAP_SIZE - 1 && map[i + 1][j] == p) goRight(maze),     sendByte(RGHT + blowup);
        else if(i != 0            && map[i - 1][j] == p) goLeft(maze),      sendByte(LEFT + blowup);
        else if(j != 0            && map[i][j - 1] == p) goTurn(maze),      sendByte(TURN + blowup);
    } else if(maze->d[maze->i - 1] == 'l') {
        if     (i != 0            && map[i - 1][j] == p) goForward(maze),   sendByte(FORW + blowup);
        else if(j != MAP_SIZE - 1 && map[i][j + 1] == p) goRight(maze),     sendByte(RGHT + blowup);
        else if(j != 0            && map[i][j - 1] == p) goLeft(maze),      sendByte(LEFT + blowup);
        else if(i != MAP_SIZE - 1 && map[i + 1][j] == p) goTurn(maze),      sendByte(TURN + blowup);
    }

    testPrint(maze);
}

/* this function returns the amount of mines on the maze->map */
int countMines(Maze *maze) {
    int i, j, n = 0;

    for(i = 0; i < MAP_SIZE; i++)
        for (j = 0; j < MAP_SIZE; j++)
            if(!maze->map[i][j]) n++;

    return 77 - n;
}

/* this function checks whether the maze is explored */
/* and will even check if there are as many mines left to explore as */
/* there are undiscovered spots left on the maze->map and */
/* will fill these mines in in the maze */
int isExplored(Maze *maze) {
    int i, j, e = 0, map[MAP_SIZE][MAP_SIZE];

    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            map[i][j] = maze->map[i][j];

    for(i = 1; i <= 12; i++)
        map[station(i) / MAP_SIZE][station(i) % MAP_SIZE] = 1;

    for(i = 1; i < MAP_SIZE; i += 2)
        for(j = 1; j < MAP_SIZE; j += 2)
            map[i][j] = 1;

    for(i = 0; i < maze->i - 1; i++)
        map[maze->x[i]][maze->y[i]] = 1;

    for(i = 0; i < MAP_SIZE; i++) 
        for(j = 0; j < MAP_SIZE; j++)
            e += !map[i][j];

    if(13 - countMines(maze) == e) {
        e = 0;

        for(i = 0; i < MAP_SIZE; i++) 
            for(j = 0; j < MAP_SIZE; j++)
                if(!map[i][j])
                    maze->map[i][j] = -2;
    }

    if(countMines(maze) == 13) e = 0;

    return !e;
}

/* this function returns the coordinates of the nearest undiscovered spot on the map */
/* and will try to go forward when possible */
int closestNonExplored(Maze *maze) {
    int d = 1, dCurrent, i, j, x, y, map[MAP_SIZE][MAP_SIZE];

    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            map[i][j] = maze->map[i][j];

    for(i = 0; i < maze->i; i++)
        map[maze->x[i]][maze->y[i]] = 1;

    for(i = 1; i < MAP_SIZE; i += 2)
        for(j = 1; j < MAP_SIZE; j += 2)
            map[i][j] = 1;

    for(i = 1; i <= 12; i++)
        map[station(i) / MAP_SIZE][station(i) % MAP_SIZE] = -1;

    x = maze->x[maze->i - 1];
    y = maze->y[maze->i - 1];
    
    if(isEdge(x, y)) d = 2;
    
    switch(maze->d[maze->i - 1]) {
        case 'u': y += d; break;
        case 'd': y -= d; break;
        case 'r': x += d; break;
        case 'l': x -= d; break;
    }

    d = 50;
    
    if(!(notExceed(x, y) && map[x][y] == 0)) {
        for(i = 0; i < MAP_SIZE; i++) {
            for(j = 0; j < MAP_SIZE; j++) {
                if(map[i][j]) continue;

                dCurrent = stationDistance(maze, maze->x[maze->i - 1], maze->y[maze->i - 1], i, j);

                switch(maze->d[maze->i - 1]) {
                    case 'u': if(j < maze->y[maze->i - 1]) dCurrent++; break;
                    case 'd': if(j > maze->y[maze->i - 1]) dCurrent++; break;
                    case 'r': if(i < maze->x[maze->i - 1]) dCurrent++; break;
                    case 'l': if(i > maze->x[maze->i - 1]) dCurrent++; break; 
                }

                if(dCurrent < d && map[i][j] == 0) {
                    d = dCurrent;
                    x = i;
                    y = j;
                }
            }
        }
    }

    if(dCurrent == 50) printf("Something goes wrong in the maze struct...\n");

    return x * MAP_SIZE + y;
}

/* this function sets blow to 1, this makes the robot blow up mines, in stead of turning around automatically */
void driveOverMines(Maze *maze) {
    maze->blow = 1;
}

/* start the racing */
void startRacing(Maze *maze) {
    resetTime(maze);
    sendByte(STRT + BLOW * maze->blow);
}