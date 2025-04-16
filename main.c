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
	free(mb);
	fclose(fs);
}

int main() {
    int option;
    char inputFile[100], binaryFile[100];

    do {
        printf("\nMenu:\n");
        printf("1. Generate binary file from CSV\n");
        printf("2. Print all records and header\n");
        printf("3. Search for a record by criteria\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                scanf("%s", inputFile);
                scanf("%s", binaryFile);
                generateBinaryFile(inputFile, binaryFile);
                binarioNaTela(binaryFile);
                break;

            case 2:
                printf("Enter the name of the binary file: ");
                scanf("%s", binaryFile);
                printAllUntilId(binaryFile);
                break;

            case 3: {
                printf("Enter the name of the binary file: ");
                scanf("%s", binaryFile);

                int repeatCount;
                printf("Enter the number of times to repeat the search: ");
                scanf("%d", &repeatCount);

                for (int r = 0; r < repeatCount; r++) {
                    printf("Search iteration %d:\n", r + 1);

                    int numCriteria;
                    printf("Enter the number of criteria (1-3): ");
                    scanf("%d", &numCriteria);

                    char criteria[3][256];
                    char values[3][256];
                    for (int i = 0; i < numCriteria; i++) {
                        printf("MERDA");

                        printf("Enter criteria %d: ", i + 1);
                        scanf("%s", criteria[i]);
                        if (strcmp(criteria[i], "country") == 0 || 
                            strcmp(criteria[i], "targetIndustry") == 0 || 
                            strcmp(criteria[i], "defenseStrategy") == 0 || 
                            strcmp(criteria[i], "attackType") == 0) {
                            scan_quote_string(values[i]);
                            printf("merda");

                        } else {
                            scanf("%s", values[i]);
                        }
                    }

                    sequentialSearch(binaryFile, numCriteria, criteria, values);
                }
                break;
            }

            case 4:
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (option != 4);

    return 0;
}