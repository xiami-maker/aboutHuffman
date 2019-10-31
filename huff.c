#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "mec.h"
#include "huffman.h"

FREQ *getFreq(const char *fileName, int *chCount);
void codding(const char *sourceFileName, const char *targetFileName,
		FREQ *freq, int alphaCount,
		HUFF_TAB *huffTab, int *huftabIndex);
int getBitCount(HUFF_TAB *huffTab, int alphaCount);

int getBitCount(HUFF_TAB *huffTab, int alphaCount) {
	int i;
	int sum = 0;

	for (i = 0; i < alphaCount; i++) {
		sum += strlen(huffTab[i].code) * huffTab[i].freq.freq;
	}

	return sum;
}

void codding(const char *sourceFileName, const char *targetFileName, 
		FREQ *freq, int alphaCount,
		HUFF_TAB *huffTab, int *huftabIndex) {
	FILE *in;
	FILE *out;
	int ch;
	char *code;
	int i;
	u8 byte;
	int byteIndex = 0;
	MEC_HUFF_HEAD head = {0};

	head.flag[0] = 'M';
	head.flag[1] = 'E';
	head.flag[2] = 'C';
	head.alphaCount = alphaCount;
	head.bitCount = getBitCount(huffTab, alphaCount);

	in = fopen(sourceFileName, "rb");
	out = fopen(targetFileName, "wb");

	fwrite(&head, sizeof(MEC_HUFF_HEAD), 1, out);
	fwrite(freq, sizeof(FREQ), alphaCount, out);

	ch = fgetc(in);
	while (!feof(in)) {
		code = huffTab[huftabIndex[ch]].code;
		for (i = 0; code[i]; i++) {
			code[i] == '0' ? CLR(byte, byteIndex) : SET(byte, byteIndex);
			++byteIndex;
			if (byteIndex >= 8) {
				byteIndex = 0;
				fputc((int) byte, out);
			}
		}

		ch = fgetc(in);
	}
	if (byteIndex != 0) {
		fputc((int) byte, out);
	}
// 1010 1100  1111 1010  1100 1011  0011 0010  0010 0000  1110 1111  0110 1101  1111 0110
// A    C     F    A     C    B     3    2     2    0     E    F     6    D     F    6
// 0011 1110  1110 1111  0111 0111  101X XXXX
// 3    E     E    F     7    7     B    7
	fclose(in);
	fclose(out);
}

FREQ *getFreq(const char *fileName, int *chCount) {
	FILE *fp;
	int ch;
	FREQ *freq = NULL;
	int bytes[256] = {0};
	int index;
	int t = 0;

	fp = fopen(fileName, "rb");
	ch = fgetc(fp);
	while (!feof(fp)) {
		++bytes[ch];
		ch = fgetc(fp);
	}

	*chCount = 0;
	for (index = 0; index < 256; index++) {
		if (bytes[index] != 0) {
			++*chCount;
		}
	}

	freq = (FREQ *) calloc(sizeof(FREQ), *chCount);
	for (index = 0; index < 256; index++) {
		if (bytes[index] != 0) {
			freq[t].ch = index;
			freq[t].freq = bytes[index];
			t++;
		}
	}

	fclose(fp);

	return freq;
}

void showCoddingResource(char *fileName, HUFF_TAB *huffTab, int *huftabIndex);

// 1 01 01 1 001 1 1 1 1 01 01 1 001 01 1 001
// 1 001 0001 ... 01 1 1 1
void showCoddingResource(char *fileName, HUFF_TAB *huffTab, int *huftabIndex) {
	FILE *fp;
	int ch;

	fp = fopen(fileName, "rb");
	ch = fgetc(fp);
	while (!feof(fp)) {
		printf("%s", huffTab[huftabIndex[ch]].code);
		ch = fgetc(fp);
	}
	printf("\n");

	fclose(fp);
}

int main(int ac, char **str) {
	char sourceFileName[1024];
	char targetFileName[1024];
	FREQ *freq;
	HUFF_TAB *huffTab = NULL;
	int alphaCount = 0;
	char string[256] = {0};
	int huftabIndex[256] = {0};

	if (ac != 3) {
		printf("用法:test <源文件> <目标文件名><br>");
		return;
	}
	strcpy(sourceFileName, str[1]);
	strcpy(targetFileName, str[2]);
	strcat(targetFileName, ".mecHuf");
	if (!isFileExist(sourceFileName)) {
		printf("源文件[%s]不存在！\n", sourceFileName);
		return;
	}
	freq = getFreq(sourceFileName, &alphaCount);
	huffTab = initHuffTab(freq, huftabIndex, alphaCount);
	createHuffTree(huffTab, alphaCount);
	createHuffCode(huffTab, 2*alphaCount - 2, 0, string);
/*
	showFreq(freq, alphaCount);
	printf("-----------------------\n");
	showHuffTab(huffTab, alphaCount);

	printf(".....................\n");
	showCoddingResource(sourceFileName, huffTab, huftabIndex);
	printf("\n");
*/	codding(sourceFileName, targetFileName, freq, alphaCount, huffTab, huftabIndex);

	free(freq);
	destoryHuffTab(huffTab, alphaCount);

	return 0;
}

/*
a -> 16
d -> 4
e -> 1
f -> 35
s -> 2

下标	字节	频度	左孩子	右孩子
0		a		16		
1		d		4
2		e		1
3		f		35
4		s		2
 */
