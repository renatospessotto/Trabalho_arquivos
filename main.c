#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Incluído para corrigir o uso de fabs
#include "binary_operations.h"
#include "utils.h"

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

                    int removed = deleteRecordByCriteria(binaryFile, numCriteria, criteria, values);
                    if (removed >= 0) {
                        binarioNaTela(binaryFile);
                    } else {
                        printf("Falha ao remover registro(s).\n");
                    }
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
                    updateRecords(binaryFile, repeatCount, numPairs, criteria, values, numUpdates, updateFields, updateValues);
                }

                binarioNaTela(binaryFile);

                fflush(stdout);
                return 0;
                break;
            }

            case 7: {
                // Opção 7: Chama a função find_last_removed
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
            }

            case 8:
                // Opção 8: Encerra o programa
                return 0;
                break;

            default:
                // Opção inválida
                printf("Invalid choice. Please try again.\n");
                fflush(stdout); // Garante que a saída seja exibida imediatamente
                return 0; // Encerra o programa
        }
    } while (option != 8);

    return 0;
}