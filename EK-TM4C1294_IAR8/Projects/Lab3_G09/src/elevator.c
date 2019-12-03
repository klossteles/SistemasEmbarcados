#include "elevator.h"

int pos = 0;
char levelE = 'x';
char levelC = 'x';
char levelD = 'x';

void rule(char* param, Elevator elev){
    char command[BUFFER];
    switch(param[1]){
    case 'I':
        if(elev.level != param[2]){
            //todo: proteger com mutex
            controlList[pos++] = turnOnLight(param[0]); // acende a luz do painel interno
            controlList[pos++] = closeDoor(param[0]);   // fecha a porta
            if(elev.level != 'p' && (int) elev.level < (int) param[2]){
                controlList[pos++] = goUp(param[0]);    // subir
            } else if(elev.level != 'a' && (int) elev.level > (int) param[2]){
                controlList[pos++] = goDown(param[0]);    // descer
            }
        }

        break;
    case 'E':
        break;
    default: break;
    }
}

char* goUp(char elev) {
    printf(strcat(strcat("going up ", elev),"\n"));
    char command[BUFFER];
    command[0] = elev;  // qual elevador
    command[1] = 's';
    command[2] = '\r';
    command[3] = '\0';
    return &command;
}

char* goDown(char elev) {
    printf(strcat(strcat("going down ", elev),"\n"));
    char command[BUFFER];
    command[0] = elev;  // qual elevador
    command[1] = 'd';
    command[2] = '\r';
    command[3] = '\0';
    return &command;
}

char* stop(char elev) {
    printf(strcat(strcat("stopping ", elev),"\n"));
    char command[BUFFER];
    command[0] = elev;  // qual elevador
    command[1] = 'p';
    command[2] = '\r';
    command[3] = '\0';
    return &command;
}

char* closeDoor(char elev) {
    printf(strcat(strcat("closing door ", elev),"\n"));
    char command[BUFFER];
    command[0] = elev;  // qual elevador
    command[1] = 'f';
    command[2] = '\r';
    command[3] = '\0';
    return &command;
}

char* openDoor(char elev) {
    printf(strcat(strcat("opening door ", elev),"\n"));
    char command[BUFFER];
    command[0] = elev;  // qual elevador
    command[1] = 'a';
    command[2] = '\r';
    command[3] = '\0';
    return &command;
}

char* turnOnLight(char* param) {
    printf(strcat(strcat("turning light on ", param),"\n"));
    char command[BUFFER];
    command[0] = param[0];  // qual elevador
    command[1] = 'L';       // acende a luz no simulador
    command[2] = param[2];  // qual andar
    command[3] = '\r';
    command[4] = '\0';
    return &command;
}
