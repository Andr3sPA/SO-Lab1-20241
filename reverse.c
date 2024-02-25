#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void checkFiles(const char *inFileName, const char *outFileName)
{
    if (strcmp(inFileName, outFileName) == 0)
    {
        fprintf(stderr, "reverse: input and output file must differ\n");
        exit(1);
    }

    struct stat inFileStat = {0};
    struct stat outFileStat = {0};

    if (stat(inFileName, &inFileStat) == -1)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", inFileName);
            exit(1);
        }
    }

    if (stat(outFileName, &outFileStat) == 0)
    {
        if (inFileStat.st_dev == outFileStat.st_dev && inFileStat.st_ino == outFileStat.st_ino)
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }
}

// allocates memory for the returning string, so we need to use free() outside
char *readFile(const char *fileName, size_t *size)
{
    FILE *inFile;
    if (fileName != NULL)
    {
        inFile = fopen(fileName, "r+");
    }
    else
    {
        inFile = stdin;
    }

    if (!inFile)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", fileName);
            exit(1);
        }
    }

    fseek(inFile, 0, SEEK_END);
    *size = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);
    char *content = (char *)malloc(sizeof(char) * (*size));
    fread(content, sizeof(char), *size, inFile);

    if (fileName != NULL)
    {
        fclose(inFile);
    }
    return content;
}

void writeFile(const char *fileName, const char *text, size_t size)
{
    FILE *outFile;
    if (fileName != NULL)
    {
        outFile = fopen(fileName, "w+");
    }
    else
    {
        outFile = stdout;
    }

    fwrite(text, sizeof(char), size, outFile);

    fclose(outFile);
}

// returns a pointer to the reversed string, so we need to use free() before exit;
char *reverse(const char *str, size_t size)
{

    char *result = malloc(sizeof(char) * size);
    memset(result, '\n', size);

    char *line;
    size_t offset = size;
    bool lastLine = true;

    // each item points to a line in the original string, so no allocation is done
    // so when the original string is freed, these items will be invalid pointers!!
    // note: strtok replaces the delimiter with \0 (null character)
    line = strtok((char *)str, "\n");

    while (line)
    {
        size_t lineLength = strlen(line);
        offset -= lineLength + !lastLine; // if not last line add space for new line character (\n)
        strncpy(result + offset, line, lineLength);

        if (!lastLine)
        {
            result[offset + lineLength] = '\n';
        }
        else
        {
            lastLine = false;
        }

        line = strtok(NULL, "\n"); // NULL to continue with the same string
    }

    return result;
}

int main(int argc, char const *argv[])
{
    if (argc > 3)
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }

    const char *inName = argc >= 2 ? argv[1] : NULL;

    const char *outName = argc > 2 ? argv[2] : NULL;

    if (argc == 3)
    {
        checkFiles(inName, outName);
    }

    size_t size = 0;
    char *inContent = readFile(inName, &size);
    char *outContent = reverse(inContent, size);
    writeFile(outName, outContent, size);

    free(inContent);
    free(outContent);
    return 0;
}
