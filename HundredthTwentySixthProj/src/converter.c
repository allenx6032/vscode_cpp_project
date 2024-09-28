#include "FileLoader.h"
#include <getopt.h>

int main(int argc,char** argv)
{
    if(argc < 3)
    {
        printf("Usage:\nconverter -i file.obj -o file.mdl\n");

        return 1;
    }
    int opt;
    char* input;
    char* output;
    while ((opt = getopt(argc,argv,"i:o:")) != -1)
    {
        switch (opt)
        {
        case 'i':
        //input = malloc(strlen( optarg));
        input = malloc(strlen(optarg));
        strcpy(input,optarg);
        break;
        case 'o':
        output = malloc(strlen(optarg));
        strcpy(output,optarg);
        break;
        }
    }
    
    printf("Processed args\n");

    Mesh m;
    if(!LoadOBJModel(input,&m))
    {
        printf("ERROR");
        return 1;
    }
    printf("Loaded obj\n");
    SaveMDL(m,output);

    return 0;
}