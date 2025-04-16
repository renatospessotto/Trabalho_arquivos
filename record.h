#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>

#define MAX_FIELD 256 // Added definition for MAX_FIELD

typedef struct {
    char removido;
    int tamanhoRegistro;
    long long prox;
    int id;
    int year;
    float financialLoss;
    char *country;
    char *attackType;
    char *targetIndustry;
    char *defenseStrategy;
} Record;

void printRecord(Record record);
int readRecord(FILE *file, Record *record);
void writeVariableArray(FILE *file, const char *array, char index);
void readVariableArray(FILE *file, char *buffer, int expectedIndex);

#endif // RECORD_H
