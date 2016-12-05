#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h> // required for isprint
#include<unistd.h> // required for optopt, opterr and optarg.

#define GBUF 64
#define WBUF 8
#define AIG 0 /* do we want to ignore any arguments, i.e. exclude them fromt he option accounting? */

typedef struct  /* optstruct_raw, a struct for the options */
{
    unsigned char aflg;
    char *name;
    char *dstr; /* a string which will be converted to a decimal in due course */
    char *fstr; /* a string which will be converted to a decimal in due course */
    char *tstr; /* a string which will be converted to a decimal in due course */
} optstruct_raw;

typedef struct  /* opts_true: a structure for the options reflecting their true types */
{
    unsigned char a;
    char *name;
    int d1; /* a string which will be converted to a decimal in due course */
    float f1; /* a string which will be converted to a decimal in due course */
    int mm, ss, hh;
} opts_true;

int catchopts(optstruct_raw *opstru, int oargc, char **oargv)
{
    int c;
    opterr = 0;

    while ((c = getopt (oargc, oargv, "ad:f:t:n:")) != -1)
        switch (c) {
            case 'a':
                opstru->aflg = 1;
                break;
            case 'd':
                opstru->dstr = optarg;
                break;
            case 'f':
                opstru->fstr = optarg;
                break;
            case 't':
                opstru->tstr = optarg;
                break;
            case 'n':
                opstru->name = optarg;
                break;
            case '?':
                /* general error state? */
                if (optopt == 'd')
                    fprintf (stderr, "Option -%c requires a decimal posing as a string argument.\n", optopt);
                if (optopt == 'n')
                    fprintf (stderr, "Option -%c requires an string argument.\n", optopt);
                if (optopt == 'f')
                    fprintf (stderr, "Option -%c requires a float posing as a string argument.\n", optopt);
                if (optopt == 't')
                    fprintf (stderr, "Option -%c requires a time posing as a string argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "`-%c' is not a valid option for this program.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }

    return 0;
}

void prtusage(void)
{
    printf("This program tests option-catching.\n"); 
}

opts_true *processopts(optstruct_raw *rawopts)
{
    opts_true *trueopts=malloc(sizeof(opts_true));
    if(rawopts->aflg) {
        printf("The a-flag is set, so this can be a boolean of some sort.\n"); 
        trueopts->a=1;
    } else {
        trueopts->a=0;
        printf("The a-flag was not set, let its default be %d.\n", trueopts->a);
    }

    if(rawopts->name) {
        /* memcpy(trueopts->name, rawopts->fn, sizeof(strlen(rawopts->fn)));
         * No, don't bother with that: just copy the pointer. In any case, you'll get a segfault */
        trueopts->name=rawopts->name;
        printf("The fname option was defined and it is set at \"%s\".\n", trueopts->name);
    } else {
        trueopts->name="whatever.txt";
        printf("The fname option was not defined, use a default: \"%s\".\n", trueopts->name);
    }

    if(rawopts->dstr) {
        trueopts->d1=atoi(rawopts->dstr);
        printf("The decimal string was converted to %d, and multiplied by 3 it becomes %d.\n", trueopts->d1, trueopts->d1*3);
    } else {
        trueopts->d1=1;
        printf("The decimal option \"-d\" was not set, so we set a default: %d\n", trueopts->d1); 
    }
    if(rawopts->fstr) {
        trueopts->f1=atof(rawopts->fstr);
        printf("The float string was converted to %4.2f, and multiplied by 3 it becomes %4.2f.\n", trueopts->f1, trueopts->f1*3);
    } else {
        trueopts->f1=0.33;
        printf("The float option \"-f\" was not set, so we set a default: %4.2f\n", trueopts->f1); 
    }

    char *tmptr, *tmptr2, tspec[32]={0};
    if(rawopts->tstr) {
        tmptr=strchr(rawopts->tstr, ':');
        sprintf(tspec, "%.*s", (int)(tmptr-rawopts->tstr), rawopts->tstr);
        trueopts->mm=atoi(tspec);
        tmptr2=strchr(tmptr, '.');
        sprintf(tspec, "%.*s", (int)(tmptr2-tmptr-1), tmptr+1);
        trueopts->ss=atoi(tspec);
        sprintf(tspec, "%s", tmptr2+1);
        trueopts->hh=atoi(tspec);
        printf("Mins: %d, secs = %d, hundreths=%d\n", trueopts->mm, trueopts->ss, trueopts->hh);
    } else {
        trueopts->mm=133;
        trueopts->ss=59;
        trueopts->hh=99;
        printf("The time option \"-t\" was not set, so we set default:\n");
        printf("Mins: %d, secs = %d, hundreths=%d\n", trueopts->mm, trueopts->ss, trueopts->hh);
    }
    return trueopts;
}
int main(int argc, char *argv[])
{
    /* argument accounting */
    if(argc == 1) {
        prtusage();
        exit(EXIT_FAILURE);
    }

    /* options and flags */
    optstruct_raw rawopts={0};
    int argignore=AIG; /* the first arg will be excluded fromt eh normal options. */
    int oargc=argc-argignore;
    char **oargv=argv+argignore;
    catchopts(&rawopts, oargc, oargv);
    opts_true *trueopts=processopts(&rawopts);

    printf("The first argument, though excluded from the option-accounting, is mandatory and here is \"%s\"\n", argv[1]); 
    printf("(Typically this will be an input file, or config file, or maybe even a subcommand.)\n"); 

    printf("\n\n We return from main proc:\n\n"); 
    printf("The a-flag is to be %d.\n", trueopts->a);
    printf("The fname option was defined and it is set at \"%s\".\n", trueopts->name);
    printf("The decimal string was converted to %d, and multiplied by 3 it becomes %d.\n", trueopts->d1, trueopts->d1*3);
    printf("The float string was converted to %4.2f, and multiplied by 3 it becomes %4.2f.\n", trueopts->f1, trueopts->f1*3);
    printf("Mins: %d, secs = %d, hundreths=%d\n", trueopts->mm, trueopts->ss, trueopts->hh);

    free(trueopts);
    return 0;
}
