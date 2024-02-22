#include <stdio.h>
#include <stdlib.h>


void reverseStr(char *str)

{



}


int main (int argc, char *argv[]){
    FILE* f =fopen(argv[1],"r+");
    FILE* fp2 =fopen(argv[2],"w+");
    
        
    char * buffer = 0;
    long length;
    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }

    if (buffer)
    {
    // reverseStr(buffer);
    fputs(buffer, fp2); 
    printf("%s\n", buffer);
    }
free(buffer);
fclose(fp2);
return 0;
}