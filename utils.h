#ifndef UTILS_H
#define UTILS_H

#include <stdio.h> // Added to fix FILE type error


void binarioNaTela(char *nomeArquivoBinario);
float safeStringToFloat(const char *str);
int read_field(FILE *fp, char *dest);
void scan_quote_string(char *str);

#endif // UTILS_H
