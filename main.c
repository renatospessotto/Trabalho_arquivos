#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Adicionado para corrigir o uso de fabs
#include "binary_operations.h"
#include "utils.h"


void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	fflush(stdout); // Força a limpeza do buffer de saída
	free(mb);
	fclose(fs);
}

int main() {
    int option;
    char inputFile[100], binaryFile[100];

    do {
        scanf("%d", &option);

        switch (option) {
            case 1:
                scanf("%s", inputFile);
                scanf("%s", binaryFile);
                if (generateBinaryFile(inputFile, binaryFile) == 0) { // Check for success
                    binarioNaTela(binaryFile); // Call binarioNaTela only once
                }
                return 0; // Fecha o programa
                break;

            case 2:
                scanf("%s", binaryFile);
                printAllUntilId(binaryFile);
                fflush(stdout); // Força a limpeza do buffer de saída
                return 0; // Fecha o programa
                break;

            case 3: {
                scanf("%s", binaryFile);

                int repeatCount;
                scanf("%d", &repeatCount);

                int foundAny = 0; // Flag to track if any search produces results
                int processingFailed = 0; // Flag to track if processing failed

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

                    int found = sequentialSearch(binaryFile, numCriteria, criteria, values);
                    if (found == -1) { // Indicate processing failure
                        processingFailed = 1;
                        break;
                    }
                    if (found) {
                        foundAny = 1; // Mark that at least one result was found
                    }
                    fflush(stdout); // Força a limpeza do buffer de saída
                }

                if (!processingFailed && !foundAny) {
                    printf("Registro inexistente.\n\n");
                    printf("**********\n");
                }

                return 0; // Fecha o programa
                break;
            }

            case 4:
                return 0; // Fecha o programa
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                fflush(stdout); // Força a limpeza do buffer de saída
                return 0; // Fecha o programa
        }
    } while (option != 4);

    return 0;
}