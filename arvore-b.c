/**
 * @file arvore-b.c
 * @brief Implementação das funções principais da árvore-B
 * 
 * Este arquivo contém as implementações das funções principais para
 * manipulação da árvore-B, incluindo construção, busca, inserção,
 * remoção e atualização de offsets.
 */

#include "arvore-b.h"
#include "record.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INVALIDO -1


/**
 * @brief Constrói uma árvore-B a partir de um arquivo de dados
 * 
 * Esta função lê todos os registros válidos de um arquivo de dados
 * e constrói um índice árvore-B onde as chaves são os IDs dos registros
 * e os valores são os byte offsets dos registros no arquivo de dados.
 * 
 * @param dataFilename Nome do arquivo de dados
 * @param btreeFilename Nome do arquivo de índice a ser criado
 */
void buildBTreeFromDataFile(const char *dataFilename, const char *btreeFilename) {
    FILE *dataFile = fopen(dataFilename, "rb");
    if (!dataFile) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verifica se o arquivo de dados está consistente
    char status;
    if (fread(&status, sizeof(char), 1, dataFile) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(dataFile);
        return;
    }
    
    if (status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(dataFile);
        return;
    }

    FILE *btreeFile = fopen(btreeFilename, "wb+");
    if (!btreeFile) {
        printf("Falha no processamento do arquivo.\n");
        fclose(dataFile);
        return;
    }

    // Inicializa o cabeçalho da árvore-B
    inicializaCabecalhoArvoreB(btreeFile);

    // Pula o cabeçalho do arquivo de dados (276 bytes)
    if (fseek(dataFile, 276, SEEK_SET) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(dataFile);
        fclose(btreeFile);
        return;
    }

    Record record;
    int recordCount = 0;
    
    // Processa cada registro do arquivo de dados
    while (1) {
        // Salva o offset ANTES de ler o registro
        long long recordOffset = ftell(dataFile);
        
        if (!readRecord(dataFile, &record)) {
            break; // Fim do arquivo
        }
        
        // Verifica se chegou ao final dos registros válidos
        if (record.id == -1) {
            // Libera memória antes de sair
            if (record.country) free(record.country);
            if (record.attackType) free(record.attackType);
            if (record.targetIndustry) free(record.targetIndustry);
            if (record.defenseStrategy) free(record.defenseStrategy);
            break;
        }

        // Apenas registros não removidos são inseridos no índice
        if (record.removido == '0') {
            // Usa o offset salvo antes da leitura
            btree_insert(btreeFile, record.id, recordOffset);
            recordCount++;
        }

        // Libera memória alocada dinamicamente
        if (record.country) free(record.country);
        if (record.attackType) free(record.attackType);
        if (record.targetIndustry) free(record.targetIndustry);
        if (record.defenseStrategy) free(record.defenseStrategy);
    }

    // Marca o arquivo como consistente
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(btreeFile, &cab);
    cab.status = '1';
    escreverCabecalhoArvoreB(btreeFile, &cab);

    fclose(dataFile);
    fclose(btreeFile);
}

/**
 * @brief Busca e imprime um registro usando a árvore-B como índice
 * 
 * Esta função utiliza a árvore-B para localizar rapidamente um registro
 * pelo seu ID e depois lê e imprime o registro do arquivo de dados.
 * 
 * @param dataFilename Nome do arquivo de dados
 * @param btreeFilename Nome do arquivo de índice
 * @param id ID do registro a ser buscado
 */
void printRecordFromBTree(const char *dataFilename, const char *btreeFilename, int id) {
    
    FILE *btreeFile = fopen(btreeFilename, "rb");
    if (!btreeFile) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    long long offset = btree_search(btreeFile, id);
    fclose(btreeFile);
    
    int found = 0; // Flag para rastrear se o registro foi encontrado

    if (offset == -1) {
        found = 0;
    } else {
        FILE *dataFile = fopen(dataFilename, "rb");
        if (!dataFile) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }

        fseek(dataFile, offset, SEEK_SET);

        Record record;
        if (readRecord(dataFile, &record)) {
            printRecord(record);
            found = 1;

            // Libera memória alocada dinamicamente
            free(record.country);
            free(record.attackType);
            free(record.targetIndustry);
            free(record.defenseStrategy);
        } else {
            found = 0;
        }

        fclose(dataFile);
    }
    
    // Imprime resultado baseado na flag found
    if (found == 1) {
        printf("**********\n"); // Imprime separador se registros forem encontrados
    } 
    else {
        printf("Registro inexistente.\n\n**********\n");
    }
}


/**
 * @brief Remove uma chave da árvore-B
 * 
 * Esta função remove uma chave específica da árvore-B, mantendo
 * as propriedades da estrutura através de redistribuições e fusões
 * quando necessário.
 * 
 * @param btreeFile Ponteiro para o arquivo da árvore-B aberto
 * @param id ID do registro a ser removido
 * @return 1 se removido com sucesso, 0 se não encontrado
 */
