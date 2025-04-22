#include "binary_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Incluído para corrigir avisos de strncpy e strcasecmp
#include <math.h>   // Incluído para corrigir avisos de fabs

/**
 * @brief Gera um arquivo binário a partir de um arquivo CSV de entrada.
 *
 * Esta função lê um arquivo CSV, processa seu conteúdo e grava em um arquivo binário.
 *
 * @param inputFile O caminho para o arquivo CSV de entrada.
 * @param binaryFile O caminho para o arquivo binário de saída.
 * @return 0 em caso de sucesso, -1 em caso de falha.
 */
int generateBinaryFile(const char *inputFile, char *binaryFile) {
    FILE *input = fopen(inputFile, "r"); // Abre o arquivo CSV para leitura
    if (!input) {
        printf("Falha no processamento do arquivo.\n");
        return -1; // Retorna falha se o arquivo não puder ser aberto
    }

    FILE *output = fopen(binaryFile, "wb+"); // Abre o arquivo binário para escrita
    if (!output) {
        printf("Falha no processamento do arquivo.\n");
        fclose(input);
        return -1; // Retorna falha se o arquivo não puder ser criado
    }

    // Inicializa e escreve o cabeçalho no arquivo binário
    Header header = initializeAndWriteHeader(output);

    // Pula a primeira linha do CSV (cabeçalho)
    char c;
    while (fread(&c, sizeof(char), 1, input) == 1) {
        if (c == '\n') {
            break; // Sai do loop ao encontrar o caractere de nova linha
        }
    }

    Record record; // Estrutura para armazenar os dados do registro
    int recordCount = 0; // Contador de registros processados

    while (!feof(input)) {
        // Inicializa os campos do registro com valores padrão
        record.id = -1;
        record.year = -1; // Valor sentinela para ano vazio
        record.financialLoss = -1.0f; // Valor sentinela para prejuízo vazio
        record.country = NULL;
        record.attackType = NULL;
        record.targetIndustry = NULL;
        record.defenseStrategy = NULL;

        char field[7][MAX_FIELD] = {0}; // Array para armazenar os campos lidos

        // Lê os campos do CSV
        for (int i = 0; i < 7; i++) {
            if (!read_field(input, field[i])) break; // Para de ler se não houver mais campos
        }

        // Converte e atribui os valores lidos aos campos do registro
        record.id = strlen(field[0]) > 0 ? atoi(field[0]) : -1;
        if (record.id == -1) {
            break; // Para o processamento se o campo ID estiver vazio
        }

        record.year = strlen(field[1]) > 0 ? atoi(field[1]) : -1;
        record.financialLoss = strlen(field[2]) > 0 ? safeStringToFloat(field[2]) : -1.0f;

        // Aloca memória e copia os campos de texto
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

        record.removido = '0'; // Marca o registro como não removido
        record.prox = -1; // Define o próximo registro como inexistente

        // Calcula o tamanho do registro
        int variableFieldsSize = 0;

        if (record.country) {
            variableFieldsSize += strlen(record.country) + 2; // +1 para o delimitador '|'
        }
        if (record.attackType) {
            variableFieldsSize += strlen(record.attackType) + 2;
        }
        if (record.targetIndustry) {
            variableFieldsSize += strlen(record.targetIndustry) + 2;
        }
        if (record.defenseStrategy) {
            variableFieldsSize += strlen(record.defenseStrategy) + 2;
        }

        int fixedFieldsSize = sizeof(record.tamanhoRegistro) +
                              sizeof(record.prox) + sizeof(record.id) + sizeof(record.year) +
                              sizeof(record.financialLoss);

        record.tamanhoRegistro = fixedFieldsSize + variableFieldsSize - 4;

        // Escreve o registro no arquivo binário
        fwrite(&record.removido, sizeof(char), 1, output);
        fwrite(&record.tamanhoRegistro, sizeof(int), 1, output);
        fwrite(&record.prox, sizeof(long long), 1, output);
        fwrite(&record.id, sizeof(int), 1, output);

        if (record.year != -1) {
            fwrite(&record.year, sizeof(int), 1, output);
        } else {
            int sentinelYear = -1; // Representado como 0xFFFFFFFF
            fwrite(&sentinelYear, sizeof(int), 1, output);
        }

        if (record.financialLoss != -1.0f) {
            fwrite(&record.financialLoss, sizeof(float), 1, output);
        } else {
            float sentinelValue = -1.0f; // Representado como 0x80BF em IEEE 754
            fwrite(&sentinelValue, sizeof(float), 1, output);
        }

        // Escreve os campos variáveis no arquivo binário
        char index = 1;
        writeVariableArray(output, record.country, index++);
        writeVariableArray(output, record.attackType, index++);
        writeVariableArray(output, record.targetIndustry, index++);
        writeVariableArray(output, record.defenseStrategy, index);

        recordCount++; // Incrementa o contador de registros

        // Libera a memória alocada dinamicamente
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);

        // Atualiza os valores do cabeçalho
        header.nroRegArq++;
    }

    // Calcula o próximo byte offset usando ftell
    header.proxByteOffset = ftell(output);

    // Atualiza o cabeçalho no início do arquivo
    updateHeader(output, &header);

    fclose(input); // Fecha o arquivo de entrada
    fclose(output); // Fecha o arquivo de saída
    return 0; // Indica sucesso
}


/**
 * @brief Imprime todos os registros de um arquivo binário até encontrar um ID específico.
 *
 * Esta função lê registros sequencialmente de um arquivo binário e imprime seus detalhes
 * até encontrar um registro com o ID especificado.
 *
 * @param binaryFile O caminho para o arquivo binário a ser lido.
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
 * @brief Realiza uma busca sequencial em um arquivo binário com base em critérios.
 *
 * Esta função busca registros em um arquivo binário que correspondam aos critérios especificados.
 *
 * @param binaryFile O caminho para o arquivo binário a ser pesquisado.
 * @param numCriteria O número de critérios a serem correspondidos.
 * @param criteria Array com os nomes dos campos de critério.
 * @param values Array com os valores dos critérios.
 * @return 1 se registros forem encontrados, 0 se nenhum registro corresponder, -1 em caso de falha.
 */
int sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    FILE *file = fopen(binaryFile, "rb"); // Abre o arquivo binário para leitura
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1; // Indica falha no processamento
    }

    // Pula os primeiros 276 bytes (cabeçalho)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return 0; // Indica que nenhum resultado foi encontrado
    }

    Record record;
    int found = 0; // Flag para rastrear se algum registro corresponde

    while (readRecord(file, &record)) {
        int matchCount = 0; // Contador de correspondências para os critérios
        for (int i = 0; i < numCriteria; i++) {
            char adjustedValue[256];
            strncpy(adjustedValue, values[i], sizeof(adjustedValue) - 1);
            adjustedValue[sizeof(adjustedValue) - 1] = '\0';

            // Verifica se o critério corresponde ao campo do registro
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
            } else if (strcasecmp(criteria[i], "defenseMechanism") == 0 && record.defenseStrategy != NULL && strcasecmp(record.defenseStrategy, adjustedValue) == 0) {
                matchCount++;
            }
        }

        // Se todos os critérios forem atendidos e o registro não estiver removido
        if (matchCount == numCriteria && record.removido == '0') {
            printRecord(record); // Imprime o registro
            found = 1; // Marca que um registro foi encontrado
        }

        // Libera a memória alocada dinamicamente
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    if (found == 1) {
        printf("**********\n"); // Imprime separador se registros forem encontrados
    }

    fclose(file); // Fecha o arquivo
    return found; // Retorna se algum registro correspondeu
}
