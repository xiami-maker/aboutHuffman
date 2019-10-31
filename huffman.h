#pragma pack(push)
#pragma pack(1)

#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include "mec.h"

typedef struct FREQ {
	u8 ch;
	int freq;
}FREQ;

typedef struct HUFF_TAB {
	FREQ freq;
	int leftChild;
	int rightChild;
	boolean isVisited;
	char *code;
}HUFF_TAB;

typedef struct MEC_HUFF_HEAD {
	char flag[3];
	int alphaCount;
	int bitCount;
	u8 unused[5];
}MEC_HUFF_HEAD;

boolean isFileExist(const char *fileName);
void showFreq(FREQ *freq, int count);
HUFF_TAB *initHuffTab(const FREQ *freq, int *huftabIndex, int alphaCount);
void createHuffTree(HUFF_TAB *huffTab, int count);
void createHuffCode(HUFF_TAB *huffTab, int root, int index, char *str);
void destoryHuffTab(HUFF_TAB *huffTab, int alphaCount);
void showHuffTab(HUFF_TAB *huffTab, int alphaCount);

#endif

#pragma pack(pop)
