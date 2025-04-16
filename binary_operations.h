#ifndef BINARY_OPERATIONS_H
#define BINARY_OPERATIONS_H

#include "record.h"
#include "header.h"
#include "utils.h"

void generateBinaryFile(const char *inputFile, char *binaryFile);
void printAllUntilId(const char *binaryFile);
void sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]);

#endif // BINARY_OPERATIONS_H
