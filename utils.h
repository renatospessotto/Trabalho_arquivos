#ifndef UTILS_H
#define UTILS_H

#include <stdio.h> // Added to fix FILE type error

float safeStringToFloat(const char *str);
void adjustValue(char *value);
int read_field(FILE *fp, char *dest);

/**
 * @brief Reads a quoted string from the input and removes the quotes.
 *
 * @param str The buffer to store the unquoted string.
 */
void scan_quote_string(char *str);

#endif // UTILS_H
