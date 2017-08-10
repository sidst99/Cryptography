#include <stdio.h>
#include <stdlib.h>

#define NO_OF_ROUNDS 10

char invSBox[16][16][2];
char sBox[16][16][2] = {{"63", "7C", "77", "7B", "F2", "6B", "6F", "C5", "30", "01", "67", "2B", "FE", "D7", "AB", "76"},
				 {"CA", "82", "C9", "7D", "FA", "59", "47", "F0", "AD", "D4", "A2", "AF", "9C", "A4", "72", "C0"},
				 {"B7", "FD", "93", "26", "36", "3F", "F7", "CC", "34", "A5", "E5", "F1", "71", "D8", "31", "15"},
				 {"04", "C7", "23", "C3", "18", "96", "05", "9A", "07", "12", "80", "E2", "EB", "27", "B2", "75"},
				 {"09", "83", "2C", "1A", "1B", "6E", "5A", "A0", "52", "3B", "D6", "B3", "29", "E3", "2F", "84"},
				 {"53", "D1", "00", "ED", "20", "FC", "B1", "5B", "6A", "CB", "BE", "39", "4A", "4C", "58", "CF"},
				 {"D0", "EF", "AA", "FB", "43", "4D", "33", "85", "45", "F9", "02", "7F", "50", "3C", "9F", "A8"},
				 {"51", "A3", "40", "8F", "92", "9D", "38", "F5", "BC", "B6", "DA", "21", "10", "FF", "F3", "D2"},
				 {"CD", "0C", "13", "EC", "5F", "97", "44", "17", "C4", "A7", "7E", "3D", "64", "5D", "19", "73"},
				 {"60", "81", "4F", "DC", "22", "2A", "90", "88", "46", "EE", "B8", "14", "DE", "5E", "0B", "DB"},
				 {"E0", "32", "3A", "0A", "49", "06", "24", "5C", "C2", "D3", "AC", "62", "91", "95", "E4", "79"},
				 {"E7", "C8", "37", "CD", "8D", "D5", "4E", "A9", "6C", "56", "F4", "EA", "65", "7A", "AE", "08"},
				 {"BA", "78", "25", "2E", "1C", "A6", "B4", "C6", "E8", "DD", "74", "1F", "4B", "BD", "8B", "8A"},
				 {"70", "3E", "B5", "66", "48", "03", "F6", "0E", "61", "35", "57", "B9", "86", "C1", "1D", "9E"},
				 {"E1", "F8", "98", "11", "69", "D9", "8E", "94", "9B", "1E", "87", "E9", "CE", "55", "28", "DF"},
				 {"8C", "A1", "89", "0D", "BF", "E6", "42", "68", "41", "99", "2D", "0F", "B0", "54", "BB", "16"}};

