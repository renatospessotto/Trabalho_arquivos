#include "record.h"
#include <stdlib.h>
#include <string.h>

void printRecord(Record record) {
    printf("IDENTIFICADOR DO ATAQUE: %d\n", record.id);

    if (record.year != -1) {
        printf("ANO EM QUE O ATAQUE OCORREU: %d\n", record.year);
    } else {
        printf("ANO EM QUE O ATAQUE OCORREU: NADA CONSTA\n");
    }

    printf("PAIS ONDE OCORREU O ATAQUE: %s\n", record.country && strlen(record.country) > 0 ? record.country : "NADA CONSTA");
    printf("SETOR DA INDUSTRIA QUE SOFREU O ATAQUE: %s\n", record.targetIndustry && strlen(record.targetIndustry) > 0 ? record.targetIndustry : "NADA CONSTA");
    printf("TIPO DE AMEACA A SEGURANCA CIBERNETICA: %s\n", record.attackType && strlen(record.attackType) > 0 ? record.attackType : "NADA CONSTA");

    if (record.financialLoss != -1.0f) {
        printf("PREJUIZO CAUSADO PELO ATAQUE: %.2f\n", record.financialLoss);
    } else {
        printf("PREJUIZO CAUSADO PELO ATAQUE: NADA CONSTA\n");
    }

    printf("ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA: %s\n\n", record.defenseStrategy && strlen(record.defenseStrategy) > 0 ? record.defenseStrategy : "NADA CONSTA");
}

int readRecord(FILE *file, Record *record) {
    if (fread(&record->removido, sizeof(char), 1, file) != 1 ||
        fread(&record->tamanhoRegistro, sizeof(int), 1, file) != 1 ||
        fread(&record->prox, sizeof(long long), 1, file) != 1 ||
        fread(&record->id, sizeof(int), 1, file) != 1) {
        return 0; // Falha ao ler os campos fixos
    }

    // Handle year field
    long long currentPosition = ftell(file);
    if (fread(&record->year, sizeof(int), 1, file) != 1) {
        record->year = -1; // Set sentinel value if field is empty
        fseek(file, currentPosition + sizeof(int), SEEK_SET); // Skip 4 bytes
    }

    // Handle financialLoss field
    currentPosition = ftell(file);
    if (fread(&record->financialLoss, sizeof(float), 1, file) != 1) {
        record->financialLoss = -1.0f; // Set sentinel value if field is empty
        fseek(file, currentPosition + sizeof(float), SEEK_SET); // Skip 4 bytes
    }

    // Allocate memory for variable fields
    record->country = malloc(256);
    record->attackType = malloc(256);
    record->targetIndustry = malloc(256);
    record->defenseStrategy = malloc(256);

    // Read variable fields
    int index = 1;
    readVariableArray(file, record->country, index++);
    readVariableArray(file, record->attackType, index++);
    readVariableArray(file, record->targetIndustry, index++);
    readVariableArray(file, record->defenseStrategy, index++);

    return 1; // Sucesso
}

void writeVariableArray(FILE *file, const char *array, char index) {
    if (array == NULL || strlen(array) == 0) {
        return;
    }

    char asciiIndex = index + '0';
    fwrite(&asciiIndex, sizeof(char), 1, file);
    fwrite(array, sizeof(char), strlen(array), file);
    fwrite("|", sizeof(char), 1, file);
}

void readVariableArray(FILE *file, char *buffer, int expectedIndex) {
    char c, index;
    int i = 0;

    long long initialPosition = ftell(file);

    if (fread(&index, sizeof(char), 1, file) != 1 || (index - '0') != expectedIndex) {
        buffer[0] = '\0';
        fseek(file, initialPosition, SEEK_SET);
        return;
    }

    while (fread(&c, sizeof(char), 1, file) == 1 && c != '|') {
        buffer[i++] = c;
    }

    if (i == 0) {
        fseek(file, initialPosition, SEEK_SET);
    }

    buffer[i] = '\0';
}
