#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_KEY_SIZE 1000
#define MAX_CIPHER_TEXT_SIZE 1000000

void convertStringToLower(char *s) {
    while(*s) {
    	*s = tolower((unsigned char) *s);
    	s++;
  	}
}

void changeJToI(char *s) {
	while(*s) {
		if(*s == 'j') {
			*s = 'i';
		}
		s++;
	}
}

void putKeyInKeyMatrix(char **keyMatrix, char *key, int *row, int *col, int **invKeyMatrix, bool *repetition) {
	while(*key) {
		if(!repetition[(int)*key - 97]) {
			repetition[(int)*key - 97] = true;
			if(*key == 'i') {
				repetition[106] = true;
			}
			keyMatrix[*row][*col] = *key;
			invKeyMatrix[(int)*key - 97][0] = *row;
			invKeyMatrix[(int)*key - 97][1] = *col;
			(*col)++;
			if((*col) >= 5) {
				(*col) = (*col) % 5;
				(*row)++;
			}
		}
		key++;
	}
}

char** createKeyMatrix(char *key, int **invKeyMatrix) {
	int i, j, row, col;
	bool *repetition = (bool *)malloc(sizeof(bool) * 26);
	char **keyMatrix;
	keyMatrix = (char **)malloc(sizeof(char *) * 5);
	for(i = 0 ; i < 5 ; i++) {
		keyMatrix[i] = (char *)malloc(sizeof(char) * 5);
	}
	row = 0;
	col = 0;
	putKeyInKeyMatrix(keyMatrix, key, &row, &col, invKeyMatrix, repetition);
	for(i = 97 ; i < 123 ; i++) {
		if(i == 106) {
			repetition[i] = true;
			continue;
		}
		if(!repetition[i-97]) {
			repetition[i-97] = true;
			keyMatrix[row][col] = (char)i;
			invKeyMatrix[i-97][0] = row;
			invKeyMatrix[i-97][1] = col;
			col++;
			if(col >= 5) {
				col = col % 5;
				row++;
			}
		}
	}
	return keyMatrix;
}

char* encrypt(char *plainText, char **keyMatrix, int **invKeyMatrix) {
	int row1, row2, col1, col2;
	char ch1, ch2, *cipherText, *c1, *c2, *c;
	cipherText = (char *)malloc(MAX_CIPHER_TEXT_SIZE * (sizeof(char)));
	c = cipherText;
	while(*plainText) {
		ch1 = *plainText;
		if(ch1 < 'a' || ch1 > 'z') {
			*c = ch1;
			plainText++;
			c++;
			continue;
		}
		plainText++;
		c1 = c;
		c++;
		ch2 = *plainText;
		while(*plainText && (ch2 < 'a' || ch2 > 'z')) {
			*c = ch2;
			plainText++;
			c++;
			ch2 = *plainText;
		}
		if(*plainText) {
			plainText++;
		}
		else {
			ch2 = 'x';
		}
		c2 = c;
		c++;
		if(ch1 == ch2) {
			ch2 = 'x';
			plainText--;
		}
		row1 = invKeyMatrix[(int)ch1 - 97][0];
		col1 = invKeyMatrix[(int)ch1 - 97][1];
		row2 = invKeyMatrix[(int)ch2 - 97][0];
		col2 = invKeyMatrix[(int)ch2 - 97][1];
		if(row1 == row2) {
			*c1 = keyMatrix[row1][(col1+1) % 5];
			*c2 = keyMatrix[row2][(col2+1) % 5];
		} else if(col1 == col2) {
			*c1 = keyMatrix[(row1+1) % 5][col1];
			*c2 = keyMatrix[(row2+1) % 5][col2];
		} else {
			*c1 = keyMatrix[row1][col2];
			*c2 = keyMatrix[row2][col1];
		}
	}
	*c = '\0';
	return cipherText;
}

