// Se o arquivo ta aberto e pede pra abrir de novo, tira ponto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Adicionado para corrigir o uso de fabs

//modularizar fwrite e fread variavel-size arrays


typedef struct {
    char removido;           // 1 byte: '1' for logically removed, '0' otherwise
    int tamanhoRegistro;     // 4 bytes: size of the record in bytes
    long long prox;  
    
    int id;                  // 4 bytes
    int year;                // 4 bytes
    float financialLoss;     // Changed from int to float
    char *country;           // Pointer for dynamically allocated 'country'
    char *attackType;        // Pointer for dynamically allocated 'attackType'
    char *targetIndustry;    // Pointer for dynamically allocated 'targetIndustry'
    char *defenseStrategy;   // Pointer for dynamically allocated 'defenseStrategy'
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
 * @param index Índice do campo (1, 2, 3, 4).
 */
void writeVariableArray(FILE *file, const char *array, char index) {
    // Se o campo for vazio, não grava nada
    if (array == NULL || strlen(array) == 0) {
        return;
    }

    // Converte o índice para o caractere ASCII correspondente
    char asciiIndex = index + '0'; // Exemplo: 1 -> '1' (0x31)

    // Grava o índice como caractere ASCII
    fwrite(&asciiIndex, sizeof(char), 1, file);

    // Grava o array diretamente (já está em formato ASCII)
    fwrite(array, sizeof(char), strlen(array), file);

    // Grava o delimitador '|'
    fwrite("|", sizeof(char), 1, file);
}

/**
 * Lê um array de tamanho variável de um arquivo binário.
 * @param file Ponteiro para o arquivo binário.
 * @param buffer Buffer onde o array será armazenado.
 * @param expectedIndex Índice esperado para o campo.
 */
void readVariableArray(FILE *file, char *buffer, int expectedIndex) {
    char c, index;
    int i = 0;

    // Salva a posição inicial do ponteiro
    long long initialPosition = ftell(file);

    // Lê o índice como char
    if (fread(&index, sizeof(char), 1, file) != 1 || (index - '0') != expectedIndex) {
        buffer[0] = '\0'; // Campo nulo se o índice não corresponder
        fseek(file, initialPosition, SEEK_SET); // Retorna o ponteiro à posição inicial
        return;
    }

    // Lê o arquivo até encontrar o caractere '|' ou EOF
    while (fread(&c, sizeof(char), 1, file) == 1 && c != '|') {
        buffer[i++] = c;
    }

    // Se o campo for nulo, retorna o ponteiro à posição inicial
    if (i == 0) {
        fseek(file, initialPosition, SEEK_SET);
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



void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}


#define MAX_FIELD 256

// Helper function to read a field (even if empty)
int read_field(FILE *fp, char *dest) {
    int ch = fgetc(fp);
    if (ch == ',') { // Empty field
        dest[0] = '\0';
        return 1;
    } else if (ch == EOF || ch == '\n') { // End of line
        return 0;
    } else {
        ungetc(ch, fp);
        fscanf(fp, "%255[^,\n]", dest); // Read until comma or end of line
        ch = fgetc(fp); // Consume the comma (or \n)
        return (ch != EOF);
    }
}

/**
 * Converte uma string para float de forma segura.
 * 
 * @param str A string a ser convertida.
 * @return O valor float correspondente ou -1.0f em caso de erro.
 */
float safeStringToFloat(const char *str) {
    char *endptr;
    float value = strtof(str, &endptr);

    // Verifica se a conversão foi bem-sucedida
    if (endptr == str || *endptr != '\0') {
        return -1.0f; // Retorna valor sentinela em caso de erro
    }
    return value;
}

/**
 * Inicializa e escreve o cabeçalho no arquivo binário.
 * @param output Ponteiro para o arquivo binário.
 * @return O cabeçalho inicializado.
 */
Header initializeAndWriteHeader(FILE *output) {
    Header header;
    header.status = '0'; // Arquivo inconsistente ao abrir
    header.topo = -1;    // Nenhum registro logicamente removido
    header.proxByteOffset = 276; // Inicialmente após o cabeçalho
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

    // Escreve os campos do cabeçalho no arquivo binário
    fwrite(&header.status, sizeof(char), 1, output);
    fwrite(&header.topo, sizeof(long long), 1, output);
    fwrite(&header.proxByteOffset, sizeof(long long), 1, output);
    fwrite(&header.nroRegArq, sizeof(int), 1, output);
    fwrite(&header.nroRegRem, sizeof(int), 1, output);
    fwrite(header.descreveIdentificador, sizeof(header.descreveIdentificador), 1, output);
    fwrite(header.descreveYear, sizeof(header.descreveYear), 1, output);
    fwrite(header.descreveFinancialLoss, sizeof(header.descreveFinancialLoss), 1, output);
    fwrite(header.codDescreveContry, sizeof(header.codDescreveContry), 1, output);
    fwrite(header.descreveCountry, sizeof(header.descreveCountry), 1, output);
    fwrite(header.codDescreveType, sizeof(header.codDescreveType), 1, output);
    fwrite(header.descreveType, sizeof(header.descreveType), 1, output);
    fwrite(header.codDescreveTargetIndustry, sizeof(header.codDescreveTargetIndustry), 1, output);
    fwrite(header.descreveTargetIndustry, sizeof(header.descreveTargetIndustry), 1, output);
    fwrite(header.codDescreveDefense, sizeof(header.codDescreveDefense), 1, output);
    fwrite(header.descreveDefense, sizeof(header.descreveDefense), 1, output);

    return header;
}


void generateBinaryFile(const char *inputFile, char *binaryFile) {
    FILE *input = fopen(inputFile, "r");
    if (!input) {
        perror("Falha no processamento do arquivo.");
    }

    FILE *output = fopen(binaryFile, "wb+");
    if (!output) {
        perror("Falha no processamento do arquivo.");
        fclose(input);
    }

    // Inicializa e escreve o cabeçalho
    Header header = initializeAndWriteHeader(output);

    // Preenche o restante do cabeçalho até 276 bytes com '\0'
    long long currentOffset = ftell(output);
    if (currentOffset < 276) {
        char padding[276 - currentOffset];
        memset(padding, '\0', sizeof(padding));
        fwrite(padding, sizeof(char), sizeof(padding), output);
    }

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

        // Assign values to record fields
        record.id = strlen(field[0]) > 0 ? atoi(field[0]) : -1;
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
 * Lê um registro completo de um arquivo binário.
 * @param file Ponteiro para o arquivo binário.
 * @param record Ponteiro para o registro onde os dados serão armazenados.
 * @return 1 se o registro foi lido com sucesso, 0 caso contrário.
 */
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
        perror("Registro inexistente.");
        return;
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        perror("Falha no processamento do arquivo.");
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
        perror("Registro inexistente.");
        return;
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        perror("Falha no processamento do arquivo.");
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
            adjustValue(adjustedValue);

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
                scanf("%s", inputFile);
                scanf("%s", binaryFile);

                generateBinaryFile(inputFile, binaryFile); // Pass binaryFile as char*
                binarioNaTela(binaryFile);

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
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (option != 4);

    return 0;
}