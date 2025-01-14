#pragma once

//-----------------------------------------------------------------
//   TmSchema.h - Theme Manager schema (properties, parts, etc)
//-----------------------------------------------------------------
//   Note: this file is normally #include-ed twice a single .cpp 
//         file.  The 2nd time, SCHEME_STRINGS should be defined.  
//         This allows the enums and strings to be kept in a 
//         single logical table and ensure they stay in sync with
//         each other.
//-----------------------------------------------------------------

namespace Restyle
{

//-----------------------------------------------------------------
#if (defined(SCHEMA_STRINGS)) || (! defined(TMSCHEMA_H))
//-----------------------------------------------------------------
#define TMSCHEMA_H
//-----------------------------------------------------------------
#include "restyle_SchemaDef.h"
// #define SCHEMA_STRINGS // testing
// #include "restyle_SchemaDef.h"
//-----------------------------------------------------------------
#define THEMEMGR_VERSION 1  // increment if order of props changes or 
                            // any props are deleted (will prevent loading
                            // of controlsets that use older version
//-----------------------------------------------------------------
BEGIN_TM_SCHEMA(ThemeMgrSchema)

//-----------------------------------------------------------------
//   TM_ENUM (must also be declared in PROPERTIES section)
//
//    these cannot be renumbered (part of uxtheme API)
//-----------------------------------------------------------------
BEGIN_TM_ENUM(BGTYPE, "BgType")
    TM_ENUM(0, BT, IMAGEFILE, "ImageFile")
    TM_ENUM(1, BT, BORDERFILL, "BorderFill")
    TM_ENUM(2, BT, NONE, "None")
END_TM_ENUM()

BEGIN_TM_ENUM(IMAGELAYOUT, "ImageLayout")
    TM_ENUM(0, IL, VERTICAL, "Vertical")
    TM_ENUM(1, IL, HORIZONTAL, "Horizontal")
END_TM_ENUM()

BEGIN_TM_ENUM(BORDERTYPE, "BorderType")
    TM_ENUM(0, BT, RECT, "Rect")
    TM_ENUM(1, BT, ROUNDRECT, "RoundRect")
    TM_ENUM(2, BT, ELLIPSE, "Ellipse")
END_TM_ENUM()

BEGIN_TM_ENUM(FILLTYPE, "FillType")
    TM_ENUM(0, FT, SOLID, "Solid")
    TM_ENUM(1, FT, VERTGRADIENT, "VertGradient")
    TM_ENUM(2, FT, HORZGRADIENT, "HorzGradient")
    TM_ENUM(3, FT, RADIALGRADIENT, "RadialGradient")
    TM_ENUM(4, FT, TILEIMAGE, "TileImage")
END_TM_ENUM()

BEGIN_TM_ENUM(SIZINGTYPE, "SizingType")
    TM_ENUM(0, ST, TRUESIZE, "TrueSize")
    TM_ENUM(1, ST, STRETCH, "Stretch")
    TM_ENUM(2, ST, TILE, "Tile")
END_TM_ENUM()

BEGIN_TM_ENUM(HALIGN, "HAlign")
    TM_ENUM(0, HA, LEFT, "Left")
    TM_ENUM(1, HA, CENTER, "Center")
    TM_ENUM(2, HA, RIGHT, "Right")
END_TM_ENUM()

BEGIN_TM_ENUM(CONTENTALIGNMENT, "ContentAlignment")
    TM_ENUM(0, CA, LEFT, "Left")
    TM_ENUM(1, CA, CENTER, "Center")
    TM_ENUM(2, CA, RIGHT, "Right")
END_TM_ENUM()

BEGIN_TM_ENUM(VALIGN, "VAlign")
    TM_ENUM(0, VA, TOP, "Top")
    TM_ENUM(1, VA, CENTER, "Center")
    TM_ENUM(2, VA, BOTTOM, "Bottom")
END_TM_ENUM()

BEGIN_TM_ENUM(OFFSETTYPE, "OffsetType")
    TM_ENUM(0, OT, TOPLEFT, "TopLeft")
    TM_ENUM(1, OT, TOPRIGHT, "TopRight")
    TM_ENUM(2, OT, TOPMIDDLE, "TopMiddle")
    TM_ENUM(3, OT, BOTTOMLEFT, "BottomLeft")
    TM_ENUM(4, OT, BOTTOMRIGHT, "BottomRight")
    TM_ENUM(5, OT, BOTTOMMIDDLE, "BottomMiddle")
    TM_ENUM(6, OT, MIDDLELEFT, "MiddleLeft")
    TM_ENUM(7, OT, MIDDLERIGHT, "MiddleRight")
    TM_ENUM(8, OT, LEFTOFCAPTION, "LeftOfCaption")
    TM_ENUM(9, OT, RIGHTOFCAPTION, "RightOfCaption")
    TM_ENUM(10, OT, LEFTOFLASTBUTTON, "LeftOfLastButton")
    TM_ENUM(11, OT, RIGHTOFLASTBUTTON, "RightOfLastButton")
    TM_ENUM(12, OT, ABOVELASTBUTTON, "AboveLastButton")
    TM_ENUM(13, OT, BELOWLASTBUTTON, "BelowLastButton")
END_TM_ENUM()

BEGIN_TM_ENUM(ICONEFFECT, "IconEffect")
    TM_ENUM(0, ICE, NONE, "None")
    TM_ENUM(1, ICE, GLOW, "Glow")
    TM_ENUM(2, ICE, SHADOW, "Shadow")
    TM_ENUM(3, ICE, PULSE, "Pulse")
    TM_ENUM(4, ICE, ALPHA, "Alpha")
END_TM_ENUM()

BEGIN_TM_ENUM(TEXTSHADOWTYPE, "TextShadowType")
    TM_ENUM(0, TST, NONE, "None")
    TM_ENUM(1, TST, SINGLE, "Single")
    TM_ENUM(2, TST, CONTINUOUS, "Continuous")
END_TM_ENUM()

BEGIN_TM_ENUM(GLYPHTYPE, "GlyphType")
    TM_ENUM(0, GT, NONE, "None")
    TM_ENUM(1, GT, IMAGEGLYPH, "ImageGlyph")
    TM_ENUM(2, GT, FONTGLYPH, "FontGlyph")
END_TM_ENUM()

BEGIN_TM_ENUM(IMAGESELECTTYPE, "ImageSelectType")
    TM_ENUM(0, IST, NONE, "None")
    TM_ENUM(1, IST, SIZE, "Size")
    TM_ENUM(2, IST, DPI, "DPI")
END_TM_ENUM()

BEGIN_TM_ENUM(TRUESIZESCALINGTYPE, "TrueSizeScalingType")
    TM_ENUM(0, TSST, NONE, "None")
    TM_ENUM(1, TSST, SIZE, "Size")
    TM_ENUM(2, TSST, DPI, "DPI")
END_TM_ENUM()

BEGIN_TM_ENUM(GLYPHFONTSIZINGTYPE, "GlyphFontSizingType")
    TM_ENUM(0, GFST, NONE, "None")
    TM_ENUM(1, GFST, SIZE, "Size")
    TM_ENUM(2, GFST, DPI, "DPI")
END_TM_ENUM()

//-----------------------------------------------------------------
//    PROPERTIES - used by uxtheme rendering and controls
//      
//    these cannot be renumbered (part of uxtheme API)
//-----------------------------------------------------------------
BEGIN_TM_PROPS()

    //---- primitive types ----
    TM_PROP(201, TMT, STRING, "String",     STRING)
    TM_PROP(202, TMT, INT, "Int",           INT)
    TM_PROP(203, TMT, BOOL, "Bool",         BOOL)
    TM_PROP(204, TMT, COLOR, "Color",       COLOR)
    TM_PROP(205, TMT, MARGINS, "Margins",   MARGINS)
    TM_PROP(206, TMT, FILENAME, "FileName", FILENAME)
    TM_PROP(207, TMT, SIZE, "Size",         SIZE)
    TM_PROP(208, TMT, POSITION, "Position", POSITION)
    TM_PROP(209, TMT, RECT, "Rect",         RECT)
    TM_PROP(210, TMT, FONT, "Font",         FONT)
    TM_PROP(211, TMT, INTLIST, "IntList",   INTLIST)

