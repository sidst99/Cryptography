#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000000
#define NO_OF_ROUNDS 16
#define BLOCK_SIZE 64

int *IP, *InvIP, *E, *P, **S, *PC1, *PC2, *LS;
int IPSize, InvIPSize, ESize, PSize, noOfSBoxes, SSize, PC1Size, PC2Size, LSSize;

int getSize(char *fileName) {
	int c, x;
	FILE *inputFile;

	inputFile = fopen(fileName, "r");
	c = 0;
	while(fscanf(inputFile, "%d", &x) == 1) {
		c++;
	}
	fclose(inputFile);

	return c;
}

int* getTable(char *fileName, int size, int normalize) {
	int i, x, *table;
	FILE *inputFile;

	inputFile = fopen(fileName, "r");
	table = (int *)malloc(size * sizeof(int));
	i = 0;
	while(fscanf(inputFile, "%d", &x) == 1) {
		table[i] = x + normalize;
		i++;
	}
	fclose(inputFile);

	return table;
}

int getNoOfSBoxes(char *fileName) {
	int c;
	char ch;
	FILE *inputFile;

	inputFile = fopen(fileName, "r");
	c = 0;
	while(!feof(inputFile)) {
	  	ch = fgetc(inputFile);
	  	if(ch == '\n') {
	    	c++;
	  	}
	}
	c++;
	fclose(inputFile);

	return c;
}

int getSBoxSize(char *fileName) {
	int c, x;
	char str[MAX_LEN], *token;
	FILE *inputFile;

	inputFile = fopen(fileName, "r");
	c = 0;
	if(fgets(str, MAX_LEN, inputFile)) {
		token = strtok(str, " ");
		while(token != NULL) {
			if(sscanf(token, "%d", &x) > 0) {
				c++;
			}
			token = strtok(NULL, " ");
		}
	}
	fclose(inputFile);

	return c;
}

int** getSBoxes(char *fileName) {
	int i, j, x, **sBoxes;
	char str[MAX_LEN], *token;
	FILE *inputFile;

	noOfSBoxes = getNoOfSBoxes(fileName);
	SSize = getSBoxSize(fileName);
	inputFile = fopen(fileName, "r");
	sBoxes = (int **)malloc(noOfSBoxes * sizeof(int *));
	for(i = 0 ; i < noOfSBoxes ; i++) {
		sBoxes[i] = (int *)malloc(SSize * sizeof(int));
	}
	i = 0;
	while(fgets(str, MAX_LEN, inputFile)) {
		token = strtok(str, " ");
		j = 0;
		while(token != NULL) {
			if(sscanf(token, "%d", &x) > 0) {
				sBoxes[i][j] = x;
				j++;
			}
			token = strtok(NULL, " ");
		}
		i++;
	}

	return sBoxes;
}

void initializeDESTables() {
	IPSize = getSize("IP.txt");
	IP = getTable("IP.txt", IPSize, -1);
	InvIPSize = getSize("InvIP.txt");
	InvIP = getTable("InvIP.txt", InvIPSize, -1);
	ESize = getSize("E.txt");
	E = getTable("E.txt", ESize, -1);
	PSize = getSize("P.txt");
	P = getTable("P.txt", PSize, -1);
	S = getSBoxes("S.txt");
	PC1Size = getSize("PC1.txt");
	PC1 = getTable("PC1.txt", PC1Size, -1);
	PC2Size = getSize("PC2.txt");
	PC2 = getTable("PC2.txt", PC2Size, -1);
	LSSize = getSize("LS.txt");
	LS = getTable("LS.txt", LSSize, 0);
}

void display(char *str, int size, int gap) {
	int i;

	for(i = 0 ; i < size ; i++) {
		printf("%c", str[i]);
		if((i+1)%gap == 0) {
			printf(" ");
		}
	}
	printf("\n");
}

char* applyPC1(char *key) {
	int i;
	char *newKey;

	newKey = (char *)malloc((PC1Size+1) * sizeof(char));
	for(i = 0 ; i < PC1Size ; i++) {
		newKey[i] = key[PC1[i]];
	}
	newKey[PC1Size] = '\0';

	return newKey;
}

void applyLS(char *str, int len, int shift) {
	int i;
	char ch;

	while(shift > 0) {
		i = 0;
		ch = str[i];
		while(i < len-1) {
			str[i] = str[i+1];
			i++;
		}
		str[len-1] = ch;
		shift--;
	}
}

char* applyPC2(char *key) {
	int i;
	char *newKey;

	newKey = (char *)malloc((PC2Size+1) * sizeof(char));
	for(i = 0 ; i < PC2Size ; i++) {
		newKey[i] = key[PC2[i]];
	}
	newKey[PC2Size] = '\0';

	return newKey;
}

