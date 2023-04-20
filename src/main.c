#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "shellUtils.h"
#include "shellMain.h"

extern int optopt;
extern char *optarg;


int main(int argc, char *argv[]){
    int opt;

    if ( argc > 3){
        fprintf(stderr,"Argumentos Demais! \n");
        return 0;
    }

    while( (opt = getopt(argc, argv, "c:")) > 0 ) {
        
        switch ( opt ) {
            case 'c': 
                printf("%s\n", optarg);
                break;
            default:
                fprintf(stderr, "Opcao invalida `%c'\n", optopt) ;
                return -1 ;
        }
    }

    shellMain();


    return 0;
}


