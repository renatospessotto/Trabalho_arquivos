/*
 * binary_operations.c
 * 
 * Implementa operações para manipulação de arquivos binários com registros
 * de ataques cibernéticos. Inclui conversão CSV->binário, busca, inserção,
 * remoção e atualização de registros com reutilização de espaço (First Fit).
 */

#include "binary_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>   

/**
 * Gera arquivo binário a partir de CSV
 * Lê cada linha do CSV, processa os campos e escreve no formato binário
 * 
 * @param inputFile: caminho para o arquivo CSV de entrada
 * @param binaryFile: caminho onde será criado o arquivo binário de saída
 * @return: 0 se sucesso, -1 se erro
 */
int generateBinaryFile(const char *inputFile, char *binaryFile) {
    // Abertura dos arquivos de entrada e saída com validação
    FILE *input = fopen(inputFile, "r");
    if (!input) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    FILE *output = fopen(binaryFile, "wb+");
    if (!output) {
        printf("Falha no processamento do arquivo.\n");
        fclose(input);
        return -1;
    }

    // Criação e escrita do cabeçalho inicial do arquivo binário
    Header header = initializeAndWriteHeader(output);

    // Skip da linha de cabeçalho do CSV (contém nomes dos campos)
    char c;
    while (fread(&c, sizeof(char), 1, input) == 1) {
        if (c == '\n') break;
    }

    Record record;
    int recordCount = 0;

    // Loop principal de processamento das linhas do CSV
    while (!feof(input)) {
        // Inicialização de todos os campos do registro com valores padrão
        record.id = -1;
        record.year = -1;
        record.financialLoss = -1.0f;
        record.country = NULL;
        record.attackType = NULL;
        record.targetIndustry = NULL;
        record.defenseStrategy = NULL;

        char field[7][MAX_FIELD] = {0};

        // Leitura dos 7 campos da linha atual do CSV
        for (int i = 0; i < 7; i++) {
            if (!read_field(input, field[i])) break;
        }

        // Conversão e validação dos campos numéricos
        record.id = strlen(field[0]) > 0 ? atoi(field[0]) : -1;
        if (record.id == -1) break; // ID inválido indica fim dos dados válidos

        record.year = strlen(field[1]) > 0 ? atoi(field[1]) : -1;
        record.financialLoss = strlen(field[2]) > 0 ? safeStringToFloat(field[2]) : -1.0f;

        // Alocação dinâmica e cópia dos campos de texto variável
        if (allocateAndCopyField(field[3], &record.country, input, output) == -1) {
            // Limpeza de memória em caso de erro
            free(record.attackType);
            free(record.targetIndustry);
            free(record.defenseStrategy);
            fclose(input);
            fclose(output);
            return -1;
        }

        if (allocateAndCopyField(field[4], &record.attackType, input, output) == -1) {
            free(record.country);
            free(record.targetIndustry);
            free(record.defenseStrategy);
            fclose(input);
            fclose(output);
            return -1;
        }

        if (allocateAndCopyField(field[5], &record.targetIndustry, input, output) == -1) {
            free(record.country);
            free(record.attackType);
            free(record.defenseStrategy);
            fclose(input);
            fclose(output);
            return -1;
        }

        if (allocateAndCopyField(field[6], &record.defenseStrategy, input, output) == -1) {
            free(record.country);
            free(record.attackType);
            free(record.targetIndustry);
            fclose(input);
            fclose(output);
            return -1;
        }

        // Configuração dos campos de controle do registro
        record.removido = '0';
        record.prox = -1;
        record.tamanhoRegistro = calculateRecordSize(&record);

        // Escrita do registro no arquivo binário
        writeRecord(output, &record);
        recordCount++;

        // Liberação da memória alocada para os campos de texto
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);

        header.nroRegArq++;
    }

    // Finalização: atualização do cabeçalho com informações corretas
    header.proxByteOffset = ftell(output);
    updateHeader(output, &header);

    fclose(input);
    fclose(output);
    return 0;
}

/**
 * Lista todos os registros ativos do arquivo
 * Percorre sequencialmente o arquivo exibindo registros não removidos
 * 
 * @param binaryFile: caminho do arquivo binário a ser lido
 */
