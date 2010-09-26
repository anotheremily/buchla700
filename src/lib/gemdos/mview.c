/*
   =============================================================================
	mview.c -- display MIDI data
	See VERMSG below for version and date.

	Loosely based on the GEM program:
	midiview.c -- a MIDI data display program -- 7/22/87 by J. Johnson 

	Set up for Alcyon C.
   =============================================================================
*/

#define	VERMSG	"mview -- Version 2.08 -- 1989-12-12 -- D.N. Lynx Crowe"

#include "stdio.h"
#include "osbind.h"
#include "stddefs.h"

#define	void	int

#define	SNAPFILE	"MVIEWSNP.DAT"

#define	MBUFSZ		(unsigned short)(16*1024)

#define	n_str	"CCDDEFFGGAAB"		/* For data to note conversion */

#define	MAXNOL	2

#define	FORMAT1		"\n%5u  %02.2xH  %-16.16s"
#define	FORMAT2		"  %02.2xH %3u %-14.14s"
#define	FORMAT3		"\n%5u                       "

extern char	*malloc();

void	PrMIDI(), cleanbf(), interp();
void	type2tx(), n2note(), n2cc(), n2pb();
void	n2spp(), itoa(), r_strng();

int	SetMBuf(), midi_in(), m_stat(), n2stat(), n2chms();

struct iorec {		/* structure for MIDI buffer description */

	char *ibuf;
        short ibufsz;
        short ibufhd;
        short ibuftl;
        short ibuflo;
        short ibufhi;
};

unsigned int	indx;		/* MIDI input byte number */

int	feseen;			/* active sensing seen */
int	nbyte = 1;		/* counter for two byte MIDI data fields */
int	nol;			/* number of MIDI data bytes on the line */

FILE	*ofp;			/* output file pointer */
FILE	*sfp;			/* snap file pointer */

char	*newbuf;
char	*oldbuf;		/* old MIDI buffer pointer */
short	oldbsz;
short	oldbhi;
short	oldblo;

struct iorec	*m_buff;	/* MIDI iorec pointer */

/* 
*/

/* SetMBuf -- set up MIDI buffer */

int
SetMBuf()
{
        unsigned short size;

        size = MBUFSZ;	/* MIDI buffer */

        m_buff = (struct iorec *)Iorec(2); /* pointer to buffer descriptor */

	oldbuf = m_buff->ibuf;
	oldbsz = m_buff->ibufsz;
	oldbhi = m_buff->ibufhi;
	oldblo = m_buff->ibuflo;

	if ((char *)NULL EQ (newbuf = (char *)malloc(size))) {

		printf ("ERROR -- unable to allocate MIDI buffer.\n");
		return(FAILURE);
	}

	/* clear out the buffer */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

	/* we do this twice because we aren't disabling interrupts ... */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

        m_buff->ibuf   = newbuf;	/* change address of buffer */
        m_buff->ibufsz = size;		/* change size of buffer */

	indx = 0;			/* reset the byte index */

	return(SUCCESS);
}

/* get MIDI byte */

int
midi_in()
{
        return((int)Bconin(3) & 0X00FF);
}

/* output an interpreted MIDI data byte */

void
PrMIDI(M_Byte)
int M_Byte;
{
	char i_str[120];

	if ((0x00FF & M_Byte) EQ 0x00FE) {

		if (NOT feseen) {

			printf("\nActive sense is active\n");

			if ((FILE *)NULL NE ofp)
				fprintf(ofp, "\nActive sense is active\n");
		}

		nol = 0;
		feseen = TRUE;

	} else {

	        interp(M_Byte, i_str);

		++indx;

		if (0x0080 & M_Byte) {		/* new status byte */

			nol = 0;
			printf(FORMAT1, indx, M_Byte, i_str);

			if ((FILE *)NULL NE ofp)
				fprintf(ofp, FORMAT1, indx, M_Byte, i_str);

		} else {			/* data byte */

			if (++nol > MAXNOL) {

				printf(FORMAT3, indx);

				if ((FILE *)NULL NE ofp)
					fprintf(ofp, FORMAT3, indx);

				nol = 1;
			}

			printf(FORMAT2, M_Byte, M_Byte, i_str);

			if ((FILE *)NULL NE ofp)
				fprintf(ofp, FORMAT2, M_Byte, M_Byte, i_str);
		}
	}

	fflush(stdout);

	if ((FILE *)NULL NE ofp)
		fflush(ofp);
}       

/* check midi status */