    //---- special misc. properties ----
    TM_PROP(401, TMT, COLORSCHEMES, "ColorSchemes",   STRING)
    TM_PROP(402, TMT, SIZES, "Sizes",                 STRING)
    TM_PROP(403, TMT, CHARSET, "Charset",             INT)

    //---- [documentation] properties ----

#define TMT_FIRST_RCSTRING_NAME   TMT_DISPLAYNAME
#define TMT_LAST_RCSTRING_NAME    TMT_DESCRIPTION

    TM_PROP(601, TMT, DISPLAYNAME, "DisplayName",    STRING)
    TM_PROP(602, TMT, TOOLTIP, "ToolTip",            STRING)
    TM_PROP(603, TMT, COMPANY, "Company",            STRING)
    TM_PROP(604, TMT, AUTHOR, "Author",              STRING)
    TM_PROP(605, TMT, COPYRIGHT, "Copyright",        STRING)
    TM_PROP(606, TMT, URL, "URL",                    STRING)
    TM_PROP(607, TMT, VERSION, "Version",            STRING)
    TM_PROP(608, TMT, DESCRIPTION, "Description",    STRING)

    //---- theme metrics: fonts ----

#define TMT_FIRSTFONT TMT_CAPTIONFONT

#ifndef TMT_LASTFONT
#define TMT_LASTFONT  TMT_BODYFONT
#endif

    TM_PROP(801, TMT, CAPTIONFONT, "CaptionFont",        FONT)
    TM_PROP(802, TMT, SMALLCAPTIONFONT, "SmallCaptionFont",   FONT)
    TM_PROP(803, TMT, MENUFONT, "MenuFont",           FONT)
    TM_PROP(804, TMT, STATUSFONT, "StatusFont",         FONT)
    TM_PROP(805, TMT, MSGBOXFONT, "MsgBoxFont",         FONT)
    TM_PROP(806, TMT, ICONTITLEFONT, "IconTitleFont",      FONT)
    // V4 props:
    TM_PROP(807, TMT, HEADING1FONT, "Heading1Font", FONT)
    TM_PROP(808, TMT, HEADING2FONT, "Heading2Font", FONT)
    TM_PROP(809, TMT, BODYFONT, "BodyFont", FONT)

    //---- theme metrics: bools ----

#define TMT_FIRSTBOOL   TMT_FLATMENUS
#define TMT_LASTBOOL    TMT_FLATMENUS

    TM_PROP(1001, TMT, FLATMENUS, "FlatMenus",           BOOL)

    //---- theme metrics: sizes ----

#define TMT_FIRSTSIZE   TMT_SIZINGBORDERWIDTH

#ifndef TMT_LASTSIZE
#define TMT_LASTSIZE    TMT_PADDEDBORDERWIDTH
#endif

    TM_PROP(1201, TMT, SIZINGBORDERWIDTH, "SizingBorderWidth",    SIZE)
    TM_PROP(1202, TMT, SCROLLBARWIDTH, "ScrollbarWidth",       SIZE)
    TM_PROP(1203, TMT, SCROLLBARHEIGHT, "ScrollbarHeight",      SIZE)
    TM_PROP(1204, TMT, CAPTIONBARWIDTH, "CaptionBarWidth",      SIZE)
    TM_PROP(1205, TMT, CAPTIONBARHEIGHT, "CaptionBarHeight",     SIZE)
    TM_PROP(1206, TMT, SMCAPTIONBARWIDTH, "SmallCaptionBarWidth",    SIZE)
    TM_PROP(1207, TMT, SMCAPTIONBARHEIGHT, "SmallCaptionBarHeight",   SIZE)
    TM_PROP(1208, TMT, MENUBARWIDTH, "MenuBarWidth",         SIZE)
    TM_PROP(1209, TMT, MENUBARHEIGHT, "MenuBarHeight",        SIZE)
    // V4 props:
    TM_PROP(1210, TMT, PADDEDBORDERWIDTH, "PaddedBorderWidth", SIZE)

    //---- theme metrics: ints ----

#ifndef TMT_FIRSTINT
#define TMT_FIRSTINT   TMT_MINCOLORDEPTH
#endif
#ifndef TMT_LASTINT
#define TMT_LASTINT    TMT_MINCOLORDEPTH
#endif

    TM_PROP(1301, TMT, MINCOLORDEPTH, "MinColorDepth",     INT)

    //---- theme metrics: strings ----

#define TMT_FIRSTSTRING   TMT_CSSNAME

#ifndef TMT_LASTSTRING
#define TMT_LASTSTRING    TMT_ALIAS
#endif

    TM_PROP(1401, TMT, CSSNAME, "CssName",            STRING)
    TM_PROP(1402, TMT, XMLNAME, "XmlName",            STRING)
    // V4 props:
    TM_PROP(1403, TMT, LASTUPDATED, "LastUpdated", STRING)
    TM_PROP(1404, TMT, ALIAS, "Alias", STRING)

    //---- theme metrics: colors ----

#define TMT_FIRSTCOLOR  TMT_SCROLLBAR
#define TMT_LASTCOLOR   TMT_MENUBAR

    TM_PROP(1601, TMT, SCROLLBAR, "Scrollbar",         COLOR)
    TM_PROP(1602, TMT, BACKGROUND, "Background",        COLOR)
    TM_PROP(1603, TMT, ACTIVECAPTION, "ActiveCaption",     COLOR)
    TM_PROP(1604, TMT, INACTIVECAPTION, "InactiveCaption",   COLOR)
    TM_PROP(1605, TMT, MENU, "Menu",              COLOR)
    TM_PROP(1606, TMT, WINDOW, "Window",            COLOR)
    TM_PROP(1607, TMT, WINDOWFRAME, "WindowFrame",       COLOR)
    TM_PROP(1608, TMT, MENUTEXT, "MenuText",          COLOR)
    TM_PROP(1609, TMT, WINDOWTEXT, "WindowText",        COLOR)
    TM_PROP(1610, TMT, CAPTIONTEXT, "CaptionText",       COLOR)
    TM_PROP(1611, TMT, ACTIVEBORDER, "ActiveBorder",      COLOR)
    TM_PROP(1612, TMT, INACTIVEBORDER, "InactiveBorder",    COLOR)
    TM_PROP(1613, TMT, APPWORKSPACE, "AppWorkspace",      COLOR)
    TM_PROP(1614, TMT, HIGHLIGHT, "Highlight",         COLOR)
    TM_PROP(1615, TMT, HIGHLIGHTTEXT, "HighlightText",     COLOR)
    TM_PROP(1616, TMT, BTNFACE, "Btnface",           COLOR)
    TM_PROP(1617, TMT, BTNSHADOW, "BtnShadow",         COLOR)
    TM_PROP(1618, TMT, GRAYTEXT, "GrayText",          COLOR)
    TM_PROP(1619, TMT, BTNTEXT, "ButtonText",           COLOR)
    TM_PROP(1620, TMT, INACTIVECAPTIONTEXT, "InactiveCaptionText",    COLOR)
    TM_PROP(1621, TMT, BTNHIGHLIGHT, "BtnHighlight",           COLOR)
    TM_PROP(1622, TMT, DKSHADOW3D, "DkShadow3d",             COLOR)
    TM_PROP(1623, TMT, LIGHT3D, "Light3d",                COLOR)
    TM_PROP(1624, TMT, INFOTEXT, "InfoText",               COLOR)
    TM_PROP(1625, TMT, INFOBK, "InfoBk",                 COLOR)
    TM_PROP(1626, TMT, BUTTONALTERNATEFACE, "ButtonAlternateFace",    COLOR)
    TM_PROP(1627, TMT, HOTTRACKING, "HotTracking",            COLOR)
    TM_PROP(1628, TMT, GRADIENTACTIVECAPTION, "GradientActiveCaption",  COLOR)
    TM_PROP(1629, TMT, GRADIENTINACTIVECAPTION, "GradientInactiveCaption",  COLOR)
    TM_PROP(1630, TMT, MENUHILIGHT, "MenuHilight",            COLOR)
    TM_PROP(1631, TMT, MENUBAR, "MenuBar",                COLOR)

