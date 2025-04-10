#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *file1, *file2;
    unsigned char byte1, byte2;
    long position = 0;

    // Verifica se os dois arquivos foram passados como argumentos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s arquivo1.bin arquivo2.bin\n", argv[0]);
        return 1;
    }

    // Abre os arquivos
    file1 = fopen(argv[1], "rb");
    file2 = fopen(argv[2], "rb");

    if (file1 == NULL || file2 == NULL) {
        perror("Erro ao abrir um dos arquivos");
        return 1;
    }

    // Compara byte por byte
    while (1) {
        size_t read1 = fread(&byte1, 1, 1, file1);
        size_t read2 = fread(&byte2, 1, 1, file2);

        // Verifica se chegou ao fim de algum arquivo
        if (read1 == 0 || read2 == 0) {
            if (read1 != read2) {
                printf("Arquivos têm tamanhos diferentes.\n");
            }
            break;
        }

        if (byte1 != byte2) {
            printf("Diferença no byte %ld: %02X != %02X\n", position, byte1, byte2);
        }

        position++;
    }

    fclose(file1);
    fclose(file2);

    return 0;
}
