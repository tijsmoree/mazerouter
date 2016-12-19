#include "declarations.h"

/* challenge A */
void a(int *chks, int amountChks) {
    int c = 0, b, si = 1, xe, ye, t = 120;
    Maze *maze = createMaze(si);

    startRacing(maze);

    /* rearranges the checkpoints in the fastest order */
    rearrangeChks(maze, chks, amountChks, station(si) / MAP_SIZE, station(si) % MAP_SIZE);

    /* this will make the c-code catch up with the automatic first step of the robot */
    goForward(maze);
    testPrint(maze);
    printTime(maze, t);

    /* send first order to the robot */
    xe = station(chks[c]) / MAP_SIZE;
    ye = station(chks[c]) % MAP_SIZE;
    calculateNext(maze, xe, ye);
    printTime(maze, t);

    /* go to three checkpoints and assign xe and ye as the coordinates of the checkpoint */
    for(c = 0; c < amountChks; c++) {
        xe = station(chks[c]) / MAP_SIZE;
        ye = station(chks[c]) % MAP_SIZE;
        
        /* it will loop while the robot hasn't reached the checkpoint yet or the time is over */
        while((maze->x[maze->i - 2] != xe || maze->y[maze->i - 2] != ye) && !isTime(maze, t)) {
            b = receiveByte();

            /* send the robot the move it has to do when it gets to the next crossing point, station or midedge section */
            if(b == BLCK || b == WHTE)
                calculateNext(maze, xe, ye);

            printTime(maze, t);
        }

        /* checks whether the time is over */
        if(isTime(maze, t)) {
            printf("I'm out of time, sir!\n");
            break;
        }

        printf("Reached checkpoint %d, sir!\n", chks[c]);
    }

    sendByte(STOP);

    printf("We finished in %d seconds, sir!\n", (int) difftime(time(NULL), maze->start));
}

/* challenge B */
void b(int *chks, int amountChks) {
    int c = 0, si = 1, xe, ye, b = 0, t = 180;
    Maze *maze = createMaze(si);

    startRacing(maze);

    /* rearranges the checkpoints in the fastest order */
    rearrangeChks(maze, chks, amountChks, station(si) / MAP_SIZE, station(si) % MAP_SIZE);

    /* this will make the c-code catch up with the automatic first step of the robot */
    goForward(maze);
    testPrint(maze);
    printTime(maze, t);

    /* send first order to the robot */
    xe = station(chks[c]) / MAP_SIZE;
    ye = station(chks[c]) % MAP_SIZE;
    calculateNext(maze, xe, ye);
    printTime(maze, t);

    /* go to three checkpoints and assign xe and ye as the coordinates of the checkpoint */
    for(c = 0; c < amountChks; c++) {
        xe = station(chks[c]) / MAP_SIZE;
        ye = station(chks[c]) % MAP_SIZE;

        /* it will loop while the robot hasn't reached the checkpoint yet or the time is over */
        while((maze->x[maze->i - 2] != xe || maze->y[maze->i - 2] != ye) && !isTime(maze, t)) {
            b = receiveByte();

            /* send the robot the move it has to do when it gets to the next crossing point or midedge section */
            if(b == BLCK || b == WHTE) {
                calculateNext(maze, xe, ye);
            
            /* when the robot detects a mine it will automatically go backwards   
               while doing this it will communicate to the computer it ran into   
               a mine and the c-code will erase the planned step that the robot   
               had to do if there had not been a mine; the c-code also saves      
               that the robot automatically went back to the crosspoint and it    
               will save the mine to the maze->map                              */
            } else if(b == MINE) {
                maze->i--;
                maze->map[maze->x[maze->i - 1]][maze->y[maze->i - 1]] = -2;
                
                goTurn(maze);
                testPrint(maze);
                
                /* the sequence of the checkpoints the robot will visit is attenuated */
                rearrangeChks(maze, chks + c, amountChks - c, maze->x[maze->i - 1], maze->y[maze->i - 1]);
                xe = station(chks[c]) / MAP_SIZE;
                ye = station(chks[c]) % MAP_SIZE;

                calculateNext(maze, xe, ye);
            }

            printTime(maze, t);
        }

        /* the time is being checked */
        if(isTime(maze, t)) {
            printf("I'm out of time, sir!\n");
            break;
        }

        printf("Reached checkpoint %d, sir!\n", chks[c]);
    }

    sendByte(STOP);

    printf("We finished in %d seconds, sir!\n", (int) difftime(time(NULL), maze->start));
}