    //---- hue substitutions ----
    TM_PROP(1801, TMT, FROMHUE1, "FromHue1",  INT)
    TM_PROP(1802, TMT, FROMHUE2, "FromHue2",  INT)
    TM_PROP(1803, TMT, FROMHUE3, "FromHue3",  INT)
    TM_PROP(1804, TMT, FROMHUE4, "FromHue4",  INT)
    TM_PROP(1805, TMT, FROMHUE5, "FromHue5",  INT)
    TM_PROP(1806, TMT, TOHUE1, "ToHue1",    INT)
    TM_PROP(1807, TMT, TOHUE2, "ToHue2",    INT)
    TM_PROP(1808, TMT, TOHUE3, "ToHue3",    INT)
    TM_PROP(1809, TMT, TOHUE4, "ToHue4",    INT)
    TM_PROP(1810, TMT, TOHUE5, "ToHue5",    INT)

    //---- color substitutions ----
    TM_PROP(2001, TMT, FROMCOLOR1, "FromColor1",  COLOR)
    TM_PROP(2002, TMT, FROMCOLOR2, "FromColor2",  COLOR)
    TM_PROP(2003, TMT, FROMCOLOR3, "FromColor3",  COLOR)
    TM_PROP(2004, TMT, FROMCOLOR4, "FromColor4",  COLOR)
    TM_PROP(2005, TMT, FROMCOLOR5, "FromColor5",  COLOR)
    TM_PROP(2006, TMT, TOCOLOR1, "ToColor1",    COLOR)
    TM_PROP(2007, TMT, TOCOLOR2, "ToColor2",    COLOR)
    TM_PROP(2008, TMT, TOCOLOR3, "ToColor3",    COLOR)
    TM_PROP(2009, TMT, TOCOLOR4, "ToColor4",    COLOR)
    TM_PROP(2010, TMT, TOCOLOR5, "ToColor5",    COLOR)

    //---- rendering BOOL properties ----
    TM_PROP(2201, TMT, TRANSPARENT, "Transparent",   BOOL)       // image has transparent areas (see TransparentColor)
    TM_PROP(2202, TMT, AUTOSIZE, "AutoSize",      BOOL)       // if TRUE, nonclient caption width varies with text extent
    TM_PROP(2203, TMT, BORDERONLY, "BorderOnly",    BOOL)       // only draw the border area of the image
    TM_PROP(2204, TMT, COMPOSITED, "Composited",    BOOL)       // control will handle the composite drawing
    TM_PROP(2205, TMT, BGFILL, "BgFill",        BOOL)       // if TRUE, TRUESIZE images should be drawn on bg fill
    TM_PROP(2206, TMT, GLYPHTRANSPARENT, "GlyphTransparent",  BOOL)   // glyph has transparent areas (see GlyphTransparentColor)
    TM_PROP(2207, TMT, GLYPHONLY, "GlyphOnly",         BOOL)   // only draw glyph (not background)
    TM_PROP(2208, TMT, ALWAYSSHOWSIZINGBAR, "AlwaysShowSizingBar", BOOL)
    TM_PROP(2209, TMT, MIRRORIMAGE, "MirrorImage",         BOOL) // default=TRUE means image gets mirrored in RTL (Mirror) windows
    TM_PROP(2210, TMT, UNIFORMSIZING, "UniformSizing",       BOOL) // if TRUE, height & width must be uniformly sized 
    TM_PROP(2211, TMT, INTEGRALSIZING, "IntegralSizing",      BOOL) // for TRUESIZE and Border sizing; if TRUE, factor must be integer
    TM_PROP(2212, TMT, SOURCEGROW, "SourceGrow",          BOOL) // if TRUE, will scale up src image when needed
    TM_PROP(2213, TMT, SOURCESHRINK, "SourceShrink",        BOOL) // if TRUE, will scale down src image when needed
    // V4 props:
    TM_PROP(2214, TMT, DRAWBORDERS, "DrawBorders", BOOL)
    TM_PROP(2215, TMT, NOETCHEDEFFECT, "NoEtchedEffect", BOOL)
    TM_PROP(2216, TMT, TEXTAPPLYOVERLAY, "TextApplyOverlay", BOOL)
    TM_PROP(2217, TMT, TEXTGLOW, "TextGlow", BOOL)
    TM_PROP(2218, TMT, TEXTITALIC, "TextItalic", BOOL)
    TM_PROP(2219, TMT, COMPOSITEDOPAQUE, "CompositedOpaque", BOOL)
    TM_PROP(2220, TMT, LOCALIZEDMIRRORIMAGE, "LocalizedMirrorImage", BOOL)

    //---- rendering INT properties ----
    TM_PROP(2401, TMT, IMAGECOUNT, "ImageCount",        INT)    // the number of state images in an imagefile
    TM_PROP(2402, TMT, ALPHALEVEL, "AlphaLevel",        INT)    // (0-255) alpha value for an icon (DrawThemeIcon part)
    TM_PROP(2403, TMT, BORDERSIZE, "BorderSize",        INT)    // the size of the border line for bgtype=BorderFill
    TM_PROP(2404, TMT, ROUNDCORNERWIDTH, "RoundCornerWidth",  INT)    // (0-100) % of roundness for rounded rects
    TM_PROP(2405, TMT, ROUNDCORNERHEIGHT, "RoundCornerHeight", INT)    // (0-100) % of roundness for rounded rects
    TM_PROP(2406, TMT, GRADIENTRATIO1, "GradientRatio1",    INT)    // (0-255) - amt of gradient color 1 to use (all must total=255)
    TM_PROP(2407, TMT, GRADIENTRATIO2, "GradientRatio2",    INT)    // (0-255) - amt of gradient color 2 to use (all must total=255)
    TM_PROP(2408, TMT, GRADIENTRATIO3, "GradientRatio3",    INT)    // (0-255) - amt of gradient color 3 to use (all must total=255)
    TM_PROP(2409, TMT, GRADIENTRATIO4, "GradientRatio4",    INT)    // (0-255) - amt of gradient color 4 to use (all must total=255)
    TM_PROP(2410, TMT, GRADIENTRATIO5, "GradientRatio5",    INT)    // (0-255) - amt of gradient color 5 to use (all must total=255)
    TM_PROP(2411, TMT, PROGRESSCHUNKSIZE, "ProgressChunkSize", INT)    // size of progress control chunks
    TM_PROP(2412, TMT, PROGRESSSPACESIZE, "ProgressSpaceSize", INT)    // size of progress control spaces
    TM_PROP(2413, TMT, SATURATION, "Saturation",        INT)    // (0-255) amt of saturation for DrawThemeIcon() part
    TM_PROP(2414, TMT, TEXTBORDERSIZE, "TextBorderSize",    INT)    // size of border around text chars
    TM_PROP(2415, TMT, ALPHATHRESHOLD, "AlphaThreshold",    INT)    // (0-255) the min. alpha value of a pixel that is solid
    TM_PROP(2416, TMT, WIDTH, "Width",             SIZE)   // custom window prop: size of part (min. window)
    TM_PROP(2417, TMT, HEIGHT, "Height",            SIZE)   // custom window prop: size of part (min. window)
    TM_PROP(2418, TMT, GLYPHINDEX, "GlyphIndex",        INT)    // for font-based glyphs, the char index into the font
    TM_PROP(2419, TMT, TRUESIZESTRETCHMARK, "TrueSizeStretchMark", INT)  // stretch TrueSize image when target exceeds source by this percent
    TM_PROP(2420, TMT, MINDPI1, "MinDpi1",         INT)      // min DPI ImageFile1 was designed for
    TM_PROP(2421, TMT, MINDPI2, "MinDpi2",         INT)      // min DPI ImageFile1 was designed for
    TM_PROP(2422, TMT, MINDPI3, "MinDpi3",         INT)      // min DPI ImageFile1 was designed for
    TM_PROP(2423, TMT, MINDPI4, "MinDpi4",         INT)      // min DPI ImageFile1 was designed for
    TM_PROP(2424, TMT, MINDPI5, "MinDpi5",         INT)      // min DPI ImageFile1 was designed for
    // V4 props:
    TM_PROP(2425, TMT, TEXTGLOWSIZE, "TextGlowSize", INT)
    TM_PROP(2426, TMT, FRAMESPERSECOND, "FramesPerSecond", INT)
    TM_PROP(2427, TMT, PIXELSPERFRAME, "PixelsPerFrame", INT)
    TM_PROP(2428, TMT, ANIMATIONDELAY, "AnimationDelay", INT)
    TM_PROP(2429, TMT, GLOWINTENSITY, "GlowIntensity", INT)
    TM_PROP(2430, TMT, OPACITY, "Opacity", INT)
    TM_PROP(2431, TMT, COLORIZATIONCOLOR, "ColorizationColor", COLOR) // is this really int?
    TM_PROP(2432, TMT, COLORIZATIONOPACITY, "ColorizationOpacity", INT)
    TM_PROP(2433, TMT, MINDPI6, "MinDpi6", INT)
    TM_PROP(2434, TMT, MINDPI7, "MinDpi7", INT)

