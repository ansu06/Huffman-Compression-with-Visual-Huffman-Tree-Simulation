#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>

typedef struct Node {
    char ch;
    int freq;
    struct Node *left, *right;
} Node;

extern char codes[256][50];
extern int codeLength[256];

void buildFrequency(char *text, int freq[]);
Node* buildHuffmanTree(int freq[]);
void generateCodeLengths(Node *root, int depth);
void buildCanonicalCodes(int freq[]);
void encode(char *text);
void exportJSON(FILE *fp, int contextFreq[256][256]);

#endif