#include "binary_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Added to fix strncpy and strcasecmp warnings
#include <math.h>   // Added to fix fabs warnings

int generateBinaryFile(const char *inputFile, char *binaryFile) {
    FILE *input = fopen(inputFile, "r");
    if (!input) {
        printf("Falha no processamento do arquivo.\n");
        return -1; // Indicate failure
    }

    FILE *output = fopen(binaryFile, "wb+");
    if (!output) {
        printf("Falha no processamento do arquivo.\n");
        fclose(input);
        return -1; // Indicate failure
    }

    // Inicializa e escreve o cabeçalho
    Header header = initializeAndWriteHeader(output);

    // Pula a primeira linha do CSV (cabeçalho)
    char c;
    while (fread(&c, sizeof(char), 1, input) == 1) {
        if (c == '\n') {
            break; // Sai do loop ao encontrar o caractere de nova linha
        }
    }

    Record record;
    int recordCount = 0;

    while (!feof(input)) {
        // Initialize record fields
        record.id = -1;
        record.year = -1; // Sentinel value for empty year
        record.financialLoss = -1.0f; // Sentinel value for empty financialLoss
        record.country = NULL;
        record.attackType = NULL;
        record.targetIndustry = NULL;
        record.defenseStrategy = NULL;

        // Reinitialize fields for each record
        char field[7][MAX_FIELD] = {0};

        // Read fields using read_field
        for (int i = 0; i < 7; i++) {
           
            if (!read_field(input, field[i])) break;
        }

        record.id = strlen(field[0]) > 0 ? atoi(field[0]) : -1;
        if (record.id == -1) {
            break; // Stop processing if the id field is empty
        }

        // Assign values to record fields
        
        record.year = strlen(field[1]) > 0 ? atoi(field[1]) : -1;
        record.financialLoss = strlen(field[2]) > 0 ? safeStringToFloat(field[2]) : -1.0f;

        if (strlen(field[3]) > 0) {
            record.country = malloc(strlen(field[3]));
            strcpy(record.country, field[3]);
        }

        if (strlen(field[4]) > 0) {
            record.attackType = malloc(strlen(field[4]));
            strcpy(record.attackType, field[4]);
        }

        if (strlen(field[5]) > 0) {
            record.targetIndustry = malloc(strlen(field[5]));
            strcpy(record.targetIndustry, field[5]);
        }

        if (strlen(field[6]) > 0) {
            record.defenseStrategy = malloc(strlen(field[6]));
            strcpy(record.defenseStrategy, field[6]);
        }

        record.removido = '0';
        record.prox = -1;

        // Calculate record size
        int variableFieldsSize = 0;

        if (record.country) {
            variableFieldsSize += strlen(record.country) + 2; // +1 for delimiter '|'
        }
        if (record.attackType) {
            variableFieldsSize += strlen(record.attackType) + 2; // +1 for delimiter '|'
        }
        if (record.targetIndustry) {
            variableFieldsSize += strlen(record.targetIndustry) + 2; // +1 for delimiter '|'
        }
        if (record.defenseStrategy) {
            variableFieldsSize += strlen(record.defenseStrategy) + 2; // +1 for delimiter '|'
        }

        int fixedFieldsSize = sizeof(record.tamanhoRegistro) +
                              sizeof(record.prox) + sizeof(record.id) + sizeof(record.year) +
                              sizeof(record.financialLoss);

        record.tamanhoRegistro = fixedFieldsSize + variableFieldsSize -4;

        // Write record to binary file
        fwrite(&record.removido, sizeof(char), 1, output);
        fwrite(&record.tamanhoRegistro, sizeof(int), 1, output);
        fwrite(&record.prox, sizeof(long long), 1, output);
        fwrite(&record.id, sizeof(int), 1, output);

        if (record.year != -1) {
            fwrite(&record.year, sizeof(int), 1, output);
        } else {
            int sentinelYear = -1; // Represented as 0xFFFFFFFF
            fwrite(&sentinelYear, sizeof(int), 1, output);
        }

        if (record.financialLoss != -1.0f) {
            fwrite(&record.financialLoss, sizeof(float), 1, output);
        } else {
            float sentinelValue = -1.0f; // Represented as 0x80BF in IEEE 754
            fwrite(&sentinelValue, sizeof(float), 1, output);
        }

        char index = 1; // Mantido para gravação dos campos variáveis
        writeVariableArray(output, record.country, index++);
        writeVariableArray(output, record.attackType, index++);
        writeVariableArray(output, record.targetIndustry, index++);
        writeVariableArray(output, record.defenseStrategy, index);

        recordCount++;

        // Free dynamically allocated memory
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);

        // Update header values
        header.nroRegArq++;
    }

    // Usa ftell para calcular o proxByteOffset
    header.proxByteOffset = ftell(output);

    // Atualiza o cabeçalho no início do arquivo
    fseek(output, 0, SEEK_SET);
    header.status = '1'; // Arquivo consistente ao finalizar
    fwrite(&header.status, sizeof(char), 1, output);
    fwrite(&header.topo, sizeof(long long), 1, output);
    fwrite(&header.proxByteOffset, sizeof(long long), 1, output);
    fwrite(&header.nroRegArq, sizeof(int), 1, output);
    fwrite(&header.nroRegRem, sizeof(int), 1, output);

    fclose(input);
    fclose(output);
    return 0; // Indicate success
}