    //---- rendering FONT properties ----
    TM_PROP(2601, TMT, GLYPHFONT, "GlyphFont",         FONT)   // the font that the glyph is drawn with

    //---- rendering INTLIST properties ----
    // start with 2801
                                                // (from smallest to largest)
    //---- rendering FILENAME properties ----
    TM_PROP(3001, TMT, IMAGEFILE, "ImageFile",         FILENAME)   // the filename of the image (or basename, for mult. images)
    TM_PROP(3002, TMT, IMAGEFILE1, "ImageFile1",        FILENAME)   // multiresolution image file
    TM_PROP(3003, TMT, IMAGEFILE2, "ImageFile2",        FILENAME)   // multiresolution image file
    TM_PROP(3004, TMT, IMAGEFILE3, "ImageFile3",        FILENAME)   // multiresolution image file
    TM_PROP(3005, TMT, IMAGEFILE4, "ImageFile4",        FILENAME)   // multiresolution image file
    TM_PROP(3006, TMT, IMAGEFILE5, "ImageFile5",        FILENAME)   // multiresolution image file

//
//  TMT_STOCKIMAGEFILE is obsolete
//
#ifdef 0

    TM_PROP(3007, TMT, STOCKIMAGEFILE, "StockImageFile",    FILENAME)   // These are the only images that you can call GetThemeBitmap on

#endif //#ifndef SCHEMA_VERIFY_VSSYM32

    TM_PROP(3008, TMT, GLYPHIMAGEFILE, "GlyphImageFile",    FILENAME)   // the filename for the glyph image
    
    // V4 props:
    TM_PROP(3009, TMT, IMAGEFILE6, "ImageFile6", FILENAME)
    TM_PROP(3010, TMT, IMAGEFILE7, "ImageFile7", FILENAME)

    //---- rendering STRING properties ----
    TM_PROP(3201, TMT, TEXT, "Text"              STRING)
    // V4 props:
    TM_PROP(3202, TMT, CLASSICVALUE, "ClassicValue", STRING)

    //---- rendering POSITION (x and y values) properties ----
    TM_PROP(3401, TMT, OFFSET, "Offset",            POSITION)   // for window part layout
    TM_PROP(3402, TMT, TEXTSHADOWOFFSET, "TextShadowOffset",  POSITION)   // where char shadows are drawn, relative to orig. chars
    TM_PROP(3403, TMT, MINSIZE, "MinSize",           POSITION)   // min dest rect than ImageFile was designed for
    TM_PROP(3404, TMT, MINSIZE1, "MinSize1",          POSITION)   // min dest rect than ImageFile1 was designed for
    TM_PROP(3405, TMT, MINSIZE2, "MinSize2",          POSITION)   // min dest rect than ImageFile2 was designed for
    TM_PROP(3406, TMT, MINSIZE3, "MinSize3",          POSITION)   // min dest rect than ImageFile3 was designed for
    TM_PROP(3407, TMT, MINSIZE4, "MinSize4",          POSITION)   // min dest rect than ImageFile4 was designed for
    TM_PROP(3408, TMT, MINSIZE5, "MinSize5",          POSITION)   // min dest rect than ImageFile5 was designed for
    TM_PROP(3409, TMT, NORMALSIZE, "NormalSize",        POSITION)   // size of dest rect that exactly source
    // V4 props:
    TM_PROP(3410, TMT, MINSIZE6, "MinSize6", POSITION)
    TM_PROP(3411, TMT, MINSIZE7, "MinSize7", POSITION)

    //---- rendering MARGIN properties ----
    TM_PROP(3601, TMT, SIZINGMARGINS, "SizingMargins",     MARGINS)    // margins used for 9-grid sizing
    TM_PROP(3602, TMT, CONTENTMARGINS, "ContentMargins",    MARGINS)    // margins that define where content can be placed
    TM_PROP(3603, TMT, CAPTIONMARGINS, "CaptionMargins",    MARGINS)    // margins that define where caption text can be placed

    //---- rendering COLOR properties ----
    TM_PROP(3801, TMT, BORDERCOLOR, "BorderColor",      COLOR)       // color of borders for BorderFill 
    TM_PROP(3802, TMT, FILLCOLOR, "FillColor",        COLOR)       // color of bg fill 
    TM_PROP(3803, TMT, TEXTCOLOR, "TextColor",        COLOR)       // color text is drawn in
    TM_PROP(3804, TMT, EDGELIGHTCOLOR, "EdgeLightColor",     COLOR)     // edge color
    TM_PROP(3805, TMT, EDGEHIGHLIGHTCOLOR, "EdgeHighlightColor", COLOR)     // edge color
    TM_PROP(3806, TMT, EDGESHADOWCOLOR, "EdgeShadowColor",    COLOR)     // edge color
    TM_PROP(3807, TMT, EDGEDKSHADOWCOLOR, "EdgeDkShadowColor",  COLOR)     // edge color
    TM_PROP(3808, TMT, EDGEFILLCOLOR, "EdgeFillColor",  COLOR)         // edge color
    TM_PROP(3809, TMT, TRANSPARENTCOLOR, "TransparentColor", COLOR)       // color of pixels that are treated as transparent (not drawn)
    TM_PROP(3810, TMT, GRADIENTCOLOR1, "GradientColor1",   COLOR)       // first color in gradient
    TM_PROP(3811, TMT, GRADIENTCOLOR2, "GradientColor2",   COLOR)       // second color in gradient
    TM_PROP(3812, TMT, GRADIENTCOLOR3, "GradientColor3",   COLOR)       // third color in gradient
    TM_PROP(3813, TMT, GRADIENTCOLOR4, "GradientColor4",   COLOR)       // forth color in gradient
    TM_PROP(3814, TMT, GRADIENTCOLOR5, "GradientColor5",   COLOR)       // fifth color in gradient
    TM_PROP(3815, TMT, SHADOWCOLOR, "ShadowColor",      COLOR)       // color of text shadow
    TM_PROP(3816, TMT, GLOWCOLOR, "GlowColor",        COLOR)       // color of glow produced by DrawThemeIcon
    TM_PROP(3817, TMT, TEXTBORDERCOLOR, "TextBorderColor",  COLOR)       // color of text border
    TM_PROP(3818, TMT, TEXTSHADOWCOLOR, "TextShadowColor",  COLOR)       // color of text shadow
    TM_PROP(3819, TMT, GLYPHTEXTCOLOR, "GlyphTextColor",        COLOR)  // color that font-based glyph is drawn with
    TM_PROP(3820, TMT, GLYPHTRANSPARENTCOLOR, "GlyphTransparentColor", COLOR)  // color of transparent pixels in GlyphImageFile
    TM_PROP(3821, TMT, FILLCOLORHINT, "FillColorHint", COLOR)          // hint about fill color used (for custom controls)
    TM_PROP(3822, TMT, BORDERCOLORHINT, "BorderColorHint", COLOR)        // hint about border color used (for custom controls)
    TM_PROP(3823, TMT, ACCENTCOLORHINT, "AccentColorHint", COLOR)        // hint about accent color used (for custom controls)
    // V4 props:
    TM_PROP(3824, TMT, TEXTCOLORHINT, "TextColorHint", COLOR)
    TM_PROP(3825, TMT, HEADING1TEXTCOLOR, "Heading1TextColor", COLOR)
    TM_PROP(3826, TMT, HEADING2TEXTCOLOR, "Heading2TextColor", COLOR)
    TM_PROP(3827, TMT, BODYTEXTCOLOR, "BodyTextColor", COLOR)