void printAllUntilId(const char *binaryFile) {
    // Abertura e validação do arquivo binário
    FILE *file = fopen(binaryFile, "rb");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verificação da integridade do arquivo (status consistente)
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    // Pula cabeçalho (275 bytes restantes)
    if (fseek(file, 275, SEEK_CUR) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    Record record;
    int found = 0;

    // Loop de leitura sequencial de todos os registros
    while (readRecord(file, &record)) {
        // Filtro: exibe apenas registros ativos (não removidos)
        if (record.removido == '0') {
            printRecord(record);
            found = 1;
        }

        // Liberação obrigatória da memória dos campos variáveis
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    // Tratamento do caso sem registros encontrados
    if (!found) {
        printf("Registro inexistente.\n");
    }

    fclose(file);
}

/**
 * Busca sequencial com múltiplos critérios
 * Encontra registros que atendam simultaneamente a todos os critérios especificados
 * 
 * @param binaryFile: caminho do arquivo binário para busca
 * @param numCriteria: quantidade de critérios a serem verificados (máximo 3)
 * @param criteria: array com nomes dos campos para filtrar (ex: "country", "year")
 * @param values: array com valores correspondentes aos critérios
 */
void sequentialSearch(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    // Abertura e validação inicial do arquivo
    FILE *file = fopen(binaryFile, "rb");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return; 
    }

    // Verificação de consistência do arquivo
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    // Posicionamento para início da área de dados
    if (fseek(file, 275, SEEK_CUR) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return; 
    }

    Record record;
    int found = 0;

    // Busca sequencial através de todos os registros
    while (readRecord(file, &record)) {
        // Verificação de correspondência com todos os critérios
        int matchCount = matchRecord(&record, numCriteria, criteria, values);
        
        // Exibição apenas se todos os critérios foram atendidos e registro está ativo
        if (matchCount == numCriteria && record.removido == '0') {
            // Exibição dos campos de controle interno (para debug)
            printf("Removido: %c\n", record.removido);
            printf("tamanhoRegistro: %d\n", record.tamanhoRegistro);
            printf("prox: %lld\n", record.prox);
            printRecord(record);
            found = 1;
        }

        // Liberação da memória dos campos de tamanho variável
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    // Formatação da saída conforme especificação do projeto
    if (found == 1) {
        printf("**********\n");
    } 
    else {
        printf("Registro inexistente.\n\n**********\n");
    }

    fclose(file);
}

/**
 * Remove registros logicamente usando pilha de removidos
 * O algoritmo implementa uma pilha: header.topo aponta para o último removido,
 * e cada removido aponta para o anterior via campo 'prox'
 * 
 * @param binaryFile: caminho do arquivo binário a ser modificado
 * @param numCriteria: quantidade de critérios para identificar registros a remover
 * @param criteria: array com nomes dos campos de critério
 * @param values: array com valores dos critérios
 * @return: número de registros removidos, -1 se erro
 */
int deleteRecordByCriteria(const char *binaryFile, int numCriteria, char criteria[3][256], char values[3][256]) {
    // Abertura do arquivo em modo leitura/escrita
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Validação da integridade do arquivo
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return -1;
    }

    Header header;

    // Posicionamento no início da área de registros
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha ao posicionar o ponteiro no arquivo.\n");
        fclose(file);
        return -1;
    }

    Record record;
    long long recordOffset, returnOffset;
    int removedCount = 0;

    // Busca sequencial por registros que atendam aos critérios
    while (readRecord(file, &record)) {
        // Salvamento da posição atual para retorno
        returnOffset = ftell(file);
        recordOffset = ftell(file) - (sizeof(char) + sizeof(int) + record.tamanhoRegistro);
        
        // Leitura do cabeçalho atualizado
        readHeader(file, &header);
        
        // Verificação se o registro atende aos critérios de remoção
        int matchCount = matchRecord(&record, numCriteria, criteria, values);
        if (matchCount == numCriteria && record.removido == '0') {
            // Operação de empilhamento na pilha de removidos
            record.removido = '1';
            long long novoProx = header.topo; // salva o antigo topo
            header.topo = recordOffset; // define novo topo da pilha

            // Atualização física do registro no arquivo
            fseek(file, recordOffset, SEEK_SET);
            fwrite(&record.removido, sizeof(char), 1, file);
            fseek(file, sizeof(int), SEEK_CUR);
            fwrite(&novoProx, sizeof(long long), 1, file);

            // Atualização dos contadores estatísticos
            header.nroRegRem++;
            header.nroRegArq--;
            removedCount++;

            // Persistência das alterações no cabeçalho
            updateHeader(file, &header);
            fseek(file, returnOffset, SEEK_SET);
        }

        // Limpeza da memória dos campos variáveis
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    fclose(file);
    return removedCount;
}

/**
 * Insere registro com reutilização de espaço (First Fit)
 * Percorre pilha de removidos procurando primeiro espaço suficiente
 * 
 * @param binaryFile: caminho do arquivo binário onde inserir
 * @param id: identificador único do ataque
 * @param year: ano em que ocorreu o ataque
 * @param financialLoss: prejuízo financeiro em valor monetário
 * @param country: país onde ocorreu o ataque (pode ser NULL)
 * @param attackType: tipo/categoria do ataque (pode ser NULL)
 * @param targetIndustry: setor industrial alvo (pode ser NULL)
 * @param defenseStrategy: estratégia de defesa empregada (pode ser NULL)
 * @return: 0 se sucesso, -1 se erro
 */
int insertRecord(const char *binaryFile, int id, int year, float financialLoss, const char *country, const char *attackType, const char *targetIndustry, const char *defenseStrategy) {
    // Abertura e validação do arquivo
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Verificação de integridade
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        fclose(file);
        return -1;
    }

    if (fseek(file, 275, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }

    // Montagem do novo registro com os dados fornecidos
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

    // Cálculo do tamanho necessário para o novo registro
    int novoTamanhoRegistro = calculateRecordSize(&record);

    // Ajuste do tamanho para campos nulos (não armazenam delimitador)
    if (country == NULL || strlen(country) == 0) novoTamanhoRegistro -= 2;
    if (attackType == NULL || strlen(attackType) == 0) novoTamanhoRegistro -= 2;
    if (targetIndustry == NULL || strlen(targetIndustry) == 0) novoTamanhoRegistro -= 2;
    if (defenseStrategy == NULL || strlen(defenseStrategy) == 0) novoTamanhoRegistro -= 2;

    // Leitura do cabeçalho para acessar pilha de removidos
    Header header;
    readHeader(file, &header);

    // Implementação do algoritmo First Fit na pilha de removidos
    long long prevOffset = -1;
    long long currOffset = header.topo;
    long long foundOffset = -1;
    int found = 0;
    int fillBytes = 0;
    int tamanhoRegistro = 0;

    // Percorrimento da pilha buscando espaço adequado
    while (currOffset != -1) {
        Record test;
        fseek(file, currOffset, SEEK_SET);

        if (!readRecord(file, &test)) break;

        // Verificação se o espaço é suficiente
        if (test.tamanhoRegistro >= novoTamanhoRegistro) {
            found = 1;
            foundOffset = currOffset;
            fillBytes = test.tamanhoRegistro - novoTamanhoRegistro;
            tamanhoRegistro = test.tamanhoRegistro;

            // Remoção do espaço da pilha (desempilhamento)
            if (prevOffset != -1) {
                fseek(file, prevOffset + sizeof(char) + sizeof(int), SEEK_SET);
                fwrite(&test.prox, sizeof(long long), 1, file);
            } else {
                header.topo = test.prox;
            }
            break;
        }

        // Avanço para o próximo elemento na pilha
        prevOffset = currOffset;
        currOffset = test.prox;

        free(test.country);
        free(test.attackType);
        free(test.targetIndustry);
        free(test.defenseStrategy);
    }

    // Decisão de inserção: reutilização ou final do arquivo
    if (found) {
        // Reutilização de espaço encontrado na pilha
        record.tamanhoRegistro = tamanhoRegistro;
        fseek(file, foundOffset, SEEK_SET);
        writeRecord(file, &record);

        // Preenchimento do espaço restante com lixo
        if (fillBytes > 0) {
            fillWithTrash(file, fillBytes);
        }

        // Atualização dos contadores
        header.nroRegArq++;
        header.nroRegRem--;
    } else {
        // Inserção no final do arquivo
        record.tamanhoRegistro = novoTamanhoRegistro;
        fseek(file, 0, SEEK_END);
        writeRecord(file, &record);
        header.nroRegArq++;
        header.proxByteOffset = ftell(file);
    }

    // Persistência das alterações no cabeçalho
    updateHeader(file, &header);

    // Liberação da memória alocada
    free(record.country);
    free(record.attackType);
    free(record.targetIndustry);
    free(record.defenseStrategy);

    fclose(file);
    return 0;
}

/**
 * Atualiza registros baseado em critérios
 * Se novo tamanho não couber, remove antigo e insere novo
 * 
 * @param binaryFile: caminho do arquivo binário a ser modificado
 * @param numUpdates: número de operações de atualização solicitadas
 * @param numCriteria: quantidade de critérios para localizar registros
 * @param criteria: array com nomes dos campos de busca
 * @param values: array com valores dos critérios de busca
 * @param numUpdatesFields: quantidade de campos que serão atualizados
 * @param updateFields: array com nomes dos campos a atualizar
 * @param updateValues: array com novos valores para os campos
 * @return: número de registros atualizados, -1 se erro
 */
int updateRecords(const char *binaryFile, int numUpdates, int numCriteria, char criteria[3][256], char values[3][256], int numUpdatesFields, char updateFields[3][256], char updateValues[3][256]) {
    // Abertura e validação do arquivo
    FILE *file = fopen(binaryFile, "rb+");
    if (!file) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    // Verificação de integridade
    char status;
    if (fread(&status, sizeof(char), 1, file) != 1 || status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return -1;
    }

    Header header;

    // Posicionamento no início dos registros
    if (fseek(file, 276, SEEK_SET) != 0) {
        printf("Falha ao posicionar o ponteiro no arquivo.\n");
        fclose(file);
        return -1;
    }

    int updatedCount = 0;
    Record record;

    // Busca sequencial por registros que atendam aos critérios
    while (readRecord(file, &record)) {
        long long recordOffset = ftell(file) - (sizeof(char) + sizeof(int) + record.tamanhoRegistro);

        // Verificação de correspondência com critérios
        int matchCount = matchRecord(&record, numCriteria, criteria, values);

        if (matchCount == numCriteria && record.removido == '0') {
            readHeader(file, &header);

            // Criação de cópia do registro para atualização
            Record updated = record;
            updated.country = record.country ? strdup(record.country) : NULL;
            updated.attackType = record.attackType ? strdup(record.attackType) : NULL;
            updated.targetIndustry = record.targetIndustry ? strdup(record.targetIndustry) : NULL;
            updated.defenseStrategy = record.defenseStrategy ? strdup(record.defenseStrategy) : NULL;

            int changed = 0;

            // Aplicação das atualizações nos campos especificados
            for (int i = 0; i < numUpdatesFields; i++) {
                char adjustedValue[256];
                strncpy(adjustedValue, updateValues[i], sizeof(adjustedValue) - 1);
                adjustedValue[sizeof(adjustedValue) - 1] = '\0';

                // Processamento baseado no nome do campo
                if (strcasecmp(updateFields[i], "idAttack") == 0) {
                    continue; // ID é imutável
                } else if (strcasecmp(updateFields[i], "year") == 0) {
                    // Atualização do ano
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

            // Decisão de estratégia baseada no tamanho do registro atualizado
            if (changed) {
                int oldSize = record.tamanhoRegistro;
                int newSize = calculateRecordSize(&updated);

                if (newSize <= oldSize) {
                    // Atualização in-place: novo registro cabe no espaço original
                    updated.tamanhoRegistro = oldSize;
                    fseek(file, recordOffset, SEEK_SET);
                    writeRecord(file, &updated);
                    
                    // Preenchimento do espaço restante com lixo
                    int fillBytes = oldSize - newSize;
                    if (fillBytes > 0) {
                        fillWithTrash(file, fillBytes);
                    }
                } else {
                    // Estratégia de remoção + inserção: registro não cabe
                    // Inserção do novo registro
                    insertRecord(binaryFile, updated.id, updated.year, updated.financialLoss, 
                               updated.country, updated.attackType, updated.targetIndustry, updated.defenseStrategy);

                    // Remoção lógica do registro antigo
                    recordOffset += 1;
                    record.removido = '1';
                    
                    // Empilhamento na pilha de removidos
                    long long novoProx = header.topo;
                    header.topo = recordOffset;

                    // Atualização física do registro removido
                    fseek(file, recordOffset, SEEK_SET);
                    fwrite(&record.removido, sizeof(char), 1, file);
                    fseek(file, sizeof(int), SEEK_CUR);
                    fwrite(&novoProx, sizeof(long long), 1, file);

                    // Atualização dos contadores
                    header.nroRegRem++;
                    header.nroRegArq--;

                    updateHeader(file, &header);
                    fseek(file, 276, SEEK_SET);
                }
                updatedCount++;
            }

            // Liberação da memória do registro atualizado
            free(updated.country);
            free(updated.attackType);
            free(updated.targetIndustry);
            free(updated.defenseStrategy);
        }

        // Liberação da memória do registro original
        free(record.country);
        free(record.attackType);
        free(record.targetIndustry);
        free(record.defenseStrategy);
    }

    // Finalização com atualização do cabeçalho
    updateHeader(file, &header);
    fclose(file);
    return updatedCount;
}

/**
 * Função para debug - imprime registro de um offset específico
 * Útil para verificar estrutura interna e validar operações
 * 
 * @param fileName: caminho do arquivo binário
 * @param offset: posição exata em bytes onde inicia o registro
 */
void printRecordFromOffset(const char *fileName, long long offset) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo: %s\n", fileName);
        return;
    }

    fseek(file, offset, SEEK_SET);

    Record record;
    readRecord(file, &record);

    printf("Registro a partir do offset %lld:\n", offset);
    printf("removido: %c\n", record.removido);
    printf("tamanhoRegistro: %d\n", record.tamanhoRegistro);
    printf("prox: %lld\n", record.prox);
    printRecord(record);

    free(record.country);
    free(record.attackType);
    free(record.targetIndustry);
    free(record.defenseStrategy);

    fclose(file);
}