/**
 * @brief Imprime todos os registros de um arquivo binário até encontrar um ID especificado.
 *
 * Esta função lê registros sequencialmente de um arquivo binário e imprime seus detalhes
 * até encontrar um registro com o ID especificado. A iteração para assim que o ID correspondente
 * for encontrado.
 *
 * @param binaryFile O caminho para o arquivo binário a ser lido.
 *
 * @note Se o ID alvo não for encontrado no arquivo, a função imprimirá todos os registros.
 */
void printAllUntilId(const char *binaryFile) {
    FILE *file = fopen(binaryFile, "rb");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return; // Stop processing and return to the main menu
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return;
    }

    Record record;

    // Percorre o arquivo sequencialmente
    while (readRecord(file, &record)) {
        // Imprime o registro
        if (record.removido == '0') {
            printRecord(record);
        }

        // Libera a memória alocada
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    fclose(file);
}

/**
 * Performs a sequential search on an array to find the target value.
 *
 * @param arr The array to search through.
 * @param size The size of the array.
 * @param target The value to search for in the array.
 * @return The index of the target value if found, or -1 if the target is not in the array.
 *
 * This function iterates through the array elements one by one, comparing each element
 * with the target value. If a match is found, the function returns the index of the
 * matching element. If no match is found after checking all elements, it returns -1.
 */
void sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    FILE *file = fopen(binaryFile, "rb");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return; // Stop processing and return to the main menu
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return;
    }

    Record record;

    while (readRecord(file, &record)) {
        // Verifica os critérios
        int matchCount = 0;
        for (int i = 0; i < numCriteria; i++) {
            char adjustedValue[256];
            strncpy(adjustedValue, values[i], sizeof(adjustedValue) - 1);
            adjustedValue[sizeof(adjustedValue) - 1] = '\0';

            if (strcasecmp(criteria[i], "idAttack") == 0 && record.id == atoi(adjustedValue)) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "year") == 0 && record.year == atoi(adjustedValue)) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "financialLoss") == 0 && fabs(record.financialLoss - safeStringToFloat(adjustedValue)) < 0.001) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "country") == 0 && strcasecmp(record.country, adjustedValue) == 0) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "attackType") == 0 && strcasecmp(record.attackType, adjustedValue) == 0) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "targetIndustry") == 0 && strcasecmp(record.targetIndustry, adjustedValue) == 0) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "defenseMechanism") == 0 && strcasecmp(record.defenseStrategy, adjustedValue) == 0) {
                matchCount++;
            }
        }

        // Imprime o registro se todos os critérios forem atendidos
        if (matchCount == numCriteria && record.removido == '0') {
            printRecord(record);
        }

        // Libera a memória alocada
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    fclose(file);
}
