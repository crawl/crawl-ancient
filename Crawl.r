/*
 *  File:       Crawl.r
 *  Summary:   Mac resources
 *  Written by: Jesse Jones
 *
 *  Change History (most recent first):
 *
 * <1> 3/26/99 JDJ Created
 */

#include <BalloonTypes.r>
#include <Types.r>


// ============================================================================
// Finder Icon Help Message
// ============================================================================
resource 'hfdr' (-5696, "Finder Help") {
HelpMgrVersion,
hmDefaultOptions,
0, 0,

{
HMStringItem {
"Crawl is a fun game in the grand tradition of
games like Rogue, Hack and Moria."
},
}
};


// ============================================================================
// Get Info String
// ============================================================================
type 'Crwl' {pstring;};

data 'Crwl' (0, "Owner resource") {
"Crawl 3.3 ©1997 -1999 by Linley Henzell (Mac Port by Jesse Jones)"
};


// ============================================================================
// Menus
// ============================================================================

// Apple Menu
resource 'MENU' (256, "Apple") {
256, textMenuProc, allEnabled, enabled, apple,
{
"About Crawl", noIcon, noKey, noMark, plain,
"-", noIcon, noKey, noMark, plain,
}
};

// File Menu
resource 'MENU' (257, "File") {
257, textMenuProc, allEnabled, enabled, "File",
{
"Save", noIcon, "S", noMark, plain,
"-", noIcon, noKey, noMark, plain,
"Quit", noIcon, "Q", noMark, plain,
}
};

// Font Menu
resource 'MENU' (258, "Font") {
258, textMenuProc, allEnabled, enabled, "Font",
{
}
};

// Size Menu
resource 'MENU' (259, "Size") {
259, textMenuProc, allEnabled, enabled, "Size",
{
"9", noIcon, noKey, noMark, plain,
"10", noIcon, noKey, noMark, plain,
"12", noIcon, noKey, noMark, plain,
"16", noIcon, noKey, noMark, plain,
"18", noIcon, noKey, noMark, plain,
"20", noIcon, noKey, noMark, plain,
"32", noIcon, noKey, noMark, plain,
"48", noIcon, noKey, noMark, plain,
"64", noIcon, noKey, noMark, plain,
}
};



// ============================================================================
// Derezed Resources
// ============================================================================

resource 'BNDL' (128) {
'Crwl',
0,
{ /* array TypeArray: 2 elements */
/* [1] */
'FREF',
{ /* array IDArray: 2 elements */
/* [1] */
0, 128,
/* [2] */
1, 129
},
/* [2] */
'ICN#',
{ /* array IDArray: 2 elements */
/* [1] */
0, 0,
/* [2] */
1, 0
}
}
};

resource 'FREF' (128) {
'CrlF',
0,
""
};

resource 'FREF' (129) {
'APPL',
1,
""
};

resource 'DITL' (129) {
{ /* array DITLarray: 3 elements */
/* [1] */
{45, 353, 65, 411},
Button {
enabled,
"OK"
},
/* [2] */
{19, 68, 90, 339},
StaticText {
disabled,
"^0"
},
/* [3] */
{38, 21, 70, 53},
Icon {
disabled,
128
}
}
};

resource 'DITL' (130) {
{ /* array DITLarray: 3 elements */
/* [1] */
{81, 136, 101, 194},
Button {
enabled,
"No"
},
/* [2] */
{81, 37, 101, 95},
Button {
enabled,
"Yes"
},
/* [3] */
{24, 43, 56, 188},
StaticText {
disabled,
"Do you really want to quit without savin"
"g?"
}
}
};

resource 'DITL' (256, "About", purgeable) {
{ /* array DITLarray: 3 elements */
/* [1] */
{70, 220, 90, 280},
Button {
enabled,
"OK"
},
/* [2] */
{10, 70, 64, 279},
StaticText {
disabled,
"Crawl 3.3 \n© 1997-1999 by Linley Henzell"
"\nMac Port by Jesse Jones"
},
/* [3] */
{10, 20, 42, 52},
Icon {
disabled,
1
}
}
};

resource 'DITL' (131, purgeable) {
{ /* array DITLarray: 4 elements */
/* [1] */
{71, 288, 91, 348},
Button {
enabled,
"Save"
},
/* [2] */
{71, 215, 91, 275},
Button {
enabled,
"Cancel"
},
/* [3] */
{71, 75, 91, 159},
Button {
enabled,
"Don't Save"
},
/* [4] */
{10, 75, 58, 348},
StaticText {
disabled,
"Do you want to save your game before qui"
"tting?"
}
}
};

data 'ALRT' (129) {
$"0028 0028 0096 01D7 0081 5555"                      /*
.(.(.Ò.◊.≈UU */
};

data 'ALRT' (130) {
$"0090 009A 011B 0180 0082 5555"                      /*
.Í.ˆ...ƒ.«UU */
};

resource 'ALRT' (256, "About", purgeable) {
{88, 85, 184, 378},
256,
{ /* array: 4 elements */
/* [1] */
OK, visible, silent,
/* [2] */
OK, visible, silent,
/* [3] */
OK, visible, silent,
/* [4] */
OK, visible, silent
},
alertPositionMainScreen
};

resource 'ALRT' (131, "Save Changes", purgeable) {
{104, 130, 205, 488},
131,
{ /* array: 4 elements */
/* [1] */
OK, visible, silent,
/* [2] */
OK, visible, silent,
/* [3] */
OK, visible, silent,
/* [4] */
OK, visible, silent
},
alertPositionParentWindowScreen
};

resource 'clut' (256) {
{
// [0] // DOS
colors crayon colors
0xFFFF, 0xFFFF, 0xFFFF, // white

// [1]
0x0000, 0x3333, 0xCCCC, // blue

// [2]
0x0000, 0x6666, 0x3333, // green
pine

// [3]
0x6666, 0xCCCC, 0xCCCC, // cyan
fog

// [4]
0xFFFF, 0x0000, 0x3333, // red

// [5]
0xCCCC, 0x6666, 0xCCCC, // magenta
orchid

// [6]
0x9999, 0x6666, 0x3333, // brown
dirt

// [7]
0x9999, 0x9999, 0x9999, // light grey granite

// [8]
0x3333, 0x3333, 0x3333, // drag grey gabbro

// [9]
0x6666, 0x9999, 0xFFFF, // light blue sky
blue

// [10]
0x3333, 0x9999, 0x3333, // light green clover

// [11]
0x6666, 0xCCCC, 0x9999, // light cyan
ocean green

// [12]
0xFFFF, 0x6666, 0x0000, // light red fire

// [13]
0xFFFF, 0x9999, 0x0000, // light magenta orange

// [14]
0xFFFF, 0xFFFF, 0x0000, // yellow
lemon

// [15]
0x0000, 0x0000, 0x0000 // black
}
};
