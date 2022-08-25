#include "conv_code.h"

namespace 
{
    struct cp1252_to_utf8
    {
        int8_t size;
        char buf[3];
    };
    const cp1252_to_utf8 cp1252_to_utf8_map[] =
    {
        // (*0x80 * ) 0x20ac, "\xE2\x82\xAC", "EURO SIGN";
        {
            3, { (char)0xE2, (char)0x82, (char)0xAC }
        },
        //(*0x81 * ) None, None, "UNDEFINED";
        {
            0, { 0 }
        },
        //(*0x82 * ) 0x201a, "\xE2\x80\x9A", "SINGLE LOW-9 QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x9A }
        },
        //(*0x83 * ) 0x0192, "\xC6\x92", "LATIN SMALL LETTER F WITH HOOK";
        {
            2, { (char)0xC6, (char)0x92}
        },
        //(*0x84 * ) 0x201e, "\xE2\x80\x9E", "DOUBLE LOW-9 QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x9E }
        },
        //(*0x85 * ) 0x2026, "\xE2\x80\xA6", "HORIZONTAL ELLIPSIS";
        {
            3, { (char)0xE2, (char)0x80, (char)0xA6 }
        },
        //(*0x86 * ) 0x2020, "\xE2\x80\xA0", "DAGGER";
        {
            3, { (char)0xE2, (char)0x80, (char)0xA0 }
        },
        //(*0x87 * ) 0x2021, "\xE2\x80\xA1", "DOUBLE DAGGER";
        {
            3, { (char)0xE2, (char)0x80, (char)0xA1 }
        },
        //(*0x88 * ) 0x02c6, "\xCB\x86", "MODIFIER LETTER CIRCUMFLEX ACCENT";
        {
            2, { (char)0xCB, (char)0x86 }
        },
        //(*0x89 * ) 0x2030, "\xE2\x80\xB0", "PER MILLE SIGN";
        {
            3, { (char)0xE2, (char)0x80, (char)0xB0 }
        },
        //(*0x8a * ) 0x0160, "\xC5\xA0", "LATIN CAPITAL LETTER S WITH CARON";
        {
            2, { (char)0xC5, (char)0xA0 }
        },
        //(*0x8b * ) 0x2039, "\xE2\x80\xB9", "SINGLE LEFT-POINTING ANGLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0xB9 }
        },
        //(*0x8c * ) 0x0152, "\xC5\x92", "LATIN CAPITAL LIGATURE OE";
        {
            2, { (char)0xC5, (char)0x92 }
        },
        //(*0x8d * ) None, None, "UNDEFINED";
        {
            0, { 0 }
        },
        //(*0x8e * ) 0x017d, "\xC5\xBD", "LATIN CAPITAL LETTER Z WITH CARON";
        {
            2, { (char)0xC5, (char)0xBD }
        },
        //(*0x8f * ) None, None, "UNDEFINED";
        {
            0, { 0 }
        },
        //(*0x90 * ) None, None, "UNDEFINED";
        {
            0, { 0 }
        },
        //(*0x91 * ) 0x2018, "\xE2\x80\x98", "LEFT SINGLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x98 }
        },
        //(*0x92 * ) 0x2019, "\xE2\x80\x99", "RIGHT SINGLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x99 }
        },
        //(*0x93 * ) 0x201c, "\xE2\x80\x9C", "LEFT DOUBLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x9C }
        },
        //(*0x94 * ) 0x201d, "\xE2\x80\x9D", "RIGHT DOUBLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0x9D }
        },
        //(*0x95 * ) 0x2022, "\xE2\x80\xA2", "BULLET";
        {
            3, { (char)0xE2, (char)0x80, (char)0xA2 }
        },
        //(*0x96 * ) 0x2013, "\xE2\x80\x93", "EN DASH";
        {
            3, { (char)0xE2, (char)0x80, (char)0x93 }
        },
        //(*0x97 * ) 0x2014, "\xE2\x80\x94", "EM DASH";
        {
            3, { (char)0xE2, (char)0x80, (char)0x94 }
        },
        //(*0x98 * ) 0x02dc, "\xCB\x9C", "SMALL TILDE";
        {
            2, { (char)0xCB, (char)0x9C }
        },
        //(*0x99 * ) 0x2122, "\xE2\x84\xA2", "TRADE MARK SIGN";
        {
            3, { (char)0xE2, (char)0x80, (char)0xA2 }
        },
        //(*0x9a * ) 0x0161, "\xC5\xA1", "LATIN SMALL LETTER S WITH CARON";
        {
            2, { (char)0xC5, (char)0xA1 }
        },
        //(*0x9b * ) 0x203a, "\xE2\x80\xBA", "SINGLE RIGHT-POINTING ANGLE QUOTATION MARK";
        {
            3, { (char)0xE2, (char)0x80, (char)0xBA }
        },
        //(*0x9c * ) 0x0153, "\xC5\x93", "LATIN SMALL LIGATURE OE";
        {
            2, { (char)0xC5, (char)0x93 }
        },
        //(*0x9d * ) None, None, "UNDEFINED";
        {
            0, { 0 }
        },
        //(*0x9e * ) 0x017e, "\xC5\xBE", "LATIN SMALL LETTER Z WITH CARON";
        {
            2, { (char)0xC5, (char)0xBE }
        },
        //(*0x9f * ) 0x0178, "\xC5\xB8", "LATIN CAPITAL LETTER Y WITH DIAERESIS";
        {
            2, { (char)0xC5, (char)0xB8 }
        },
        //(*0xa0 * ) 0x00a0, "\xC2\xA0", "NO-BREAK SPACE";
        {
            2, { (char)0xC2, (char)0xA0 }
        },
        //(*0xa1 * ) 0x00a1, "\xC2\xA1", "INVERTED EXCLAMATION MARK";
        {
            2, { (char)0xC2, (char)0xA1 }
        },
        //(*0xa2 * ) 0x00a2, "\xC2\xA2", "CENT SIGN";
        {
            2, { (char)0xC2, (char)0xA2 }
        },
        //(*0xa3 * ) 0x00a3, "\xC2\xA3", "POUND SIGN";
        {
            2, { (char)0xC2, (char)0xA3 }
        },
        //(*0xa4 * ) 0x00a4, "\xC2\xA4", "CURRENCY SIGN";
        {
            2, { (char)0xC2, (char)0xA4 }
        },
        //(*0xa5 * ) 0x00a5, "\xC2\xA5", "YEN SIGN";
        {
            2, { (char)0xC2, (char)0xA5 }
        },
        //(*0xa6 * ) 0x00a6, "\xC2\xA6", "BROKEN BAR";
        {
            2, { (char)0xC2, (char)0xA6 }
        },
        //(*0xa7 * ) 0x00a7, "\xC2\xA7", "SECTION SIGN";
        {
            2, { (char)0xC2, (char)0xA7 }
        },
        //(*0xa8 * ) 0x00a8, "\xC2\xA8", "DIAERESIS";
        {
            2, { (char)0xC2, (char)0xA8 }
        },
        //(*0xa9 * ) 0x00a9, "\xC2\xA9", "COPYRIGHT SIGN";
        {
            2, { (char)0xC2, (char)0xA9 }
        },
        //(*0xaa * ) 0x00aa, "\xC2\xAA", "FEMININE ORDINAL INDICATOR";
        {
            2, { (char)0xC2, (char)0xAA }
        },
        //(*0xab * ) 0x00ab, "\xC2\xAB", "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK";
        {
            2, { (char)0xC2, (char)0xAB }
        },
        //(*0xac * ) 0x00ac, "\xC2\xAC", "NOT SIGN";
        {
            2, { (char)0xC2, (char)0xAC }
        },
        //(*0xad * ) 0x00ad, "\xC2\xAD", "SOFT HYPHEN";
        {
            2, { (char)0xC2, (char)0xAD }
        },
        //(*0xae * ) 0x00ae, "\xC2\xAE", "REGISTERED SIGN";
        {
            2, { (char)0xC2, (char)0xAE }
        },
        //(*0xaf * ) 0x00af, "\xC2\xAF", "MACRON";
        {
            2, { (char)0xC2, (char)0xAF }
        },
        //(*0xb0 * ) 0x00b0, "\xC2\xB0", "DEGREE SIGN";
        {
            2, { (char)0xC2, (char)0xB0 }
        },
        //(*0xb1 * ) 0x00b1, "\xC2\xB1", "PLUS-MINUS SIGN";
        {
            2, { (char)0xC2, (char)0xB1 }
        },
        //(*0xb2 * ) 0x00b2, "\xC2\xB2", "SUPERSCRIPT TWO";
        {
            2, { (char)0xC2, (char)0xB2 }
        },
        //(*0xb3 * ) 0x00b3, "\xC2\xB3", "SUPERSCRIPT THREE";
        {
            2, { (char)0xC2, (char)0xB3 }
        },
        //(*0xb4 * ) 0x00b4, "\xC2\xB4", "ACUTE ACCENT";
        {
            2, { (char)0xC2, (char)0xB4 }
        },
        //(*0xb5 * ) 0x00b5, "\xC2\xB5", "MICRO SIGN";
        {
            2, { (char)0xC2, (char)0xB5 }
        },
        //(*0xb6 * ) 0x00b6, "\xC2\xB6", "PILCROW SIGN";
        {
            2, { (char)0xC2, (char)0xB6 }
        },
        //(*0xb7 * ) 0x00b7, "\xC2\xB7", "MIDDLE DOT";
        {
            2, { (char)0xC2, (char)0xB7 }
        },
        //(*0xb8 * ) 0x00b8, "\xC2\xB8", "CEDILLA";
        {
            2, { (char)0xC2, (char)0xB8 }
        },
        //(*0xb9 * ) 0x00b9, "\xC2\xB9", "SUPERSCRIPT ONE";
        {
            2, { (char)0xC2, (char)0xB9 }
        },
        //(*0xba * ) 0x00ba, "\xC2\xBA", "MASCULINE ORDINAL INDICATOR";
        {
            2, { (char)0xC2, (char)0xBA }
        },
        //(*0xbb * ) 0x00bb, "\xC2\xBB", "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK";
        {
            2, { (char)0xC2, (char)0xBB }
        },
        //(*0xbc * ) 0x00bc, "\xC2\xBC", "VULGAR FRACTION ONE QUARTER";
        {
            2, { (char)0xC2, (char)0xBC }
        },
        //(*0xbd * ) 0x00bd, "\xC2\xBD", "VULGAR FRACTION ONE HALF";
        {
            2, { (char)0xC2, (char)0xBD }
        },
        //(*0xbe * ) 0x00be, "\xC2\xBE", "VULGAR FRACTION THREE QUARTERS";
        {
            2, { (char)0xC2, (char)0xBE }
        },
        //(*0xbf * ) 0x00bf, "\xC2\xBF", "INVERTED QUESTION MARK";
        {
            2, { (char)0xC2, (char)0xBF }
        },
        //(*0xc0 * ) 0x00c0, "\xC3\x80", "LATIN CAPITAL LETTER A WITH GRAVE";
        {
            2, { (char)0xC3, (char)0x80 }
        },
        //(*0xc1 * ) 0x00c1, "\xC3\x81", "LATIN CAPITAL LETTER A WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x81 }
        },
        //(*0xc2 * ) 0x00c2, "\xC3\x82", "LATIN CAPITAL LETTER A WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0x82 }
        },
        //(*0xc3 * ) 0x00c3, "\xC3\x83", "LATIN CAPITAL LETTER A WITH TILDE";
        {
            2, { (char)0xC3, (char)0x83 }
        },
        //(*0xc4 * ) 0x00c4, "\xC3\x84", "LATIN CAPITAL LETTER A WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0x84 }
        },
        //(*0xc5 * ) 0x00c5, "\xC3\x85", "LATIN CAPITAL LETTER A WITH RING ABOVE";
        {
            2, { (char)0xC3, (char)0x85 }
        },
        //(*0xc6 * ) 0x00c6, "\xC3\x86", "LATIN CAPITAL LETTER AE";
        {
            2, { (char)0xC3, (char)0x86 }
        },
        //(*0xc7 * ) 0x00c7, "\xC3\x87", "LATIN CAPITAL LETTER C WITH CEDILLA";
        {
            2, { (char)0xC3, (char)0x87 }
        },
        //(*0xc8 * ) 0x00c8, "\xC3\x88", "LATIN CAPITAL LETTER E WITH GRAVE";
        {
            2, { (char)0xC3, (char)0x88 }
        },
        //(*0xc9 * ) 0x00c9, "\xC3\x89", "LATIN CAPITAL LETTER E WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x89 }
        },
        //(*0xca * ) 0x00ca, "\xC3\x8A", "LATIN CAPITAL LETTER E WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0x8A }
        },
        //(*0xcb * ) 0x00cb, "\xC3\x8B", "LATIN CAPITAL LETTER E WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0x8B }
        },
        //(*0xcc * ) 0x00cc, "\xC3\x8C", "LATIN CAPITAL LETTER I WITH GRAVE";
        {
            2, { (char)0xC3, (char)0x8C }
        },
        //(*0xcd * ) 0x00cd, "\xC3\x8D", "LATIN CAPITAL LETTER I WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x8D }
        },
        //(*0xce * ) 0x00ce, "\xC3\x8E", "LATIN CAPITAL LETTER I WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0x8E }
        },
        //(*0xcf * ) 0x00cf, "\xC3\x8F", "LATIN CAPITAL LETTER I WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0x8F }
        },
        //(*0xd0 * ) 0x00d0, "\xC3\x90", "LATIN CAPITAL LETTER ETH";
        {
            2, { (char)0xC3, (char)0x90 }
        },
        //(*0xd1 * ) 0x00d1, "\xC3\x91", "LATIN CAPITAL LETTER N WITH TILDE";
        {
            2, { (char)0xC3, (char)0x91 }
        },
        //(*0xd2 * ) 0x00d2, "\xC3\x92", "LATIN CAPITAL LETTER O WITH GRAVE";
        {
            2, { (char)0xC3, (char)0x92 }
        },
        //(*0xd3 * ) 0x00d3, "\xC3\x93", "LATIN CAPITAL LETTER O WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x93 }
        },
        //(*0xd4 * ) 0x00d4, "\xC3\x94", "LATIN CAPITAL LETTER O WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0x94 }
        },
        //(*0xd5 * ) 0x00d5, "\xC3\x95", "LATIN CAPITAL LETTER O WITH TILDE";
        {
            2, { (char)0xC3, (char)0x95 }
        },
        //(*0xd6 * ) 0x00d6, "\xC3\x96", "LATIN CAPITAL LETTER O WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0x96 }
        },
        //(*0xd7 * ) 0x00d7, "\xC3\x97", "MULTIPLICATION SIGN";
        {
            2, { (char)0xC3, (char)0x97 }
        },
        //(*0xd8 * ) 0x00d8, "\xC3\x98", "LATIN CAPITAL LETTER O WITH STROKE";
        {
            2, { (char)0xC3, (char)0x98 }
        },
        //(*0xd9 * ) 0x00d9, "\xC3\x99", "LATIN CAPITAL LETTER U WITH GRAVE";
        {
            2, { (char)0xC3, (char)0x99 }
        },
        //(*0xda * ) 0x00da, "\xC3\x9A", "LATIN CAPITAL LETTER U WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x9A }
        },
        //(*0xdb * ) 0x00db, "\xC3\x9B", "LATIN CAPITAL LETTER U WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0x9B }
        },
        //(*0xdc * ) 0x00dc, "\xC3\x9C", "LATIN CAPITAL LETTER U WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0x9C }
        },
        //(*0xdd * ) 0x00dd, "\xC3\x9D", "LATIN CAPITAL LETTER Y WITH ACUTE";
        {
            2, { (char)0xC3, (char)0x9D }
        },
        //(*0xde * ) 0x00de, "\xC3\x9E", "LATIN CAPITAL LETTER THORN";
        {
            2, { (char)0xC3, (char)0x9E }
        },
        //(*0xdf * ) 0x00df, "\xC3\x9F", "LATIN SMALL LETTER SHARP S";
        {
            2, { (char)0xC3, (char)0x9F }
        },
        //(*0xe0 * ) 0x00e0, "\xC3\xA0", "LATIN SMALL LETTER A WITH GRAVE";
        {
            2, { (char)0xC3, (char)0xA0 }
        },
        //(*0xe1 * ) 0x00e1, "\xC3\xA1", "LATIN SMALL LETTER A WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xA1 }
        },
        //(*0xe2 * ) 0x00e2, "\xC3\xA2", "LATIN SMALL LETTER A WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0xA2 }
        },
        //(*0xe3 * ) 0x00e3, "\xC3\xA3", "LATIN SMALL LETTER A WITH TILDE";
        {
            2, { (char)0xC3, (char)0xA3 }
        },
        //(*0xe4 * ) 0x00e4, "\xC3\xA4", "LATIN SMALL LETTER A WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xA4 }
        },
        //(*0xe5 * ) 0x00e5, "\xC3\xA5", "LATIN SMALL LETTER A WITH RING ABOVE";
        {
            2, { (char)0xC3, (char)0xA5 }
        },
        //(*0xe6 * ) 0x00e6, "\xC3\xA6", "LATIN SMALL LETTER AE";
        {
            2, { (char)0xC3, (char)0xA6 }
        },
        //(*0xe7 * ) 0x00e7, "\xC3\xA7", "LATIN SMALL LETTER C WITH CEDILLA";
        {
            2, { (char)0xC3, (char)0xA7 }
        },
        //(*0xe8 * ) 0x00e8, "\xC3\xA8", "LATIN SMALL LETTER E WITH GRAVE";
        {
            2, { (char)0xC3, (char)0xA8 }
        },
        //(*0xe9 * ) 0x00e9, "\xC3\xA9", "LATIN SMALL LETTER E WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xA9 }
        },
        //(*0xea * ) 0x00ea, "\xC3\xAA", "LATIN SMALL LETTER E WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0xAA }
        },
        //(*0xeb * ) 0x00eb, "\xC3\xAB", "LATIN SMALL LETTER E WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xAB }
        },
        //(*0xec * ) 0x00ec, "\xC3\xAC", "LATIN SMALL LETTER I WITH GRAVE";
        {
            2, { (char)0xC3, (char)0xAC }
        },
        //(*0xed * ) 0x00ed, "\xC3\xAD", "LATIN SMALL LETTER I WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xAD }
        },
        //(*0xee * ) 0x00ee, "\xC3\xAE", "LATIN SMALL LETTER I WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0xAE }
        },
        //(*0xef * ) 0x00ef, "\xC3\xAF", "LATIN SMALL LETTER I WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xAF }
        },
        //(*0xf0 * ) 0x00f0, "\xC3\xB0", "LATIN SMALL LETTER ETH";
        {
            2, { (char)0xC3, (char)0xB0 }
        },
        //(*0xf1 * ) 0x00f1, "\xC3\xB1", "LATIN SMALL LETTER N WITH TILDE";
        {
            2, { (char)0xC3, (char)0xB1 }
        },
        //(*0xf2 * ) 0x00f2, "\xC3\xB2", "LATIN SMALL LETTER O WITH GRAVE";
        {
            2, { (char)0xC3, (char)0xB2 }
        },
        //(*0xf3 * ) 0x00f3, "\xC3\xB3", "LATIN SMALL LETTER O WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xB3 }
        },
        //(*0xf4 * ) 0x00f4, "\xC3\xB4", "LATIN SMALL LETTER O WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0xB4 }
        },
        //(*0xf5 * ) 0x00f5, "\xC3\xB5", "LATIN SMALL LETTER O WITH TILDE";
        {
            2, { (char)0xC3, (char)0xB5 }
        },
        //(*0xf6 * ) 0x00f6, "\xC3\xB6", "LATIN SMALL LETTER O WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xB6 }
        },
        //(*0xf7 * ) 0x00f7, "\xC3\xB7", "DIVISION SIGN";
        {
            2, { (char)0xC3, (char)0xB7 }
        },
        //(*0xf8 * ) 0x00f8, "\xC3\xB8", "LATIN SMALL LETTER O WITH STROKE";
        {
            2, { (char)0xC3, (char)0xB8 }
        },
        //(*0xf9 * ) 0x00f9, "\xC3\xB9", "LATIN SMALL LETTER U WITH GRAVE";
        {
            2, { (char)0xC3, (char)0xB9 }
        },
        //(*0xfa * ) 0x00fa, "\xC3\xBA", "LATIN SMALL LETTER U WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xBA }
        },
        //(*0xfb * ) 0x00fb, "\xC3\xBB", "LATIN SMALL LETTER U WITH CIRCUMFLEX";
        {
            2, { (char)0xC3, (char)0xBB }
        },
        //(*0xfc * ) 0x00fc, "\xC3\xBC", "LATIN SMALL LETTER U WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xBC }
        },
        //(*0xfd * ) 0x00fd, "\xC3\xBD", "LATIN SMALL LETTER Y WITH ACUTE";
        {
            2, { (char)0xC3, (char)0xBD }
        },
        //(*0xfe * ) 0x00fe, "\xC3\xBE", "LATIN SMALL LETTER THORN";
        {
            2, { (char)0xC3, (char)0xBE }
        },
        //(*0xff * ) 0x00ff, "\xC3\xBF", "LATIN SMALL LETTER Y WITH DIAERESIS";
        {
            2, { (char)0xC3, (char)0xBF }
        },
    };
}

