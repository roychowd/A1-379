#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char **splitString(char *line)
{
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= bufsize)
        {
            bufsize += bufsize;
            tokens = realloc(tokens, bufsize * sizeof(char *));
        }

        token = strtok(NULL, " ");
    }
    tokens[position] = NULL;
    return tokens;
}

int main(int argc, char const *argv[])
{
    /* code */
    char *const args[] = {
        "xclock", "xclock", "-geometry", "200x200", "-update", "1"};
    // execlp("xclock", "xclock", "-geometry", "200x200", "-update", "1", (char *)NULL);
    execvp(args[0], args);
    return 0;
}

// int getWordCount(char *word)
// {
//     char *w = strdup(word);
//     char *pch = NULL;
//     int count = 0;
//     pch = strtok(w, " ");
//     while (pch != NULL)
//     {
//         count++;
//         printf("The characters: %s\n", pch);
//         pch = strtok(NULL, " ");
//     }
//     return count;
// }

// void returnStringArray(char *line)
// {
//     char *p = strdup(line);
//     int x = getWordCount(p);
//     printf("%d", x);
//     char *values[x];
//     int i = 0;
//     p = strtok(p, " ");
//     while (p != NULL)
//     {
//         values[i][i] = *p;
//         i++;
//         printf("%s", p);
//     }
// }

// void grabUserCMD(unsigned int *length, char *userCMD)
// {
//     int c, i = 0;
//     while (((c = getchar()) != '\n' && c != EOF))
//     {
//         userCMD[i++] = (char)c;

//     }
//     userCMD[i] = '\0';
//     return;
// }

//(int argc, char const *argv[])