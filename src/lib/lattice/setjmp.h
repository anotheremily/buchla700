/**
*
* This structure is used by the setjmp/longjmp functions to save the
* current environment on the 68000.
*
*/
struct JMP_BUF
        {
        long jmpret;            /* return address */
        long jmp_d1;
        long jmp_d2;
        long jmp_d3;
        long jmp_d4;
        long jmp_d5;
        long jmp_d6;
        long jmp_d7;
        long jmp_a1;
        long jmp_a2;
        long jmp_a3;
        long jmp_a4;
        long jmp_a5;
        long jmp_a6;
        long jmp_a7;
        };

typedef struct JMP_BUF jmp_buf;