int
m_stat()
{
        return((int)Bconstat(3) ? TRUE : FALSE);
}

/* clean out MIDI buffer */

void
cleanbf()
{
	int mstat;

	printf("Clearing MIDI input buffer ...\n");

	/* clear out the buffer by resetting the head and tail indices */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

	/* we do this twice because we aren't disabling interrupts ... */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

	/* make sure it's really drained */

	mstat = m_stat();

        while (mstat) {

		midi_in();
		mstat = m_stat();
	}

	indx = 0;
}

void
interp(M_Byte, i_str)
int M_Byte;
char *i_str;
{
	static int type;

        if (M_Byte & 0x0080) {		/* status byte */

                type = n2stat(M_Byte, i_str, type);

	} else if (nbyte == 1) {	/* 1st data byte */

		switch (type) {

			case 0:
                                type2tx(type, i_str);
                                break;
                        case 1:
                        case 2:
                                n2note(M_Byte, i_str);
                                break;
                        case 3:
                                n2cc(M_Byte, i_str);
                                break;
                        case 4:
                                type2tx(type, i_str);
                                break;
                        case 5:
                                n2pb(M_Byte, i_str);
                                break;
                        case 6:
                                n2spp(M_Byte, i_str);
                                break;
                }

        } else {			/* subsequent data bytes */

		switch (type) {

			case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                                type2tx(type, i_str);
                                break;
                        case 5:
                                n2pb(M_Byte, i_str);
                                break;
                        case 6:
                                n2spp(M_Byte, i_str);
                                break;
                }
        }

        return;
}

/* convert MIDI status byte to text description */

int
n2stat(M_Byte, i_str, O_type)
int M_Byte, O_type;
char *i_str;
{
	char *int_str;
        int N_type;

        N_type = O_type;

        if (M_Byte < 240) {

		N_type = n2chms(M_Byte, i_str, N_type);
                nbyte = 1;

        } else {

		switch (M_Byte) {

			case 240:
                                int_str = "SysEx";
                                N_type = 4;
                                nbyte = 1;
                                break;
                        case 242:
                                int_str = "Song Pos";
                                N_type = 6;
                                nbyte = 1;
                                break;
                        case 243:
                                int_str = "Song Sel";
                                N_type = 4;
                                nbyte = 1;
                                break;
                        case 246:
                                int_str = "Tune";
                                N_type = 0;
                                break;
                        case 247:
                                int_str = "End SysEx";
                                N_type = 0;
                                break;
                        case 248:
                                int_str = "Clock";
                                break;
                        case 250:
                                int_str = "Start";
                                break;
                        case 251:
                                int_str = "Continue";
                                break;
                        case 252:
                                int_str = "Stop";
                                break;
                        case 254:
                                int_str = "Act Sense";
                                break;
                        case 255:
                                int_str = "Sys Reset";
                                break;
                        default:
                                int_str = "Undefined";
                                N_type = 0;
                                break;
                }

	        strcpy(i_str, int_str);
        }

        return(N_type);
}

/* convert MIDI byte to channel message description */

int
n2chms(M_Byte, i_str, O_type)
int M_Byte, O_type;
char *i_str;
{
	int T_nybl, channel, N_type;
        char ch_strg[3], *int_str;

        N_type  = O_type;
        T_nybl  = ((M_Byte & 0x0070) >> 4);
        channel =  (M_Byte & 0x000F) + 1;
	int_str = "???";

        switch (T_nybl) {

		case 0:
                        int_str = "Note Off, Ch ";
                        N_type = 1;
                        break;
                case 1:
                        int_str = "Note On , Ch ";
                        N_type = 1;
                        break;
                case 2:
                        int_str = "Key Pres, Ch ";
                        N_type = 2;
                        break;
                case 3:
                        int_str = "Ctrl Chg, Ch ";
                        N_type = 3;
                        break;
                case 4:
                        int_str = "Prog Chg, Ch ";
                        N_type = 4;
                        break;
                case 5:
                        int_str = "Chan Prs, Ch ";
                        N_type = 2;
                        break;
                case 6:
                        int_str = "Pch Bend, Ch ";
                        N_type = 5;
                        break;
        }

        strcpy(i_str, int_str);
        itoa(channel, ch_strg);
        strcat(i_str, ch_strg);
        return(N_type);
}

/* convert MIDI data byte to text description */

