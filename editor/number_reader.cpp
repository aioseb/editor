#include "number_reader.h"
///READER FUNCTIONS
///All of them stop at the null character.

//Read int from char buffer
//Stops at first char after the number
int readInt(char*& p) {
    skipSpaces(p);
    int number = 0, theSign = 1;
    if (*p == '-') { theSign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
        number *= 10;
        number += *p - '0';

        p++;
    }
    return number * theSign;
}

//Read float from char buffer
//Stops at first char after the number
float readFloat(char*& p) {
    skipSpaces(p);
    float number = 0.0f; int theSign = 1;
    if (*p == '-') { theSign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
        number *= 10;
        number += *p - '0';

        p++;
    }
    if (*p != '.') {
        return number * theSign;
    }
    p++;
    float frac = 1.0f;
    while (*p >= '0' && *p <= '9') {
        frac /= 10;
        number += frac * (*p - '0');

        p++;
    }
    return number * theSign;
}

///OTHER USEFUL FUNCTIONS FOR READING

void skipSpaces(char*& p) {
    while (*p == ' ') { p++; }
}

void skipSpacesAndEmptyLines(char*& p) {
    while (*p == ' ' || *p == '\n') { p++; }
}

//Moves p to the next space or
//Stops at the end of the line if there are
//no spaces up to that point
void skipNonSpaces(char*& p) {
    while (*p && *p != ' ' && *p != '\n') { p++; }
}

void goToEndOfLine(char*& p) {
    while (*p && *p != '\n') { p++; }
}
