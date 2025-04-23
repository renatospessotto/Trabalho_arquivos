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

            case 4:
                // Opção 4: Encerra o programa
                return 0;
                break;

            default:
                // Opção inválida
                printf("Invalid choice. Please try again.\n");
                fflush(stdout); // Garante que a saída seja exibida imediatamente
                return 0; // Encerra o programa
        }
    } while (option != 4);

    return 0;
}