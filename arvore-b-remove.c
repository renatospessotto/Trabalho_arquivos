#include "arvore-b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INVALIDO -1

// Função para encontrar a chave sucessora em nó folha
int encontrarSucessora(FILE *fp, int rrn, int *chaveSuccessora, long long *prSuccessor) {  // CORRIGIDO: tipos trocados
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrn, &pag);
    
    // Se é folha, retorna a primeira chave
    if (pag.tipoNo == -1) {
        if (pag.nroChaves > 0) {
            *chaveSuccessora = pag.chaves[0];
            *prSuccessor = pag.pr[0];
            return rrn;
        }
        return -1;
    }
    
    // Se não é folha, vai para o primeiro filho
    if (pag.ponteiros[0] != INVALIDO) {
        return encontrarSucessora(fp, pag.ponteiros[0], chaveSuccessora, prSuccessor);
    }
    
    return -1;
}

// Função para encontrar a chave predecessor em nó folha
int encontrarPredecessor(FILE *fp, int rrn, int *chavePred, long long *prPred) {  // CORRIGIDO: tipos trocados
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrn, &pag);
    
    // Se é folha, retorna a última chave
    if (pag.tipoNo == -1) {
        if (pag.nroChaves > 0) {
            *chavePred = pag.chaves[pag.nroChaves - 1];
            *prPred = pag.pr[pag.nroChaves - 1];
            return rrn;
        }
        return -1;
    }
    
    // Se não é folha, vai para o último filho
    if (pag.ponteiros[pag.nroChaves] != INVALIDO) {
        return encontrarPredecessor(fp, pag.ponteiros[pag.nroChaves], chavePred, prPred);
    }
    
    return -1;
}

// Função para encontrar índice de uma chave no nó
int buscarIndiceChave(PaginaArvoreB *pag, int chave) {  // CORRIGIDO: int chave
    int idx = 0;
    while (idx < pag->nroChaves && pag->chaves[idx] < chave) {
        idx++;
    }
    return idx;
}

// Função para remover chave de nó folha
void removerDeFolha(FILE *fp, int rrnNo, int idx) {
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrnNo, &pag);
    
    // Move chaves e pr para a esquerda
    for (int i = idx + 1; i < pag.nroChaves; i++) {
        pag.chaves[i - 1] = pag.chaves[i];
        pag.pr[i - 1] = pag.pr[i];
    }
    
    // Limpa a última posição
    pag.chaves[pag.nroChaves - 1] = -1;
    pag.pr[pag.nroChaves - 1] = -1;
    pag.nroChaves--;
    
    escreverPaginaArvoreB(fp, rrnNo, &pag);
}

// Função para pegar chave do irmão anterior
void tomarDoAnterior(FILE *fp, int paiRRN, int idx) {
    PaginaArvoreB pai, filho, irmao;
    lerPaginaArvoreB(fp, paiRRN, &pai);
    lerPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    lerPaginaArvoreB(fp, pai.ponteiros[idx - 1], &irmao);
    
    // Move chaves do filho para a direita
    for (int i = filho.nroChaves; i > 0; i--) {
        filho.chaves[i] = filho.chaves[i - 1];
        filho.pr[i] = filho.pr[i - 1];
    }
    
    // Move ponteiros do filho para a direita (se não for folha)
    if (filho.tipoNo != -1) {
        for (int i = filho.nroChaves + 1; i > 0; i--) {
            filho.ponteiros[i] = filho.ponteiros[i - 1];
        }
    }
    
    // Move chave do pai para o filho
    filho.chaves[0] = pai.chaves[idx - 1];
    filho.pr[0] = pai.pr[idx - 1]; // PRESERVA o pr original
    
    // Move último ponteiro do irmão para o filho (se não for folha)
    if (filho.tipoNo != -1) {
        filho.ponteiros[0] = irmao.ponteiros[irmao.nroChaves];
    }
    
    // Move última chave do irmão para o pai
    pai.chaves[idx - 1] = irmao.chaves[irmao.nroChaves - 1];
    pai.pr[idx - 1] = irmao.pr[irmao.nroChaves - 1]; // PRESERVA o pr original
    
    // Limpa última posição do irmão
    irmao.chaves[irmao.nroChaves - 1] = -1;
    irmao.pr[irmao.nroChaves - 1] = -1;
    if (irmao.tipoNo != -1) {
        irmao.ponteiros[irmao.nroChaves] = INVALIDO;
    }
    
    // Atualiza contadores
    irmao.nroChaves--;
    filho.nroChaves++;
    
    // Escreve páginas atualizadas
    escreverPaginaArvoreB(fp, paiRRN, &pai);
    escreverPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    escreverPaginaArvoreB(fp, pai.ponteiros[idx - 1], &irmao);
}