void
type2tx(number, text)
int number;
char *text;
{
	char *string;

        switch (number) {

		default:
		case 0:
                        string = "????";
                        break;
                case 1:
                        string = "Vel ";
                        nbyte = 1;
                        break;
                case 2:
                case 3:
                        string = "Val ";
                        nbyte = 1;
                        break;
                case 4:
                        string = "Data";
                        break;
        }

        strcpy(text, string);
}

/* convert MIDI note number to Dr. T pitch notation */
/* data types 1 & 2 */

void
n2note(number, note)
int number;
char *note;
{
	int octave, pitch;

        nbyte = 2;		/* next byte is velocity value */
        *(note + 3) = '\0';
        octave = (number / 12) - 1;

        switch (octave) {

		case -1:
                        *(note + 2) = '-';
                        break;

                default:
                        *(note + 2) = octave + 48;
                        break;
        } 

        pitch = number % 12;
        *note = n_str[pitch];

        switch (pitch) {

		case 1:
                case 3:
                case 6:
                case 8:
                case 10:
                        *(note + 1) = '#';
                        break;
                default:
                        *(note + 1) = ' ';
                        break;
        }
}


/* convert MIDI controller to text description */
/* data type 3 */

void
n2cc(number, ctrler)
int number;
char *ctrler;
{
	char *string;

        nbyte = 2;

        switch(number) {

		case 1:
                case 33:
                        string = "Mod Wheel";
                        break;
                case 2:
                case 34:
                        string = "Brth Ctrl";
                        break;
                case 3:		/* not in the 9/1/86 MIDI controller list */
                case 35:
                        string = "DX7 Prs";
                        break;
                case 4:
                case 36:
                        string = "Pedal";
                        break;
                case 5:
                case 37:
                        string = "Port Time";
                        break;
                case 6:
                case 38:
                        string = "Data Entry";
                        break;
                case 7:
                case 39:
                        string = "Volume";
                        break;
                case 8:
                case 40:
                        string = "Balance";
                        break;
                case 10:
                case 42:
                        string = "Pan";
                        break;
                case 11:
                case 43:
                        string = "Expression";
                        break;
                case 16:
                case 48:
                        string = "GPC #1";
                        break;
                case 17:
                case 49:
                        string = "GPC #2";
                        break;
                case 18:
                case 50:
                        string = "GPC #3";
                        break;
                case 19:
                case 51:
                        string = "GPC #4";
                        break;
                case 64:
                        string = "Sustain Sw";
                        break;
                case 65:
                        string = "Port Sw";
                        break;
                case 66:
                        string = "Sustenuto Sw";
                        break;
                case 67:
                        string = "Soft Pedal";
                        break;
                case 69:
                        string = "Hold 2 Sw";
                        break;
                case 80:
                        string = "GPC #5";
                        break;
                case 81:
                        string = "GPC #6";
                        break;
                case 82:
                        string = "GPC #7";
                        break;
                case 83:
                        string = "GPC #8";
                        break;
		case 91:
			string = "FX Depth";
			break;
                case 92:
                        string = "Tremolo";
                        break;
                case 93:
                        string = "Chorus";
                        break;
                case 94:
                        string = "Celeste";
                        break;
                case 95:
                        string = "Phaser";
                        break;
                case 96:
                        string = "Data Inc";
                        break;
                case 97:
                        string = "Data Dec";
                        break;
                case 98:
                        string = "NReg Param MSB";	/* ? */
                        break;
                case 99:
                        string = "NReg Param LSB";	/* ? */
                        break;
                case 100:
                        string = "Reg Param MSB";	/* ? */
                        break;
                case 101:
                        string = "Reg Param LSB";	/* ? */
                        break;
                case 122:
                        string = "Local Control";
                        break;
                case 123:
                        string = "All Notes Off";
                        break;
                case 124:
                        string = "Omni Off";
                        break;
                case 125:
                        string = "Omni On";
                        break;
                case 126:
                        string = "Mono On";
                        break;
                case 127:
                        string = "Poly On";
                        break;
                default:
                        string = "Unasg Ctrl";
                        break;
        }

        strcpy(ctrler, string);

        if ((number > 31) && (number < 64))
                strcat(ctrler, ", LSB");
}

/* convert MIDI pitch bend to value */
/* data type 5 */

void
n2pb(number, value)
int number;
char *value;
{
        char *string;
        static int P_bend;

        if (nbyte == 1) {

		string = "    ";
                P_bend = number;
                nbyte = 2;
                strcpy(value, string);

        } else {

		P_bend += (number << 7);
                P_bend -= 8192;
                itoa(P_bend, value);
                nbyte = 1;
        }
}

