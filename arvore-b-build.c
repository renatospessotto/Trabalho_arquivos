/**
 * @file arvore-b-build.c
 * @brief Implementação das funções de construção e inserção da árvore-B
 * 
 * Este arquivo contém as implementações das funções responsáveis pela
 * construção da árvore-B, incluindo inicialização, inserção de chaves
 * e operações de split quando necessário.
 */

#include "arvore-b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INVALIDO -1


/**
 * @brief Inicializa o cabeçalho da árvore-B
 * 
 * Cria e escreve o cabeçalho inicial da árvore-B no arquivo,
 * definindo o status como inconsistente e inicializando os campos.
 * 
 * @param fp Ponteiro para o arquivo da árvore-B
 */
void inicializaCabecalhoArvoreB(FILE *fp) {
    if (!fp) return;
    
    CabecalhoArvoreB cab;
    cab.status = '0';
    cab.noRaiz = -1;
    cab.proxRRN = 0;
    cab.nroNos = 0;
    memset(cab.lixo, '$', 31);
    
    if (fseek(fp, 0, SEEK_SET) != 0) return;
    
    if (fwrite(&cab.status, sizeof(char), 1, fp) != 1 ||
        fwrite(&cab.noRaiz, sizeof(int), 1, fp) != 1 ||
        fwrite(&cab.proxRRN, sizeof(int), 1, fp) != 1 ||
        fwrite(&cab.nroNos, sizeof(int), 1, fp) != 1 ||
        fwrite(&cab.lixo, sizeof(char), 31, fp) != 31) {
        return;
    }
    
    fflush(fp);
}

/**
 * @brief Inicializa uma página da árvore-B
 * 
 * Limpa todos os campos de uma página e define o tipo do nó.
 * Inicializa ponteiros como inválidos e limpa chaves e valores PR.
 * 
 * @param pag Ponteiro para a página a ser inicializada
 * @param tipoNo Tipo do nó (-1: folha, 0: raiz, 1: intermediário)
 */
void inicializaPagina(PaginaArvoreB *pag, int tipoNo) {
    pag->tipoNo = tipoNo;
    pag->nroChaves = 0;
    
    // Limpa todos os ponteiros
    for (int i = 0; i < MAX_PONTEIROS; i++) {
        pag->ponteiros[i] = INVALIDO;
    }
    
    // Limpa todas as chaves e valores de byte offset
    for (int i = 0; i < MAX_CHAVES; i++) {
        pag->chaves[i] = -1;
        pag->pr[i] = -1;
    }
    
    pag->rrn = -1;
}

/**
 * @brief Escreve o cabeçalho da árvore-B no arquivo
 * 
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para a estrutura do cabeçalho
 */
void escreverCabecalhoArvoreB(FILE *fp, CabecalhoArvoreB *cab) {
    fseek(fp, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, fp);
    fwrite(&cab->noRaiz, sizeof(int), 1, fp);
    fwrite(&cab->proxRRN, sizeof(int), 1, fp);
    fwrite(&cab->nroNos, sizeof(int), 1, fp);
    fwrite(&cab->lixo, sizeof(char), 31, fp);
    fflush(fp);
}

/**
 * @brief Lê o cabeçalho da árvore-B do arquivo
 * 
 * Lê o cabeçalho preservando a posição original do ponteiro do arquivo.
 * Em caso de erro na leitura, inicializa com valores padrão.
 * 
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para a estrutura do cabeçalho
 */
void lerCabecalhoArvoreB(FILE *fp, CabecalhoArvoreB *cab) {
    if (!fp || !cab) return;
    
    // Salva a posição atual do ponteiro do arquivo
    long posicaoOriginal = ftell(fp);
    
    // Vai para o início do arquivo para ler o cabeçalho
    if (fseek(fp, 0, SEEK_SET) != 0) {
        if (posicaoOriginal >= 0) {
            fseek(fp, posicaoOriginal, SEEK_SET);
        }
        return;
    }
    
    if (fread(&cab->status, sizeof(char), 1, fp) != 1 ||
        fread(&cab->noRaiz, sizeof(int), 1, fp) != 1 ||
        fread(&cab->proxRRN, sizeof(int), 1, fp) != 1 ||
        fread(&cab->nroNos, sizeof(int), 1, fp) != 1 ||
        fread(&cab->lixo, sizeof(char), 31, fp) != 31) {
        // Em caso de erro, inicializa com valores padrão
        cab->status = '0';
        cab->noRaiz = -1;
        cab->proxRRN = 0;
        cab->nroNos = 0;
        memset(cab->lixo, '$', 31);
    }
    
    // Restaura a posição original do ponteiro do arquivo
    if (posicaoOriginal >= 0) {
        fseek(fp, posicaoOriginal, SEEK_SET);
    }
}

