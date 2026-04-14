#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"
#include "adaptive.h"

int main(){
    char text[500];

    printf("Enter text: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text,"\n")] = '\0';

    adaptivePPM(text);

    FILE *fp = fopen("visualizer/visualizer.html","r");
    if(!fp){
        printf("HTML not found\n");
        return 1;
    }

    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *content = malloc(size+1);
    fread(content,1,size,fp);
    content[size]='\0';
    fclose(fp);

    char *start = strstr(content,"<script id=\"treeData\"");
    start = strstr(start,">")+1;
    char *end = strstr(start,"</script>");

    fp = fopen("visualizer/visualizer.html","w");

    fwrite(content,1,start-content,fp);
    exportJSON(fp,contextFreq);
    fwrite(end,1,strlen(end),fp);

    fclose(fp);
    free(content);

    printf("\nExport complete!\n");
}
