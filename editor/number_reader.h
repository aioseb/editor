#pragma once

#include <fstream>

//Reader functions
int readInt(char*& p);
float readFloat(char*& p);

//Other useful functions for reading
void skipSpaces(char*& p);
void skipSpacesAndEmptyLines(char*& p);
void skipNonSpaces(char*& p);
void goToEndOfLine(char*& p);