/**
 * @brief Escreve uma página da árvore-B no arquivo
 * 
 * Escreve uma página no arquivo na posição correspondente ao seu RRN.
 * A estrutura segue a ordem: TipoNo, nroChaves, P1, C1, PR1, P2, C2, PR2, P3.
 * 
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN da página (posição relativa)
 * @param pag Ponteiro para a página a ser escrita
 */
void escreverPaginaArvoreB(FILE *fp, int rrn, PaginaArvoreB *pag) {
    if (!fp || !pag || rrn < 0) return;
    
    // Calcula o offset da página no arquivo (cabeçalho + RRN * tamanho_página)
    long long offset = 44 + (long long)rrn * 44;
    if (fseek(fp, offset, SEEK_SET) != 0) return;
    
    // Escreve a página seguindo a ordem exata dos campos
    if (fwrite(&pag->tipoNo, sizeof(int), 1, fp) != 1 ||
        fwrite(&pag->nroChaves, sizeof(int), 1, fp) != 1 ||
        fwrite(&pag->ponteiros[0], sizeof(int), 1, fp) != 1 ||    // P1
        fwrite(&pag->chaves[0], sizeof(int), 1, fp) != 1 ||       // C1
        fwrite(&pag->pr[0], sizeof(long long), 1, fp) != 1 ||     // PR1
        fwrite(&pag->ponteiros[1], sizeof(int), 1, fp) != 1 ||    // P2
        fwrite(&pag->chaves[1], sizeof(int), 1, fp) != 1 ||       // C2
        fwrite(&pag->pr[1], sizeof(long long), 1, fp) != 1 ||     // PR2
        fwrite(&pag->ponteiros[2], sizeof(int), 1, fp) != 1) {    // P3
        return;
    }
    
    fflush(fp);
}

/**
 * @brief Lê uma página da árvore-B do arquivo
 * 
 * Lê uma página do arquivo baseada no seu RRN. Em caso de erro,
 * inicializa a página como folha vazia.
 * 
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN da página a ser lida
 * @param pag Ponteiro para a página onde os dados serão carregados
 */
void lerPaginaArvoreB(FILE *fp, int rrn, PaginaArvoreB *pag) {
    if (!fp || !pag || rrn < 0) {
        if (pag) inicializaPagina(pag, -1);
        return;
    }
    
    // Calcula o offset da página no arquivo
    long long offset = 44 + (long long)rrn * 44;
    if (fseek(fp, offset, SEEK_SET) != 0) {
        inicializaPagina(pag, -1);
        return;
    }
    
    // Lê a página seguindo a ordem exata dos campos
    if (fread(&pag->tipoNo, sizeof(int), 1, fp) != 1 ||
        fread(&pag->nroChaves, sizeof(int), 1, fp) != 1 ||
        fread(&pag->ponteiros[0], sizeof(int), 1, fp) != 1 ||    // P1
        fread(&pag->chaves[0], sizeof(int), 1, fp) != 1 ||       // C1
        fread(&pag->pr[0], sizeof(long long), 1, fp) != 1 ||     // PR1
        fread(&pag->ponteiros[1], sizeof(int), 1, fp) != 1 ||    // P2
        fread(&pag->chaves[1], sizeof(int), 1, fp) != 1 ||       // C2
        fread(&pag->pr[1], sizeof(long long), 1, fp) != 1 ||     // PR2
        fread(&pag->ponteiros[2], sizeof(int), 1, fp) != 1) {    // P3
        inicializaPagina(pag, -1);
        return;
    }
}


/**
 * @brief Função principal de inserção na árvore-B
 * 
 * Coordena a inserção de uma nova chave na árvore-B, tratando tanto
 * o caso de árvore vazia quanto inserções em árvores existentes.
 * Gerencia splits e criação de nova raiz quando necessário.
 * 
 * @param btreeFile Ponteiro para o arquivo da árvore-B
 * @param id ID do registro (chave)
 * @param byteOffset Byte offset do registro no arquivo de dados
 */
