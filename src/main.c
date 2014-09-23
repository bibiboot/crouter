#include "globals.h"
#include "hashl.h"

void init(){}

int cmd_parser(int argc, char *argv[]){
    /*
    if (argc != 3) {
        return 1;
    }

    int num = sscanf(argv[2], "%[^:]:%s", globals.hostname_b, globals.recv_filename);
    if (num != 2) {
        return 1;
    }
    */
    return 0;
}

void start(){
    //void *val;
    //pthread_create(&globals.sen_th, 0, reciever, val);
    //pthread_create(&globals.rev_th, 0, sender, val);
}

int main(int argc, char *argv[]){

    // Command line parsing
    if (cmd_parser(argc, argv) != 0) {
        DBG("Error in parsing command line");
    }

    // Initilaization
    init();

    // Create threads
    start();

    return 0;
}
