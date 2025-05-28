#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SEQUENCE_LENGTH 8

/**
 * @brief Encontra uma sequência de bytes em um arquivo binário.
 *
 * Esta função procura uma sequência específica de bytes dentro de um arquivo binário aberto.
 *
 * @param file Ponteiro para o arquivo binário.
 * @param sequence Ponteiro para a sequência de bytes a ser encontrada.
 * @param sequenceLength O comprimento da sequência de bytes.
 * @return A posição da sequência de bytes no arquivo, ou -1 se a sequência não for encontrada.
 */
int findByteSequence(FILE *file, const uint8_t *sequence, size_t sequenceLength) {
    uint8_t buffer[SEQUENCE_LENGTH]; // Buffer para armazenar os bytes lidos
    size_t bytesRead = 0;
    long long position = 0;

    // Lê o arquivo byte a byte
    while (fread(&buffer[bytesRead], sizeof(uint8_t), 1, file) == 1) {
        bytesRead++;

        // Verifica se o buffer contém a sequência
        if (bytesRead == sequenceLength) {
            if (memcmp(buffer, sequence, sequenceLength) == 0) {
                // Retorna a posição inicial da sequência
                return position;
            }

            // Desloca o buffer para frente
            memmove(buffer, buffer + 1, sequenceLength - 1);
            bytesRead--;
        }

        position++;
    }

    // Retorna -1 se a sequência não for encontrada
    return -1;
}

int main() {
    // Sequência de bytes a ser encontrada
    const uint8_t sequence[SEQUENCE_LENGTH] = {0x04, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Nome do arquivo binário
    const char *fileName = "ataques7_final.bin";

    // Abre o arquivo binário
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo: %s\n", fileName);
        return 1;
    }

    // Procura pela sequência de bytes
    int position = findByteSequence(file, sequence, SEQUENCE_LENGTH);
    if (position != -1) {
        printf("Sequência encontrada na posição: %d\n", position);
    } else {
        printf("Sequência não encontrada.\n");
    }

    // Fecha o arquivo
    fclose(file);

    return 0;
}
