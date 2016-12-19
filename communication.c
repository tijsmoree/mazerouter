#include "declarations.h"

/* this function returns a string value of the signal it sends to the robot */
char *signal2name(int s) {
    char *name;

    name = malloc(10 * sizeof(char));

    s %= 8;

    switch(s) {
        case 1: name = "forwards";  break;
        case 2: name = "left";      break;
        case 3: name = "backwards"; break;
        case 4: name = "start";     break;
        case 5: name = "right";     break;
        case 6: name = "turn";      break;
        case 7: name = "stop";      break;
    }

    return name;
}

/* sets the variables of the hSerial HANDLE, so the communication will go well */
/* wrong is set to 1, when an error occures and it will make an error message pop up */
void initSio(HANDLE hSerial, int *wrong){
    COMMTIMEOUTS timeouts = {0};
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
        *wrong = 1;

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams))
        *wrong = 1;

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts))
        *wrong = 1;
}

/* starts the communication with the robot */
HANDLE openCom(int c) {
    HANDLE hSerial;
    int wrong = 0;

    char coms[] = "COM1";
    coms[3] = c + 48;

    hSerial = CreateFile(coms,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if(hSerial == INVALID_HANDLE_VALUE)
        wrong = 1;

    initSio(hSerial, &wrong);

    if(wrong) {
        MessageBox(NULL, "There is something wrong with the connection. You could try another COM-port or DEBUG-mode.", "No connection possible!", MB_ICONSTOP);

        resetGUI();
    }

    return hSerial;
}

/* ends the communication with the robot */
void closeCom(HANDLE hSerial) {
    CloseHandle(hSerial);
}

/* function to receive information from the robot */
int receiveByte() {
    if(debug) {
        int d;
        printf("Byte: ");
        scanf("%d", &d);
        printf("\n");

        return d;
    } else {
        unsigned char buffRead[BUFSIZ + 1];
        DWORD dwBytesRead = 0;

        buffRead[0] = 0;
        
        printf("Byte: ");

        do ReadFile(hSerial, buffRead, 1, &dwBytesRead, NULL);
            while(buffRead[0]);

        while(!buffRead[0])
            ReadFile(hSerial, buffRead, 1, &dwBytesRead, NULL);

        printf("%d\n", buffRead[0]);

        return buffRead[0];
    }
}

/* function to send information to the robot */
void sendByte(int b) {
    if(debug) {
        printf("[%d: %s]\n", b, signal2name(b));
    } else {
        DWORD dwBytesWritten = 0;

        unsigned char buffWrite[BUFSIZ + 1];
        buffWrite[0] = (char) b;

        if (!WriteFile(hSerial, buffWrite, 1, &dwBytesWritten, NULL))
            printf("Boss, they don't co%cperate, they won't accept this!\n", 148);

        printf("[%d: %s]\n", b, signal2name(b));
    }
}