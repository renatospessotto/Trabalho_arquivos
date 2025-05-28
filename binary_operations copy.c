#include "binary_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>   


/**
 * @brief Gera um arquivo binário a partir de um arquivo CSV de entrada.
 *
 * Lê um arquivo CSV, processa seu conteúdo e grava os registros em um arquivo binário.
 *
 * @param inputFile Caminho para o arquivo CSV de entrada.
 * @param binaryFile Caminho para o arquivo binário de saída.
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
        if (allocateAndCopyField(field[3], &record.country, input, output) == -1) {
            free(record.attackType);
            free(record.targetIndustry);
            free(record.defenseStrategy);
            return -1;
        }

        if (allocateAndCopyField(field[4], &record.attackType, input, output) == -1) {
            free(record.country);
            free(record.targetIndustry);
            free(record.defenseStrategy);
            return -1;
        }

        if (allocateAndCopyField(field[5], &record.targetIndustry, input, output) == -1) {
            free(record.country);
            free(record.attackType);
            free(record.defenseStrategy);
            return -1;
        }

        if (allocateAndCopyField(field[6], &record.defenseStrategy, input, output) == -1) {
            free(record.country);
            free(record.attackType);
            free(record.targetIndustry);
            return -1;
        }

        record.removido = '0'; // Marca o registro como não removido
        record.prox = -1; // Define o próximo registro como inexistente

        // Calcula o tamanho do registro
        record.tamanhoRegistro = calculateRecordSize(&record);

        // Escreve o registro no arquivo binário
        writeRecord(output, &record);

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
 * Lê registros sequencialmente de um arquivo binário e imprime seus detalhes
 * até encontrar um registro com o ID especificado.
 *
 * @param binaryFile Caminho para o arquivo binário a ser lido.
 */