char** keyScheduling(char *key, int noOfRounds) {
	int i, halfSize;
	char *newKey, **roundKeys;

	roundKeys = (char **)malloc(noOfRounds * sizeof(char *));
	newKey = applyPC1(key);
	halfSize = PC1Size / 2;
	for(i = 0 ; i < noOfRounds ; i++) {
		applyLS(newKey, halfSize, LS[i]);
		applyLS(newKey + halfSize, halfSize, LS[i]);
		roundKeys[i] = applyPC2(newKey);
	}

	return roundKeys;
}

char* applyE(char *right) {
	int i;
	char *newText;

	newText = (char *)malloc((ESize+1) * sizeof(char));
	for(i = 0 ; i < ESize ; i++) {
		newText[i] = right[E[i]];
	}
	newText[ESize] = '\0';

	return newText;
}

void xor(char *p, char *k) {
	while(*p && *k) {
		*p = (char)(((int)*p - 48)^((int)*k - 48) + 48);
		p++;
		k++;
	}
}

int binaryToInteger(char *bin, int l) {
	int res;

	res = 0;
	while(l > 0) {
		res = 2*res + ((int)*bin - 48);
		bin++;
		l--;
	}

	return res;
}

char* integerToBinary(int num, int l) {
	int i;
	char *res;

	i = l;
	res = (char *)malloc((l+1) * sizeof(char));
	while(l > 0) {
		res[l-1] = (char)(num%2 + 48);
		num = num / 2;
		l--;
	}
	res[i] = '\0';

	return res;
}

char* SBox(char *s, int len, int sBoxNo) {
	int i, j;
	char *a;

	a = (char *)malloc(2 * sizeof(char));
	a[0] = s[0];
	a[1] = s[len-1];
	i = binaryToInteger(a, 2);
	j = binaryToInteger(s+1, len-2);

	return integerToBinary(S[sBoxNo][i*16 + j], len-2);
}

char* applyS(char *text) {
	int i, j, size, sBoxInputSize, sBoxOutputSize;
	char *newText, *p;

	size = BLOCK_SIZE / 2;
	sBoxInputSize = ESize / noOfSBoxes;
	sBoxOutputSize = size / noOfSBoxes;
	newText = (char *)malloc((size+1) * sizeof(char));
	for(i = 0 ; i < noOfSBoxes ; i++) {
		p = text;
		p += i * sBoxInputSize;
		p = SBox(p, sBoxInputSize, i);
		for(j = 0 ; j < sBoxOutputSize ; j++) {
			newText[i*sBoxOutputSize + j] = p[j];
		}
	}

	return newText;
}

char* applyP(char *text) {
	int i;
	char *newText;

	newText = (char *)malloc((PSize+1) * sizeof(char));
	for(i = 0 ; i < PSize ; i++) {
		newText[i] = text[P[i]];
	}
	newText[PSize] = '\0';

	return newText;
}

char* applyFFunction(char *right, char *roundKey) {
	char *newText;

	newText = applyE(right);
	xor(newText, roundKey);
	newText = applyS(newText);
	newText = applyP(newText);

	return newText;
}

char* applyIP(char *text) {
	int i;
	char *newText;

	newText = (char *)malloc((BLOCK_SIZE+1) * sizeof(char));
	for(i = 0 ; i < BLOCK_SIZE ; i++) {
		newText[i] = text[IP[i]];
	}
	newText[BLOCK_SIZE] = '\0';

	return newText;
}

void swap(char *text) {
	int i;
	char temp;

	for(i = 0 ; i < BLOCK_SIZE/2 ; i++) {
		temp = text[i];
		text[i] = text[BLOCK_SIZE/2 + i];
		text[BLOCK_SIZE/2 + i] = temp;
	}
}

char* applyInvIP(char *text) {
	int i;
	char *newText;

	newText = (char *)malloc((BLOCK_SIZE+1) * sizeof(char));
	for(i = 0 ; i < BLOCK_SIZE ; i++) {
		newText[i] = text[InvIP[i]];
	}
	newText[BLOCK_SIZE] = '\0';

	return newText;
}

char* encryptDES(char *plainText, char *key, int textSize) {
	int i, j, k, noOfRounds, noOfBlocks, halfSize;
	char *cipherText, *blockText, *left, *right, *afterF, **roundKeys;

	cipherText = (char *)malloc((textSize+1) * sizeof(char));
	noOfRounds = NO_OF_ROUNDS;
	roundKeys = keyScheduling(key, noOfRounds);
	noOfBlocks = textSize / BLOCK_SIZE;
	for(i = 0 ; i < noOfBlocks ; i++) {
		blockText = plainText;
		blockText += i*BLOCK_SIZE;
		
		blockText = applyIP(blockText);

		halfSize = BLOCK_SIZE / 2;
		for(j = 0 ; j < noOfRounds ; j++) {
			left = blockText;
			right = blockText + halfSize;
			afterF = applyFFunction(right, roundKeys[j]);
			xor(afterF, left);
			for(k = 0 ; k < halfSize ; k++) {
				blockText[k] = right[k];
			}
			for(k = 0 ; k < halfSize ; k++) {
				blockText[k + halfSize] = afterF[k];
			}
		}

		swap(blockText);
		blockText = applyInvIP(blockText);

		for(j = 0 ; j < BLOCK_SIZE ; j++) {
			cipherText[i*BLOCK_SIZE + j] = blockText[j];
		}
	}
	cipherText[textSize] = '\0';

	return cipherText;
}

