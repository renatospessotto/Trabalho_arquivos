/**
 * @file arvore-b.h
 * @brief Cabeçalho para implementação de Árvore-B de ordem 3
 * 
 * Este arquivo contém as definições de estruturas e funções para
 * manipulação de uma árvore-B usada como índice para registros
 * de ataques cibernéticos.
 */

#include <stdio.h>
#include <stdbool.h>

#define ORDEM 3
#define MAX_CHAVES (ORDEM - 1)
#define MAX_PONTEIROS ORDEM
#define MIN_CHAVES_NAO_RAIZ ((ORDEM + 1) / 2 - 1)
#define MIN_CHAVES_PARA_REDISTRIBUIR (MIN_CHAVES_NAO_RAIZ + 1)

/**
 * @brief Estrutura do cabeçalho da árvore-B
 */
typedef struct {
    char status;        // Status do arquivo ('0' inconsistente, '1' consistente)
    int noRaiz;         // RRN do nó raiz (-1 se árvore vazia)
    int proxRRN;        // Próximo RRN disponível
    int nroNos;         // Número total de nós na árvore
    char lixo[31];      // Preenchimento para completar 44 bytes
} CabecalhoArvoreB;

/**
 * @brief Estrutura de uma página (nó) da árvore-B
 */
typedef struct PaginaArvoreB {
    int tipoNo;                       // -1 (folha), 0 (raiz), 1 (intermediário)
    int nroChaves;                    // Número de chaves presentes no nó
    int ponteiros[MAX_PONTEIROS];     // RRN dos filhos
    int chaves[MAX_CHAVES];           // Chaves (IDs dos registros)
    long long pr[MAX_CHAVES];         // Byte offsets dos registros
    int rrn;                          // RRN deste nó
} PaginaArvoreB;

/**
 * @brief Estrutura para resultado de operação de split
 */
typedef struct {
    bool houveSplit;          // Indica se houve divisão do nó
    int chavePromovida;       // Chave promovida para o nível superior
    long long prPromovido;    // Byte offset da chave promovida
    int ponteiroEsq;          // RRN do nó à esquerda
    int ponteiroDir;          // RRN do nó à direita
} ResultadoSplit;

/**
 * @brief Estrutura para resultado de operação de remoção
 */
typedef struct {
    bool precisaAjuste;       // Indica se o nó precisa de ajuste
    bool diminuiuAltura;      // Indica se a altura da árvore diminuiu
} ResultadoRemocao;

// ================= FUNÇÕES PRINCIPAIS =================

/**
 * @brief Constrói uma árvore-B a partir de um arquivo de dados
 * @param dataFilename Nome do arquivo de dados
 * @param btreeFilename Nome do arquivo de índice a ser criado
 */
void buildBTreeFromDataFile(const char *dataFilename, const char *btreeFilename);

/**
 * @brief Busca e imprime um registro usando a árvore-B
 * @param btreeFilename Nome do arquivo de índice
 * @param dataFilename Nome do arquivo de dados
 * @param id ID do registro a ser buscado
 */
void printRecordFromBTree(const char *btreeFilename, const char *dataFilename, int id);

/**
 * @brief Imprime todas as páginas da árvore-B para debug
 * @param btreeFilename Nome do arquivo de índice
 */
void printCabecalhoEPaginasArvoreB(const char *btreeFilename);

/**
 * @brief Insere uma chave na árvore-B
 * @param btreeFile Ponteiro para o arquivo da árvore-B
 * @param id ID do registro (chave)
 * @param byteOffset Byte offset do registro no arquivo de dados
 */
void btree_insert(FILE *btreeFile, int id, long long byteOffset);

/**
 * @brief Remove uma chave da árvore-B
 * @param btreeFile Ponteiro para o arquivo da árvore-B
 * @param id ID do registro a ser removido
 * @return 1 se removido com sucesso, 0 caso contrário
 */
int btree_remove(FILE *btreeFile, int id);

/**
 * @brief Busca uma chave na árvore-B
 * @param fp Ponteiro para o arquivo da árvore-B
 * @param id ID do registro a ser buscado
 * @return Byte offset do registro ou -1 se não encontrado
 */
long long btree_search(FILE *fp, int id);

/**
 * @brief Atualiza o byte offset de uma chave existente na árvore-B
 * @param btreeFile Ponteiro para o arquivo da árvore-B
 * @param id ID do registro
 * @param newOffset Novo byte offset
 * @return 1 se atualizado com sucesso, 0 caso contrário
 */
int btree_update_offset(FILE *btreeFile, int id, long long newOffset);

/**
 * @brief Insere uma entrada diretamente na árvore-B
 * @param btreeFile Nome do arquivo de índice
 * @param id ID do registro
 * @param offset Byte offset do registro
 */
void insertBtreeEntry(const char *btreeFile, int id, long long offset);

// ================= FUNÇÕES DE I/O BÁSICAS =================

/**
 * @brief Inicializa o cabeçalho da árvore-B
 * @param fp Ponteiro para o arquivo
 */
void inicializaCabecalhoArvoreB(FILE *fp);

/**
 * @brief Inicializa uma página da árvore-B
 * @param pag Ponteiro para a página
 * @param tipoNo Tipo do nó (-1: folha, 0: raiz, 1: intermediário)
 */
void inicializaPagina(PaginaArvoreB *pag, int tipoNo);

