#include <stdio.h>
#include <stdlib.h>
#include "hm.h"

int main() {
    char* buff = (char*)mon_malloc(10);
    if (buff == NULL) {
        printf("Allocation échouée\n");
        return 1;
    }

    buff[0] = '1';
    buff[1] = '\0';
    printf("Buffer: %s\n", buff);

    mon_free(buff); // Libère la mémoire allouée
    printf("Buffer after my_free: %s\n", buff);
    //free(buff);
    //print("Existance de l'adresse de la memoire desalloué ?",&buff);
    //printf("Buffer apres le free connu: %s\n", buff);
    printf("Allocation Buffer dn't exist \n");
    return 0;
}