bool cp1252_to_utf8(const char* buf, int len, std::string& text)
{
    bool result = false;
    int cur = 0;
    int pos = 0;
    char* temp = NULL;
    do {
        if (len <= 0) {
            text = std::string("");
            result = true;
            break;
        }
        if (buf == NULL) break;
        temp = (char*)malloc(len * sizeof(char) * 3);
        if (temp == NULL) break;
        memset(temp, 0, len);
        while (cur < len)
        {
            char c = *(buf + cur);
            if (c & 0x80)
            {
                int index = c & 0x7f;
                if (cp1252_to_utf8_map[index].size > 0)
                {
                    memcpy(temp + pos, cp1252_to_utf8_map[index].buf, cp1252_to_utf8_map[index].size);
                    pos += cp1252_to_utf8_map[index].size;
                }
            }
            else
            {
                *(temp + pos) = c;
                ++pos;
            }
            ++cur;
        }
        text = std::string(temp, pos);
        result = true;
    } while (false);
    if (temp) {
        free(temp);
    }
    return result;
}

bool StringToNumber(const std::string& text, int32_t& v)
{
    bool result = true;
    int r = 0;
    bool negative = false;
    std::string temp = text;
    if (temp.size() > 1) {
        if (text[0] == '-') {
            negative = true;
            temp = temp.substr(1);
        }
    }
    for (const char& c: temp) {
        if (c >= '0' && c <= '9') {
            r = r * 10 + (c - '0');
            continue;
        }
        if (c == '.') break;
        result = false;
        break;
    }
    if (negative) r = r * (-1);
    if (result) v = r;
    return result;
}