void printAllUntilId(const char *binaryFile) {
    FILE *file = fopen(binaryFile, "rb");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return; // Stop processing and return to the main menu
    }

    // Verifica se o primeiro byte do arquivo é "1"
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    // Pula os próximos 275 bytes (restante do cabeçalho)
    if (fseek(file, 275, SEEK_CUR) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    Record record;
    int found = 0; // Flag para verificar se algum registro foi encontrado
    // Percorre o arquivo sequencialmente
    while (readRecord(file, &record)) {

        if (record.removido == '0') {
            printRecord(record);
            found = 1; // Marca que pelo menos um registro foi encontrado
        }

        // Libera a memória alocada
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    // Se nenhum registro foi encontrado, imprime a mensagem
    if (!found) {
        printf("Registro inexistente.\n");
    }

    fclose(file);
}

/**
 * @brief Realiza uma busca sequencial em um arquivo binário com base em critérios.
 *
 * Busca registros em um arquivo binário que correspondam aos critérios especificados.
 *
 * @param binaryFile Caminho para o arquivo binário a ser pesquisado.
 * @param numCriteria Número de critérios a serem correspondidos.
 * @param criteria Array com os nomes dos campos de critério.
 * @param values Array com os valores dos critérios.
 */
void sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    FILE *file = fopen(binaryFile, "rb"); // Abre o arquivo binário para leitura
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return; 
    }

    // Verifica se o primeiro byte do arquivo é "1"
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    // Pula os próximos 275 bytes (restante do cabeçalho)
    if (fseek(file, 275, SEEK_CUR) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    Record record;
    int found = 0; // Flag para rastrear se algum registro corresponde

    while (readRecord(file, &record)) {
        int matchCount = matchRecord(&record, numCriteria, criteria, values);
        if (matchCount == numCriteria && record.removido == '0') {
            printRecord(record);
            found = 1;
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
    else {
        printf("Registro inexistente.\n\n**********\n");
    }

    fclose(file); // Fecha o arquivo
}



/**
 * @brief Marca registros como removidos com base em critérios múltiplos.
 *
 * Busca registros no arquivo binário que correspondam aos critérios e altera o campo
 * 'removido' para '1', indicando que o registro foi logicamente excluído.
 *
 * @param binaryFile Caminho para o arquivo binário.
 * @param numCriteria Número de critérios.
 * @param criteria Array com os nomes dos campos de critério.
 * @param values Array com os valores dos critérios.
 * @return Número de registros removidos, -1 em caso de falha.
 */
int deleteRecordByCriteria(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Verifica se o primeiro byte do arquivo é "1"
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return -1;
    }

    // Declara e lê o cabeçalho
    Header header;
    readHeader(file, &header);

    // Move o ponteiro para o byte 276 (início dos registros)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha ao posicionar o ponteiro no arquivo.\n");
        fclose(file);
        return -1;
    }
    Record record;
    long long recordOffset = -1; // Initialize recordOffset to a default value
    int removedCount = 0;

    while (readRecord(file, &record)) {
        recordOffset = ftell(file) - (sizeof(char) + sizeof(int) + record.tamanhoRegistro);

        int matchCount = matchRecord(&record, numCriteria, criteria, values);
        if (matchCount == numCriteria && record.removido == '0') {
            printRecord(record); // Imprime o registro encontrado

            readHeader(file, &header); // Lê o cabeçalho e restaura o ponteiro automaticamente
            // Marca como removido
            record.removido = '1';

            // Atualiza o campo prox do registro removido para apontar para o antigo topo
            long long novoProx = header.topo;
            header.topo = recordOffset;

            // Atualiza o registro removido no arquivo
            fseek(file, recordOffset, SEEK_SET);
            fwrite(&record.removido, sizeof(char), 1, file);
            fseek(file, sizeof(int), SEEK_CUR); // pula tamanhoRegistro
            fwrite(&novoProx, sizeof(long long), 1, file);

            // Atualiza contadores de removidos e ativos
            header.nroRegRem++;
            header.nroRegArq--;

            removedCount++;
        }

        // Libera memória alocada dinamicamente
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    // Atualiza o cabeçalho no início do arquivo
    updateHeader(file, &header);

    fclose(file);
    return removedCount;
}





/**
 * @brief Insere um registro no arquivo binário, reaproveitando registros removidos (First Fit).
 *
 * Insere um novo registro no arquivo binário, reutilizando espaço de registros logicamente removidos
 * ou adicionando no final do arquivo, caso não haja espaço suficiente.
 *
 * @param binaryFile Caminho para o arquivo binário.
 * @param id Identificador do ataque.
 * @param year Ano do ataque.
 * @param financialLoss Prejuízo financeiro causado pelo ataque.
 * @param country País onde ocorreu o ataque.
 * @param attackType Tipo de ataque.
 * @param targetIndustry Setor alvo do ataque.
 * @param defenseStrategy Estratégia de defesa utilizada.
 * @return 0 em caso de sucesso, -1 em caso de falha.
 */
int insertRecord(const char *binaryFile, int id, int year, float financialLoss, const char *country, const char *attackType, const char *targetIndustry, const char *defenseStrategy) {
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Verifica se o primeiro byte do arquivo é "1"
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return -1;
    }

    // Declara e lê o cabeçalho
    Header header;
    readHeader(file, &header);

    // Move o ponteiro para o byte 276 (início dos registros)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha ao posicionar o ponteiro no arquivo.\n");
        fclose(file);
        return -1;
    }

    // Monta o registro a ser inserido
    Record record;
    record.removido = '0';
    record.id = id;
    record.year = year;
    record.financialLoss = financialLoss;
    record.country = country ? strdup(country) : NULL;
    record.attackType = attackType ? strdup(attackType) : NULL;
    record.targetIndustry = targetIndustry ? strdup(targetIndustry) : NULL;
    record.defenseStrategy = defenseStrategy ? strdup(defenseStrategy) : NULL;
    record.prox = -1;

    // Calcula tamanho do registro novo
    int novoTamanhoRegistro = calculateRecordSize(&record);

    // Estratégia First Fit: procura espaço removido suficiente
    long long prevOffset = -1, currOffset = header.topo, foundOffset = -1, prox = 0;
    int found = 0;
    int fillBytes = 0; // Variável para armazenar o número de bytes de lixo
    int tamanhoRegistro; // Este é o tamanho do registro removido lido do arquivo


    while (currOffset != -1) {
        fseek(file, currOffset, SEEK_SET);
        char removido;
        fread(&removido, sizeof(char), 1, file);
        fread(&tamanhoRegistro, sizeof(int), 1, file);
        fread(&prox, sizeof(long long), 1, file);

        // Verifica se o espaço é suficiente
        if (tamanhoRegistro >= novoTamanhoRegistro) {
            found = 1;
            foundOffset = currOffset;
            fillBytes = tamanhoRegistro - novoTamanhoRegistro;
            if (country == NULL || strlen(country) == 0) {
                fillBytes += 2;
            }
            if (attackType == NULL || strlen(attackType) == 0) {
                fillBytes += 2;
            }
            if (targetIndustry == NULL || strlen(targetIndustry) == 0) {
                fillBytes += 2;
            }
            if (defenseStrategy == NULL || strlen(defenseStrategy) == 0) {
                fillBytes += 2;
            }

            // Atualiza o 'prox' do prevOffset para o 'prox' do currOffset
            if (prevOffset != -1) {
                fseek(file, prevOffset + sizeof(char) + sizeof(int), SEEK_SET);
                fwrite(&prox, sizeof(long long), 1, file);
            } else {
                // Se prevOffset for -1, atualiza o topo da pilha
                header.topo = prox;
            }
            break;
        }

        // Atualiza os offsets
        prevOffset = currOffset;
        currOffset = prox;
    }

    if (found) {
        record.tamanhoRegistro = tamanhoRegistro;
        // Reutiliza espaço removido
        fseek(file, foundOffset, SEEK_SET);
        writeRecord(file, &record);

        // Preenche com '$' se necessário
        if (fillBytes > 0) {
            fillWithTrash(file, fillBytes);
        }

        // Atualiza contadores de registros
        header.nroRegArq++;
        header.nroRegRem--;
    } else {
        record.tamanhoRegistro = novoTamanhoRegistro;
        // Insere no final do arquivo
        fseek(file, 0, SEEK_END);
        writeRecord(file, &record);
        header.nroRegArq++;
        header.proxByteOffset = ftell(file);
    }

    // Atualiza o cabeçalho no início do arquivo
    updateHeader(file, &header);

    free(record.country);
    free(record.attackType);
    free(record.targetIndustry);
    free(record.defenseStrategy);

    fclose(file);
    return 0;
}




