#include <stdlib.h>
#include "heap.h"

void swap(Node **a, Node **b){
    Node *t=*a; *a=*b; *b=t;
}

void heapify(MinHeap *h,int i){
    int smallest=i;
    int l=2*i+1, r=2*i+2;

    if(l<h->size && h->arr[l]->freq<h->arr[smallest]->freq)
        smallest=l;

    if(r<h->size && h->arr[r]->freq<h->arr[smallest]->freq)
        smallest=r;

    if(smallest!=i){
        swap(&h->arr[i],&h->arr[smallest]);
        heapify(h,smallest);
    }
}

void insertHeap(MinHeap *h,Node *node){
    int i=h->size++;
    h->arr[i]=node;

    while(i && h->arr[(i-1)/2]->freq > h->arr[i]->freq){
        swap(&h->arr[i],&h->arr[(i-1)/2]);
        i=(i-1)/2;
    }
}

Node* extractMin(MinHeap *h){
    Node *root=h->arr[0];
    h->arr[0]=h->arr[--h->size];
    heapify(h,0);
    return root;
}