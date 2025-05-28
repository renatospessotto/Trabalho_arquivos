#ifndef BINARY_OPERATIONS_H
#define BINARY_OPERATIONS_H

#include "record.h"
#include "header.h"
#include "utils.h"

int generateBinaryFile(const char *inputFile, char *binaryFile);
void printAllUntilId(const char *binaryFile);
void sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]);
int deleteRecordByCriteria(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]);
int insertRecord(const char *binaryFile, int id, int year, float financialLoss, const char *country, const char *attackType, const char *targetIndustry, const char *defenseStrategy);
int updateRecords(const char *binaryFile, int numUpdates, int numCriteria, char criteria[3][256], char values[3][256], int numUpdatesFields, char updateFields[3][256], char updateValues[3][256]);
void printRecordFromOffset(const char *fileName, long long offset);


#endif // BINARY_OPERATIONS_H