void btree_insert(FILE *btreeFile, int id, long long byteOffset) {
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(btreeFile, &cab);

    if (cab.noRaiz == -1) {
        // Primeira inserção - cria a raiz
        PaginaArvoreB nova;
        inicializaPagina(&nova, -1); // Nó folha (que também é raiz inicial)
        nova.chaves[0] = id;
        nova.pr[0] = byteOffset;
        nova.nroChaves = 1;
        escreverPaginaArvoreB(btreeFile, cab.proxRRN, &nova);
        cab.noRaiz = cab.proxRRN;
        cab.proxRRN++;
        cab.nroNos++;
        escreverCabecalhoArvoreB(btreeFile, &cab);
        return;
    }

    // Inserção recursiva
    ResultadoSplit resultado = inserirRecursivo(btreeFile, &cab, cab.noRaiz, id, byteOffset);

    if (resultado.houveSplit) {
        // Quando há split da raiz, a antiga raiz precisa ter tipo atualizado
        // Lê a antiga raiz para saber seu tipo atual
        PaginaArvoreB antigaRaiz;
        lerPaginaArvoreB(btreeFile, resultado.ponteiroEsq, &antigaRaiz);
        
        // Atualiza o tipo da antiga raiz baseado em seu conteúdo
        if (antigaRaiz.tipoNo == 0) {
            // Era raiz, agora precisa ser classificada pelo seu conteúdo
            if (antigaRaiz.ponteiros[0] != INVALIDO) {
                antigaRaiz.tipoNo = 1; // Intermediária
            } else {
                antigaRaiz.tipoNo = -1; // Folha
            }
            escreverPaginaArvoreB(btreeFile, resultado.ponteiroEsq, &antigaRaiz);
        }
        
        // Cria nova raiz para acomodar o split
        PaginaArvoreB novaRaiz;
        inicializaPagina(&novaRaiz, 0); // Nova raiz
        novaRaiz.chaves[0] = resultado.chavePromovida;
        novaRaiz.pr[0] = resultado.prPromovido;
        novaRaiz.ponteiros[0] = resultado.ponteiroEsq;
        novaRaiz.ponteiros[1] = resultado.ponteiroDir;
        novaRaiz.nroChaves = 1;

        escreverPaginaArvoreB(btreeFile, cab.proxRRN, &novaRaiz);
        cab.noRaiz = cab.proxRRN;
        cab.proxRRN++;
        cab.nroNos++;
    }

    escreverCabecalhoArvoreB(btreeFile, &cab);
}


/**
 * @brief Insere uma chave ordenadamente em uma página
 * 
 * Encontra a posição correta para inserir uma nova chave mantendo
 * a ordem crescente das chaves na página.
 * 
 * @param pag Ponteiro para a página
 * @param id ID do registro (chave)
 * @param pr Byte offset do registro
 */
void insereOrdenadoNaPagina(PaginaArvoreB *pag, int id, long long pr) {
    int i;
    // Move chaves maiores para a direita
    for (i = pag->nroChaves - 1; i >= 0 && pag->chaves[i] > id; i--) {
        pag->chaves[i + 1] = pag->chaves[i];
        pag->pr[i + 1] = pag->pr[i];
    }
    // Insere a nova chave na posição correta
    pag->chaves[i + 1] = id;
    pag->pr[i + 1] = pr;
    pag->nroChaves++;
}

/**
 * @brief Função recursiva para inserção na árvore-B
 * 
 * Implementa a lógica recursiva de inserção, tratando casos de:
 * - Inserção em nó folha com espaço disponível
 * - Split de nó folha quando cheio
 * - Inserção em nó interno e propagação de splits
 * - Split de nó interno quando necessário
 * 
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param rrnAtual RRN do nó atual
 * @param id ID do registro a ser inserido
 * @param pr Byte offset do registro
 * @return Resultado da operação de split
 */
