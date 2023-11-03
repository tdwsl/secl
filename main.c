#include "secl.h"
#include <stdio.h>

#define BUFSZ 2400

void runFile(const char *s) {
    FILE *fp;
    char buf[BUFSZ];
    fp = fopen(s, "r");
    if(!fp) { printf("failed to open %s\n", s); return; }
    while(!feof(fp)) {
        fread(buf, 1, BUFSZ, fp);
        secl_run(buf);
    }
}

void intfun(SECL_W i) {
    switch(i) {
    case 0:
        fputc(secl_stack[--secl_sp], stdout);
        break;
    }
}

int main(int argc, char **args) {
    int i;
    secl_int = intfun;
    for(i = 1; i < argc; i++) runFile(args[i]);
    return 0;
}