// Função para pegar chave do próximo irmão
void tomarDoProximo(FILE *fp, int paiRRN, int idx) {
    PaginaArvoreB pai, filho, irmao;
    lerPaginaArvoreB(fp, paiRRN, &pai);
    lerPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    lerPaginaArvoreB(fp, pai.ponteiros[idx + 1], &irmao);
    
    // Move chave do pai para o filho
    filho.chaves[filho.nroChaves] = pai.chaves[idx];
    filho.pr[filho.nroChaves] = pai.pr[idx]; // PRESERVA o pr original
    
    // Move primeiro ponteiro do irmão para o filho (se não for folha)
    if (filho.tipoNo != -1) {
        filho.ponteiros[filho.nroChaves + 1] = irmao.ponteiros[0];
    }
    
    filho.nroChaves++;
    
    // Move primeira chave do irmão para o pai
    pai.chaves[idx] = irmao.chaves[0];
    pai.pr[idx] = irmao.pr[0]; // PRESERVA o pr original
    
    // Move chaves do irmão para a esquerda
    for (int i = 1; i < irmao.nroChaves; i++) {
        irmao.chaves[i - 1] = irmao.chaves[i];
        irmao.pr[i - 1] = irmao.pr[i];
    }
    
    // Move ponteiros do irmão para a esquerda (se não for folha)
    if (irmao.tipoNo != -1) {
        for (int i = 1; i <= irmao.nroChaves; i++) {
            irmao.ponteiros[i - 1] = irmao.ponteiros[i];
        }
    }
    
    // Limpa última posição do irmão
    irmao.chaves[irmao.nroChaves - 1] = -1;
    irmao.pr[irmao.nroChaves - 1] = -1;
    if (irmao.tipoNo != -1) {
        irmao.ponteiros[irmao.nroChaves] = INVALIDO;
    }
    
    irmao.nroChaves--;
    
    // Escreve páginas atualizadas
    escreverPaginaArvoreB(fp, paiRRN, &pai);
    escreverPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    escreverPaginaArvoreB(fp, pai.ponteiros[idx + 1], &irmao);
}

// Função para fundir dois nós
void fundir(FILE *fp, CabecalhoArvoreB *cab, int paiRRN, int idx) {
    PaginaArvoreB pai, filho, irmao;
    lerPaginaArvoreB(fp, paiRRN, &pai);
    lerPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    lerPaginaArvoreB(fp, pai.ponteiros[idx + 1], &irmao);
    
    if (filho.tipoNo == -1 && irmao.tipoNo == -1) {
        // Fusão de nós folha: apenas copia as chaves do irmão para o filho
        for (int i = 0; i < irmao.nroChaves; i++) {
            filho.chaves[filho.nroChaves] = irmao.chaves[i];
            filho.pr[filho.nroChaves] = irmao.pr[i]; // PRESERVA o pr original
            filho.nroChaves++;
        }
        
        // Limpa posições não utilizadas
        for (int i = filho.nroChaves; i < MAX_CHAVES; i++) {
            filho.chaves[i] = -1;
            filho.pr[i] = -1;
        }
    } else {
        // Fusão de nós internos: move chave do pai para o filho
        filho.chaves[filho.nroChaves] = pai.chaves[idx];
        filho.pr[filho.nroChaves] = pai.pr[idx]; // PRESERVA o pr da chave do pai
        filho.nroChaves++;
        
        // Copia chaves do irmão para o filho
        for (int i = 0; i < irmao.nroChaves; i++) {
            filho.chaves[filho.nroChaves] = irmao.chaves[i];
            filho.pr[filho.nroChaves] = irmao.pr[i]; // PRESERVA o pr original
            filho.nroChaves++;
        }
        
        // Copia ponteiros do irmão para o filho
        for (int i = 0; i <= irmao.nroChaves; i++) {
            filho.ponteiros[filho.nroChaves - irmao.nroChaves + i] = irmao.ponteiros[i];
        }
        
        // Limpa posições não utilizadas
        for (int i = filho.nroChaves; i < MAX_CHAVES; i++) {
            filho.chaves[i] = -1;
            filho.pr[i] = -1;
        }
        for (int i = filho.nroChaves + 1; i < MAX_PONTEIROS; i++) {
            filho.ponteiros[i] = INVALIDO;
        }
    }
    
    // Remove chave do pai e ajusta ponteiros
    for (int i = idx + 1; i < pai.nroChaves; i++) {
        pai.chaves[i - 1] = pai.chaves[i];
        pai.pr[i - 1] = pai.pr[i];
    }
    for (int i = idx + 2; i <= pai.nroChaves; i++) {
        pai.ponteiros[i - 1] = pai.ponteiros[i];
    }
    
    // Limpa última posição do pai
    pai.chaves[pai.nroChaves - 1] = -1;
    pai.pr[pai.nroChaves - 1] = -1;
    pai.ponteiros[pai.nroChaves] = INVALIDO;
    pai.nroChaves--;
    
    // Escreve páginas atualizadas
    escreverPaginaArvoreB(fp, paiRRN, &pai);
    escreverPaginaArvoreB(fp, pai.ponteiros[idx], &filho);
    
    cab->nroNos--; // Diminui número de nós
}

