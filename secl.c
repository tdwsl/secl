#include "secl.h"
#include <stdio.h>

#define SECL_LIT '_'

SECL_W secl_stack[256];
SECL_B secl_sp=0;
SECL_W secl_rsp=0x16;
SECL_W secl_funs[256];
SECL_B secl_memory[SECL_MEMORYSZ];
SECL_W secl_here=4096, secl_old=4096;
SECL_B secl_fun=0;
SECL_W secl_br[256];
SECL_B secl_brp=0;
SECL_W secl_pc;

void (*secl_int)(SECL_W);

char *secl_addnum(char *s) {
    secl_memory[secl_here-1] = SECL_LIT;
    *(SECL_W*)&secl_memory[secl_here] = 0;
    for(;;) {
        if(*s >= '0' && *s <= '9') {
            *(SECL_W*)&secl_memory[secl_here] <<= 4;
            *(SECL_W*)&secl_memory[secl_here] |= *s - '0';
        } else if(*s >= 'a' && *s <= 'f') {
            *(SECL_W*)&secl_memory[secl_here] <<= 4;
            *(SECL_W*)&secl_memory[secl_here] |= *s - 'a' + 10;
        } else break;
        s++;
    }
    //printf("n:%x\n", *(SECL_W*)&secl_memory[secl_here]);
    secl_here += 2;
    return s;
}

char *secl_addstr(char *s) {
    while(*s && *s != '"') secl_memory[secl_here++] = *(s++);
    if(*s == '"') s++;
    return s;
}

void secl_printstack() {
    SECL_W i;
    printf("stack: ");
    for(i = 0; i < secl_sp; i++) printf("%x ", secl_stack[i]);
    printf("\n");
}

void secl_mrun(SECL_W pc) {
    secl_pc = pc;
    for(;;) {
        //secl_printstack();
        //printf("%.4x %c\n", secl_pc, secl_memory[secl_pc]);
        switch(secl_memory[secl_pc++]) {
        case '*': return;
        case SECL_LIT:
            secl_stack[secl_sp++] = *(SECL_W*)&secl_memory[secl_pc];
            secl_pc += 2;
            break;
        case '>':
            *(SECL_W*)&secl_memory[0x100+secl_rsp] = secl_pc+2;
            secl_rsp += 2;
            secl_pc = *(SECL_W*)&secl_memory[secl_pc];
            break;
        case ')':
            secl_rsp -= 2;
            secl_pc = *(SECL_W*)&secl_memory[0x100+secl_rsp];
            break;
        case ':':
            if(!secl_stack[--secl_sp])
                secl_pc = *(SECL_W*)&secl_memory[secl_pc];
            else secl_pc += 2;
            break;
        case ']':
            secl_pc = *(SECL_W*)&secl_memory[secl_pc];
            break;
        case '?':
            secl_stack[secl_sp++] = secl_rsp;
            break;
        case '$':
            secl_rsp = secl_stack[--secl_sp];
            break;
        case '+':
            secl_stack[secl_sp-2] += secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '-':
            secl_stack[secl_sp-2] -= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '&':
            secl_stack[secl_sp-2] &= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '|':
            secl_stack[secl_sp-2] |= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '^':
            secl_stack[secl_sp-2] ^= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '<':
            secl_stack[secl_sp-2] <<= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '%':
            secl_stack[secl_sp-2] >>= secl_stack[secl_sp-1];
            secl_sp--;
            break;
        case '=':
            secl_stack[secl_sp-1] = !secl_stack[secl_sp-1];
            break;
        case '#':
            secl_stack[secl_sp] = secl_stack[secl_sp-1];
            secl_sp++;
            break;
        case '~':
            secl_sp--;
            break;
        case '@':
            secl_stack[secl_sp-1] =
                *(SECL_W*)&secl_memory[secl_stack[secl_sp-1]];
            break;
        case '!':
            *(SECL_W*)&secl_memory[secl_stack[secl_sp-1]] =
                secl_stack[secl_sp-2];
            secl_sp -= 2;
            break;
        case '.':
            secl_int(secl_stack[--secl_sp]);
            break;
        }
    }
}

void secl_run(char *s) {
    while(*s) {
        //printf("%c ", *s);
        secl_memory[secl_here++] = *s;
        switch(*(s++)) {
        case '(':
            secl_here--;
            secl_fun=*(s++);
            secl_funs[secl_fun] = secl_here;
            break;
        case ')':
            secl_fun = 0;
            secl_old = secl_here;
            break;
        case '*':
            secl_mrun(secl_old);
            secl_here = secl_old;
            break;
        case '[':
            secl_br[secl_brp++] = --secl_here;
            secl_br[secl_brp++] = 0;
            break;
        case ':':
            secl_br[secl_brp++] = secl_here;
            secl_here += 2;
            break;
        case ']':
            secl_here += 2;
            for(secl_brp--; secl_br[secl_brp]; secl_brp--)
                *(SECL_W*)&secl_memory[secl_br[secl_brp]] = secl_here;
            *(SECL_W*)&secl_memory[secl_here-2] = secl_br[--secl_brp];
            break;
        case '>':
            if(secl_funs[*s]) {
                *(SECL_W*)&secl_memory[secl_here] = secl_funs[*s];
                secl_here += 2;
            } else secl_here--;
            s++;
            break;
        case '\'':
            if(secl_funs[*s]) {
                secl_memory[secl_here-1] = SECL_LIT;
                *(SECL_W*)&secl_memory[secl_here] = secl_funs[*s];
                secl_here += 2;
            } else secl_here--;
            s++;
            break;
        case '\\':
            secl_memory[secl_here-1] = SECL_LIT;
            *(SECL_W*)&secl_memory[secl_here] = *(s++);
            secl_here += 2;
            break;
        case '/':
            secl_here--;
            break;
        case '"':
            secl_here--;
            s = secl_addstr(s);
            break;
        case ',':
            secl_here--;
            *(SECL_W*)&secl_memory[secl_here-3] =
            *(SECL_W*)&secl_memory[secl_here-2];
            secl_here--;
            break;
        case ';':
            secl_here--;
            while(*s && *s != '\n') s++;
            break;
        default:
            if((*(s-1) >= '0' && *(s-1) <= '9')
            || (*(s-1) >= 'a' && *(s-1) <= 'f'))
                s = secl_addnum(s-1);
            else if(*(s-1) <= ' ') secl_here--;
            break;
        }
    }
}

