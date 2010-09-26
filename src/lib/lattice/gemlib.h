/**
*
*  This file contains useful macros and structures for use with GEMLIB.
*
**/

/* EVENT Manager definitions */

/* multi flags */

#define MU_KEYBD  0x0001   
#define MU_BUTTON 0x0002
#define MU_M1     0x0004
#define MU_M2     0x0008
#define MU_MESAG  0x0010
#define MU_TIMER  0x0020

/* keyboard states */

#define K_RSHIFT 0x0001
#define K_LSHIFT 0x0002
#define K_CTRL   0x0004
#define K_ALT    0x0008

/* message values */

#define MN_SELECTED 10
#define WM_REDRAW   20
#define WM_TOPPED   21
#define WM_CLOSED   22
#define WM_FULLED   23
#define WM_ARROWED  24
#define WM_HSLID    25
#define WM_VSLID    26
#define WM_SIZED    27
#define WM_MOVED    28
#define WM_NEWTOP   29
#define AC_OPEN     40
#define AC_CLOSE    41

/* FORM Manager Definitions */

/* Form flags */

#define FMD_START  0
#define FMD_GROW   1
#define FMD_SHRINK 2
#define FMD_FINISH 3

/* RESOURCE Manager Definitions */

/* data structure types */

#define R_TREE      0
#define R_OBJECT    1
#define R_TEDINFO   2
#define R_ICONBLK   3
#define R_BITBLK    4
#define R_STRING    5      /* gets pointer to free strings */
#define R_IMAGEDATA 6      /* gets pointer to free images */
#define R_OBSPEC    7
#define R_TEPTEXT   8      /* sub ptrs in TEDINFO */
#define R_TEPTMPLT  9
#define R_TEPVALID 10
#define R_IBPMASK  11      /* sub ptrs in ICONBLK */
#define R_IBPDATA  12
#define R_IBPTEXT  13
#define R_BIPDATA  14      /* sub ptrs in BITBLK  */
#define R_FRSTR    15      /* gets addr of ptr to free strings */
#define R_FRIMG    16      /* gets addr of ptr to free images */

/* WINDOW Manager Definitions */

/* Window Attributes */

#define NAME    0x0001
#define CLOSER  0x0002
#define FULLER  0x0004
#define MOVER   0x0008
#define INFO    0x0010
#define SIZER   0x0020
#define UPARROW 0x0040
#define DNARROW 0x0080
#define VSLIDE  0x0100
#define LFARROW 0x0200
#define RTARROW 0x0400
#define HSLIDE  0x0800

/* wind_create flags */

#define WC_BORDER 0
#define WC_WORK   1

/* wind_get flags */

#define WF_KIND       1
#define WF_NAME       2
#define WF_INFO       3
#define WF_WORKXYWH   4
#define WF_CURRXYWH   5
#define WF_PREVXYWH   6
#define WF_FULLXYWH   7
#define WF_HSLIDE     8
#define WF_VSLIDE     9
#define WF_TOP       10
#define WF_FIRSTXYWH 11
#define WF_NEXTXYWH  12
#define WF_RESVD     13
#define WF_NEWDESK   14
#define WF_HSLSIZE   15
#define WF_VSLSIZE   16
#define WF_SCREEN    17

/* update flags */

#define   END_UPDATE 0
#define   BEG_UPDATE 1
#define   END_MCTRL  2
#define   BEG_MCTRL  3

/* GRAPHICS Manager Definitions */

/* Mouse Forms */

#define   ARROW       0
#define   TEXT_CRSR   1
#define   BUSYBEE     2
#define   POINT_HAND  3
#define   FLAT_HAND   4
#define   THIN_CROSS  5
#define   THICK_CROSS 6
#define   OUTLN_CROSS 7
#define   USER_DEF  255
#define   M_OFF     256
#define   M_ON      257

/* polyline end styles */

#define SQUARED 0
#define ARROWED 1
#define ROUNDED 2

/* polyline line styles */

#define SOLID      1
#define LDASHED    2
#define DOTTED     3
#define DASHDOT    4
#define DASHED     5
#define DASHDOTDOT 6

/* interior types for filled areas */

#define HOLLOW      0
/* #define SOLID 1  same as above  */
#define PATTERN     2
#define HATCH       3
#define UDFILLSTYLE 4

/* a selection of fill patterns */

#define DOTS   3
#define GRID   6
#define BRICKS 9
#define WEAVE 16

/* text special effects */

#define THICKENED  0x0001
#define SHADED     0x0002
#define SKEWED     0x0004
#define UNDERLINED 0x0008
#define OUTLINE    0x0010
#define SHADOW     0x0020

/* gem writing modes */

#define MD_REPLACE 1
#define MD_TRANS   2
#define MD_XOR     3
#define MD_ERASE   4

/* bit blt rules   */

#define ALL_WHITE  0
#define S_AND_D    1
#define S_AND_NOTD 2
#define S_ONLY     3
#define NOTS_AND_D 4
#define D_ONLY     5
#define S_XOR_D    6
#define S_OR_D     7
#define NOT_SORD   8
#define NOT_SXORD  9
#define D_INVERT  10
#define NOT_D     11
#define S_OR_NOTD 12
#define NOTS_OR_D 13
#define NOT_SANDD 14
#define ALL_BLACK 15

