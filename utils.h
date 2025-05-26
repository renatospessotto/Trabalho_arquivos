#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "record.h" // Adicione este include para definir Record


void binarioNaTela(char *nomeArquivoBinario);
float safeStringToFloat(const char *str);
int read_field(FILE *fp, char *dest);
void scan_quote_string(char *str);
int allocateAndCopyField(const char *field, char **destination, FILE *input, FILE *output);
long find_last_removed(FILE *fp, long header_topo);
int calculateRecordSize(const Record *record);
int matchRecord(const Record *record, int numCriteria, char criteria[3][256], char values[3][256]);
void fillWithTrash(FILE *file, int fillBytes); // Declaration of fillWithTrash

#endif // UTILS_H