/**
 * @brief Atualiza registros no arquivo binário com base em critérios e novos valores.
 *
 * Busca registros que correspondam aos critérios especificados e atualiza seus campos
 * com os novos valores fornecidos. Reutiliza espaço ou realoca registros, se necessário.
 *
 * @param binaryFile Caminho para o arquivo binário.
 * @param numUpdates Número de atualizações a serem realizadas.
 * @param numCriteria Número de critérios para correspondência.
 * @param criteria Array com os nomes dos campos de critério.
 * @param values Array com os valores dos critérios.
 * @param numUpdatesFields Número de campos a serem atualizados.
 * @param updateFields Array com os nomes dos campos a serem atualizados.
 * @param updateValues Array com os novos valores para os campos.
 * @return Número de registros atualizados, -1 em caso de falha.
 */
int updateRecords(const char *binaryFile, int numUpdates, int numCriteria, char criteria[3][256], char values[3][256], int numUpdatesFields, char updateFields[3][256], char updateValues[3][256]) {
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Verifica se o primeiro byte do arquivo é "1"
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return -1;
    }

    // Declara e lê o cabeçalho
    Header header;
    readHeader(file, &header);

    // Move o ponteiro para o byte 276 (início dos registros)
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha ao posicionar o ponteiro no arquivo.\n");
        fclose(file);
        return -1;
    }

    int updatedCount = 0;
    Record record;

    while (readRecord(file, &record)) {
        long long recordOffset = ftell(file) - (sizeof(char) + sizeof(int) + record.tamanhoRegistro);

        int matchCount = matchRecord(&record, numCriteria, criteria, values);

        if (matchCount == numCriteria && record.removido == '0') {
            Record updated = record;
            updated.country = record.country ? strdup(record.country) : NULL;
            updated.attackType = record.attackType ? strdup(record.attackType) : NULL;
            updated.targetIndustry = record.targetIndustry ? strdup(record.targetIndustry) : NULL;
            updated.defenseStrategy = record.defenseStrategy ? strdup(record.defenseStrategy) : NULL;

            int changed = 0;
            for (int i = 0; i < numUpdatesFields; i++) {
                char adjustedValue[256];
                strncpy(adjustedValue, updateValues[i], sizeof(adjustedValue) - 1);
                adjustedValue[sizeof(adjustedValue) - 1] = '\0';

                if (strcasecmp(updateFields[i], "idAttack") == 0) {
                    continue;
                } else if (strcasecmp(updateFields[i], "year") == 0) {
                    int newYear = atoi(adjustedValue);
                    if (updated.year != newYear) {
                        updated.year = newYear;
                        changed = 1;
                    }
                } else if (strcasecmp(updateFields[i], "financialLoss") == 0) {
                    float newLoss = safeStringToFloat(adjustedValue);
                    if (fabs(updated.financialLoss - newLoss) > 0.001) {
                        updated.financialLoss = newLoss;
                        changed = 1;
                    }
                } else if (strcasecmp(updateFields[i], "country") == 0 && updated.country != NULL) {
                    if (strcmp(updated.country, adjustedValue) != 0) {
                        free(updated.country);
                        updated.country = strdup(adjustedValue);
                        changed = 1;
                    }
                } else if (strcasecmp(updateFields[i], "attackType") == 0 && updated.attackType != NULL) {
                    if (strcmp(updated.attackType, adjustedValue) != 0) {
                        free(updated.attackType);
                        updated.attackType = strdup(adjustedValue);
                        changed = 1;
                    }
                } else if (strcasecmp(updateFields[i], "targetIndustry") == 0 && updated.targetIndustry != NULL) {
                    if (strcmp(updated.targetIndustry, adjustedValue) != 0) {
                        free(updated.targetIndustry);
                        updated.targetIndustry = strdup(adjustedValue);
                        changed = 1;
                    }
                } else if (strcasecmp(updateFields[i], "defenseMechanism") == 0 && updated.defenseStrategy != NULL) {
                    if (strcmp(updated.defenseStrategy, adjustedValue) != 0) {
                        free(updated.defenseStrategy);
                        updated.defenseStrategy = strdup(adjustedValue);
                        changed = 1;
                    }
                }
            }

            if (changed) {
                int oldSize = record.tamanhoRegistro;
                int newSize = calculateRecordSize(&updated);

                if (newSize <= oldSize) {
                    updated.tamanhoRegistro = oldSize;
                    fseek(file, recordOffset, SEEK_SET);
                    writeRecord(file, &updated);
                    int fillBytes = oldSize - newSize;
                    if (fillBytes > 0) {
                        fillWithTrash(file, fillBytes);
                    }
                } else {
                    deleteRecordByCriteria(binaryFile, 1, criteria, values);

                    fclose(file);
                    insertRecord(binaryFile, updated.id, updated.year, updated.financialLoss, updated.country, updated.attackType, updated.targetIndustry, updated.defenseStrategy);
                    file = fopen(binaryFile, "rb+");
                    fseek(file, 276, SEEK_SET);
                }
                updatedCount++;
            }

            free(updated.country);
            free(updated.attackType);
            free(updated.targetIndustry);
            free(updated.defenseStrategy);
        }

        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    updateHeader(file, &header);
    fclose(file);
    return updatedCount;
}