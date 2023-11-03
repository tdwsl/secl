#ifndef SECL_H
#define SECL_H

/* should be 2 bytes */
#define SECL_W unsigned short
/* should be 1 byte */
#define SECL_B unsigned char

/* size of memory, only 65536 can be addressed ordinarily */
#define SECL_MEMORYSZ 65536

/* secl interrupt handler, set this */
extern void (*secl_int)(SECL_W);

extern SECL_W secl_stack[];
extern SECL_B secl_memory[];
extern SECL_B secl_sp;
extern SECL_W secl_pc, secl_rsp;

void secl_run(char *s);

#endif

