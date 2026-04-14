#ifndef HEAP_H
#define HEAP_H

#include "huffman.h"

typedef struct {
    int size;
    Node *arr[256];
} MinHeap;

void insertHeap(MinHeap *h, Node *node);
Node* extractMin(MinHeap *h);
void heapify(MinHeap *h, int i);

#endif