ResultadoSplit inserirRecursivo(FILE *fp, CabecalhoArvoreB *cab, int rrnAtual, int id, long long pr) {
    ResultadoSplit resultado = {false, 0, 0, INVALIDO, INVALIDO};
    PaginaArvoreB pagina;
    
    lerPaginaArvoreB(fp, rrnAtual, &pagina);
    
    // Caso 1: Nó folha
    if (pagina.tipoNo == -1) {
        if (pagina.nroChaves < MAX_CHAVES) {
            // Tem espaço, insere diretamente
            insereOrdenadoNaPagina(&pagina, id, pr);
            escreverPaginaArvoreB(fp, rrnAtual, &pagina);
            return resultado; // Sem split
        } else {
            // Folha cheia, precisa dividir (split)
            int tempChaves[MAX_CHAVES + 1];
            long long tempPr[MAX_CHAVES + 1];
            
            // Copia chaves existentes
            for (int i = 0; i < MAX_CHAVES; i++) {
                tempChaves[i] = pagina.chaves[i];
                tempPr[i] = pagina.pr[i];
            }
            
            // Insere nova chave ordenadamente
            int pos = MAX_CHAVES;
            while (pos > 0 && tempChaves[pos - 1] > id) {
                tempChaves[pos] = tempChaves[pos - 1];
                tempPr[pos] = tempPr[pos - 1];
                pos--;
            }
            tempChaves[pos] = id;
            tempPr[pos] = pr;
            
            // Cria nova folha para a direita
            PaginaArvoreB novaFolha;
            inicializaPagina(&novaFolha, -1); // Mantém como folha
            
            // Distribui as chaves: primeira fica na folha original
            pagina.chaves[0] = tempChaves[0];
            pagina.pr[0] = tempPr[0];
            // Limpa posições não utilizadas
            for (int i = 1; i < MAX_CHAVES; i++) {
                pagina.chaves[i] = -1;
                pagina.pr[i] = -1;
            }
            pagina.nroChaves = 1;
            
            // Terceira chave vai para nova folha
            novaFolha.chaves[0] = tempChaves[2];
            novaFolha.pr[0] = tempPr[2];
            // Limpa posições não utilizadas
            for (int i = 1; i < MAX_CHAVES; i++) {
                novaFolha.chaves[i] = -1;
                novaFolha.pr[i] = -1;
            }
            novaFolha.nroChaves = 1;
            
            // Escreve as páginas atualizadas
            escreverPaginaArvoreB(fp, rrnAtual, &pagina);
            escreverPaginaArvoreB(fp, cab->proxRRN, &novaFolha);
            
            // Prepara resultado para promoção (chave do meio sobe)
            resultado.houveSplit = true;
            resultado.chavePromovida = tempChaves[1]; // Chave do meio
            resultado.prPromovido = tempPr[1]; // Preserva o byte offset
            resultado.ponteiroEsq = rrnAtual;
            resultado.ponteiroDir = cab->proxRRN;
            
            cab->proxRRN++;
            cab->nroNos++;
            
            return resultado;
        }
    } else {
        // Caso 2: Nó interno - encontra filho correto para inserção
        int i;
        for (i = 0; i < pagina.nroChaves; i++) {
            if (id < pagina.chaves[i]) {
                break;
            }
        }
        
        int filhoRRN = pagina.ponteiros[i];
        if (filhoRRN == INVALIDO) {
            return resultado; // Erro - ponteiro inválido
        }
        
        // Chama recursivamente para o filho apropriado
        ResultadoSplit resultadoFilho = inserirRecursivo(fp, cab, filhoRRN, id, pr);
        
        if (!resultadoFilho.houveSplit) {
            return resultado; // Não houve split no filho
        }
        
        // Houve split no filho, precisa inserir chave promovida
        if (pagina.nroChaves < MAX_CHAVES) {
            // Tem espaço no nó atual para a chave promovida
            // Insere a chave promovida ordenadamente
            int pos;
            for (pos = pagina.nroChaves - 1; pos >= 0 && pagina.chaves[pos] > resultadoFilho.chavePromovida; pos--) {
                pagina.chaves[pos + 1] = pagina.chaves[pos];
                pagina.pr[pos + 1] = pagina.pr[pos];
                pagina.ponteiros[pos + 2] = pagina.ponteiros[pos + 1];
            }
            
            pagina.chaves[pos + 1] = resultadoFilho.chavePromovida;
            pagina.pr[pos + 1] = resultadoFilho.prPromovido; // Preserva o byte offset
            pagina.ponteiros[pos + 1] = resultadoFilho.ponteiroEsq;
            pagina.ponteiros[pos + 2] = resultadoFilho.ponteiroDir;
            pagina.nroChaves++;
            
            escreverPaginaArvoreB(fp, rrnAtual, &pagina);
            return resultado; // Sem split
        } else {
            // Nó interno cheio, precisa dividir (split)
            int tempChaves[MAX_CHAVES + 1];
            long long tempPr[MAX_CHAVES + 1];
            int tempPonteiros[MAX_PONTEIROS + 1];
            
            // Copia dados existentes para arrays temporários
            for (int j = 0; j < MAX_CHAVES; j++) {
                tempChaves[j] = pagina.chaves[j];
                tempPr[j] = pagina.pr[j];
            }
            for (int j = 0; j < MAX_PONTEIROS; j++) {
                tempPonteiros[j] = pagina.ponteiros[j];
            }
            
            // Insere chave promovida ordenadamente nos arrays temporários
            int pos;
            for (pos = MAX_CHAVES - 1; pos >= 0 && tempChaves[pos] > resultadoFilho.chavePromovida; pos--) {
                tempChaves[pos + 1] = tempChaves[pos];
                tempPr[pos + 1] = tempPr[pos];
                tempPonteiros[pos + 2] = tempPonteiros[pos + 1];
            }
            
            tempChaves[pos + 1] = resultadoFilho.chavePromovida;
            tempPr[pos + 1] = resultadoFilho.prPromovido; // Preserva o byte offset
            tempPonteiros[pos + 1] = resultadoFilho.ponteiroEsq;
            tempPonteiros[pos + 2] = resultadoFilho.ponteiroDir;
            
            // Cria novo nó interno para a direita
            PaginaArvoreB novoInterno;
            inicializaPagina(&novoInterno, 1); // Marca como nó intermediário
            
            // Distribui as chaves: primeira fica no nó original
            pagina.chaves[0] = tempChaves[0];
            pagina.pr[0] = tempPr[0]; // Preserva o byte offset
            pagina.ponteiros[0] = tempPonteiros[0];
            pagina.ponteiros[1] = tempPonteiros[1];
            // Limpa posições não utilizadas
            for (int i = 1; i < MAX_CHAVES; i++) {
                pagina.chaves[i] = -1;
                pagina.pr[i] = -1;
            }
            for (int i = 2; i < MAX_PONTEIROS; i++) {
                pagina.ponteiros[i] = INVALIDO;
            }
            pagina.nroChaves = 1;
            
            // Terceira chave vai para novo nó
            novoInterno.chaves[0] = tempChaves[2];
            novoInterno.pr[0] = tempPr[2]; // Preserva o byte offset
            novoInterno.ponteiros[0] = tempPonteiros[2];
            novoInterno.ponteiros[1] = tempPonteiros[3];
            novoInterno.nroChaves = 1;
            
            // Escreve as páginas atualizadas
            escreverPaginaArvoreB(fp, rrnAtual, &pagina);
            escreverPaginaArvoreB(fp, cab->proxRRN, &novoInterno);
            
            // Prepara resultado para promoção (chave do meio sobe)
            resultado.houveSplit = true;
            resultado.chavePromovida = tempChaves[1];
            resultado.prPromovido = tempPr[1]; // Preserva o byte offset da chave do meio
            resultado.ponteiroEsq = rrnAtual;
            resultado.ponteiroDir = cab->proxRRN;
            
            cab->proxRRN++;
            cab->nroNos++;
            
            return resultado;
        }
    }
}



