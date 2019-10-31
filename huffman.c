#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "mec.h"
#include "huffman.h"

static int findMinFreq(HUFF_TAB *huffTab, int count);

boolean isFileExist(const char *fileName) {
	FILE *fp;

	fp = fopen(fileName, "r");
	if (NULL == fp) {
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}

void showFreq(FREQ *freq, int count) {
	int i;

	for (i = 0; i < count; i++) {
		printf("%d:[%c] -> %d\n", freq[i].ch, freq[i].ch, freq[i].freq);
	}
}

HUFF_TAB *initHuffTab(const FREQ *freq, int *huftabIndex, int alphaCount) {
	HUFF_TAB *huffTab = NULL;
	int index = 0;

	huffTab = (HUFF_TAB *) calloc(sizeof(HUFF_TAB), 2*alphaCount - 1);
	for (; index < alphaCount; index++) {
		huffTab[index].freq.ch = freq[index].ch;
		huffTab[index].freq.freq = freq[index].freq;
		huffTab[index].isVisited = FALSE;
		huffTab[index].leftChild = huffTab[index].rightChild = -1;
		huffTab[index].code = (char *) calloc(sizeof(char), alphaCount);
		huftabIndex[freq[index].ch] = index;
	}

	return huffTab;
}

void createHuffCode(HUFF_TAB *huffTab, int root, int index, char *str) {
	if (huffTab[root].leftChild == -1) {
		str[index] = 0;
		strcpy(huffTab[root].code, str);
	} else {
		str[index] = '0';
		createHuffCode(huffTab, huffTab[root].leftChild, index+1, str);
		str[index] = '1';
		createHuffCode(huffTab, huffTab[root].rightChild, index+1, str);
	}
}

void createHuffTree(HUFF_TAB *huffTab, int count) {
	int leftChild;
	int rightChild;
	int i;
	int t = count;

	for (i = 0; i < count-1; i++) {
		leftChild = findMinFreq(huffTab, count + i);
		rightChild = findMinFreq(huffTab, count + i);
		huffTab[t].freq.ch = '#';
		huffTab[t].freq.freq = huffTab[leftChild].freq.freq + huffTab[rightChild].freq.freq;
		huffTab[t].leftChild = leftChild;
		huffTab[t].rightChild = rightChild;
		huffTab[t].isVisited = FALSE;
		huffTab[t].code = NULL;
		t++;
	}
}

int findMinFreq(HUFF_TAB *huffTab, int count) {
	int minIndex = -1;
	int index;

	for (index = 0; index < count; index++) {
		if (!huffTab[index].isVisited && (
				minIndex == -1 || huffTab[index].freq.freq < huffTab[minIndex].freq.freq)) {
			minIndex = index;
		}
	}

	huffTab[minIndex].isVisited = TRUE;

	return minIndex;
}

void destoryHuffTab(HUFF_TAB *huffTab, int alphaCount) {
	int i;

	for (i = 0; i < alphaCount; i++) {
		free(huffTab[i].code);
	}

	free(huffTab);
}

void showHuffTab(HUFF_TAB *huffTab, int alphaCount) {
	int index;

	printf("%-5s%-5s%-5s%-7s%-7s%s\n", "下标", "字节", "频度", "左孩子", "右孩子", "编码");
	for (index = 0; index < 2*alphaCount-1; index++) {
		printf("%-5d%-5d%-5d%-7d%-7d%s\n", 
			index,
			huffTab[index].freq.ch,
			huffTab[index].freq.freq,
			huffTab[index].leftChild,
			huffTab[index].rightChild,
			huffTab[index].code == NULL ? "无编码" : huffTab[index].code);
	}
}
