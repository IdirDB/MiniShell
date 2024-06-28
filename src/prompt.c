#include "prompt.h"

/**
* @brief Affiche "$> " sur la sortie standard
*/
void 
prompt() {
    write(STDOUT_FILENO, "$> ", 3);
}
