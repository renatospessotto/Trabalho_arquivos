#include "record.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Imprime os detalhes de um registro.
 *
 * Esta função imprime os campos de um registro em um formato legível.
 *
 * @param record O registro a ser impresso.
 */
void printRecord(Record record) {
    // Imprime o identificador do ataque
    printf("IDENTIFICADOR DO ATAQUE: %d\n", record.id);

    // Verifica se o ano é válido e imprime
    if (record.year != -1) {
        printf("ANO EM QUE O ATAQUE OCORREU: %d\n", record.year);
    } else {
        printf("ANO EM QUE O ATAQUE OCORREU: NADA CONSTA\n");
    }

    // Imprime o país onde ocorreu o ataque ou "NADA CONSTA" se vazio
    printf("PAIS ONDE OCORREU O ATAQUE: %s\n", record.country && strlen(record.country) > 0 ? record.country : "NADA CONSTA");

    // Imprime o setor da indústria alvo ou "NADA CONSTA" se vazio
    printf("SETOR DA INDUSTRIA QUE SOFREU O ATAQUE: %s\n", record.targetIndustry && strlen(record.targetIndustry) > 0 ? record.targetIndustry : "NADA CONSTA");

    // Imprime o tipo de ameaça ou "NADA CONSTA" se vazio
    printf("TIPO DE AMEACA A SEGURANCA CIBERNETICA: %s\n", record.attackType && strlen(record.attackType) > 0 ? record.attackType : "NADA CONSTA");

    // Verifica se o prejuízo financeiro é válido e imprime
    if (record.financialLoss != -1.0f) {
        printf("PREJUIZO CAUSADO PELO ATAQUE: %.2f\n", record.financialLoss);
    } else {
        printf("PREJUIZO CAUSADO PELO ATAQUE: NADA CONSTA\n");
    }

    // Imprime a estratégia de defesa ou "NADA CONSTA" se vazio
    printf("ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA: %s\n\n", record.defenseStrategy && strlen(record.defenseStrategy) > 0 ? record.defenseStrategy : "NADA CONSTA");
}

/**
 * @brief Lê um registro de um arquivo binário.
 *
 * Esta função lê um registro de um arquivo binário, incluindo campos fixos e variáveis.
 *
 * @param file Ponteiro para o arquivo binário.
 * @param record Ponteiro para a estrutura de registro onde os dados serão armazenados.
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int readRecord(FILE *file, Record *record) {
    // Lê os campos fixos do registro
    if (fread(&record->removido, sizeof(char), 1, file) != 1 ||
        fread(&record->tamanhoRegistro, sizeof(int), 1, file) != 1 ||
        fread(&record->prox, sizeof(long long), 1, file) != 1 ||
        fread(&record->id, sizeof(int), 1, file) != 1) {
        return 0; // Retorna falha se não conseguir ler os campos fixos
    }

    // Lê o campo "year" (ano) e verifica se é válido
    long long currentPosition = ftell(file); // Salva a posição atual no arquivo
    if (fread(&record->year, sizeof(int), 1, file) != 1) {
        record->year = -1; // Define valor sentinela se o campo estiver vazio
        fseek(file, currentPosition + sizeof(int), SEEK_SET); // Pula 4 bytes
    }

    // Lê o campo "financialLoss" (prejuízo financeiro) e verifica se é válido
    currentPosition = ftell(file); // Salva a posição atual no arquivo
    if (fread(&record->financialLoss, sizeof(float), 1, file) != 1) {
        record->financialLoss = -1.0f; // Define valor sentinela se o campo estiver vazio
        fseek(file, currentPosition + sizeof(float), SEEK_SET); // Pula 4 bytes
    }

    // Aloca memória para os campos variáveis
    record->country = malloc(256);
    record->attackType = malloc(256);
    record->targetIndustry = malloc(256);
    record->defenseStrategy = malloc(256);

    if (!record->country || !record->attackType || !record->targetIndustry || !record->defenseStrategy) {
        // Libera memória em caso de falha na alocação
        free(record->country);
        free(record->attackType);
        free(record->targetIndustry);
        free(record->defenseStrategy);
        return 0; // Falha
    }

    // Lê os campos variáveis do registro
    int index = 1;
    readVariableArray(file, record->country, index++); // Lê o campo "country"
    readVariableArray(file, record->attackType, index++); // Lê o campo "attackType"
    readVariableArray(file, record->targetIndustry, index++); // Lê o campo "targetIndustry"
    readVariableArray(file, record->defenseStrategy, index++); // Lê o campo "defenseStrategy"

    return 1; // Sucesso
}

/**
 * @brief Escreve um array de tamanho variável em um arquivo binário.
 *
 * Esta função escreve uma string de tamanho variável em um arquivo binário, precedida por um índice.
 *
 * @param file Ponteiro para o arquivo binário.
 * @param array A string a ser escrita.
 * @param index O índice do campo.
 */
void writeVariableArray(FILE *file, const char *array, char index) {
    // Verifica se o array é nulo ou vazio
    if (array == NULL || strlen(array) == 0) {
        return; // Não escreve nada se o array estiver vazio
    }

    // Converte o índice para ASCII e escreve no arquivo
    char asciiIndex = index + '0';
    fwrite(&asciiIndex, sizeof(char), 1, file);

    // Escreve o conteúdo do array no arquivo
    fwrite(array, sizeof(char), strlen(array), file);

    // Escreve o delimitador '|' no final
    fwrite("|", sizeof(char), 1, file);
}

/**
 * @brief Lê um array de tamanho variável de um arquivo binário.
 *
 * Esta função lê uma string de tamanho variável de um arquivo binário, com base em um índice esperado.
 *
 * @param file Ponteiro para o arquivo binário.
 * @param buffer Ponteiro para o buffer onde a string será armazenada.
 * @param expectedIndex O índice esperado do campo.
 */
void readVariableArray(FILE *file, char *buffer, int expectedIndex) {
    char c, index;
    int i = 0;

    // Salva a posição inicial no arquivo
    long long initialPosition = ftell(file);

    // Lê o índice do campo e verifica se corresponde ao esperado
    if (fread(&index, sizeof(char), 1, file) != 1 || (index - '0') != expectedIndex) {
        buffer[0] = '\0'; // Define o buffer como vazio
        fseek(file, initialPosition, SEEK_SET); // Retorna à posição inicial
        return;
    }

    // Lê os caracteres do campo até encontrar o delimitador '|'
    while (fread(&c, sizeof(char), 1, file) == 1 && c != '|') {
        buffer[i++] = c;
    }

    // Se nenhum caractere foi lido, retorna à posição inicial
    if (i == 0) {
        fseek(file, initialPosition, SEEK_SET);
    }

    buffer[i] = '\0'; // Finaliza a string com o caractere nulo
}
