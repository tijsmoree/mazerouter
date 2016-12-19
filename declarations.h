#define DEBUG 1         /* debuggable by choosing DEBUG in the COM-port list */

#define MAX_ROUTE 150   /* size of the maze->x, y, d arrays */
#define MAP_SIZE 11     /* size of the maze->map            */

#define BLCK 64         /* codes for the information the computer */
#define WHTE 128        /* will receive from the robot            */
#define MINE 192

#define LEFT 2          /* codes for the information the computer */
#define RGHT 5          /* will send to the robot                 */
#define FORW 1
#define TURN 6
#define BACK 3
#define STOP 7
#define STRT 4
#define BLOW 8          /* this code decides whether the robot should drive on when encountering a mine */

#define BAUDRATE CBR_9600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <gtk/gtk.h>
#include <conio.h>

#include "maze.h"
#include "utilities.h"
#include "communication.h"
#include "challenges.h"
#include "interface.h"

int debug;