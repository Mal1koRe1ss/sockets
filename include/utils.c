#include "utils.h"
#include <ctype.h>

void lowercase(char *str) {
    for (int i = 0; str[i]; i++) { // ? Taking every character of the string. Until it reaches the max.
        str[i] = tolower((unsigned char)str[i]); // ? Using the "tolower" method from "ctype"
    }
}