int btree_remove(FILE *btreeFile, int id) {  // CORRIGIDO: int id
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(btreeFile, &cab);
    
    if (cab.noRaiz == -1) {
        return 0; // Árvore vazia
    }
    
    // Busca primeiro para verificar se a chave existe
    long long offset = btree_search(btreeFile, id);  // CORRIGIDO: int id
    if (offset == -1) {
        return 0; // Chave não encontrada
    }
    
    // Executa a remoção
    remover(btreeFile, &cab, cab.noRaiz, id);  // CORRIGIDO: int id
    
    // Verifica se a raiz ficou vazia (redução de altura)
    PaginaArvoreB raiz;
    lerPaginaArvoreB(btreeFile, cab.noRaiz, &raiz);
    
    if (raiz.nroChaves == 0 && raiz.tipoNo != -1) {
        // Raiz ficou vazia, promove o único filho
        if (raiz.ponteiros[0] != INVALIDO) {
            int novaRaizRRN = raiz.ponteiros[0];
            cab.noRaiz = novaRaizRRN;
            
            // Atualiza o tipo do novo nó raiz
            PaginaArvoreB novaRaiz;
            lerPaginaArvoreB(btreeFile, novaRaizRRN, &novaRaiz);
            novaRaiz.tipoNo = 0; // Marca como raiz
            escreverPaginaArvoreB(btreeFile, novaRaizRRN, &novaRaiz);
            
            cab.nroNos--;
        } else {
            // Árvore ficou completamente vazia
            cab.noRaiz = -1;
            cab.nroNos = 0;
        }
    }
    
    escreverCabecalhoArvoreB(btreeFile, &cab);
    return 1; // Sucesso
}



/**
 * @brief Imprime informações detalhadas da árvore-B para debug
 * 
 * Esta função imprime o cabeçalho e todas as páginas da árvore-B,
 * mostrando a estrutura interna para fins de depuração e verificação.
 * 
 * @param btreeFilename Nome do arquivo de índice
 */
void printCabecalhoEPaginasArvoreB(const char *btreeFilename) {
    FILE *fp = fopen(btreeFilename, "rb");
    if (!fp) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(fp, &cab);

    printf("status: %c\n", cab.status);
    printf("noRaiz: %d\n", cab.noRaiz);
    printf("proxRRN: %d\n", cab.proxRRN);
    printf("nroNos: %d\n", cab.nroNos);

    for (int rrn = 0; rrn < cab.proxRRN; rrn++) {
        PaginaArvoreB pag;
        lerPaginaArvoreB(fp, rrn, &pag);

        printf("Página RRN %d\n", rrn);
        printf("  tipoNo: %d\n", pag.tipoNo);
        printf("  nroChaves: %d\n", pag.nroChaves);
        printf("  ponteiros: ");
        for (int i = 0; i < MAX_PONTEIROS; i++) {
            printf("%d ", pag.ponteiros[i]);
        }
        printf("\n  chaves: ");
        for (int i = 0; i < pag.nroChaves; i++) {
            printf("%d ", pag.chaves[i]);
        }
        printf("\n  pr: ");
        for (int i = 0; i < pag.nroChaves; i++) {
            printf("%lld ", pag.pr[i]);
        }
        printf("\n");
    }

    fclose(fp);
}


/**
 * @brief Insere uma chave diretamente na árvore-B usando offset já conhecido
 * 
 * Esta função adiciona uma entrada na árvore-B sem necessidade de buscar
 * no arquivo de dados, utilizando diretamente o ID e o offset fornecidos.
 * 
 * @param btreeFile Nome do arquivo de índice da árvore-B.
 * @param id ID do registro a ser inserido no índice.
 * @param offset Byte offset do registro no arquivo de dados.
 */
void insertBtreeEntry(const char *btreeFile, int id, long long offset) {
    FILE *btreeFilePointer = fopen(btreeFile, "rb+");
    if (!btreeFilePointer) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Insere diretamente na árvore-B usando a função existente
    btree_insert(btreeFilePointer, id, offset);

    // Atualiza o status para consistente
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(btreeFilePointer, &cab);
    cab.status = '1';
    escreverCabecalhoArvoreB(btreeFilePointer, &cab);

    fclose(btreeFilePointer);
}


/**
 * @brief Atualiza o byte offset de uma chave existente na árvore-B
 * 
 * Esta função busca uma chave específica na árvore-B e atualiza
 * seu byte offset associado. É utilizada quando um registro é
 * realocado no arquivo de dados.
 * 
 * @param btreeFile Ponteiro para o arquivo da árvore-B aberto
 * @param id ID do registro cuja chave será atualizada
 * @param newOffset Novo byte offset do registro
 * @return 1 se atualizado com sucesso, 0 se não encontrado
 */
int btree_update_offset(FILE *btreeFile, int id, long long newOffset) {
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(btreeFile, &cab);
    
    // Verifica se o arquivo de índice tem status válido
    if (cab.status != '1') {
        return 0; // Falha
    }
    
    if (cab.noRaiz == -1) return 0; // Árvore vazia

    int rrn = cab.noRaiz;
    while (1) {
        PaginaArvoreB pag;
        lerPaginaArvoreB(btreeFile, rrn, &pag);
        
        for (int i = 0; i < pag.nroChaves; i++) {
            if (pag.chaves[i] == id) {
                // Encontrou a chave, atualiza o byte offset
                pag.pr[i] = newOffset;
                escreverPaginaArvoreB(btreeFile, rrn, &pag);
                return 1; // Sucesso
            }
            if (id < pag.chaves[i]) {
                if (pag.ponteiros[i] == INVALIDO) return 0;
                rrn = pag.ponteiros[i];
                goto proximo_nivel;
            }
        }
        if (pag.ponteiros[pag.nroChaves] == INVALIDO) return 0;
        rrn = pag.ponteiros[pag.nroChaves];
    proximo_nivel:;
    }
    return 0; // Chave não encontrada
}





