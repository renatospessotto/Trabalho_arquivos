#include <stdio.h>
#include <stdint.h>

#define BYTES_TO_PRINT 100 // Number of bytes to print

/**
 * @brief Imprime os bytes de um arquivo binário a partir de um offset.
 *
 * Esta função lê e imprime os próximos 100 bytes de um arquivo binário a partir de um offset especificado.
 *
 * @param fileName Nome do arquivo binário.
 * @param offset Offset inicial para leitura.
 */
void printBytesFromOffset(const char *fileName, long long offset) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo: %s\n", fileName);
        return;
    }

    // Move o ponteiro do arquivo para o offset especificado
    if (fseek(file, offset, SEEK_SET) != 0) {
        printf("Erro ao mover para o offset %lld no arquivo.\n", offset);
        fclose(file);
        return;
    }

    // Lê e imprime os próximos 100 bytes
    printf("Bytes a partir do offset %lld:\n", offset);
    for (int i = 0; i < BYTES_TO_PRINT; i++) {
        uint8_t byte;
        if (fread(&byte, sizeof(uint8_t), 1, file) != 1) {
            printf("\nFim do arquivo alcançado.\n");
            break;
        }
        printf("%02X ", byte); // Imprime o byte em formato hexadecimal
        if ((i + 1) % 16 == 0) {
            printf("\n"); // Quebra de linha a cada 16 bytes
        }
    }

    fclose(file);
}

int main() {
    const char *fileName = "ataques7.bin"; // Nome do arquivo binário
    long long offset = 198786;           // Offset inicial para leitura

    printBytesFromOffset(fileName, offset);

    return 0;
}
