#include "header.h"
#include <string.h>

/**
 * @brief Inicializa e escreve o cabeçalho em um arquivo binário.
 *
 * Esta função cria uma estrutura de cabeçalho, inicializa seus campos e escreve no
 * início de um arquivo binário.
 *
 * @param output Ponteiro para o arquivo binário.
 * @return A estrutura de cabeçalho inicializada.
 */
Header initializeAndWriteHeader(FILE *output) {
    Header header;
    header.status = '0';
    header.topo = -1;
    header.proxByteOffset = 276;
    header.nroRegArq = 0;
    header.nroRegRem = 0;

    memcpy(header.descreveIdentificador, "IDENTIFICADOR DO ATAQUE", sizeof(header.descreveIdentificador));
    memcpy(header.descreveYear, "ANO EM QUE O ATAQUE OCORREU", sizeof(header.descreveYear));
    memcpy(header.descreveFinancialLoss, "PREJUIZO CAUSADO PELO ATAQUE", sizeof(header.descreveFinancialLoss));
    memcpy(header.codDescreveContry, "1", sizeof(header.codDescreveContry));
    memcpy(header.descreveCountry, "PAIS ONDE OCORREU O ATAQUE", sizeof(header.descreveCountry));
    memcpy(header.codDescreveType, "2", sizeof(header.codDescreveType));
    memcpy(header.descreveType, "TIPO DE AMEACA A SEGURANCA CIBERNETICA", sizeof(header.descreveType));
    memcpy(header.codDescreveTargetIndustry, "3", sizeof(header.codDescreveTargetIndustry));
    memcpy(header.descreveTargetIndustry, "SETOR DA INDUSTRIA QUE SOFREU O ATAQUE", sizeof(header.descreveTargetIndustry));
    memcpy(header.codDescreveDefense, "4", sizeof(header.codDescreveDefense));
    memcpy(header.descreveDefense, "ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA", sizeof(header.descreveDefense));

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

/**
 * @brief Atualiza o cabeçalho de um arquivo binário.
 *
 * Esta função atualiza os campos do cabeçalho em um arquivo binário, marcando-o como consistente.
 *
 * @param output Ponteiro para o arquivo binário.
 * @param header Ponteiro para a estrutura de cabeçalho a ser atualizada.
 */
void updateHeader(FILE *output, Header *header) {
    fseek(output, 0, SEEK_SET);
    header->status = '1'; // Arquivo consistente ao finalizar
    fwrite(&header->status, sizeof(char), 1, output);
    fwrite(&header->topo, sizeof(long long), 1, output);
    fwrite(&header->proxByteOffset, sizeof(long long), 1, output);
    fwrite(&header->nroRegArq, sizeof(int), 1, output);
    fwrite(&header->nroRegRem, sizeof(int), 1, output);
}

/**
 * @brief Lê os campos principais do cabeçalho de um arquivo binário.
 *
 * Esta função lê os campos status, topo, proxByteOffset, nroRegArq e nroRegRem do cabeçalho.
 *
 * @param input Ponteiro para o arquivo binário.
 * @param header Ponteiro para a estrutura de cabeçalho a ser preenchida.
 */
void readHeader(FILE *input, Header *header) {
    fseek(input, 0, SEEK_SET);
    fread(&header->status, sizeof(char), 1, input);
    fread(&header->topo, sizeof(long long), 1, input);
    fread(&header->proxByteOffset, sizeof(long long), 1, input);
    fread(&header->nroRegArq, sizeof(int), 1, input);
    fread(&header->nroRegRem, sizeof(int), 1, input);
}

