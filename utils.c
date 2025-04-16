#include "utils.h"
#include <stdlib.h>
#include <ctype.h> // Added to fix isspace warning
#include <string.h>
#include <stdio.h> // Added to fix FILE type error


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


float safeStringToFloat(const char *str) {
    char *endptr;
    float value = strtof(str, &endptr);

    if (endptr == str || *endptr != '\0') {
        return -1.0f;
    }
    return value;
}

void adjustValue(char *value) {
    size_t len = strlen(value);
    if (value[0] == '"' && value[len - 1] == '"') {
        memmove(value, value + 1, len - 2);
        value[len - 2] = '\0';
    }
}

int read_field(FILE *fp, char *dest) {
    int ch = fgetc(fp);
    if (ch == ',') {
        dest[0] = '\0';
        return 1;
    } else if (ch == EOF || ch == '\n') {
        return 0;
    } else {
        ungetc(ch, fp);
        fscanf(fp, "%255[^,\n]", dest);
        ch = fgetc(fp);
        return (ch != EOF);
    }
}
