#include "utils.h"
#include <stdlib.h>
#include <ctype.h> // Added to fix isspace warning
#include <string.h>
#include <stdio.h> // Added to fix FILE type error


/**
 * @brief Exibe o conteúdo de um arquivo binário na tela.
 *
 * Esta função lê o arquivo binário e imprime seu conteúdo em um formato específico.
 * Ela é usada para depuração e comparação.
 *
 * @param nomeArquivoBinario O nome do arquivo binário a ser exibido.
 */
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


/**
 * @brief Lê uma string entre aspas da entrada e remove as aspas.
 *
 * Esta função lê uma string delimitada por aspas duplas (") e remove as aspas.
 * Se a entrada for "NULO", define a string como vazia.
 *
 * @param str Ponteiro para a string onde o resultado será armazenado.
 */
void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

/**
 * @brief Converte uma string para float de forma segura.
 *
 * Esta função tenta converter uma string para um valor float. Caso a string seja inválida
 * ou contenha caracteres não numéricos, ela retorna -1.0f.
 *
 * @param str A string que será convertida.
 * @return O valor float convertido ou -1.0f se a conversão falhar.
 */
float safeStringToFloat(const char *str) {
    char *endptr; // Ponteiro para verificar onde a conversão parou
    float value = strtof(str, &endptr); // Converte a string para float

    // Verifica se a conversão falhou ou se há caracteres inválidos
    if (endptr == str || *endptr != '\0') {
        return -1.0f; // Retorna -1.0f em caso de erro
    }
    return value; // Retorna o valor convertido
}

/**
 * @brief Lê um campo de um arquivo CSV.
 *
 * Esta função lê um campo de um arquivo CSV, lidando com os delimitadores (vírgula)
 * e caracteres de fim de linha. Ela armazena o valor lido na string de destino.
 *
 * @param fp Ponteiro para o arquivo sendo lido.
 * @param dest Ponteiro para a string onde o campo será armazenado.
 * @return 1 se um campo foi lido com sucesso, 0 se o fim da linha ou do arquivo for alcançado.
 */
int read_field(FILE *fp, char *dest) {
    int ch = fgetc(fp); // Lê o próximo caractere do arquivo

    // Verifica se o caractere é uma vírgula (campo vazio)
    if (ch == ',') {
        dest[0] = '\0'; // Define a string como vazia
        return 1; // Retorna sucesso
    } 
    // Verifica se é o fim do arquivo ou fim da linha
    else if (ch == EOF || ch == '\n') {
        return 0; // Retorna falha
    } 
    // Caso contrário, lê o campo normalmente
    else {
        ungetc(ch, fp); // Devolve o caractere lido ao buffer
        fscanf(fp, "%255[^,\n]", dest); // Lê até encontrar uma vírgula ou fim de linha
        ch = fgetc(fp); // Lê o próximo caractere
        return (ch != EOF); // Retorna sucesso se não for o fim do arquivo
    }
}

/**
 * @brief Aloca memória e copia o conteúdo de um campo para o registro.
 *
 * @param field O campo do CSV a ser copiado.
 * @param recordField Ponteiro para o campo do registro onde o valor será armazenado.
 * @param input Ponteiro para o arquivo de entrada (para fechamento em caso de erro).
 * @param output Ponteiro para o arquivo de saída (para fechamento em caso de erro).
 * @return 0 em caso de sucesso, -1 em caso de falha.
 */
 int allocateAndCopyField(const char *field, char **recordField, FILE *input, FILE *output) {
    if (strlen(field) > 0) {
        *recordField = malloc(strlen(field) + 1); // +1 para o terminador nulo
        if (!*recordField) {
            fclose(input);
            fclose(output);
            return -1;
        }
        strcpy(*recordField, field);
    }
    return 0;
}

