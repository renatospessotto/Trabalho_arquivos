#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Incluído para corrigir o uso de fabs
#include "binary_operations.h"
#include "utils.h"
#include "arvore-b.h"

/**
 * @brief Função principal para lidar com a entrada do usuário e executar opções.
 *
 * Esta função fornece uma interface baseada em menu para o usuário interagir com
 * o programa. Ela lida com opções como gerar arquivos binários, imprimir registros
 * e realizar buscas.
 *
 * @return 0 em caso de execução bem-sucedida.
 */
int main() {
    int option; // Armazena a opção escolhida pelo usuário
    char inputFile[100], binaryFile[100]; // Armazena os nomes dos arquivos de entrada e binário

    do {
        // Lê a opção escolhida pelo usuário
        scanf("%d", &option);

        switch (option) {
            case 1:
                // Opção 1: Gera um arquivo binário a partir de um arquivo CSV
                scanf("%s", inputFile);
                scanf("%s", binaryFile);
                if (generateBinaryFile(inputFile, binaryFile) == 0) { // Verifica sucesso
                    binarioNaTela(binaryFile); // Exibe o conteúdo do arquivo binário
                }
                return 0; // Encerra o programa
                break;

            case 2:
                // Opção 2: Imprime todos os registros de um arquivo binário
                scanf("%s", binaryFile);
                printAllUntilId(binaryFile);
                fflush(stdout); // Garante que a saída seja exibida imediatamente
                return 0; // Encerra o programa
                break;

            case 3: {
                // Opção 3: Realiza buscas sequenciais no arquivo binário
                scanf("%s", binaryFile);

                int repeatCount; // Número de buscas a serem realizadas
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int numCriteria; // Número de critérios de busca
                    scanf("%d", &numCriteria);

                    char criteria[3][256]; // Armazena os nomes dos critérios
                    char values[3][256]; // Armazena os valores dos critérios
                    for (int i = 0; i < numCriteria; i++) {
                        scanf("%s", criteria[i]);
                        // Verifica se o critério é uma string e lê o valor entre aspas
                        if (strcmp(criteria[i], "country") == 0 || 
                            strcmp(criteria[i], "targetIndustry") == 0 || 
                            strcmp(criteria[i], "defenseMechanism") == 0 || 
                            strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    // Realiza a busca sequencial com os critérios fornecidos
                    sequentialSearch(binaryFile, numCriteria, criteria, values);
                }

                return 0; // Encerra o programa
                break;
            }

            case 4: {
                // Opção 4: Exclui registros com múltiplos critérios
                scanf("%s", binaryFile);
                int removed = 0; // Contador de registros removidos
                int repeatCount;
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int numCriteria;
                    scanf("%d", &numCriteria);

                    char criteria[3][256];
                    char values[3][256];
                    for (int i = 0; i < numCriteria; i++) {
                        scanf("%s", criteria[i]);
                        if (strcmp(criteria[i], "country") == 0 ||
                            strcmp(criteria[i], "targetIndustry") == 0 ||
                            strcmp(criteria[i], "defenseMechanism") == 0 ||
                            strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    removed = deleteRecordByCriteria(binaryFile, numCriteria, criteria, values);
                   
                }

                if (removed >= 0) {
                    binarioNaTela(binaryFile);
                } else {
                    printf("Falha ao remover registro(s).\n");
                }

                fflush(stdout);
                return 0;
                break;
            }

            case 5: {
                // Opção 5: Inserção de registros
                scanf("%s", binaryFile);

                int repeatCount;
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int id, year;
                    float financialLoss;
                    char country[256], attackType[256], targetIndustry[256], defenseStrategy[256];

                    scanf("%d", &id);

                    // Leitura do campo year (pode ser NULO)
                    char yearStr[32];
                    scanf("%s", yearStr);
                    if (strcmp(yearStr, "NULO") == 0)
                        year = -1;
                    else
                        year = atoi(yearStr);

                    // Leitura do campo financialLoss (pode ser NULO)
                    char lossStr[32];
                    scanf("%s", lossStr);
                    if (strcmp(lossStr, "NULO") == 0)
                        financialLoss = -1.0f;
                    else
                        financialLoss = atof(lossStr);

                    // Leitura dos campos string (podem ser NULO)
                    scan_quote_string(country);
                    scan_quote_string(attackType);
                    scan_quote_string(targetIndustry);
                    scan_quote_string(defenseStrategy);

                    // Chama a função de inserção para cada registro
                    insertRecord(binaryFile, id, year, financialLoss,
                        (strcmp(country, "NULO") == 0 ? NULL : country),
                        (strcmp(attackType, "NULO") == 0 ? NULL : attackType),
                        (strcmp(targetIndustry, "NULO") == 0 ? NULL : targetIndustry),
                        (strcmp(defenseStrategy, "NULO") == 0 ? NULL : defenseStrategy));
                }

                // Após inserção, exiba o binário na tela
                binarioNaTela(binaryFile);

                fflush(stdout);
                return 0;
                break;
            }

            case 6: {
                // Opção 6: Atualização de registros
                scanf("%s", binaryFile);

                int repeatCount;
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int numPairs;
                    scanf("%d", &numPairs);

                    char criteria[3][256];
                    char values[3][256];
                    for (int i = 0; i < numPairs; i++) {
                        // Lê critério e valor
                        scanf("%s", criteria[i]);
                        if (strcmp(criteria[i], "country") == 0 ||
                            strcmp(criteria[i], "targetIndustry") == 0 ||
                            strcmp(criteria[i], "defenseMechanism") == 0 ||
                            strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    int numUpdates;
                    scanf("%d", &numUpdates);

                    char updateFields[3][256];
                    char updateValues[3][256];
                    for (int i = 0; i < numUpdates; i++) {
                        // Lê campo de atualização e novo valor
                        scanf("%s", updateFields[i]);
                        if (strcmp(updateFields[i], "country") == 0 ||
                            strcmp(updateFields[i], "targetIndustry") == 0 ||
                            strcmp(updateFields[i], "defenseMechanism") == 0 ||
                            strcmp(updateFields[i], "attackType") == 0) {
                            scan_quote_string(updateValues[i]);
                        } else {
                            scanf("%s", updateValues[i]);
                        }
                    }

                    // Chama a função de atualização
                    updateRecords(binaryFile, repeatCount, numPairs, criteria, values, numUpdates, updateFields, updateValues, NULL, 0);
                }

                binarioNaTela(binaryFile);

                return 0;
                break;
            }

            case 7: {
                // Opção 7: Constrói a árvore-B a partir do arquivo de dados
                char dataFile[100], btreeFile[100];
                scanf("%s", dataFile);
                scanf("%s", btreeFile);
                buildBTreeFromDataFile(dataFile, btreeFile);

                binarioNaTela(btreeFile); // Exibe o conteúdo da árvore-B
                return 0;
                break;
            }

            case 8: {
                // Opção 8: Busca híbrida que usa árvore-B para idAttack ou busca sequencial para outros critérios
                char btreeFile[100], dataFile[100];
                
                // Lê os nomes dos arquivos
                scanf("%s", dataFile);
                scanf("%s", btreeFile);

                int repeatCount; // Número de buscas a serem realizadas
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int numCriteria; // Número de critérios de busca
                    scanf("%d", &numCriteria);

                    char criteria[3][256]; // Armazena os nomes dos critérios
                    char values[3][256]; // Armazena os valores dos critérios
                    int idAttack = 0; // Flag para verificar se há busca por idAttack
                    int idValue = 0; // Valor do ID para busca na árvore-B
                    
                    for (int i = 0; i < numCriteria; i++) {
                        scanf("%s", criteria[i]);
                        
                        // Verifica se o critério é idAttack
                        if (strcmp(criteria[i], "idAttack") == 0) {
                            idAttack = 1;
                            scanf("%s", values[i]);
                            idValue = atoi(values[i]);
                        }
                        // Verifica se o critério é uma string e lê o valor entre aspas
                        else if (strcmp(criteria[i], "country") == 0 || 
                                 strcmp(criteria[i], "targetIndustry") == 0 || 
                                 strcmp(criteria[i], "defenseMechanism") == 0 || 
                                 strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    // Decide qual tipo de busca usar
                    if (idAttack == 1) {
                        // Usa busca por árvore-B para idAttack
                        printRecordFromBTree(dataFile, btreeFile, idValue);
                    } else {
                        // Usa busca sequencial para outros critérios
                        sequentialSearch(dataFile, numCriteria, criteria, values);
                    }
                }

                return 0;
                break;
            }

            case 9: {
                // Opção 9: Remove um registro da árvore-B
                char btreeFile[100];
                int id;  // CORRIGIDO: int ao invés de long long

                scanf("%s", btreeFile);
                scanf("%d", &id);  // CORRIGIDO: %d ao invés de %lld

                FILE *btreeFilePointer = fopen(btreeFile, "rb+");
                if (!btreeFilePointer) {
                    printf("Falha ao abrir o arquivo de índice.\n");
                    return 0;
                }

                if (btree_remove(btreeFilePointer, id)) {
                    printf("Registro removido com sucesso.\n");
                } else {
                    printf("Registro inexistente.\n");
                }

                fclose(btreeFilePointer);
                return 0;
                break;
            }

            case 10: {
                // Opção 10: Inserção de registros com atualização do índice (árvore-B)
                char dataFile[100], btreeFile[100];
                
                // Lê os nomes dos arquivos
                scanf("%s", dataFile);
                scanf("%s", btreeFile);

                int repeatCount;
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int id, year;
                    float financialLoss;
                    char country[256], attackType[256], targetIndustry[256], defenseStrategy[256];

                    scanf("%d", &id);

                    // Leitura do campo year (pode ser NULO)
                    char yearStr[32];
                    scanf("%s", yearStr);
                    if (strcmp(yearStr, "NULO") == 0)
                        year = -1;
                    else
                        year = atoi(yearStr);

                    // Leitura do campo financialLoss (pode ser NULO)
                    char lossStr[32];
                    scanf("%s", lossStr);
                    if (strcmp(lossStr, "NULO") == 0)
                        financialLoss = -1.0f;
                    else
                        financialLoss = atof(lossStr);

                    // Leitura dos campos string (podem ser NULO)
                    scan_quote_string(country);
                    scan_quote_string(attackType);
                    scan_quote_string(targetIndustry);
                    scan_quote_string(defenseStrategy);

                    // Insere o registro no arquivo de dados e obtém o offset
                    long long recordOffset = insertRecord(dataFile, id, year, financialLoss,
                        (strcmp(country, "NULO") == 0 ? NULL : country),
                        (strcmp(attackType, "NULO") == 0 ? NULL : attackType),
                        (strcmp(targetIndustry, "NULO") == 0 ? NULL : targetIndustry),
                        (strcmp(defenseStrategy, "NULO") == 0 ? NULL : defenseStrategy));

                    // Se a inserção foi bem-sucedida, adiciona na árvore-B
                    if (recordOffset != -1) {
                        insertBtreeEntry(btreeFile, id, recordOffset);
                    }
                }

                // Após inserção, exibe o binário na tela
                binarioNaTela(dataFile);
                binarioNaTela(btreeFile); // Exibe o conteúdo da árvore-B

                fflush(stdout);
                return 0;
                break;
            }
            case 11: {
                // Opção 11: Atualização de registros com atualização da árvore-B (updateWBTree)
                char dataFile[100], btreeFile[100];
                scanf("%s", dataFile);
                scanf("%s", btreeFile);

                int repeatCount;
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    int numPairs;
                    scanf("%d", &numPairs);

                    char criteria[3][256];
                    char values[3][256];
                    for (int i = 0; i < numPairs; i++) {
                        // Lê critério e valor
                        scanf("%s", criteria[i]);
                        if (strcmp(criteria[i], "country") == 0 ||
                            strcmp(criteria[i], "targetIndustry") == 0 ||
                            strcmp(criteria[i], "defenseMechanism") == 0 ||
                            strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    int numUpdates;
                    scanf("%d", &numUpdates);

                    char updateFields[3][256];
                    char updateValues[3][256];
                    for (int i = 0; i < numUpdates; i++) {
                        // Lê campo de atualização e novo valor
                        scanf("%s", updateFields[i]);
                        if (strcmp(updateFields[i], "country") == 0 ||
                            strcmp(updateFields[i], "targetIndustry") == 0 ||
                            strcmp(updateFields[i], "defenseMechanism") == 0 ||
                            strcmp(updateFields[i], "attackType") == 0) {
                            scan_quote_string(updateValues[i]);
                        } else {
                            scanf("%s", updateValues[i]);
                        }
                    }

                    // Chama a função de atualização com atualização da árvore-B
                    updateRecords(dataFile, repeatCount, numPairs, criteria, values, numUpdates, updateFields, updateValues, btreeFile, 1);
                }

                binarioNaTela(dataFile);
                binarioNaTela(btreeFile); // Exibe o conteúdo da árvore-B

                return 0;
                break;
            }

            case 12: {
                  // Opção 12: Encerra o programa
                return 0;
                break;
                
            }

            case 13: {
                // Opção 13: Lê e imprime um registro a partir de um offset
                char binaryFile[100];
                long long offset;

                scanf("%s", binaryFile);
                scanf("%lld", &offset);

                printRecordFromOffset(binaryFile, offset);

                return 0;
                break;
            }

            case 14:
                // Opção 14: Chama a função find_last_removed
                char binaryFile[100];
                scanf("%s", binaryFile);

                FILE *file = fopen(binaryFile, "rb");
                if (!file) {
                    printf("Falha no processamento do arquivo.\n");
                    return 0;
                }

                Header header;
                readHeader(file, &header);

                long lastRemovedOffset = find_last_removed(file, header.topo);
                if (lastRemovedOffset != -1) {
                    printf("Último registro removido está no offset: %ld\n", lastRemovedOffset);
                } else {
                    printf("Nenhum registro removido encontrado.\n");
                }

                fclose(file);
                return 0;
                break;
            case 15: {
                // Opção 10: Imprime o cabeçalho e todas as páginas da árvore-B
                char btreeFile[100];
                scanf("%s", btreeFile);
                printCabecalhoEPaginasArvoreB(btreeFile);
                return 0;
                break;
            }
            
            default:
                // Opção inválida
                printf("Invalid choice. Please try again.\n");
                fflush(stdout); // Garante que a saída seja exibida imediatamente
                return 0; // Encerra o programa
        }
    } while (option != 14);

    return 0;
}