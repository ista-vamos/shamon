#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 2048

char buffer[BUFSIZE];

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: interact [filename] {[}reportfile}\n");
        return 0;
    }
    FILE *inputfile = fopen(argv[1], "r");
    if (inputfile == NULL) {
        printf("Could not find input file\n");
        return 0;
    }
    char  *line          = NULL;
    size_t linesize      = 0;
    char  *inputline     = NULL;
    size_t inputlinesize = 0;
    while (getline(&line, &linesize, inputfile) >= 0) {
        if (linesize == 0 && feof(inputfile)) {
            break;
        }
        if (getdelim(&inputline, &inputlinesize, '>', stdin) < 0) {
            return -1;
        }
        printf("%s", line);
        fflush(stdout);
    }
    FILE *rfile;
    if (freopen("/dev/tty", "w", stdout) == NULL) {
        return -1;
    }
    if (argc > 2) {
        rfile = fopen(argv[2], "w");
    } else {
        rfile = stdout;
    }
    if (!rfile) {
        return -1;
    }
    while (getdelim(&inputline, &inputlinesize, ':', stdin) >= 0) {
        if (strncmp(inputline + strlen(inputline) - 17,
                    "Errors generated:", 17) == 0) {
            if (getdelim(&inputline, &inputlinesize, '\n', stdin) < 0) {
                if (rfile != stdout) {
                    fclose(rfile);
                }
                return -1;
            }
            long errorcount = atol(inputline + 1);
            fprintf(rfile, "Errors generated: %li\n", errorcount);
        } else if (strncmp(inputline + strlen(inputline) - 5, "Time:", 5) ==
                   0) {
            if (getdelim(&inputline, &inputlinesize, 's', stdin) < 0) {
                if (rfile != stdout) {
                    fclose(rfile);
                }
                return -1;
            }
            double time = atof(inputline + 1);
            fprintf(rfile, "Time: %f\n", time);
            if (rfile != stdout) {
                fclose(rfile);
            }
            return 0;
        }
    }
    if (rfile != stdout) {
        fclose(rfile);
    }
    return -1;
    // if(getdelim(&inputline, &inputlinesize, 'T', stdin)<0)
    // {
    // 	return -1;
    // }
    // if(getdelim(&inputline, &inputlinesize, 's', stdin)<0)
    // {
    // 	return -1;
    // }
    // if(strncmp(inputline, "ime: ", 5)==0)
    // {
    // 	double time = atof(inputline+5);
    // 	printf("Time: %f\n",time);
    // }
    // return 0;
}