    //---- rendering enum properties (must be declared in TM_ENUM section above) ----
    TM_PROP(4001, TMT, BGTYPE, "BgType",           ENUM)        // basic drawing type for each part
    TM_PROP(4002, TMT, BORDERTYPE, "BorderType",       ENUM)        // type of border for BorderFill parts
    TM_PROP(4003, TMT, FILLTYPE, "FillType",         ENUM)        // fill shape for BorderFill parts
    TM_PROP(4004, TMT, SIZINGTYPE, "SizingType",       ENUM)        // how to size ImageFile parts
    TM_PROP(4005, TMT, HALIGN, "HAlign",           ENUM)        // horizontal alignment for TRUESIZE parts & glyphs
    TM_PROP(4006, TMT, CONTENTALIGNMENT, "ContentAlignment", ENUM)        // custom window prop: how text is aligned in caption
    TM_PROP(4007, TMT, VALIGN, "VAlign",           ENUM)        // horizontal alignment for TRUESIZE parts & glyphs
    TM_PROP(4008, TMT, OFFSETTYPE, "OffsetType",       ENUM)        // how window part should be placed
    TM_PROP(4009, TMT, ICONEFFECT, "IconEffect",       ENUM)        // type of effect to use with DrawThemeIcon
    TM_PROP(4010, TMT, TEXTSHADOWTYPE, "TextShadowType",   ENUM)        // type of shadow to draw with text
    TM_PROP(4011, TMT, IMAGELAYOUT, "ImageLayout",      ENUM)        // how multiple images are arranged (horz. or vert.)
    TM_PROP(4012, TMT, GLYPHTYPE, "GlyphType",             ENUM)   // controls type of glyph in imagefile objects
    TM_PROP(4013, TMT, IMAGESELECTTYPE, "ImageSelectType",       ENUM)   // controls when to select from IMAGEFILE1...IMAGEFILE5
    TM_PROP(4014, TMT, GLYPHFONTSIZINGTYPE, "GlyphFontSizingType",   ENUM)   // controls when to select a bigger/small glyph font size
    TM_PROP(4015, TMT, TRUESIZESCALINGTYPE, "TrueSizeScalingType",   ENUM)   // controls how TrueSize image is scaled
    
    //---- custom properties (used only by controls/shell) ----
    TM_PROP(5001, TMT, USERPICTURE, "UserPicture",           BOOL)
    TM_PROP(5002, TMT, DEFAULTPANESIZE, "DefaultPaneSize",       RECT)
    TM_PROP(5003, TMT, BLENDCOLOR, "BlendColor",           COLOR)
    // V4 props:
    TM_PROP(5004, TMT, CUSTOMSPLITRECT, "CustomSplitRect", POSITION) // probably POSITION
    TM_PROP(5005, TMT, ANIMATIONBUTTONRECT, "AnimationButtonRect", POSITION) // probably POSITION
    TM_PROP(5006, TMT, ANIMATIONDURATION, "AnimationDuration", INT)
    
    // ---- transition (v4 only) ----
    TM_PROP(6000, TMT, TRANSITIONDURATIONS, "TransitionDurations", INT) // TODO: check type
    
    // ---- scaled background (v4 only) ---
    TM_PROP(7001, TMT, SCALEDBACKGROUND, "ScaledBackground", INT) // TODO: check type
    
    // --- atlas (v4 only) ---
    TM_PROP(8000, TMT, ATLASIMAGE, "AtlasImage", FILENAME) // TODO: check type
    TM_PROP(8001, TMT, ATLASINPUTIMAGE, "AtlasInputImage", FILENAME) // TODO: check type
    TM_PROP(8002, TMT, ATLASRECT, "AtlasRect", POSITION) // TODO: check type

END_TM_PROPS()

//---------------------------------------------------------------------------------------
//   "Window" (i.e., non-client) Parts & States
//
//    these cannot be renumbered (part of uxtheme API)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(WINDOW)
    TM_PART(1, WP, CAPTION)
    TM_PART(2, WP, SMALLCAPTION)
    TM_PART(3, WP, MINCAPTION)
    TM_PART(4, WP, SMALLMINCAPTION)
    TM_PART(5, WP, MAXCAPTION)
    TM_PART(6, WP, SMALLMAXCAPTION)
    TM_PART(7, WP, FRAMELEFT)
    TM_PART(8, WP, FRAMERIGHT)
    TM_PART(9, WP, FRAMEBOTTOM)
    TM_PART(10, WP, SMALLFRAMELEFT)
    TM_PART(11, WP, SMALLFRAMERIGHT)
    TM_PART(12, WP, SMALLFRAMEBOTTOM)
    //---- window frame buttons ----
    TM_PART(13, WP, SYSBUTTON)
    TM_PART(14, WP, MDISYSBUTTON)
    TM_PART(15, WP, MINBUTTON)
    TM_PART(16, WP, MDIMINBUTTON)
    TM_PART(17, WP, MAXBUTTON)
    TM_PART(18, WP, CLOSEBUTTON)
    TM_PART(19, WP, SMALLCLOSEBUTTON)
    TM_PART(20, WP, MDICLOSEBUTTON)
    TM_PART(21, WP, RESTOREBUTTON)
    TM_PART(22, WP, MDIRESTOREBUTTON)
    TM_PART(23, WP, HELPBUTTON)
    TM_PART(24, WP, MDIHELPBUTTON)
    //---- scrollbars 
    TM_PART(25, WP, HORZSCROLL)
    TM_PART(26, WP, HORZTHUMB)
    TM_PART(27, WP, VERTSCROLL)
    TM_PART(28, WP, VERTTHUMB)
    //---- dialog ----
    TM_PART(29, WP, DIALOG)
    //---- hit-test templates ---
    TM_PART(30, WP, CAPTIONSIZINGTEMPLATE)
    TM_PART(31, WP, SMALLCAPTIONSIZINGTEMPLATE)
    TM_PART(32, WP, FRAMELEFTSIZINGTEMPLATE)
    TM_PART(33, WP, SMALLFRAMELEFTSIZINGTEMPLATE)
    TM_PART(34, WP, FRAMERIGHTSIZINGTEMPLATE)
    TM_PART(35, WP, SMALLFRAMERIGHTSIZINGTEMPLATE)
    TM_PART(36, WP, FRAMEBOTTOMSIZINGTEMPLATE)
    TM_PART(37, WP, SMALLFRAMEBOTTOMSIZINGTEMPLATE)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(FRAME)
    TM_STATE(1, FS, ACTIVE)
    TM_STATE(2, FS, INACTIVE)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CAPTION)
    TM_STATE(1, CS, ACTIVE)
    TM_STATE(2, CS, INACTIVE)
    TM_STATE(3, CS, DISABLED)
END_TM_PART_STATES()
    