// Função auxiliar para remover sucessora de forma direta (sempre em folha)
void removerSucessoraSimples(FILE *fp, int rrnNo, int chave) {  // CORRIGIDO: int chave
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrnNo, &pag);
    
    // Se é folha, remove diretamente
    if (pag.tipoNo == -1) {
        for (int i = 0; i < pag.nroChaves; i++) {
            if (pag.chaves[i] == chave) {
                // Remove a chave
                for (int j = i; j < pag.nroChaves - 1; j++) {
                    pag.chaves[j] = pag.chaves[j + 1];
                    pag.pr[j] = pag.pr[j + 1];
                }
                // Limpa última posição
                pag.chaves[pag.nroChaves - 1] = -1;
                pag.pr[pag.nroChaves - 1] = -1;
                pag.nroChaves--;
                escreverPaginaArvoreB(fp, rrnNo, &pag);
                return;
            }
        }
    } else {
        // Se não é folha, continua procurando pelo primeiro filho (esquerda)
        if (pag.ponteiros[0] != INVALIDO) {
            removerSucessoraSimples(fp, pag.ponteiros[0], chave);
        }
    }
}

// Função auxiliar para remover predecessora de forma direta (sempre em folha)
void removerPredecessoraSimples(FILE *fp, int rrnNo, int chave) {  // CORRIGIDO: int chave
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrnNo, &pag);
    
    // Se é folha, remove diretamente
    if (pag.tipoNo == -1) {
        for (int i = 0; i < pag.nroChaves; i++) {
            if (pag.chaves[i] == chave) {
                // Remove a chave
                for (int j = i; j < pag.nroChaves - 1; j++) {
                    pag.chaves[j] = pag.chaves[j + 1];
                    pag.pr[j] = pag.pr[j + 1];
                }
                // Limpa última posição
                pag.chaves[pag.nroChaves - 1] = -1;
                pag.pr[pag.nroChaves - 1] = -1;
                pag.nroChaves--;
                escreverPaginaArvoreB(fp, rrnNo, &pag);
                return;
            }
        }
    } else {
        // Se não é folha, continua procurando pelo último filho (direita)
        if (pag.ponteiros[pag.nroChaves] != INVALIDO) {
            removerPredecessoraSimples(fp, pag.ponteiros[pag.nroChaves], chave);
        }
    }
}

// Função para remover de nó interno com correção
void removerDeInterno(FILE *fp, CabecalhoArvoreB *cab, int rrnNo, int idx) {
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrnNo, &pag);
    
    int chaveOriginal = pag.chaves[idx];  // CORRIGIDO: int
    
    // Estratégia 1: PRIMEIRO tenta usar predecessor
    if (pag.ponteiros[idx] != INVALIDO) {
        PaginaArvoreB subarvoreEsq;
        lerPaginaArvoreB(fp, pag.ponteiros[idx], &subarvoreEsq);
        
        if (subarvoreEsq.nroChaves >= 2) {
            int chavePred;              // CORRIGIDO: int
            long long prPred;           // CORRIGIDO: long long
            int rrnPred = encontrarPredecessor(fp, pag.ponteiros[idx], &chavePred, &prPred);
            
            if (rrnPred != -1) {
                // PRIMEIRO: Substitui a chave no nó interno
                pag.chaves[idx] = chavePred;
                pag.pr[idx] = prPred; // PRESERVA o pr do predecessor
                escreverPaginaArvoreB(fp, rrnNo, &pag);
                
                // DEPOIS: Remove o predecessor de sua posição original
                removerPredecessoraSimples(fp, pag.ponteiros[idx], chavePred);
                return;
            }
        }
    }
    
    // Estratégia 2: Se predecessor causaria underflow, tenta sucessor
    if (pag.ponteiros[idx + 1] != INVALIDO) {
        PaginaArvoreB subarvoreDir;
        lerPaginaArvoreB(fp, pag.ponteiros[idx + 1], &subarvoreDir);
        
        if (subarvoreDir.nroChaves >= 2) {
            int chaveSucc;              // CORRIGIDO: int
            long long prSucc;           // CORRIGIDO: long long
            int rrnSucc = encontrarSucessora(fp, pag.ponteiros[idx + 1], &chaveSucc, &prSucc);
            
            if (rrnSucc != -1) {
                // PRIMEIRO: Substitui a chave no nó interno
                pag.chaves[idx] = chaveSucc;
                pag.pr[idx] = prSucc; // PRESERVA o pr do sucessor
                escreverPaginaArvoreB(fp, rrnNo, &pag);
                
                // DEPOIS: Remove o sucessor de sua posição original
                removerSucessoraSimples(fp, pag.ponteiros[idx + 1], chaveSucc);
                return;
            }
        }
    }
    
    // Estratégia 3: Precisa fazer concatenação antes de remover
    if (pag.ponteiros[idx] != INVALIDO && pag.ponteiros[idx + 1] != INVALIDO) {
        // Funde a subárvore esquerda with a direita
        fundir(fp, cab, rrnNo, idx);
        
        // Agora remove recursivamente da subárvore fundida
        remover(fp, cab, pag.ponteiros[idx], chaveOriginal);
        return;
    }
    
    printf("Erro: não foi possível processar remoção em nó interno.\n");
}

