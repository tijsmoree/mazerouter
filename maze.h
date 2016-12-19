typedef struct {
    int map[MAP_SIZE][MAP_SIZE];
    int x[MAX_ROUTE], y[MAX_ROUTE];
    char d[MAX_ROUTE];
    int i;
    int blow;
    time_t start;
} Maze;

int station(int);

int notExceed(int, int);

int isEdge(int, int);
int isCross(int, int);
int isStation(int, int);

void setDir(Maze *, int);

int stationDistance(Maze *, int, int, int, int);
void rearrangeChks(Maze *, int[], int, int, int);

Maze *createMaze(int);

void resetTime(Maze *);

void goLeft(Maze *);
void goRight(Maze *);
void goForward(Maze *);
void goTurn(Maze *);
void goBackward(Maze *);

void calculateNext(Maze *, int, int);

int countMines(Maze *);
int isExplored(Maze *);
int closestNonExplored(Maze *);

void startRacing(Maze *);

void driveOverMines(Maze *);