/**
 * @brief Busca uma chave na árvore-B
 * 
 * Realiza busca binária na árvore-B para encontrar uma chave específica.
 * Retorna o byte offset do registro se encontrado.
 * 
 * @param fp Ponteiro para o arquivo da árvore-B
 * @param id ID do registro a ser buscado
 * @return Byte offset do registro ou -1 se não encontrado
 */
long long btree_search(FILE *fp, int id) {
    CabecalhoArvoreB cab;
    lerCabecalhoArvoreB(fp, &cab);
    
    // Verifica se o arquivo de índice tem status válido
    if (cab.status != '1') {
        return -1;
    }
    
    if (cab.noRaiz == -1) return -1;

    int rrn = cab.noRaiz;
    while (1) {
        PaginaArvoreB pag;
        lerPaginaArvoreB(fp, rrn, &pag);
        
        // Busca na página atual
        for (int i = 0; i < pag.nroChaves; i++) {
            if (pag.chaves[i] == id) {
                return pag.pr[i]; // Encontrou a chave
            }
            if (id < pag.chaves[i]) {
                if (pag.ponteiros[i] == INVALIDO) return -1;
                rrn = pag.ponteiros[i];
                goto proximo_nivel;
            }
        }
        // Se chegou aqui, vai para o último ponteiro
        if (pag.ponteiros[pag.nroChaves] == INVALIDO) return -1;
        rrn = pag.ponteiros[pag.nroChaves];
    proximo_nivel:;
    }
    return -1;
}

