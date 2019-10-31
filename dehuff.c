#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "mec.h"
#include "huffman.h"

MEC_HUFF_HEAD readFileType(const char *sourceFileName);
boolean isMecHuffFile(MEC_HUFF_HEAD head);
FREQ *getFreq(const char *sourceFileName, MEC_HUFF_HEAD head);
void decodding(const char *sourceFileName, const char *targetFileName,
		HUFF_TAB *huffTab, int alphaCount, int bitCount);

void decodding(const char *sourceFileName, const char *targetFileName,
		HUFF_TAB *huffTab, int alphaCount, int bitCount) {
	FILE *in;
	FILE *out;
	int ch;
	int count = 0;
	int root = 2*alphaCount - 2;
	int bitIndex = 8;

	in = fopen(sourceFileName, "rb");
	out = fopen(targetFileName, "wb");

	fseek(in, sizeof(MEC_HUFF_HEAD) + alphaCount * sizeof(FREQ), SEEK_SET);
	while (count <= bitCount) {
		if (huffTab[root].leftChild == -1) {
			fputc(huffTab[root].freq.ch, out);
			root = 2*alphaCount - 2;
			if (count >= bitCount) {
				break;
			}
		} else {
			if (bitIndex >= 8) {
				ch = fgetc(in);
				bitIndex = 0;
			}
			if (GET(ch, bitIndex) == 0) {
				root = huffTab[root].leftChild;
			} else {
				root = huffTab[root].rightChild;
			}
			count++;
			bitIndex++;
		}
	}

	fclose(in);
	fclose(out);
}

FREQ *getFreq(const char *sourceFileName, MEC_HUFF_HEAD head) {
	FREQ *freq = NULL;
	FILE *fp;
	int index;

	freq = (FREQ *) calloc(sizeof(FREQ), head.alphaCount);
	fp = fopen(sourceFileName, "rb");

	fseek(fp, sizeof(MEC_HUFF_HEAD), SEEK_SET);
	fread(freq, sizeof(FREQ), head.alphaCount, fp);

	return freq;
}

boolean isMecHuffFile(MEC_HUFF_HEAD head) {
	return head.flag[0] == 'M' && head.flag[1] == 'E' && head.flag[2] == 'C';
}

MEC_HUFF_HEAD readFileType(const char *sourceFileName) {
	FILE *fp;
	MEC_HUFF_HEAD head = {0};

	fp = fopen(sourceFileName, "rb");
	fread(&head, sizeof(MEC_HUFF_HEAD), 1, fp);
	fclose(fp);

	return head;
}

int main(int argc, char **argv) {
	char sourceFileName[1024];
	char targetFileName[1024];
	MEC_HUFF_HEAD head;
	FREQ *freq = NULL;
	HUFF_TAB *huffTab = NULL;
	int alphaCount = 0;
	char string[256] = {0};
	int huftabIndex[256] = {0};

	if (argc != 3) {
		printf("用法: dehuff <源文件名称> <目标文件名称> <br>");
		return 0;
	}
	strcpy(sourceFileName, argv[1]);
	strcpy(targetFileName, argv[2]);
	if (!isFileExist(sourceFileName)) {
		printf("源文件[%s]不存在！\n", sourceFileName);
		return 0;
	}
	head = readFileType(sourceFileName);
	if (!isMecHuffFile(head)) {
		printf("不可识别的格式！\n");
		return 0;
	}
	alphaCount = head.alphaCount;

	freq = getFreq(sourceFileName, head);
	huffTab = initHuffTab(freq, huftabIndex, alphaCount);
	createHuffTree(huffTab, alphaCount);
	createHuffCode(huffTab, 2*alphaCount - 2, 0, string);

	decodding(sourceFileName, targetFileName, huffTab, alphaCount, head.bitCount);

	destoryHuffTab(huffTab, alphaCount);
	free(freq);

	return 0;
}