char* decrypt(char *cipherText, char **keyMatrix, int **invKeyMatrix) {
	int row1, row2, col1, col2;
	char ch1, ch2, *decryptedText, *c1, *c2, *c;
	decryptedText = (char *)malloc(MAX_CIPHER_TEXT_SIZE * (sizeof(char)));
	c = decryptedText;
	while(*cipherText) {
		ch1 = *cipherText;
		if(ch1 < 'a' || ch1 > 'z') {
			*c = ch1;
			cipherText++;
			c++;
			continue;
		}
		cipherText++;
		c1 = c;
		c++;
		ch2 = *cipherText;
		while(*cipherText && (ch2 < 'a' || ch2 > 'z')) {
			*c = ch2;
			cipherText++;
			c++;
			ch2 = *cipherText;
		}
		if(*cipherText) {
			cipherText++;
		}
		else {
			ch2 = 'x';
		}
		c2 = c;
		c++;
		if(ch1 == ch2) {
			ch2 = 'x';
			cipherText--;
		}
		row1 = invKeyMatrix[(int)ch1 - 97][0];
		col1 = invKeyMatrix[(int)ch1 - 97][1];
		row2 = invKeyMatrix[(int)ch2 - 97][0];
		col2 = invKeyMatrix[(int)ch2 - 97][1];
		if(row1 == row2) {
			*c1 = keyMatrix[row1][((col1 > 0) ? (col1-1) : (col1+4)) % 5];
			*c2 = keyMatrix[row2][((col2 > 0) ? (col2-1) : (col2+4)) % 5];
		} else if(col1 == col2) {
			*c1 = keyMatrix[((row1 > 0) ? (row1-1) : (row1+4)) % 5][col1];
			*c2 = keyMatrix[((row2 > 0) ? (row2-1) : (row2+4)) % 5][col2];
		} else {
			*c1 = keyMatrix[row1][col2];
			*c2 = keyMatrix[row2][col1];
		}
	}
	*c = '\0';
	return decryptedText;
}

int main() {
	int i, j, textSize, **invKeyMatrix;
	char ch, *key, *plainText, *cipherText, *decryptedText, **keyMatrix;
	FILE *inputFile, *outputFile;

	inputFile = fopen("plainText.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	textSize = ftell(inputFile);
	rewind(inputFile);
	plainText = (char *)malloc(textSize * (sizeof(char)));
	fread(plainText, sizeof(char), textSize, inputFile);
	fclose(inputFile);
	convertStringToLower(plainText);
	changeJToI(plainText);

	printf("PLAYFAIR CIPHER\n\n");
	
	key = (char *)malloc(sizeof(char) * MAX_KEY_SIZE);
	printf("Enter the key : ");
	scanf("%s", key);
	convertStringToLower(key);
	changeJToI(key);

	invKeyMatrix = (int **)malloc(sizeof(int*) * 26);
	for(i = 0 ; i < 26 ; i++) {
		invKeyMatrix[i] = (int *)malloc(sizeof(int) * 2);
	}
	
	keyMatrix = createKeyMatrix(key, invKeyMatrix);
	cipherText = encrypt(plainText, keyMatrix, invKeyMatrix);
	decryptedText = decrypt(cipherText, keyMatrix, invKeyMatrix);

	outputFile = fopen("cipherText.txt", "w");
	fprintf(outputFile, "%s", cipherText);
	fclose(outputFile);

	outputFile = fopen("decryptedText.txt", "w");
	fprintf(outputFile, "%s", decryptedText);
	fclose(outputFile);

	printf("\nKey Matrix : \n");
	for(i = 0 ; i < 5 ; i++) {
		for(j = 0 ; j < 5 ; j++) {
			printf("%c ", keyMatrix[i][j]);
		}
		printf("\n");
	}
	printf("\nPlain Text : %s\n", plainText);
	printf("\nCipherText : %s\n", cipherText);
	printf("\nDecrypted Text : %s\n", decryptedText);

	return 0;
}	