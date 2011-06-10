/*
   =============================================================================
	setjmp.h -- header for setjmp() and longjmp() functions
	Version 1 -- 1987-06-11 -- D.N. Lynx Crowe

	Registers a0 and d0 are assumed to be scratch registers,
	and are not saved by these functions.

	Register d0 is usually used as the return value register.
	Exception:  Motorola C uses a0 for the return value if it's a pointer.

	This is true of most C compilers for the 68000, including:
	Digital Research / Alcyon, Lattice, Aztec / Manx, Greenhills, Motorola.
   =============================================================================
*/

struct JMP_BUF
{				/* setjmp() / longjmp() environment structure */

  long jmpret;			/* return address */

  long jmp_d1;			/* data registers */
  long jmp_d2;
  long jmp_d3;
  long jmp_d4;
  long jmp_d5;
  long jmp_d6;
  long jmp_d7;

  long jmp_a1;			/* address registers */
  long jmp_a2;
  long jmp_a3;
  long jmp_a4;
  long jmp_a5;
  long jmp_a6;
  long jmp_a7;
};

typedef struct JMP_BUF jmp_buf;
