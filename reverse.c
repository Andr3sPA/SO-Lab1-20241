#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define raise_error(message)          \
    fprintf(stderr, "%s\n", message); \
    exit(1)

int main(int argc, char *argv[])
{
    FILE *input_file;
    FILE *output_file;
    char *input_filename;
    char *output_filename;
    char **lines;
    size_t linesSize = 0;
    size_t linesCapacity = 2;
    size_t len = 0;

    // check if too many command-line arguments
    if (argc < 1 || argc > 3)
    {
        raise_error("usage: reverse <input> <output>");
    }

    input_filename = argv[1];
    output_filename = argc > 2 ? argv[2] : NULL;

    // Check if input and output files names are the same
    if (output_filename != NULL && strcmp(input_filename, output_filename) == 0)
    {
        raise_error("reverse: input and output file must differ");
    }

    // Check if input and output files are the same
    if (output_filename != NULL)
    {
        struct stat input_stat, output_stat;
        if (stat(input_filename, &input_stat) == 0 && stat(output_filename, &output_stat) == 0)
        {
            if (input_stat.st_ino == output_stat.st_ino)
            {
                raise_error("reverse: input and output file must differ");
            }
        }
    }

    // Open input file
    if (input_filename != NULL)
    {
        input_file = fopen(input_filename, "r");
        if (input_file == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", input_filename);
            exit(1);
        }
    }
    else
    {
        input_file = stdin;
    }

    // Open output file or stdout
    if (output_filename != NULL)
    {
        output_file = fopen(output_filename, "w");
        if (output_file == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", output_filename);
            exit(1);
        }
    }
    else
    {
        output_file = stdout;
    }

    // Allocate memory for each line
    lines = (char **)malloc(linesCapacity * sizeof(char *));

    if (lines == NULL)
    {
        raise_error("reverse: malloc failed");
    }

    // read lines
    lines[0] = NULL;
    while (getline(&lines[linesSize], &len, input_file) != -1)
    {
        linesSize++;

        // resize vector if size exceeds capacity
        if (linesSize == linesCapacity)
        {
            linesCapacity *= 2;
            lines = (char **)realloc(lines, linesCapacity * sizeof(char *));
            if (lines == NULL)
            {
                raise_error("reverse: realloc failed");
            }
        }
        lines[linesSize] = NULL;
        len = 0;
    }

    // Write lines from the array in reverse order to output
    for (int i = linesSize - 1; i >= 0; i--)
    {
        fprintf(output_file, "%s", lines[i]);
    }

    // Cleanup
    for (int i = 0; i < linesSize; i++)
    {
        free(lines[i]); // free allocated memory space for each lines
    }
    free(lines); // free allocated space for vector of lines

    if (input_file != stdin)
        fclose(input_file);
    if (output_file != stdout)
        fclose(output_file);

    return 0;
}