char* decryptDES(char *cipherText, char *key, int textSize) {
	int i, j, k, noOfRounds, noOfBlocks, halfSize;
	char *plainText, *blockText, *left, *right, *afterF, **roundKeys;

	plainText = (char *)malloc((textSize+1) * sizeof(char));
	noOfRounds = NO_OF_ROUNDS;
	roundKeys = keyScheduling(key, noOfRounds);
	noOfBlocks = textSize / BLOCK_SIZE;
	for(i = 0 ; i < noOfBlocks ; i++) {
		blockText = cipherText;
		blockText += i*BLOCK_SIZE;
		
		blockText = applyIP(blockText);
		swap(blockText);

		halfSize = BLOCK_SIZE / 2;
		for(j = 0 ; j < noOfRounds ; j++) {
			left = blockText;
			right = blockText + halfSize;
			afterF = applyFFunction(left, roundKeys[noOfRounds - j - 1]);
			xor(afterF, right);
			for(k = 0 ; k < halfSize ; k++) {
				blockText[k + halfSize] = left[k];
			}
			for(k = 0 ; k < halfSize ; k++) {
				blockText[k] = afterF[k];
			}
		}

		blockText = applyInvIP(blockText);

		for(j = 0 ; j < BLOCK_SIZE ; j++) {
			plainText[i*BLOCK_SIZE + j] = blockText[j];
		}
	}
	plainText[textSize] = '\0';

	return plainText;
}

char* encrypt(char *plainText, char *key1, char *key2, int textSize) {
	char *cipherText;
	
	cipherText = encryptDES(plainText, key1, textSize);
	cipherText = decryptDES(cipherText, key2, textSize);
	cipherText = encryptDES(cipherText, key1, textSize);

	return cipherText;
}

char* decrypt(char *cipherText, char *key1, char *key2, int textSize) {
	char *decryptedText;
	
	decryptedText = decryptDES(cipherText, key1, textSize);
	decryptedText = encryptDES(decryptedText, key2, textSize);
	decryptedText = decryptDES(decryptedText, key1, textSize);

	return decryptedText;
}

int main() {
	int textSize, key1Size, key2Size;
	FILE *inputFile, *outputFile;
	char *plainText, *key1, *key2, *cipherText, *decryptedText;

	initializeDESTables();

	inputFile = fopen("plainText.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	textSize = ftell(inputFile);
	rewind(inputFile);
	plainText = (char *)malloc((textSize+1) * sizeof(char));
	fread(plainText, sizeof(char), textSize, inputFile);
	fclose(inputFile);
	plainText[textSize] = '\0';

	inputFile = fopen("key1.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	key1Size = ftell(inputFile);
	rewind(inputFile);
	key1 = (char *)malloc((key1Size+1) * sizeof(char));
	fread(key1, sizeof(char), key1Size, inputFile);
	fclose(inputFile);
	key1[key1Size] = '\0';

	inputFile = fopen("key2.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	key2Size = ftell(inputFile);
	rewind(inputFile);
	key2 = (char *)malloc((key2Size+1) * sizeof(char));
	fread(key2, sizeof(char), key2Size, inputFile);
	fclose(inputFile);
	key2[key2Size] = '\0';

	cipherText = encrypt(plainText, key1, key2, textSize);
	decryptedText = decrypt(cipherText, key1, key2, textSize);

	outputFile = fopen("cipherText.txt", "w");
	fprintf(outputFile, "%s", cipherText);
	fclose(outputFile);

	outputFile = fopen("decryptedText.txt", "w");
	fprintf(outputFile, "%s", decryptedText);
	fclose(outputFile);

	printf("TRIPLE DATA ENCRYPTION STANDARD\n");
	printf("\nKey 1 : ");
	display(key1, key1Size, 8);
	printf("\nKey 2 : ");
	display(key2, key2Size, 8);
	printf("\nPlain Text : \n");
	display(plainText, textSize, 8);
	printf("\nCipher Text : \n");
	display(cipherText, textSize, 8);
	printf("\nDecrypted Text : \n");
	display(decryptedText, textSize, 8);

	return 0;
}