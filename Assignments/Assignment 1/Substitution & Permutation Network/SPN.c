#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

char* copy(char *s, int size) {
	int i;
	char *c;
	c = (char *)malloc(size * sizeof(char));
	for(i = 0 ; i < size ; i++) {
		c[i] = s[i];
	}
	return c;
}

int computeLengthOfEachBlock(char *plainText) {
	int l;
	l = 0;
	while(*plainText != ' ') {
		l++;
		plainText++;
	}
	return l;
}

char** keyScheduling(char *key, int noOfRounds, int textSize, int l) {
	int i, j;
	char **roundKeys;

	roundKeys = (char **)malloc((noOfRounds + 1) * sizeof(char *));
	for(i = 0 ; i <= noOfRounds ; i++) {
		roundKeys[i] = (char *)malloc(textSize * sizeof(char));
	}
	for(i = 0 ; i <= noOfRounds ; i++) {
		for(j = 0 ; j < textSize ; j++) {
			roundKeys[i][j] = key[(l+1)*i + j];
		}
	}
	return roundKeys;
}

void swap (int *a, int *b)
{
	int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void randomize(int *substitution, int size, int seed) {
	int i, j;
	srand(time(NULL) + seed);
	for(i = size-1 ; i > 0 ; i--) {
		j = rand() % (i+1);
		swap(&substitution[i], &substitution[j]);
	}
}

int* generateSubstitution(int size) {
	int i, *substitution;
	substitution = (int *)malloc(size * sizeof(int));
	for(i = 0 ; i < size ; i++) {
		substitution[i] = i;
	}
	randomize(substitution, size, 0);
	return substitution;
}

int* generatePermutation(int size) {
	int i, *permutation;
	permutation = (int *)malloc(size * sizeof(int));
	for(i = 0 ; i < size ; i++) {
		permutation[i] = i;
	}
	randomize(permutation, size, 1);
	return permutation;
}

void xor(char *p, char *k) {
	while(*p && *k) {
		if(*p == ' ' || *k == ' ') {
			p++;
			k++;
			continue;
		}
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

void substitute(char *a, int l, int *substitution) {
	int i, j;
	char *bin;
	i = binaryToInteger(a, l);
	j = substitution[i];
	bin = integerToBinary(j, l);
	while(*bin) {
		*a = *bin;
		a++;
		bin++;
	}
}

char* removeSpace(char *p, int textSize, int permSize) {
	int i, j;
	char *c;
	c = (char *)malloc(permSize * sizeof(char));
	j = 0;
	for(i = 0 ; i < textSize ; i++) {
		if(p[i] != ' ') {
			c[j] = p[i];
			j++;
		}
	}
	return c;
}

void permute(char *p, int textSize, int *permutation, int permSize) {
	int i, space;
	char *q;

	q = removeSpace(p, textSize, permSize);
	
	space = 0;
	for(i = 0 ; i < textSize ; i++) {
		if(p[i] == ' ') {
			space++;
			continue;
		}
		p[i] = q[permutation[i-space]];
	}
}

char* encrypt(char *plainText, char **roundKeys, int *substitution, int *permutation, int noOfRounds, int textSize, int l, int permSize) {
	int i;
	char *cipherText, *a;

	cipherText = copy(plainText, textSize);

	for(i = 0 ; i < noOfRounds-1 ; i++) {
		xor(cipherText, roundKeys[i]);
		a = cipherText;
		while(*a) {
			while(*a && *a != ' ') {
				a++;
			}
			substitute(a-l, l, substitution);
			*a = ' ';
			a++;
		}
		permute(cipherText, textSize, permutation, permSize);
	}
	xor(cipherText, roundKeys[noOfRounds-1]);
	a = cipherText;
	while(*a) {
		while(*a && *a != ' ') {
			a++;
		}
		substitute(a-l, l, substitution);
		a++;
	}
	xor(cipherText, roundKeys[noOfRounds]);
	return cipherText;
}

int* inverse(int *arr, int size) {
	int i, *inv;
	inv = (int *)malloc(size * sizeof(int));
	for(i = 0 ; i < size ; i++) {
		inv[arr[i]] = i;
	}
	return inv;
}

char* decrypt(char *cipherText, char **roundKeys, int *substitution, int *permutation, int textSize, int subSize, int permSize, int noOfRounds, int l) {
	int i, *invSubstitution, *invPermutation;
	char *decryptedText, *a;

	invSubstitution = inverse(substitution, subSize);
	invPermutation = inverse(permutation, permSize);
	decryptedText = copy(cipherText, textSize);

	xor(decryptedText, roundKeys[noOfRounds]);
	a = decryptedText;
	while(*a) {
		while(*a && *a != ' ') {
			a++;
		}
		substitute(a-l, l, invSubstitution);
		a++;
	}
	xor(decryptedText, roundKeys[noOfRounds-1]);
	for(i = noOfRounds-2 ; i >= 0 ; i--) {
		permute(decryptedText, textSize, invPermutation, permSize);
		a = decryptedText;
		while(*a) {
			while(*a && *a != ' ') {
				a++;
			}
			substitute(a-l, l, invSubstitution);
			a++;
		}
		xor(decryptedText, roundKeys[i]);
	}
	return decryptedText;
}

int main() {
	int i, textSize, keySize, noOfRounds, l, permSize, subSize, *substitution, *permutation;
	char *key, *plainText, *cipherText, *decryptedText, **roundKeys;
	FILE *inputFile, *outputFile;

	inputFile = fopen("plainText.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	textSize = ftell(inputFile);
	rewind(inputFile);
	plainText = (char *)malloc(textSize * sizeof(char));
	fread(plainText, sizeof(char), textSize, inputFile);
	fclose(inputFile);
	
	inputFile = fopen("key.txt", "r");
	fseek(inputFile, 0, SEEK_END);
	keySize = ftell(inputFile);
	rewind(inputFile);
	key = (char *)malloc(keySize * sizeof(char));
	fread(key, sizeof(char), keySize, inputFile);
	fclose(inputFile);

	l = computeLengthOfEachBlock(plainText);
	noOfRounds = (keySize - textSize) / (l + 1);
	roundKeys = keyScheduling(key, noOfRounds, textSize, l);
	subSize = (int)pow(2, l);
	substitution = generateSubstitution(subSize);
	permSize = (l * (textSize + 1)) / (l+1);
	permutation = generatePermutation(permSize);
	cipherText = encrypt(plainText, roundKeys, substitution, permutation, noOfRounds, textSize, l, permSize);
	decryptedText = decrypt(cipherText, roundKeys, substitution, permutation, textSize, subSize, permSize, noOfRounds, l);

	outputFile = fopen("cipherText.txt", "w");
	fprintf(outputFile, "%s", cipherText);
	fclose(outputFile);

	outputFile = fopen("decryptedText.txt", "w");
	fprintf(outputFile, "%s", decryptedText);
	fclose(outputFile);

	printf("SUBSTITUTION & PERMUTATION NETWORK\n");
	printf("\nKey : %s\n", key);
	printf("\nPlain Text : %s\n", plainText);
	printf("\nRound Keys : \n");
	for(i = 0 ; i <= noOfRounds ; i++) {
		printf("K%d : %s\n", i+1, roundKeys[i]);
	}
	printf("\nSubstitution Function : \n");
	for(i = 0 ; i < subSize ; i++) {
		printf("%d\t", i);
	}
	printf("\n");
	for(i = 0 ; i < subSize ; i++) {
		printf("%d\t", substitution[i]);
	}
	printf("\n");
	printf("\nPermutation Function : \n");
	for(i = 0 ; i < permSize ; i++) {
		printf("%d\t", i);
	}
	printf("\n");
	for(i = 0 ; i < permSize ; i++) {
		printf("%d\t", permutation[i]);
	}
	printf("\n");
	printf("\nCipher Text : %s\n", cipherText);
	printf("\nDecrypted Text : %s\n", decryptedText);
}