#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

typedef struct {
    char status;
    long long topo;
    long long proxByteOffset;
    int nroRegArq;
    int nroRegRem;
    char descreveIdentificador[23];
    char descreveYear[27];
    char descreveFinancialLoss[28];
    char codDescreveContry[1];
    char descreveCountry[26];
    char codDescreveType[1];
    char descreveType[38];
    char codDescreveTargetIndustry[1];
    char descreveTargetIndustry[38];
    char codDescreveDefense[1];
    char descreveDefense[67];
} Header;

Header initializeAndWriteHeader(FILE *output);
void updateHeader(FILE *output, Header *header);
void readHeader(FILE *input, Header *header);


#endif // HEADER_H