BEGIN_TM_PART_STATES(MAXCAPTION)
    TM_STATE(1, MXCS, ACTIVE)
    TM_STATE(2, MXCS, INACTIVE)
    TM_STATE(3, MXCS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MINCAPTION)
    TM_STATE(1, MNCS, ACTIVE)
    TM_STATE(2, MNCS, INACTIVE)
    TM_STATE(3, MNCS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HORZSCROLL)
    TM_STATE(1, HSS, NORMAL)
    TM_STATE(2, HSS, HOT)
    TM_STATE(3, HSS, PUSHED)
    TM_STATE(4, HSS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HORZTHUMB)
    TM_STATE(1, HTS, NORMAL)
    TM_STATE(2, HTS, HOT)
    TM_STATE(3, HTS, PUSHED)
    TM_STATE(4, HTS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(VERTSCROLL)
    TM_STATE(1, VSS, NORMAL)
    TM_STATE(2, VSS, HOT)
    TM_STATE(3, VSS, PUSHED)
    TM_STATE(4, VSS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(VERTTHUMB)
    TM_STATE(1, VTS, NORMAL)
    TM_STATE(2, VTS, HOT)
    TM_STATE(3, VTS, PUSHED)
    TM_STATE(4, VTS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SYSBUTTON)
    TM_STATE(1, SBS, NORMAL)
    TM_STATE(2, SBS, HOT)
    TM_STATE(3, SBS, PUSHED)
    TM_STATE(4, SBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MINBUTTON)
    TM_STATE(1, MINBS, NORMAL)
    TM_STATE(2, MINBS, HOT)
    TM_STATE(3, MINBS, PUSHED)
    TM_STATE(4, MINBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MAXBUTTON)
    TM_STATE(1, MAXBS, NORMAL)
    TM_STATE(2, MAXBS, HOT)
    TM_STATE(3, MAXBS, PUSHED)
    TM_STATE(4, MAXBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(RESTOREBUTTON)
    TM_STATE(1, RBS, NORMAL)
    TM_STATE(2, RBS, HOT)
    TM_STATE(3, RBS, PUSHED)
    TM_STATE(4, RBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HELPBUTTON)
    TM_STATE(1, HBS, NORMAL)
    TM_STATE(2, HBS, HOT)
    TM_STATE(3, HBS, PUSHED)
    TM_STATE(4, HBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CLOSEBUTTON)
    TM_STATE(1, CBS, NORMAL)
    TM_STATE(2, CBS, HOT)
    TM_STATE(3, CBS, PUSHED)
    TM_STATE(4, CBS, DISABLED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Button" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(BUTTON)
    TM_PART(1, BP, PUSHBUTTON)
    TM_PART(2, BP, RADIOBUTTON)
    TM_PART(3, BP, CHECKBOX)
    TM_PART(4, BP, GROUPBOX)
    TM_PART(5, BP, USERBUTTON)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(PUSHBUTTON)
    TM_STATE(1, PBS, NORMAL)
    TM_STATE(2, PBS, HOT)
    TM_STATE(3, PBS, PRESSED)
    TM_STATE(4, PBS, DISABLED)
    TM_STATE(5, PBS, DEFAULTED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(RADIOBUTTON)
    TM_STATE(1, RBS, UNCHECKEDNORMAL)
    TM_STATE(2, RBS, UNCHECKEDHOT)
    TM_STATE(3, RBS, UNCHECKEDPRESSED)
    TM_STATE(4, RBS, UNCHECKEDDISABLED)
    TM_STATE(5, RBS, CHECKEDNORMAL)
    TM_STATE(6, RBS, CHECKEDHOT)
    TM_STATE(7, RBS, CHECKEDPRESSED)
    TM_STATE(8, RBS, CHECKEDDISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CHECKBOX)
    TM_STATE(1, CBS, UNCHECKEDNORMAL)
    TM_STATE(2, CBS, UNCHECKEDHOT)
    TM_STATE(3, CBS, UNCHECKEDPRESSED)
    TM_STATE(4, CBS, UNCHECKEDDISABLED)
    TM_STATE(5, CBS, CHECKEDNORMAL)
    TM_STATE(6, CBS, CHECKEDHOT)
    TM_STATE(7, CBS, CHECKEDPRESSED)
    TM_STATE(8, CBS, CHECKEDDISABLED)
    TM_STATE(9, CBS, MIXEDNORMAL)
    TM_STATE(10, CBS, MIXEDHOT)
    TM_STATE(11, CBS, MIXEDPRESSED)
    TM_STATE(12, CBS, MIXEDDISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(GROUPBOX)
    TM_STATE(1, GBS, NORMAL)
    TM_STATE(2, GBS, DISABLED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Rebar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(REBAR)
    TM_PART(1, RP, GRIPPER)
    TM_PART(2, RP, GRIPPERVERT)
    TM_PART(3, RP, BAND)
    TM_PART(4, RP, CHEVRON)
    TM_PART(5, RP, CHEVRONVERT)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(CHEVRON)
    TM_STATE(1, CHEVS, NORMAL)
    TM_STATE(2, CHEVS, HOT)
    TM_STATE(3, CHEVS, PRESSED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Toolbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TOOLBAR)
    TM_PART(1, TP, BUTTON)
    TM_PART(2, TP, DROPDOWNBUTTON)
    TM_PART(3, TP, SPLITBUTTON)
    TM_PART(4, TP, SPLITBUTTONDROPDOWN)
    TM_PART(5, TP, SEPARATOR)
    TM_PART(6, TP, SEPARATORVERT)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TOOLBAR)
    TM_STATE(1, TS, NORMAL)
    TM_STATE(2, TS, HOT)
    TM_STATE(3, TS, PRESSED)
    TM_STATE(4, TS, DISABLED)
    TM_STATE(5, TS, CHECKED)
    TM_STATE(6, TS, HOTCHECKED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Status" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(STATUS)
    TM_PART(1, SP, PANE)
    TM_PART(2, SP, GRIPPERPANE)
    TM_PART(3, SP, GRIPPER)
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Menu" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(MENU)
    TM_PART(1, MP, MENUITEM)
    TM_PART(2, MP, MENUDROPDOWN)
    TM_PART(3, MP, MENUBARITEM)
    TM_PART(4, MP, MENUBARDROPDOWN)
    TM_PART(5, MP, CHEVRON)
    TM_PART(6, MP, SEPARATOR)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MENU)
    TM_STATE(1, MS, NORMAL)
    TM_STATE(2, MS, SELECTED)
    TM_STATE(3, MS, DEMOTED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "ListView" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(LISTVIEW)
    TM_PART(1, LVP, LISTITEM)
    TM_PART(2, LVP, LISTGROUP)
    TM_PART(3, LVP, LISTDETAIL)
    TM_PART(4, LVP, LISTSORTEDDETAIL)
    TM_PART(5, LVP, EMPTYTEXT)
END_TM_CLASS_PARTS()

//
//  LIS prefix has been changed to LISS
//
#ifndef SCHEMA_VERIFY_VSSYM32

BEGIN_TM_PART_STATES(LISTITEM)
    TM_STATE(1, LIS, NORMAL)
    TM_STATE(2, LIS, HOT)
    TM_STATE(3, LIS, SELECTED)
    TM_STATE(4, LIS, DISABLED)
    TM_STATE(5, LIS, SELECTEDNOTFOCUS)
END_TM_PART_STATES()

#endif //#ifndef SCHEMA_VERIFY_VSSYM32

//---------------------------------------------------------------------------------------
//   "Header" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(HEADER)
    TM_PART(1, HP, HEADERITEM)
    TM_PART(2, HP, HEADERITEMLEFT)
    TM_PART(3, HP, HEADERITEMRIGHT)
    TM_PART(4, HP, HEADERSORTARROW)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(HEADERITEM)
    TM_STATE(1, HIS, NORMAL)
    TM_STATE(2, HIS, HOT)
    TM_STATE(3, HIS, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERITEMLEFT)
    TM_STATE(1, HILS, NORMAL)
    TM_STATE(2, HILS, HOT)
    TM_STATE(3, HILS, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERITEMRIGHT)
    TM_STATE(1, HIRS, NORMAL)
    TM_STATE(2, HIRS, HOT)
    TM_STATE(3, HIRS, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERSORTARROW)
    TM_STATE(1, HSAS, SORTEDUP)
    TM_STATE(2, HSAS, SORTEDDOWN)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Progress" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PROGRESS)
    TM_PART(1, PP, BAR)
    TM_PART(2, PP, BARVERT)
    TM_PART(3, PP, CHUNK)
    TM_PART(4, PP, CHUNKVERT)
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Tab" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TAB)
    TM_PART(1, TABP, TABITEM)
    TM_PART(2, TABP, TABITEMLEFTEDGE)
    TM_PART(3, TABP, TABITEMRIGHTEDGE)
    TM_PART(4, TABP, TABITEMBOTHEDGE)
    TM_PART(5, TABP, TOPTABITEM)
    TM_PART(6, TABP, TOPTABITEMLEFTEDGE)
    TM_PART(7, TABP, TOPTABITEMRIGHTEDGE)
    TM_PART(8, TABP, TOPTABITEMBOTHEDGE)
    TM_PART(9, TABP, PANE)
    TM_PART(10, TABP, BODY)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TABITEM)
    TM_STATE(1, TIS, NORMAL)
    TM_STATE(2, TIS, HOT)
    TM_STATE(3, TIS, SELECTED)
    TM_STATE(4, TIS, DISABLED)
    TM_STATE(5, TIS, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMLEFTEDGE)
    TM_STATE(1, TILES, NORMAL)
    TM_STATE(2, TILES, HOT)
    TM_STATE(3, TILES, SELECTED)
    TM_STATE(4, TILES, DISABLED)
    TM_STATE(5, TILES, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMRIGHTEDGE)
    TM_STATE(1, TIRES, NORMAL)
    TM_STATE(2, TIRES, HOT)
    TM_STATE(3, TIRES, SELECTED)
    TM_STATE(4, TIRES, DISABLED)
    TM_STATE(5, TIRES, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMBOTHEDGES)
    TM_STATE(1, TIBES, NORMAL)
    TM_STATE(2, TIBES, HOT)
    TM_STATE(3, TIBES, SELECTED)
    TM_STATE(4, TIBES, DISABLED)
    TM_STATE(5, TIBES, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEM)
    TM_STATE(1, TTIS, NORMAL)
    TM_STATE(2, TTIS, HOT)
    TM_STATE(3, TTIS, SELECTED)
    TM_STATE(4, TTIS, DISABLED)
    TM_STATE(5, TTIS, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMLEFTEDGE)
    TM_STATE(1, TTILES, NORMAL)
    TM_STATE(2, TTILES, HOT)
    TM_STATE(3, TTILES, SELECTED)
    TM_STATE(4, TTILES, DISABLED)
    TM_STATE(5, TTILES, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMRIGHTEDGE)
    TM_STATE(1, TTIRES, NORMAL)
    TM_STATE(2, TTIRES, HOT)
    TM_STATE(3, TTIRES, SELECTED)
    TM_STATE(4, TTIRES, DISABLED)
    TM_STATE(5, TTIRES, FOCUSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMBOTHEDGES)
    TM_STATE(1, TTIBES, NORMAL)
    TM_STATE(2, TTIBES, HOT)
    TM_STATE(3, TTIBES, SELECTED)
    TM_STATE(4, TTIBES, DISABLED)
    TM_STATE(5, TTIBES, FOCUSED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Trackbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TRACKBAR)
    TM_PART(1, TKP, TRACK)
    TM_PART(2, TKP, TRACKVERT)
    TM_PART(3, TKP, THUMB)
    TM_PART(4, TKP, THUMBBOTTOM)
    TM_PART(5, TKP, THUMBTOP)
    TM_PART(6, TKP, THUMBVERT)
    TM_PART(7, TKP, THUMBLEFT)
    TM_PART(8, TKP, THUMBRIGHT)
    TM_PART(9, TKP, TICS)
    TM_PART(10, TKP, TICSVERT)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TRACKBAR)
    TM_STATE(1, TKS, NORMAL)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRACK)
    TM_STATE(1, TRS, NORMAL)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRACKVERT)
    TM_STATE(1, TRVS, NORMAL)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMB)
    TM_STATE(1, TUS, NORMAL)
    TM_STATE(2, TUS, HOT)
    TM_STATE(3, TUS, PRESSED)
    TM_STATE(4, TUS, FOCUSED)
    TM_STATE(5, TUS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBBOTTOM)
    TM_STATE(1, TUBS, NORMAL)
    TM_STATE(2, TUBS, HOT)
    TM_STATE(3, TUBS, PRESSED)
    TM_STATE(4, TUBS, FOCUSED)
    TM_STATE(5, TUBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBTOP)
    TM_STATE(1, TUTS, NORMAL)
    TM_STATE(2, TUTS, HOT)
    TM_STATE(3, TUTS, PRESSED)
    TM_STATE(4, TUTS, FOCUSED)
    TM_STATE(5, TUTS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBVERT)
    TM_STATE(1, TUVS, NORMAL)
    TM_STATE(2, TUVS, HOT)
    TM_STATE(3, TUVS, PRESSED)
    TM_STATE(4, TUVS, FOCUSED)
    TM_STATE(5, TUVS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBLEFT)
    TM_STATE(1, TUVLS, NORMAL)
    TM_STATE(2, TUVLS, HOT)
    TM_STATE(3, TUVLS, PRESSED)
    TM_STATE(4, TUVLS, FOCUSED)
    TM_STATE(5, TUVLS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBRIGHT)
    TM_STATE(1, TUVRS, NORMAL)
    TM_STATE(2, TUVRS, HOT)
    TM_STATE(3, TUVRS, PRESSED)
    TM_STATE(4, TUVRS, FOCUSED)
    TM_STATE(5, TUVRS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TICS)
    TM_STATE(1, TSS, NORMAL)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TICSVERT)
    TM_STATE(1, TSVS, NORMAL)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Tooltips" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TOOLTIP)
    TM_PART(1, TTP, STANDARD)
    TM_PART(2, TTP, STANDARDTITLE)
    TM_PART(3, TTP, BALLOON)
    TM_PART(4, TTP, BALLOONTITLE)
    TM_PART(5, TTP, CLOSE)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(CLOSE)
    TM_STATE(1, TTCS, NORMAL)
    TM_STATE(2, TTCS, HOT)
    TM_STATE(3, TTCS, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(STANDARD)
    TM_STATE(1, TTSS, NORMAL)
    TM_STATE(2, TTSS, LINK)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(BALLOON)
    TM_STATE(1, TTBS, NORMAL)
    TM_STATE(2, TTBS, LINK)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "TreeView" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TREEVIEW)
    TM_PART(1, TVP, TREEITEM)
    TM_PART(2, TVP, GLYPH)
    TM_PART(3, TVP, BRANCH)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TREEITEM)
    TM_STATE(1, TREIS, NORMAL)
    TM_STATE(2, TREIS, HOT)
    TM_STATE(3, TREIS, SELECTED)
    TM_STATE(4, TREIS, DISABLED)
    TM_STATE(5, TREIS, SELECTEDNOTFOCUS)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(GLYPH)
    TM_STATE(1, GLPS, CLOSED)
    TM_STATE(2, GLPS, OPENED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Spin" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SPIN)
    TM_PART(1, SPNP, UP)
    TM_PART(2, SPNP, DOWN)
    TM_PART(3, SPNP, UPHORZ)
    TM_PART(4, SPNP, DOWNHORZ)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(UP)
    TM_STATE(1, UPS, NORMAL)
    TM_STATE(2, UPS, HOT)
    TM_STATE(3, UPS, PRESSED)
    TM_STATE(4, UPS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DOWN)
    TM_STATE(1, DNS, NORMAL)
    TM_STATE(2, DNS, HOT)
    TM_STATE(3, DNS, PRESSED)
    TM_STATE(4, DNS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(UPHORZ)
    TM_STATE(1, UPHZS, NORMAL)
    TM_STATE(2, UPHZS, HOT)
    TM_STATE(3, UPHZS, PRESSED)
    TM_STATE(4, UPHZS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DOWNHORZ)
    TM_STATE(1, DNHZS, NORMAL)
    TM_STATE(2, DNHZS, HOT)
    TM_STATE(3, DNHZS, PRESSED)
    TM_STATE(4, DNHZS, DISABLED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Page" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PAGE)
    TM_PART(1, PGRP, UP)
    TM_PART(2, PGRP, DOWN)
    TM_PART(3, PGRP, UPHORZ)
    TM_PART(4, PGRP, DOWNHORZ)
END_TM_CLASS_PARTS()

//--- Pager uses same states as Spin ---

//---------------------------------------------------------------------------------------
//   "Scrollbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SCROLLBAR)
    TM_PART(1, SBP, ARROWBTN)
    TM_PART(2, SBP, THUMBBTNHORZ)
    TM_PART(3, SBP, THUMBBTNVERT)
    TM_PART(4, SBP, LOWERTRACKHORZ)
    TM_PART(5, SBP, UPPERTRACKHORZ)
    TM_PART(6, SBP, LOWERTRACKVERT)
    TM_PART(7, SBP, UPPERTRACKVERT)
    TM_PART(8, SBP, GRIPPERHORZ)
    TM_PART(9, SBP, GRIPPERVERT)
    TM_PART(10, SBP, SIZEBOX)
END_TM_CLASS_PARTS()



BEGIN_TM_PART_STATES(ARROWBTN)
    TM_STATE(1, ABS, UPNORMAL)
    TM_STATE(2, ABS, UPHOT)
    TM_STATE(3, ABS, UPPRESSED)
    TM_STATE(4, ABS, UPDISABLED)
    TM_STATE(5, ABS, DOWNNORMAL)
    TM_STATE(6, ABS, DOWNHOT)
    TM_STATE(7, ABS, DOWNPRESSED)
    TM_STATE(8, ABS, DOWNDISABLED)
    TM_STATE(9, ABS, LEFTNORMAL)
    TM_STATE(10, ABS, LEFTHOT)
    TM_STATE(11, ABS, LEFTPRESSED)
    TM_STATE(12, ABS, LEFTDISABLED)
    TM_STATE(13, ABS, RIGHTNORMAL)
    TM_STATE(14, ABS, RIGHTHOT)
    TM_STATE(15, ABS, RIGHTPRESSED)
    TM_STATE(16, ABS, RIGHTDISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SCROLLBAR)
    TM_STATE(1, SCRBS, NORMAL)
    TM_STATE(2, SCRBS, HOT)
    TM_STATE(3, SCRBS, PRESSED)
    TM_STATE(4, SCRBS, DISABLED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SIZEBOX)
    TM_STATE(1, SZB, RIGHTALIGN)
    TM_STATE(2, SZB, LEFTALIGN)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Edit" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(EDIT)
    TM_PART(1, EP, EDITTEXT)
    TM_PART(2, EP, CARET)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(EDITTEXT)
    TM_STATE(1, ETS, NORMAL)
    TM_STATE(2, ETS, HOT)
    TM_STATE(3, ETS, SELECTED)
    TM_STATE(4, ETS, DISABLED)
    TM_STATE(5, ETS, FOCUSED)
    TM_STATE(6, ETS, READONLY)
    TM_STATE(7, ETS, ASSIST)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "ComboBox" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(COMBOBOX)
    TM_PART(1, CP, DROPDOWNBUTTON)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(COMBOBOX)
    TM_STATE(1, CBXS, NORMAL)
    TM_STATE(2, CBXS, HOT)
    TM_STATE(3, CBXS, PRESSED)
    TM_STATE(4, CBXS, DISABLED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Taskbar Clock" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(CLOCK)
    TM_PART(1, CLP, TIME)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(CLOCK)
    TM_STATE(1, CLS, NORMAL)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Tray Notify" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TRAYNOTIFY)
    TM_PART(1, TNP, BACKGROUND)
    TM_PART(2, TNP, ANIMBACKGROUND)
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBAR)
    TM_PART(1, TBP, BACKGROUNDBOTTOM)
    TM_PART(2, TBP, BACKGROUNDRIGHT)
    TM_PART(3, TBP, BACKGROUNDTOP)
    TM_PART(4, TBP, BACKGROUNDLEFT)
    TM_PART(5, TBP, SIZINGBARBOTTOM)
    TM_PART(6, TBP, SIZINGBARRIGHT)
    TM_PART(7, TBP, SIZINGBARTOP)
    TM_PART(8, TBP, SIZINGBARLEFT)
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBand" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBAND)
    TM_PART(1, TDP, GROUPCOUNT)
    TM_PART(2, TDP, FLASHBUTTON)
    TM_PART(3, TDP, FLASHBUTTONGROUPMENU)
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "StartPanel" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(STARTPANEL)
    TM_PART(1, SPP, USERPANE)
    TM_PART(2, SPP, MOREPROGRAMS)
    TM_PART(3, SPP, MOREPROGRAMSARROW)
    TM_PART(4, SPP, PROGLIST)
    TM_PART(5, SPP, PROGLISTSEPARATOR)
    TM_PART(6, SPP, PLACESLIST)
    TM_PART(7, SPP, PLACESLISTSEPARATOR)
    TM_PART(8, SPP, LOGOFF)
    TM_PART(9, SPP, LOGOFFBUTTONS)
    TM_PART(10, SPP, USERPICTURE)
    TM_PART(11, SPP, PREVIEW)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MOREPROGRAMSARROW)
    TM_STATE(1, SPS, NORMAL)
    TM_STATE(2, SPS, HOT)
    TM_STATE(3, SPS, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(LOGOFFBUTTONS)
    TM_STATE(1, SPLS, NORMAL)
    TM_STATE(2, SPLS, HOT)
    TM_STATE(3, SPLS, PRESSED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "ExplorerBar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(EXPLORERBAR)
    TM_PART(1, EBP, HEADERBACKGROUND)
    TM_PART(2, EBP, HEADERCLOSE)
    TM_PART(3, EBP, HEADERPIN)
    TM_PART(4, EBP, IEBARMENU)
    TM_PART(5, EBP, NORMALGROUPBACKGROUND)
    TM_PART(6, EBP, NORMALGROUPCOLLAPSE)
    TM_PART(7, EBP, NORMALGROUPEXPAND)
    TM_PART(8, EBP, NORMALGROUPHEAD)
    TM_PART(9, EBP, SPECIALGROUPBACKGROUND)
    TM_PART(10, EBP, SPECIALGROUPCOLLAPSE)
    TM_PART(11, EBP, SPECIALGROUPEXPAND)
    TM_PART(12, EBP, SPECIALGROUPHEAD)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(HEADERCLOSE)
    TM_STATE(1, EBHC, NORMAL)
    TM_STATE(2, EBHC, HOT)
    TM_STATE(3, EBHC, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERPIN)
    TM_STATE(1, EBHP, NORMAL)
    TM_STATE(2, EBHP, HOT)
    TM_STATE(3, EBHP, PRESSED)
    TM_STATE(4, EBHP, SELECTEDNORMAL)
    TM_STATE(5, EBHP, SELECTEDHOT)
    TM_STATE(6, EBHP, SELECTEDPRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(IEBARMENU)
    TM_STATE(1, EBM, NORMAL)
    TM_STATE(2, EBM, HOT)
    TM_STATE(3, EBM, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(NORMALGROUPCOLLAPSE)
    TM_STATE(1, EBNGC, NORMAL)
    TM_STATE(2, EBNGC, HOT)
    TM_STATE(3, EBNGC, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(NORMALGROUPEXPAND)
    TM_STATE(1, EBNGE, NORMAL)
    TM_STATE(2, EBNGE, HOT)
    TM_STATE(3, EBNGE, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SPECIALGROUPCOLLAPSE)
    TM_STATE(1, EBSGC, NORMAL)
    TM_STATE(2, EBSGC, HOT)
    TM_STATE(3, EBSGC, PRESSED)
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SPECIALGROUPEXPAND)
    TM_STATE(1, EBSGE, NORMAL)
    TM_STATE(2, EBSGE, HOT)
    TM_STATE(3, EBSGE, PRESSED)
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "TaskBand" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(MENUBAND)
    TM_PART(1, MDP, NEWAPPBUTTON)
    TM_PART(2, MDP, SEPERATOR)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MENUBAND)
    TM_STATE(1, MDS, NORMAL)
    TM_STATE(2, MDS, HOT)
    TM_STATE(3, MDS, PRESSED)
    TM_STATE(4, MDS, DISABLED)
    TM_STATE(5, MDS, CHECKED)
    TM_STATE(6, MDS, HOTCHECKED)
END_TM_PART_STATES()
//---------------------------------------------------------------------------
END_TM_SCHEMA(ThemeMgrSchema)
//---------------------------------------------------------------------------
} // namespace Restyle
#endif      // TMSCHEMA_H