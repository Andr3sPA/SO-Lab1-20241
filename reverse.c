#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 1024

void print_error(const char *message)
{
    fprintf(stderr, "%s\n", message);
}

int main(int argc, char *argv[])
{
    FILE *input_file;
    FILE *output_file;
    char *input_filename;
    char *output_filename;
    char *lines[MAX_LINE_LENGTH];
    size_t len = 0;
    ssize_t read;

    // check if too many command-line arguments
    if (argc < 1 || argc > 3)
    {
        print_error("usage: reverse <input> <output>");
        exit(1);
    }

    input_filename = argv[1];
    output_filename = argc > 2 ? argv[2] : NULL;

    // Check if inputname and outputname files
    if (output_filename != NULL && strcmp(input_filename, output_filename) == 0)
    {
        print_error("reverse: input and output file must differ");
        exit(1);
    }

    // Check if input and output files are the same
    if (output_filename != NULL)
    {
        struct stat input_stat, output_stat;
        if (stat(input_filename, &input_stat) == 0 && stat(output_filename, &output_stat) == 0)
        {
            if (input_stat.st_ino == output_stat.st_ino)
            {
                print_error("reverse: input and output file must differ");
                exit(1);
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
            fclose(input_file);
            exit(1);
        }
    }
    else
    {
        output_file = stdout;
    }

    // Allocate memory for each line
    for (int i = 0; i < MAX_LINE_LENGTH; i++)
    {
        lines[i] = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
        if (lines[i] == NULL)
        {
            print_error("reverse: malloc failed");
            exit(1);
        }
    }

    // Read lines from input and store them in an array
    int num_lines = 0;
    while ((read = getline(&lines[num_lines], &len, input_file)) != -1)
    {

        num_lines++;
    }

    // Write lines from the array in reverse order to output
    for (int i = num_lines - 1; i >= 0; i--)
    {
        fprintf(output_file, "%s", lines[i]);
    }

    // Cleanup
    for (int i = 0; i < MAX_LINE_LENGTH; i++)
    {
        free(lines[i]); // Liberar memoria asignada para cada línea
    }
    fclose(input_file);
    if (output_file != stdout)
        fclose(output_file);

    return 0;
}
