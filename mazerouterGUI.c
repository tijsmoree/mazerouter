#include "declarations.h"

int main(int argc, char *argv[]) {
    debug = 0;

    /* this creates the GUI and it continues until it is closed */
    gui(argc, argv);

    /* the function of the requested challenge is opened */
    switch(chal) {
        case 'a': a(chks, amountChks); break;
        case 'b': b(chks, amountChks); break;
        case 'c': c();                 break;
    }

    /* the communication is closed with the robot when not in DEBUG mode */
    if(!debug)
        closeCom(hSerial);

    getch();

    return 0;
}