#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Adicionado para corrigir o uso de fabs

//modularizar fwrite e fread variavel-size arrays


typedef struct {
    char removido;           // 1 byte: '1' for logically removed, '0' otherwise
    int tamanhoRegistro;     // 4 bytes: size of the record in bytes
    long long prox;  
    
    int id;               // 4 bytes
    int year;             // 4 bytes
    float financialLoss;    // Changed from int to float
    char country[256];        // Fixed-size array for 'country'
    char attackType[256];     // Fixed-size array for 'attackType'
    char targetIndustry[256]; // Fixed-size array for 'targetIndustry'
    char defenseStrategy[256];// Fixed-size array for 'defenseStrategy'
} Record;


typedef struct {
    char status;                // 1 byte
    long long topo;             // 8 bytes
    long long proxByteOffset;   // 8 bytes
    int nroRegArq;              // 4 bytes
    int nroRegRem;              // 4 bytes
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




/**
 * Grava um array de tamanho variável em um arquivo binário.
 * @param file Ponteiro para o arquivo binário.
 * @param array Array a ser gravado.
 */
void writeVariableArray(FILE *file, const char *array, char index) {
    int size = strlen(array); // Inclui o caractere nulo
    fwrite(&index, sizeof(char), 1, file); // Grava o índice como char
    fwrite(array, sizeof(char), size, file); // Grava o array
    fwrite("|", sizeof(char), 1, file); // Grava o delimitador '|'
}

/**
 * Lê um array de tamanho variável de um arquivo binário.
 * @param file Ponteiro para o arquivo binário.
 * @param buffer Buffer onde o array será armazenado.
 */
void readVariableArray(FILE *file, char *buffer) {
    char c, index;
    int i = 0;

    // Lê o índice como char
    fread(&index, sizeof(char), 1, file);

    // Lê o arquivo até encontrar o caractere '|'
    while (fread(&c, sizeof(char), 1, file) == 1 && c != '|') {
        buffer[i++] = c;
    }

    buffer[i] = '\0'; // Garante que o array seja nulo terminado
}


/**
 * Imprime os dados de um registro.
 * 
 * @param record O registro a ser impresso.
 */
void printRecord(Record record) {
    printf("IDENTIFICADOR DO ATAQUE: %d\n", record.id);
    printf("ANO EM QUE O ATAQUE OCORREU: %d\n", record.year);
    printf("PAIS ONDE OCORREU O ATAQUE: %s\n", record.country);
    printf("SETOR DA INDUSTRIA QUE SOFREU O ATAQUE: %s\n", record.targetIndustry);
    printf("TIPO DE AMEACA A SEGURANCA CIBERNETICA: %s\n", record.attackType);
    printf("PREJUIZO CAUSADO PELO ATAQUE: %.2f\n", record.financialLoss);
    printf("ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA: %s\n\n", record.defenseStrategy);
}

/**
 * Gera e grava o cabeçalho no arquivo binário.
 * 
 * @param file Ponteiro para o arquivo binário.
 */
void writeHeader(FILE *file) {
    Header header;

    // Inicializa os campos fixos do cabeçalho
    header.status = '0'; // Arquivo inconsistente ao abrir
    header.topo = -1;    // Nenhum registro logicamente removido
    header.proxByteOffset = 0; // Próximo byte offset disponível
    header.nroRegArq = 0; // Nenhum registro não removido inicialmente
    header.nroRegRem = 0; // Nenhum registro logicamente removido inicialmente

    // Preenche os campos de descrição com valores fixos
    strncpy(header.descreveIdentificador, "IDENTIFICADOR DO ATAQUE", sizeof(header.descreveIdentificador));
    strncpy(header.descreveYear, "ANO EM QUE O ATAQUE OCORREU", sizeof(header.descreveYear));
    strncpy(header.descreveFinancialLoss, "PREJUIZO CAUSADO PELO ATAQUE", sizeof(header.descreveFinancialLoss));
    strncpy(header.codDescreveContry, "1", sizeof(header.codDescreveContry));
    strncpy(header.descreveCountry, "PAIS ONDE OCORREU O ATAQUE", sizeof(header.descreveCountry));
    strncpy(header.codDescreveType, "2", sizeof(header.codDescreveType));
    strncpy(header.descreveType, "TIPO DE AMEACA A SEGURANCA CIBERNETICA", sizeof(header.descreveType));
    strncpy(header.codDescreveTargetIndustry, "3", sizeof(header.codDescreveTargetIndustry));
    strncpy(header.descreveTargetIndustry, "SETOR DA INDUSTRIA QUE SOFREU O ATAQUE", sizeof(header.descreveTargetIndustry));
    strncpy(header.codDescreveDefense, "4", sizeof(header.codDescreveDefense));
    strncpy(header.descreveDefense, "ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA", sizeof(header.descreveDefense));

    // Grava o cabeçalho no arquivo binário
    fwrite(&header.status, sizeof(char), 1, file);
    fwrite(&header.topo, sizeof(long long), 1, file);
    fwrite(&header.proxByteOffset, sizeof(long long), 1, file);
    fwrite(&header.nroRegArq, sizeof(int), 1, file);
    fwrite(&header.nroRegRem, sizeof(int), 1, file);
    fwrite(header.descreveIdentificador, sizeof(header.descreveIdentificador), 1, file);
    fwrite(header.descreveYear, sizeof(header.descreveYear), 1, file);
    fwrite(header.descreveFinancialLoss, sizeof(header.descreveFinancialLoss), 1, file);
    fwrite(header.codDescreveContry, sizeof(header.codDescreveContry), 1, file);
    fwrite(header.descreveCountry, sizeof(header.descreveCountry), 1, file);
    fwrite(header.codDescreveType, sizeof(header.codDescreveType), 1, file);
    fwrite(header.descreveType, sizeof(header.descreveType), 1, file);
    fwrite(header.codDescreveTargetIndustry, sizeof(header.codDescreveTargetIndustry), 1, file);
    fwrite(header.descreveTargetIndustry, sizeof(header.descreveTargetIndustry), 1, file);
    fwrite(header.codDescreveDefense, sizeof(header.codDescreveDefense), 1, file);
    fwrite(header.descreveDefense, sizeof(header.descreveDefense), 1, file);

    // Preenche o restante do cabeçalho até 276 bytes com '\0'
    long long currentOffset = ftell(file);
    if (currentOffset < 276) {
        char padding[276 - currentOffset];
        memset(padding, '\0', sizeof(padding));
        fwrite(padding, sizeof(char), sizeof(padding), file);
    }
}


/**
 * Lê um campo do CSV, delimitado por vírgula.
 * 
 * @param field Ponteiro para o início do campo.
 * @param buffer Buffer onde o campo será armazenado.
 * @param maxSize Tamanho máximo do buffer.
 * @return Ponteiro para o próximo campo ou NULL se não houver mais campos.
 */
char* readFieldFromCSV(char *field, char *buffer, size_t maxSize) {
    char *next = strchr(field, ',');
    if (next) *next = '\0';
    strncpy(buffer, field ? field : "", maxSize - 1);
    buffer[maxSize - 1] = '\0';
    return next ? next + 1 : NULL;
}

void generateBinaryFile(const char *inputFile, const char *binaryFile) {
    FILE *input = fopen(inputFile, "r");
    if (!input) {
        perror("Error opening CSV file");
        exit(EXIT_FAILURE);
    }

    FILE *output = fopen(binaryFile, "wb");
    if (!output) {
        perror("Error creating binary file");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    // Escreve o cabeçalho no arquivo binário
    printf("Writing header to binary file...\n");
    writeHeader(output);

    // Pula a primeira linha do CSV (cabeçalho) sem usar fgets
    char c;
    while (fread(&c, sizeof(char), 1, input) == 1) {
        if (c == '\n') {
            break; // Sai do loop ao encontrar o caractere de nova linha
        }
    }

    Record record;
    char buffer[1024];
    size_t bytesRead;
    char line[1024];
    size_t lineLength = 0;

    printf("Processing records...\n");
    while ((bytesRead = fread(buffer, sizeof(char), sizeof(buffer) - 1, input)) > 0) {
        buffer[bytesRead] = '\0'; // Garante que o buffer seja uma string válida
        char *start = buffer;
        char *end;

        while ((end = strchr(start, '\n')) != NULL) {
            *end = '\0';
            strncpy(line + lineLength, start, sizeof(line) - lineLength - 1);
            line[sizeof(line) - 1] = '\0';

            // Processa a linha completa
            char *field = line;
            char tempBuffer[256];

            // Campo 1: ID
            field = readFieldFromCSV(field, tempBuffer, sizeof(tempBuffer));
            record.id = tempBuffer[0] ? atoi(tempBuffer) : -1;

            // Campo 2: Year
            field = readFieldFromCSV(field, tempBuffer, sizeof(tempBuffer));
            record.year = tempBuffer[0] ? atoi(tempBuffer) : 0;

            // Campo 3: FinancialLoss
            field = readFieldFromCSV(field, tempBuffer, sizeof(tempBuffer));
            record.financialLoss = tempBuffer[0] ? atof(tempBuffer) : 0.0f;

            // Campo 4: Country
            field = readFieldFromCSV(field, record.country, sizeof(record.country));

            // Campo 5: AttackType
            field = readFieldFromCSV(field, record.attackType, sizeof(record.attackType));

            // Campo 6: TargetIndustry
            field = readFieldFromCSV(field, record.targetIndustry, sizeof(record.targetIndustry));

            // Campo 7: DefenseStrategy
            readFieldFromCSV(field, record.defenseStrategy, sizeof(record.defenseStrategy));

            record.removido = '0';
            record.prox = -1;

            // Calcula o tamanho total do registro
            int variableFieldsSize = strlen(record.country) + 1 + strlen(record.attackType) + 1 +
                                     strlen(record.targetIndustry) + 1 + strlen(record.defenseStrategy) + 1;
            int fixedFieldsSize = sizeof(record.removido) + sizeof(record.tamanhoRegistro) +
                                  sizeof(record.prox) + sizeof(record.id) + sizeof(record.year) +
                                  sizeof(record.financialLoss);
            record.tamanhoRegistro = fixedFieldsSize + variableFieldsSize;

            // Escreve os campos no arquivo binário
            fwrite(&record.removido, sizeof(char), 1, output);
            fwrite(&record.tamanhoRegistro, sizeof(int), 1, output);
            fwrite(&record.prox, sizeof(long long), 1, output);
            fwrite(&record.id, sizeof(int), 1, output);
            fwrite(&record.year, sizeof(int), 1, output);
            fwrite(&record.financialLoss, sizeof(float), 1, output);

            char index = 1;
            writeVariableArray(output, record.country, index++);
            writeVariableArray(output, record.attackType, index++);
            writeVariableArray(output, record.targetIndustry, index++);
            writeVariableArray(output, record.defenseStrategy, index);

            // Reinicia a linha
            lineLength = 0;
            start = end + 1;
        }

        // Copia o restante do buffer para a próxima iteração
        lineLength = strlen(start);
        strncpy(line, start, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
    }

    fclose(input);
    fclose(output);

    printf("Binary file '%s' generated successfully.\n", binaryFile);
}



/**
 * Remove aspas do início e do fim de uma string, se existirem.
 * 
 * @param value String a ser ajustada.
 */
 void adjustValue(char *value) {
    size_t len = strlen(value);
    if (value[0] == '"' && value[len - 1] == '"') {
        memmove(value, value + 1, len - 2);
        value[len - 2] = '\0';
    }
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
        perror("Error opening binary file");
        return;
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        perror("Error seeking to offset 276");
        fclose(file);
        return;
    }

    Record record;

    // Percorre o arquivo sequencialmente
    while (fread(&record.removido, sizeof(char), 1, file) == 1) {
        if (fread(&record.tamanhoRegistro, sizeof(int), 1, file) != 1 ||
            fread(&record.prox, sizeof(long long), 1, file) != 1 ||
            fread(&record.id, sizeof(int), 1, file) != 1 ||
            fread(&record.year, sizeof(int), 1, file) != 1 ||
            fread(&record.financialLoss, sizeof(float), 1, file) != 1) {
            printf("Failed to read fixed fields.\n");
            break;
        }

        readVariableArray(file, record.country);
        readVariableArray(file, record.attackType);
        readVariableArray(file, record.targetIndustry);
        readVariableArray(file, record.defenseStrategy);

        // Imprime o registro
        if (record.removido == '0') {
            printRecord(record);
        }
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
        perror("Error opening binary file");
        return;
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        perror("Error seeking to offset 276");
        fclose(file);
        return;
    }

    Record record;

    while (fread(&record.removido, sizeof(char), 1, file) == 1) {
        if (fread(&record.tamanhoRegistro, sizeof(int), 1, file) != 1 ||
            fread(&record.prox, sizeof(long long), 1, file) != 1 ||
            fread(&record.id, sizeof(int), 1, file) != 1 ||
            fread(&record.year, sizeof(int), 1, file) != 1 ||
            fread(&record.financialLoss, sizeof(float), 1, file) != 1) {
            printf("Failed to read fixed fields.\n");
            break;
        }

        readVariableArray(file, record.country);
        readVariableArray(file, record.attackType);
        readVariableArray(file, record.targetIndustry);
        readVariableArray(file, record.defenseStrategy);

        // Verifica os critérios
        int matchCount = 0;
        for (int i = 0; i < numCriteria; i++) {
            char adjustedValue[256];
            strncpy(adjustedValue, values[i], sizeof(adjustedValue) - 1);
            adjustedValue[sizeof(adjustedValue) - 1] = '\0';
            adjustValue(adjustedValue);

            if (strcasecmp(criteria[i], "idAttack") == 0 && record.id == atoi(adjustedValue)) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "year") == 0 && record.year == atoi(adjustedValue)) {
                matchCount++;
            } else if (strcasecmp(criteria[i], "financialLoss") == 0 && fabs(record.financialLoss - atof(adjustedValue)) < 0.001) {
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
    }

    fclose(file);
}



int main() {
    int option;
    char inputFile[100], binaryFile[100];
    int id;

    do {
        printf("\nMenu:\n");
        printf("1. Generate binary file from CSV\n");
        printf("2. Print all records and header\n");
        printf("3. Search for a record by criteria\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                printf("Enter the name of the CSV file: ");
                scanf("%s", inputFile);
                printf("Enter the name of the binary file to generate: ");
                scanf("%s", binaryFile);

                generateBinaryFile(inputFile, binaryFile);
                printf("Binary file '%s' generated successfully.\n", binaryFile);
                break;

            case 2:
                printf("Enter the name of the binary file: ");
                scanf("%s", binaryFile);

                printAllUntilId(binaryFile);
                break;

            case 3: {
                printf("Enter the name of the binary file: ");
                scanf("%s", binaryFile);

                int repeatCount;
                printf("Enter the number of times to repeat the search: ");
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    printf("Search iteration %d:\n", r + 1);

                    int numCriteria;
                    printf("Enter the number of criteria (1-3): ");
                    scanf("%d", &numCriteria);

                    char criteria[3][256];
                    char values[3][256];
                    for (int i = 0; i < numCriteria; i++) {
                        printf("Enter criteria %d (e.g., idAttack, year, financialLoss, country, attackType, targetIndustry, defenseMechanism): ", i + 1);
                        scanf("%s", criteria[i]);
                        printf("Enter value for %s: ", criteria[i]);
                        scanf("%s", values[i]);
                    }

                    sequentialSearch(binaryFile, numCriteria, criteria, values);
                }
                

            }

            case 4:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (option != 4);

    return 0;
}