// Função para verificar se um nó precisa de ajuste após remoção
int verificarUnderflow(FILE *fp, int rrn, int rrnRaiz) {
    if (rrn == rrnRaiz) return 0; // Raiz pode ter qualquer número de chaves ≥ 0
    
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrn, &pag);
    
    // ✅ Fórmula genérica: nós não-raiz devem ter pelo menos ⌈m/2⌉ - 1 chaves
    return (pag.nroChaves < MIN_CHAVES_NAO_RAIZ);
}

// Função para tratar underflow em um filho
void tratarUnderflow(FILE *fp, CabecalhoArvoreB *cab, int paiRRN, int indiceFilho) {
    PaginaArvoreB pai;
    lerPaginaArvoreB(fp, paiRRN, &pai);
    
    // Tenta redistribuir com irmão anterior
    if (indiceFilho > 0) {
        PaginaArvoreB irmaoEsq;
        lerPaginaArvoreB(fp, pai.ponteiros[indiceFilho - 1], &irmaoEsq);
        
        // ✅ Irmão deve ter mais que o mínimo para poder doar
        if (irmaoEsq.nroChaves > MIN_CHAVES_NAO_RAIZ) {
            tomarDoAnterior(fp, paiRRN, indiceFilho);
            return;
        }
    }
    
    // Tenta redistribuir com irmão posterior
    if (indiceFilho < pai.nroChaves) {
        PaginaArvoreB irmaoDir;
        lerPaginaArvoreB(fp, pai.ponteiros[indiceFilho + 1], &irmaoDir);
        
        // ✅ Mesmo critério para irmão direito
        if (irmaoDir.nroChaves > MIN_CHAVES_NAO_RAIZ) {
            tomarDoProximo(fp, paiRRN, indiceFilho);
            return;
        }
    }
    
    // Não conseguiu redistribuir, precisa fazer concatenação
    if (indiceFilho < pai.nroChaves) {
        // Concatena com irmão da direita
        fundir(fp, cab, paiRRN, indiceFilho);
    } else {
        // Concatena com irmão da esquerda
        fundir(fp, cab, paiRRN, indiceFilho - 1);
    }
}

// FUNÇÃO PRINCIPAL DE REMOÇÃO (ÚNICA VERSÃO)
void remover(FILE *fp, CabecalhoArvoreB *cab, int rrnNo, int chave) {  // CORRIGIDO: int chave
    PaginaArvoreB pag;
    lerPaginaArvoreB(fp, rrnNo, &pag);
    
    int idx = buscarIndiceChave(&pag, chave);
    
    if (idx < pag.nroChaves && pag.chaves[idx] == chave) {
        if (pag.tipoNo == -1) {
            // Caso 1: Remoção em folha
            removerDeFolha(fp, rrnNo, idx);
        } else {
            // Caso 2: Remoção em nó interno
            removerDeInterno(fp, cab, rrnNo, idx);
        }
    } else {
        if (pag.tipoNo == -1) return; // Chave não encontrada
        
        // Encontra o filho correto e continua a busca
        int indiceFilho = idx;
        if (pag.ponteiros[indiceFilho] != INVALIDO) {
            remover(fp, cab, pag.ponteiros[indiceFilho], chave);
            
            // APÓS a remoção recursiva, verifica se o filho precisa de ajuste
            if (verificarUnderflow(fp, pag.ponteiros[indiceFilho], cab->noRaiz)) {
                tratarUnderflow(fp, cab, rrnNo, indiceFilho);
            }
        }
    }
}