/* challenge C */
void c() {
    int i, j, xy, b, t = 270, treasure = IDNO, minesDetonated = 0, mustBeTreasure = 0;
    Maze *exploration = createMaze(1), *hunt = createMaze(1);

    /* the counter is reset and the start-signal is sent to the robot */
    startRacing(exploration);

    /* this will make the c-code catch up with the automatic first step of the robot */
    goForward(exploration);
    testPrint(exploration);
    printTime(exploration, t);

    /* send first order to the robot */
    xy = closestNonExplored(exploration);
    calculateNext(exploration, xy / MAP_SIZE, xy % MAP_SIZE);

    /* the exploration will go on untill the exploration->map is fully explored or the time is over */
    while(!isExplored(exploration) && !isTime(exploration, t)) {
        xy = closestNonExplored(exploration);

        b = receiveByte();

        /* send the robot the move it has to do when it gets to the next crossing point or midedge section */
        if(b == BLCK || b == WHTE) {
            calculateNext(exploration, xy / MAP_SIZE, xy % MAP_SIZE);
        
        /* when the robot detects a mine it will automatically go backwards   
           while doing this it will communicate to the computer it ran into   
           a mine and the c-code will erase the planned step that the robot   
           had to do if there had not been a mine; the c-code also saves      
           that the robot automatically went back to the crosspoint and it    
           will save the mine to the maze->map                              */
        } else if(b == MINE) {
            exploration->i--;
            exploration->map[exploration->x[exploration->i - 1]][exploration->y[exploration->i - 1]] = -2;
            
            goTurn(exploration);

            xy = closestNonExplored(exploration);
            calculateNext(exploration, xy / MAP_SIZE, xy % MAP_SIZE);
        }

        printTime(exploration, t);
    }

    printf("I discovered %d mines, so there are %d left, sir!\n", countMines(exploration), 13 - countMines(exploration));

    sendByte(STOP);

    /* the hunt starts here */

    MessageBox(NULL, "Put me back at station 1 and press OK, sir.", "Back?", MB_ICONINFORMATION);

    /* the robot normally turns around automatically, but in the hunting phase this should not happen */
    driveOverMines(hunt);

    /* the robot starts driving again */
    startRacing(hunt);
    t = 300;

    /* the mines of the exploration->map are being copied to the hunt->map */
    for(i = 0; i < MAP_SIZE; i++)
        for(j = 0; j < MAP_SIZE; j++)
            hunt->map[i][j] = exploration->map[i][j];

    /* the c-code catches up with the automatic first step of the robot */
    goForward(hunt);
    testPrint(hunt);
    printTime(hunt, t);

    xy = closestNonExplored(hunt);
    calculateNext(hunt, xy / MAP_SIZE, xy % MAP_SIZE);
    printTime(hunt, t);

    /* it will keep looking for the treasure until it has found it, the time is over or the robot got blown up */
    while(treasure == IDNO && !isTime(hunt, t) && minesDetonated < 4) {
        xy = closestNonExplored(hunt);

        b = receiveByte();

        /* if the robot detects a crossing section or a mid-edge section it will just keep driving */
        if(b == BLCK || b == WHTE) {
            calculateNext(hunt, xy / MAP_SIZE, xy % MAP_SIZE);

        /* if the robot detects a mine, it asks whether it was the treasure or just a regular mine,
           if it was a regular mine, the robot will be set back to station 1 and the mine will not
           be added to the exploration->map, the minesDetonated counter will be raised by one       */
        } else if(b == MINE) {
            mustBeTreasure = 0;

            for(i = 0; i < exploration->i - 1; i++)
                if(hunt->x[hunt->i - 2] == exploration->x[i] && hunt->y[hunt->i - 2] == exploration->y[i])
                    mustBeTreasure = 1;

            if(countMines(exploration) == 13 || mustBeTreasure) {
                treasure = IDYES;
            } else {
                treasure = MessageBox(NULL, "Did we hit the treasure, sir?", "Treasure?", MB_YESNO + MB_ICONINFORMATION);
            }

            hunt->i--;

            sendByte(STOP + BLOW);

            if(treasure == IDNO) {
                minesDetonated++;

                if(minesDetonated < 4) {
                    /* the robot is set back to station 1 */
                    hunt->x[hunt->i] = station(1) / MAP_SIZE;
                    hunt->y[hunt->i] = station(1) % MAP_SIZE;
                    hunt->d[hunt->i] = 'u';
                    hunt->i++;

                    MessageBox(NULL, "Put me back at station 1 and press OK, sir.", "Back?", MB_ICONINFORMATION);
                    sendByte(STRT + BLOW);

                    /* the c-code catches up with the automatic first step of the robot */
                    goForward(hunt);
                    testPrint(hunt);
                    printTime(hunt, t);

                    xy = closestNonExplored(hunt);
                    calculateNext(hunt, xy / MAP_SIZE, xy % MAP_SIZE);
                }
            }
        }

        printTime(hunt, t);
    }

    /* when the time is over, it will be printed to the user and the robot will be stopped */
    if(isTime(hunt, t))
        printf("I'm out of time, sir. We lost...\n"),
        sendByte(STOP + BLOW);

    if(treasure == IDYES)
        printf("We found the treasure in %d seconds, sir!\n", (int) difftime(time(NULL), hunt->start));

    if(minesDetonated > 3)
        printf("The robot blew up, because it encountered too many mines, sir...\n");
}