char* copy(char *s, int size) {
	int i;
	char *c;
	c = (char *)malloc((size + 1) * sizeof(char));
	for(i = 0 ; i < size ; i++) {
		c[i] = s[i];
	}
	c[size] = '\0';

	return c;
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

int hexToInt(char c) {
	if(c >= '0' && c <= '9')
		return (int)c - 48;
	else
		return (int)c - 55;
}

char intToHex(int num) {
	if(num >= 0 && num <= 9)
		return (char)(num + 48);
	else
		return (char)(num + 55);
}

void computeInvSBox() {
	int i, j, row, col;

	for(i = 0; i < 16; i++) {
		for(j = 0; j < 16; j++) {
			row = hexToInt(sBox[i][j][0]);
			col = hexToInt(sBox[i][j][1]);
			invSBox[row][col][0] = intToHex(i);
			invSBox[row][col][1] = intToHex(j);
		}
	}
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

char** getRcon(int keySize, int noOfRounds) {
	int i;
	char **rcon;

	rcon = (char **)malloc((noOfRounds + 1) * sizeof(char *));
	for(i = 0 ; i <= noOfRounds ; i++) {
		rcon[i] = (char *)malloc((keySize/4 + 1) * sizeof(char));
	}

	rcon[1] = "00000001000000000000000000000000\0";
	rcon[2] = "00000010000000000000000000000000\0";
	rcon[3] = "00000100000000000000000000000000\0";
	rcon[4] = "00001000000000000000000000000000\0";
	rcon[5] = "00010000000000000000000000000000\0";
	rcon[6] = "00100000000000000000000000000000\0";
	rcon[7] = "01000000000000000000000000000000\0";
	rcon[8] = "10000000000000000000000000000000\0";
	rcon[9] = "00011011000000000000000000000000\0";
	rcon[10] = "00110110000000000000000000000000\0";

	return rcon;
}

void rotWord(char *str, int len, int shift) {
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

void subWord(char *word, int len) {
	int i, j, row, col;
	char *c;

	for(i = 0 ; i < len/8 ; i++) {
		row = binaryToInteger(word + 8*i, 4);
		col = binaryToInteger(word + 8*i + 4, 4);
		c = integerToBinary(hexToInt(sBox[row][col][0]), 4);
		for(j = 0 ; j < 4 ; j++) {
			word[8*i + j] = c[j];
		}
		c = integerToBinary(hexToInt(sBox[row][col][1]), 4);
		for(j = 0 ; j < 4 ; j++) {
			word[8*i + 4 + j] = c[j];
		}
	}
}

char** keyExpansion(char *key, int keySize, int noOfRounds) {
	int i, j, halfSize;
	char **word, **rcon, *temp;

	word = (char **)malloc((noOfRounds + 1)*4 * sizeof(char *));
	for(i = 0 ; i < (noOfRounds + 1)*4 ; i++) {
		word[i] = (char *)malloc((keySize/4 + 1) * sizeof(char));
	}
	rcon = getRcon(keySize, noOfRounds);

	for(i =0 ; i < 4 ; i++) {
		for(j = 0 ; j < 8 ; j++) {
			word[i][j] = key[8*4*i+j];
			word[i][8+j] = key[8*(4*i+1)+j];
			word[i][16+j] = key[8*(4*i+2)+j];
			word[i][24+j] = key[8*(4*i+3)+j];
		}
		word[i][32] = '\0';
	}
	for(i = 4 ; i < 44 ; i++) {
		temp = copy(word[i-1], 32);
		if(i % 4 == 0) {
			rotWord(temp, 32, 8);
			subWord(temp, 32);
			xor(temp, rcon[i/4]);
		}
		for(j = 0 ; j <= keySize/4 ; j++) {
			word[i][j] = word[i-4][j];
		}
		xor(word[i], temp);
	}
	
	return word;
}

void addRoundKey(char *state, char **word, int keySize, int roundNo) {
	int i;

	for(i = 0 ; i < 4 ; i++) {
		xor(state + (keySize/4)*i, word[4*roundNo + i]);
	}
}

void subBytes(char *state, int textSize) {
	int i, j, row, col;
	char *c;

	for(i = 0 ; i < textSize/8 ; i++) {
		row = binaryToInteger(state + 8*i, 4);
		col = binaryToInteger(state + 8*i + 4, 4);
		c = integerToBinary(hexToInt(sBox[row][col][0]), 4);
		for(j = 0 ; j < 4 ; j++) {
			state[8*i + j] = c[j];
		}
		c = integerToBinary(hexToInt(sBox[row][col][1]), 4);
		for(j = 0 ; j < 4 ; j++) {
			state[8*i + 4 + j] = c[j];
		}
	}
}

void shiftRow(char *state, int textSize, int keySize) {
	int i;

	for(i = 1 ; i < 4 ; i++) {
		rotWord(state + (keySize/4)*i, keySize/4, 8*i);
	}
}

int fieldMultiplication(int m, int n) {
	if(n == 1) {
		return m;
	}
	if(n % 2 == 1) {
		return fieldMultiplication(m, n-1)^m;
	} else {
		int temp = fieldMultiplication(m, n/2)*2;
		return (temp > 255) ? (temp ^ 283) : (temp);
	}
}

void mixColumns(char *state, int textSize, int keySize) {
	char *c;
	int i, j, k, temp, temp2, stateMat[4][4];
	int constantMat[4][4] = {{2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2}};

	for(i = 0 ; i < 4 ; i++) {
		for(j = 0 ; j < 4 ; j++) {
			stateMat[i][j] = binaryToInteger(state + i*(keySize/4) + 8*j, 8);
		}
	}

	for(i = 0 ; i < 4 ; i++) {
		for(j = 0 ; j < 4 ; j++) {
			temp = 0;
			for(k = 0 ; k < 4 ; k++) {
				temp2 = fieldMultiplication(stateMat[k][j], constantMat[i][k]);
				temp = temp ^ temp2;
			}
			c = integerToBinary(temp, 8);
			for(k = 0 ; k < 8 ; k++) {
				state[i*(keySize/4) + 8*j + k] = c[k];
			}
		}
	}
}

char* encrypt(char *plainText, char **word, int keySize, int textSize, int noOfRounds) {
	int i;
	char *cipherText;

	cipherText = copy(plainText, textSize);
	addRoundKey(cipherText, word, keySize, 0);

	for(i = 1 ; i < noOfRounds ; i++) {
		subBytes(cipherText, textSize);
		shiftRow(cipherText, textSize, keySize);
		mixColumns(cipherText, textSize, keySize);
		addRoundKey(cipherText, word, keySize, i);
	}
	subBytes(cipherText, textSize);
	shiftRow(cipherText, textSize, keySize);
	addRoundKey(cipherText, word, keySize, 10);

	return cipherText;
}

void invShiftRow(char *state, int textSize, int keySize) {
	int i;

	for(i = 1 ; i < 4 ; i++) {
		rotWord(state + (keySize/4)*i, keySize/4, (keySize/4) - 8*i);
	}
}

void invSubBytes(char *state, int textSize) {
	int i, j, row, col;
	char *c;

	for(i = 0 ; i < textSize/8 ; i++) {
		row = binaryToInteger(state + 8*i, 4);
		col = binaryToInteger(state + 8*i + 4, 4);
		c = integerToBinary(hexToInt(invSBox[row][col][0]), 4);
		for(j = 0 ; j < 4 ; j++) {
			state[8*i + j] = c[j];
		}
		c = integerToBinary(hexToInt(invSBox[row][col][1]), 4);
		for(j = 0 ; j < 4 ; j++) {
			state[8*i + 4 + j] = c[j];
		}
	}
}

void invMixColumns(char *state, int textSize, int keySize) {
	char *c;
	int i, j, k, temp, temp2, stateMat[4][4];
	int constantMat[4][4] = {{14, 11, 13, 9}, {9, 14, 11, 13}, {13, 9, 14, 11}, {11, 13, 9, 14}};

	for(i = 0 ; i < 4 ; i++) {
		for(j = 0 ; j < 4 ; j++) {
			stateMat[i][j] = binaryToInteger(state + i*(keySize/4) + 8*j, 8);
		}
	}

	for(i = 0 ; i < 4 ; i++) {
		for(j = 0 ; j < 4 ; j++) {
			temp = 0;
			for(k = 0 ; k < 4 ; k++) {
				temp2 = fieldMultiplication(stateMat[k][j], constantMat[i][k]);
				temp = temp ^ temp2;
			}
			c = integerToBinary(temp, 8);
			for(k = 0 ; k < 8 ; k++) {
				state[i*(keySize/4) + 8*j + k] = c[k];
			}
		}
	}
}

char* decrypt(char *cipherText, char **word, int keySize, int textSize, int noOfRounds) {
	int i;
	char *decryptedText;

	decryptedText = copy(cipherText, textSize);
	addRoundKey(decryptedText, word, keySize, noOfRounds);

	for(i = noOfRounds-1 ; i > 0 ; i--) {
		invShiftRow(decryptedText, textSize, keySize);
		invSubBytes(decryptedText, textSize);
		addRoundKey(decryptedText, word, keySize, i);
		invMixColumns(decryptedText, textSize, keySize);
	}
	invShiftRow(decryptedText, textSize, keySize);
	invSubBytes(decryptedText, textSize);
	addRoundKey(decryptedText, word, keySize, 0);

	return decryptedText;
}

int main() {
	int textSize, keySize, noOfRounds;
	FILE *inputFile, *outputFile;
	char *plainText, *key, *cipherText, *decryptedText, **word;

	inputFile = fopen("plainText.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	textSize = ftell(inputFile);
	rewind(inputFile);
	plainText = (char *)malloc((textSize+1) * sizeof(char));
	fread(plainText, sizeof(char), textSize, inputFile);
	fclose(inputFile);
	plainText[textSize] = '\0';

	inputFile = fopen("key.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	keySize = ftell(inputFile);
	rewind(inputFile);
	key = (char *)malloc((keySize+1) * sizeof(char));
	fread(key, sizeof(char), keySize, inputFile);
	fclose(inputFile);
	key[keySize] = '\0';

	computeInvSBox();
	noOfRounds = NO_OF_ROUNDS;
	word = keyExpansion(key, keySize, noOfRounds);
	cipherText = encrypt(plainText, word, keySize, textSize, noOfRounds);
	decryptedText = decrypt(cipherText, word, keySize, textSize, noOfRounds);

	outputFile = fopen("cipherText.txt", "w");
	fprintf(outputFile, "%s", cipherText);
	fclose(outputFile);

	outputFile = fopen("decryptedText.txt", "w");
	fprintf(outputFile, "%s", decryptedText);
	fclose(outputFile);

	printf("ADVANCED ENCRYPTION STANDARD (AES-128)\n");
	
	printf("\nKey : \n");
	display(key, keySize, 8);
	printf("\nPlain Text : \n");
	display(plainText, textSize, 8);
	printf("\nCipher Text : \n");
	display(cipherText, textSize, 8);
	printf("\nDecrypted Text : \n");
	display(decryptedText, textSize, 8);

	return 0;
}