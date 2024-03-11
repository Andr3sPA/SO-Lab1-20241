#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// compares inFile and outFile paths and their hardlinks, exiting and printing an error if there are inconsistencies
void compareFiles(const char *inFileName, const char *outFileName)
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

// returns a pointer within the string where the new is located line suffixed by '\n', and its size with 'len'
char *nextLine(const char *str, size_t *len)
{
    // stores locally the idx to get the next line;
    static size_t startIdx = 0;
    static const char *prevStr = NULL;

    // resets the idx if the string is a different one
    if (prevStr != str)
    {
        startIdx = 0;
        prevStr = str;
    }

    // if there is no more lines return NULL
    if (startIdx >= strlen(str))
    {
        return NULL;
    }

    char *line = (char *)&str[startIdx];
    *len = 0;

    while (startIdx + *len < strlen(str) && str[startIdx + *len] != '\n')
    {
        (*len)++;
    }

    (*len)++;
    startIdx += *len;
    return line;
}

// returns a pointer an allocated reversed string, so we need to use free() before exit;
char *reverse(const char *str, size_t size)
{
    char *result = malloc(sizeof(char) * size);

    char *line;            // pointer within the str to track lines
    size_t offset = size;  // offset that starts at the end of the result to reverse the original string
    bool firstLine = true; // the incoming firstLine will be the outcoming lastLine, so this is used to remove the new line character ('\n')

    size_t lineLength;
    line = nextLine(str, &lineLength);

    while (line)
    {
        lineLength -= firstLine; // if first incoming line, exclude last character
        offset -= lineLength;
        strncpy(result + offset, line, lineLength);

        if (firstLine)
        {
            firstLine = false;
        }

        line = nextLine(str, &lineLength);
    }

    result[offset + (lineLength - 1)] = '\n'; // incoming lastLine didn't have newline character, so we add '\n' at the end of the first outcoming line
    if (result[0] == '\0')
    {
        result[0] = '\n';
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

    fseek(stdin, 0, SEEK_END);
    if (argc == 1 && ftell(stdin) <= 0)
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }
    rewind(stdin);

    const char *inName = argc >= 2 ? argv[1] : NULL;

    const char *outName = argc > 2 ? argv[2] : NULL;

    if (argc == 3)
    {
        compareFiles(inName, outName);
    }

    size_t size = 0;
    char *inContent = readFile(inName, &size);
    char *outContent = reverse(inContent, size);
    writeFile(outName, outContent, size);

    free(inContent);
    free(outContent);
    return 0;
}