/* Graphic types of objects */

#define G_BOX      20
#define G_TEXT     21
#define G_BOXTEXT  22
#define G_IMAGE    23
#define G_USERDEF  24
#define G_IBOX     25
#define G_BUTTON   26
#define G_BOXCHAR  27
#define G_STRING   28
#define G_FTEXT    29
#define G_FBOXTEXT 30
#define G_ICON     31
#define G_TITLE    32

/* Object flags */

#define NONE       0x0000
#define SELECTABLE 0x0001
#define DEFAULT    0x0002
#define EXIT       0x0004
#define EDITABLE   0x0008
#define RBUTTON    0x0010
#define LASTOB     0x0020
#define TOUCHEXIT  0x0040
#define HIDETREE   0x0080
#define INDIRECT   0x0100

/* Object states   */

#define NORMAL   0x0000
#define SELECTED 0x0001
#define CROSSED  0x0002
#define CHECKED  0x0004
#define DISABLED 0x0008
#define OUTLINED 0x0010
#define SHADOWED 0x0020

/* Object colour numbers */

#define WHITE     0
#define BLACK     1
#define RED       2
#define GREEN     3
#define BLUE      4
#define CYAN      5
#define YELLOW    6
#define MAGENTA   7
#define LWHITE    8
#define LBLACK    9
#define LRED     10
#define LGREEN   11
#define LBLUE    12
#define LCYAN    13
#define LYELLOW  14
#define LMAGENTA 15

/* editable text field definitions */

#define EDSTART 0
#define EDINIT  1
#define EDCHAR  2
#define EDEND   3

/* editable text justification */

#define TE_LEFT  0
#define TE_RIGHT 1
#define TE_CNTR  2

/* Structure Definitions */

typedef struct object
{
  short          ob_next;    /* -> object's next sibling */
  short          ob_head;    /* -> head of object's children */
  short          ob_tail;    /* -> tail of object's children */
  unsigned short ob_type;    /* type of object- BOX, CHAR,... */
  unsigned short ob_flags;   /* flags  */
  unsigned short ob_state;   /* state- SELECTED, OPEN, ...  */
  long           ob_spec;    /* "out"- -> anything else  */
  short          ob_x;       /* upper left corner of object */
  short          ob_y;       /* upper left corner of object */
  short          ob_width;   /* width of object */
  short          ob_height;  /* height of object */
} OBJECT;

typedef struct orect
{
  struct orect *o_link;
  short         o_x;
  short         o_y;
  short         o_w;
  short         o_h;
} ORECT;

typedef struct grect
{
  short   g_x;
  short   g_y;
  short   g_w;
  short   g_h;
} GRECT;

typedef struct text_edinfo
{
  long  te_ptext;     /* ptr to text (must be 1st) */
  long  te_ptmplt;    /* ptr to template */
  long  te_pvalid;    /* ptr to validation chrs. */
  short te_font;      /* font */
  short te_junk1;     /* junk short */
  short te_just;      /* justification- left, right... */
  short te_color;     /* color information short */
  short te_junk2;     /* junk short */
  short te_thickness; /* border thickness */
  short te_txtlen;    /* length of text string */
  short te_tmplen;    /* length of template string */
} TEDINFO;

typedef struct icon_block
{
  long  ib_pmask;
  long  ib_pdata;
  long  ib_ptext;
  short ib_char;
  short ib_xchar;
  short ib_ychar;
  short ib_xicon;
  short ib_yicon;
  short ib_wicon;
  short ib_hicon;
  short ib_xtext;
  short ib_ytext;
  short ib_wtext;
  short ib_htext;
} ICONBLK;

typedef struct bit_block
{
  long  bi_pdata;   /* ptr to bit forms data */
  short bi_wb;      /* width of form in bytes */
  short bi_hl;      /* height in lines */
  short bi_x;       /* source x in bit form */
  short bi_y;       /* source y in bit form */
  short bi_colour;  /* fg colour of blt */
} BITBLK;

typedef struct user_blk
{
  long ub_code;
  long ub_parm;
} USERBLK;

typedef struct parm_blk
{
  long  pb_tree;
  short pb_obj;
  short pb_prevstate;
  short pb_currstate;
  short pb_x;
  short pb_y;
  short pb_w;
  short pb_h;
  short pb_xc;
  short pb_yc;
  short pb_wc;
  short pb_hc;
  long  pb_parm;
} PARMBLK;

typedef struct fdbstr
{
  long  fd_addr;
  short fd_w;
  short fd_h;
  short fd_wdwidth;
  short fd_stand;
  short fd_nplanes;
  short fd_r1;
  short fd_r2;
  short fd_r3;
} FDB;

typedef struct mfstr
{
  short mf_xhot;
  short mf_yhot;
  short mf_nplanes;
  short mf_fg;
  short mf_bg;
  short mf_mask[16];
  short mf_data[16];
} MFORM;