/**
 * @brief Escreve o cabeçalho no arquivo
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 */
void escreverCabecalhoArvoreB(FILE *fp, CabecalhoArvoreB *cab);

/**
 * @brief Lê o cabeçalho do arquivo
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 */
void lerCabecalhoArvoreB(FILE *fp, CabecalhoArvoreB *cab);

/**
 * @brief Escreve uma página no arquivo
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN da página
 * @param pag Ponteiro para a página
 */
void escreverPaginaArvoreB(FILE *fp, int rrn, PaginaArvoreB *pag);

/**
 * @brief Lê uma página do arquivo
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN da página
 * @param pag Ponteiro para a página
 */
void lerPaginaArvoreB(FILE *fp, int rrn, PaginaArvoreB *pag);

// ================= FUNÇÕES DE INSERÇÃO =================

/**
 * @brief Insere uma chave ordenadamente em uma página
 * @param pag Ponteiro para a página
 * @param id ID do registro (chave)
 * @param pr Byte offset do registro
 */
void insereOrdenadoNaPagina(PaginaArvoreB *pag, int id, long long pr);

/**
 * @brief Função recursiva para inserção na árvore-B
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param rrnAtual RRN do nó atual
 * @param id ID do registro a ser inserido
 * @param pr Byte offset do registro
 * @return Resultado da operação de split
 */
ResultadoSplit inserirRecursivo(FILE *fp, CabecalhoArvoreB *cab, int rrnAtual, int id, long long pr);








// ================= FUNÇÕES DE REMOÇÃO =================

/**
 * @brief Função principal para remoção recursiva
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param rrnNo RRN do nó atual
 * @param chave Chave a ser removida
 */
void remover(FILE *fp, CabecalhoArvoreB *cab, int rrnNo, int chave);

/**
 * @brief Remove sucessora simples (em nó folha)
 * @param fp Ponteiro para o arquivo
 * @param rrnNo RRN do nó
 * @param chave Chave a ser removida
 */
void removerSucessoraSimples(FILE *fp, int rrnNo, int chave);

/**
 * @brief Remove predecessora simples (em nó folha)
 * @param fp Ponteiro para o arquivo
 * @param rrnNo RRN do nó
 * @param chave Chave a ser removida
 */
void removerPredecessoraSimples(FILE *fp, int rrnNo, int chave);

/**
 * @brief Trata underflow em um filho
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param paiRRN RRN do nó pai
 * @param indiceFilho Índice do filho com underflow
 */
void tratarUnderflow(FILE *fp, CabecalhoArvoreB *cab, int paiRRN, int indiceFilho);

/**
 * @brief Verifica se um nó tem underflow
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN do nó
 * @param rrnRaiz RRN da raiz
 * @return 1 se tem underflow, 0 caso contrário
 */
int verificarUnderflow(FILE *fp, int rrn, int rrnRaiz);

/**
 * @brief Encontra a chave sucessora
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN do nó
 * @param chaveSuccessora Ponteiro para armazenar a chave sucessora
 * @param prSuccessor Ponteiro para armazenar o PR sucessor
 * @return RRN do nó onde está a sucessora
 */
int encontrarSucessora(FILE *fp, int rrn, int *chaveSuccessora, long long *prSuccessor);

/**
 * @brief Encontra a chave predecessora
 * @param fp Ponteiro para o arquivo
 * @param rrn RRN do nó
 * @param chavePred Ponteiro para armazenar a chave predecessora
 * @param prPred Ponteiro para armazenar o PR predecessor
 * @return RRN do nó onde está a predecessora
 */
int encontrarPredecessor(FILE *fp, int rrn, int *chavePred, long long *prPred);

/**
 * @brief Busca o índice de uma chave em uma página
 * @param pag Ponteiro para a página
 * @param chave Chave a ser buscada
 * @return Índice da chave ou posição onde deveria estar
 */
int buscarIndiceChave(PaginaArvoreB *pag, int chave);

/**
 * @brief Remove chave de nó folha
 * @param fp Ponteiro para o arquivo
 * @param rrnNo RRN do nó
 * @param idx Índice da chave a ser removida
 */
void removerDeFolha(FILE *fp, int rrnNo, int idx);

/**
 * @brief Toma uma chave do irmão anterior
 * @param fp Ponteiro para o arquivo
 * @param paiRRN RRN do nó pai
 * @param idx Índice do filho
 */
void tomarDoAnterior(FILE *fp, int paiRRN, int idx);

/**
 * @brief Toma uma chave do próximo irmão
 * @param fp Ponteiro para o arquivo
 * @param paiRRN RRN do nó pai
 * @param idx Índice do filho
 */
void tomarDoProximo(FILE *fp, int paiRRN, int idx);

/**
 * @brief Funde dois nós irmãos
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param paiRRN RRN do nó pai
 * @param idx Índice do primeiro filho a ser fundido
 */
void fundir(FILE *fp, CabecalhoArvoreB *cab, int paiRRN, int idx);

/**
 * @brief Remove chave de nó interno
 * @param fp Ponteiro para o arquivo
 * @param cab Ponteiro para o cabeçalho
 * @param rrnNo RRN do nó
 * @param idx Índice da chave a ser removida
 */
void removerDeInterno(FILE *fp, CabecalhoArvoreB *cab, int rrnNo, int idx);