/* convert MIDI song position pointer to text description */
/* data type 6 */

void
n2spp(number, pointer)
int number;
char *pointer;
{
        int measure, beat, clock;
        static int spp;
        char *string, b[3], c[3], *colon;

        colon = ":";

        if (nbyte == 1) {

		string = "    ";
                spp = number;
                nbyte = 2;
                strcpy(pointer, string);

        } else {

		spp += number * 128;
                measure = (spp / 16) + 1;
                itoa(measure, pointer);
                beat = ((spp % 16) / 4) + 1;
                itoa(beat,b);
                clock = (6 * ((spp % 16) % 4) + 1);
                itoa(clock, c);
                strcat(pointer, colon);
                strcat(pointer, b);
                strcat(pointer, colon);
                strcat(pointer, c);
                nbyte = 1;
        }
}

/* convert integer to string */

void
itoa(number, string)
char string[];
int number;
{
	int i, sign;

        if ((sign = number) < 0 )
                number = -number;

        i = 0;

        do {

		string[i++] = number % 10 + '0';

	} while ((number /= 10) > 0);

        if (sign < 0)
                string[i++] = '-';

        string[i] = '\0';
        r_strng(string);
}

/* reverse string in place */

void
r_strng(string)
char string[];
{
	int c, i, j;

        for (i = 0, j = strlen(string) - 1; i < j; i++, j--) {

		c = string[i];
                string[i] = string[j];
                string[j] = c;
        }
}       


/* 
*/

main(argc, argv)
int argc;
char *argv[];
{
	int ch, runtag;

	ofp = (FILE *)NULL;

	printf("\033E%s\n\n", VERMSG);
	printf("ESC to quit,  / to clear buffer,  space to pause output.\n\n");

	if (SetMBuf())		/* move MIDI buffer & increase its size */
		exit(2);

	printf("%u byte MIDI buffer allocated at 0x%08.8lx\n",
		MBUFSZ, (long)newbuf);

        cleanbf();		/* clear out MIDI buffer */

	if (argc EQ 2) {

		if ((FILE *)NULL EQ (ofp = fopen(argv[1], "w"))) {

			printf("ERROR -- Unable to open \"%s\" for output.\n",
				argv[1]);

			exit(2);

		} else {

			printf("Outputting to file \"%s\".\n", argv[1]);
		}
	}

        printf("Ready for MIDI data.\n");
	runtag = TRUE;

        while (runtag) {

		if (Bconstat(2)) {

			ch = 0x00FF & Bconin(2);

			switch (ch) {

			case '\033':	/* escape */

				runtag = FALSE;
				break;

			case ' ':	/* space = pause */

				printf("PAUSED");
				Bconin(2);
				printf("\b\b\b\b\b\b      \b\b\b\b\b\b");

				break;

			case '/':	/* / = clear buffer and screen */

				cleanbf();
				printf("\033E");
			        printf("Ready for MIDI data.\n");

				if ((FILE *)NULL NE ofp) {

					fprintf(ofp, "\n\nMIDI buffer flushed.\n\n");
					fflush(ofp);
				}

				break;

			case 'w':	/* w = write to SNAPFILE */

				if ((FILE *)NULL EQ (sfp = fopenb(SNAPFILE, "w"))) {

					printf("ERROR -- Unable to open \"%s\" for output.\n",
						SNAPFILE);

					exit(2);

				} else {

					printf("\n\nOutputting to file \"%s\".\n", SNAPFILE);
				}

				fwrite(newbuf, indx, 1, sfp);

				fflush(sfp);
				fclose(sfp);

				printf("\nFile written and closed.\n\n");
				break;
			}
		}

                if (m_stat())
                        PrMIDI(midi_in());

		if ((FILE *)NULL NE ofp)
			fflush(ofp);
        } 

	if ((FILE *)NULL NE ofp) {

		fprintf(ofp, "\n");
		fflush(ofp);
		fclose(ofp);
	}

	/* clear out the buffer */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

	/* we do this twice because we aren't disabling interrupts ... */

	m_buff->ibufhd = 0;		/* reset the head index */
	m_buff->ibuftl = 0;		/* reset the tail index */

	m_buff->ibufsz = oldbsz;	/* restore the old buffer size */
	m_buff->ibuf   = oldbuf;	/* restore the old buffer address */

	free(newbuf);			/* give back the big MIDI buffer */

	printf("\n");
	fflush(stdout);
	exit(0);
}
