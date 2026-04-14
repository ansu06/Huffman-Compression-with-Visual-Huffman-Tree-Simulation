#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "heap.h"
#include "adaptive.h"

char codes[256][50];
int codeLength[256] = {0};

Node* newNode(char ch, int freq){
    Node* n = (Node*)malloc(sizeof(Node));
    n->ch = ch;
    n->freq = freq;
    n->left = n->right = NULL;
    return n;
}

Node* buildHuffmanTree(int freq[]){
    MinHeap heap = {0};

    for(int i=0;i<256;i++){
        if(freq[i] > 0)
            insertHeap(&heap, newNode(i, freq[i]));
    }

    if(heap.size == 0) return NULL;

    if(heap.size == 1){
        Node *only = extractMin(&heap);
        Node *root = newNode('#', only->freq);
        root->left = only;
        return root;
    }

    while(heap.size > 1){
        Node *l = extractMin(&heap);
        Node *r = extractMin(&heap);

        Node *p = newNode('#', l->freq + r->freq);
        p->left = l;
        p->right = r;

        insertHeap(&heap, p);
    }

    return extractMin(&heap);
}

void generateCodeLengths(Node *root, int depth){
    if(!root) return;

    if(!root->left && !root->right){
        codeLength[(unsigned char)root->ch] = depth ? depth : 1;
        return;
    }

    generateCodeLengths(root->left, depth+1);
    generateCodeLengths(root->right, depth+1);
}

void buildCanonicalCodes(int freq[]){
    int symbols[256], count = 0;

    for(int i=0;i<256;i++)
        if(freq[i] > 0)
            symbols[count++] = i;

    for(int i=0;i<count;i++){
        for(int j=i+1;j<count;j++){
            if(codeLength[symbols[i]] > codeLength[symbols[j]] ||
              (codeLength[symbols[i]] == codeLength[symbols[j]] &&
               symbols[i] > symbols[j])){
                int t = symbols[i];
                symbols[i] = symbols[j];
                symbols[j] = t;
            }
        }
    }

    int code = 0, prevLen = 0;

    for(int i=0;i<count;i++){
        int len = codeLength[symbols[i]];
        code <<= (len - prevLen);

        for(int j=0;j<len;j++)
            codes[symbols[i]][len-j-1] = ((code >> j) & 1) + '0';

        codes[symbols[i]][len] = '\0';

        code++;
        prevLen = len;
    }
}

void exportJSON(FILE *fp, int contextFreq[256][256]){
    Step *steps = getSteps();
    int totalSteps = getStepCount();

    fprintf(fp, "{");

    // contexts
    fprintf(fp, "\"contexts\":{");

    int firstCtx = 1;

    for(int c=0;c<256;c++){
        int hasData = 0;
        for(int j=0;j<256;j++){
            if(contextFreq[c][j] > 0){
                hasData = 1;
                break;
            }
        }
        if(!hasData) continue;

        if(!firstCtx) fprintf(fp, ",");
        fprintf(fp, "\"%d\":{", c);

        int first = 1;

        int tempFreq[256] = {0};
        for(int k=0;k<256;k++)
            tempFreq[k] = contextFreq[c][k];

        Node *root = buildHuffmanTree(tempFreq);

        for(int x=0;x<256;x++) codeLength[x] = 0;
        generateCodeLengths(root,0);
        buildCanonicalCodes(tempFreq);

        for(int j=0;j<256;j++){
            if(contextFreq[c][j] > 0){
                if(!first) fprintf(fp, ",");

                fprintf(fp,
                    "\"%d\":{\"code\":\"%s\",\"freq\":%d}",
                    j, codes[j], contextFreq[c][j]
                );

                first = 0;
            }
        }

        fprintf(fp, "}");
        firstCtx = 0;
    }

    fprintf(fp, "},\"steps\":[");

    for(int i=0;i<totalSteps;i++){
        if(i>0) fprintf(fp, ",");

        fprintf(fp,
            "{\"ctx\":%d,\"char\":%d,\"code\":\"%s\"}",
            steps[i].ctx,
            steps[i].ch,
            steps[i].code
        );
    }

    fprintf(fp, "]}");
}