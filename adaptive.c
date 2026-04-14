#include <stdio.h>
#include <string.h>
#include <math.h>
#include "huffman.h"
#include "adaptive.h"

int contextFreq[ALPHA][ALPHA] = {0};
int prevContextFreq[ALPHA][ALPHA] = {0};
Step steps[MAX_STEPS];
int stepCount = 0;

static double lambda = 1.0;

double computeKL(int *oldFreq, int *newFreq){
    double kl = 0.0;
    int sumOld=0, sumNew=0;

    for(int i=0;i<ALPHA;i++){
        sumOld += oldFreq[i];
        sumNew += newFreq[i];
    }

    if(sumOld==0 || sumNew==0) return 0;

    for(int i=0;i<ALPHA;i++){
        if(oldFreq[i] && newFreq[i]){
            double p = (double)oldFreq[i]/sumOld;
            double q = (double)newFreq[i]/sumNew;
            kl += p * log(p/q);
        }
    }
    return kl;
}

// SMOOTH adaptive decay
void updateLambda(double kl){
    if(kl < 0.2) lambda = 0.5;
    else if(kl < 0.5) lambda = 0.7;
    else if(kl < 1.0) lambda = 0.85;
    else lambda = 1.0;
}

void applyDecay(int *freq){
    for(int i=0;i<ALPHA;i++){
        if(freq[i] > 0){
            freq[i] = (int)(freq[i] * (1.0 - lambda));
            if(freq[i] == 0) freq[i] = 1;
        }
    }
}

void adaptivePPM(char *text){
    int prev = 255;
    int prevFreq[ALPHA] = {0};

    printf("\n--- Adaptive PPM ---\n");

    for(int i=0;text[i];i++){
        int curr = (unsigned char)text[i];

        int tempUpdated[ALPHA];
        memcpy(tempUpdated, contextFreq[prev], sizeof(tempUpdated));
        tempUpdated[curr]++;

        double kl = computeKL(prevContextFreq[prev], tempUpdated);
        updateLambda(kl);

        if(kl > 0.5){
            printf("   -> Decay applied (lambda = %.2f)\n", lambda);
            applyDecay(contextFreq[prev]);
        }

        contextFreq[prev][curr]++;

        int tempFreq[ALPHA] = {0};
        memcpy(tempFreq, contextFreq[prev], sizeof(tempFreq));

        Node *root = buildHuffmanTree(tempFreq);

        for(int x=0;x<256;x++) codeLength[x]=0;
        generateCodeLengths(root,0);
        buildCanonicalCodes(tempFreq);

        printf("CTX %d -> %c : %s  (KL = %.3f, lambda = %.2f)\n", prev, curr, codes[curr], kl, lambda);

        if(stepCount < MAX_STEPS){
            steps[stepCount++] = (Step){prev, curr, ""};
            strcpy(steps[stepCount-1].code, codes[curr]);
        }

        memcpy(prevContextFreq[prev], contextFreq[prev], sizeof(int) * ALPHA);
        prev = curr;
    }
}

Step* getSteps(){ return steps; }
int getStepCount(){ return stepCount; }