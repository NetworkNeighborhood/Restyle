//#pragma once

//-----------------------------------------------------------------
//   TmSchema.h - Theme Manager schema (properties, parts, etc)
//-----------------------------------------------------------------
//   Note: this file is normally #include-ed twice a single .cpp 
//         file.  The 2nd time, SCHEME_STRINGS should be defined.  
//         This allows the enums and strings to be kept in a 
//         single logical table and ensure they stay in sync with
//         each other.
//-----------------------------------------------------------------

//-----------------------------------------------------------------
#if (defined(SCHEMA_STRINGS)) || (! defined(TMSCHEMA_H))
//-----------------------------------------------------------------
#define TMSCHEMA_H
//-----------------------------------------------------------------
#include "restyle_SchemaDef.h"

namespace Restyle
{

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

// Recent addition. Private. Referred to in msstyleEditor as "HIGHCONTRASTTYPE"
BEGIN_TM_ENUM(HIGHCONTRASTCOLOR, "HighContrastColor")
    TM_ENUM(0, HCC, COLOR_ACTIVECAPTION, "Color_ActiveCaption")
    TM_ENUM(1, HCC, COLOR_CAPTIONTEXT, "Color_CaptionText")
    TM_ENUM(2, HCC, COLOR_BTNFACE, "Color_BtnFace")
    TM_ENUM(3, HCC, COLOR_BTNTEXT, "Color_BtnText")
    TM_ENUM(4, HCC, COLOR_DESKTOP, "Color_Desktop")
    TM_ENUM(5, HCC, COLOR_GRAYTEXT, "Color_GrayText")
    TM_ENUM(6, HCC, COLOR_HOTLIGHT, "Color_HotLight")
    TM_ENUM(7, HCC, COLOR_INACTIVECAPTION, "Color_InactiveCaption")
    TM_ENUM(8, HCC, COLOR_INACTIVECAPTIONTEXT, "Color_InactiveCaptionText")
    TM_ENUM(9, HCC, COLOR_HIGHLIGHT, "Color_Highlight")
    TM_ENUM(10, HCC, COLOR_HIGHLIGHTTEXT, "Color_HighlightText")
    TM_ENUM(11, HCC, COLOR_WINDOW, "Color_Window")
    TM_ENUM(12, HCC, COLOR_WINDOWTEXT, "Color_WindowText")
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
    TM_PROP(1206, TMT, SMCAPTIONBARWIDTH, "SmCaptionBarWidth",    SIZE)
    TM_PROP(1207, TMT, SMCAPTIONBARHEIGHT, "SmCaptionBarHeight",   SIZE)
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
    TM_PROP(1619, TMT, BTNTEXT, "BtnText",           COLOR)
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
#if 0

    TM_PROP(3007, TMT, STOCKIMAGEFILE, "StockImageFile",    FILENAME)   // These are the only images that you can call GetThemeBitmap on

#endif //#ifndef SCHEMA_VERIFY_VSSYM32

    TM_PROP(3008, TMT, GLYPHIMAGEFILE, "GlyphImageFile",    FILENAME)   // the filename for the glyph image
    
    // V4 props:
    TM_PROP(3009, TMT, IMAGEFILE6, "ImageFile6", FILENAME)
    TM_PROP(3010, TMT, IMAGEFILE7, "ImageFile7", FILENAME)

    //---- rendering STRING properties ----
    TM_PROP(3201, TMT, TEXT, "Text",              STRING)
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
    TM_PART(1, WP, CAPTION, "Caption")
    TM_PART(2, WP, SMALLCAPTION, "SmallCaption")
    TM_PART(3, WP, MINCAPTION, "MinCaption")
    TM_PART(4, WP, SMALLMINCAPTION, "SmallMinCaption")
    TM_PART(5, WP, MAXCAPTION, "MaxCaption")
    TM_PART(6, WP, SMALLMAXCAPTION, "SmallMaxCaption")
    TM_PART(7, WP, FRAMELEFT, "FrameLeft")
    TM_PART(8, WP, FRAMERIGHT, "FrameRight")
    TM_PART(9, WP, FRAMEBOTTOM, "FrameBottom")
    TM_PART(10, WP, SMALLFRAMELEFT, "SmallFrameLeft")
    TM_PART(11, WP, SMALLFRAMERIGHT, "SmallFrameRight")
    TM_PART(12, WP, SMALLFRAMEBOTTOM, "SmallFrameBottom")
    //---- window frame buttons ----
    TM_PART(13, WP, SYSBUTTON, "SysButton")
    TM_PART(14, WP, MDISYSBUTTON, "MdiSysButton")
    TM_PART(15, WP, MINBUTTON, "MinButton")
    TM_PART(16, WP, MDIMINBUTTON, "MdiMinButton")
    TM_PART(17, WP, MAXBUTTON, "MaxButton")
    TM_PART(18, WP, CLOSEBUTTON, "CloseButton")
    TM_PART(19, WP, SMALLCLOSEBUTTON, "SmallCloseButton")
    TM_PART(20, WP, MDICLOSEBUTTON, "MdiCloseButton")
    TM_PART(21, WP, RESTOREBUTTON, "RestoreButton")
    TM_PART(22, WP, MDIRESTOREBUTTON, "MdiRestoreButton")
    TM_PART(23, WP, HELPBUTTON, "HelpButton")
    TM_PART(24, WP, MDIHELPBUTTON, "MdiHelpButton")
    //---- scrollbars 
    TM_PART(25, WP, HORZSCROLL, "HorzScroll")
    TM_PART(26, WP, HORZTHUMB, "HorzThumb")
    TM_PART(27, WP, VERTSCROLL, "VertScroll")
    TM_PART(28, WP, VERTTHUMB, "VertThumb")
    //---- dialog ----
    TM_PART(29, WP, DIALOG, "Dialog")
    //---- hit-test templates ---
    TM_PART(30, WP, CAPTIONSIZINGTEMPLATE, "CaptionSizingTemplate")
    TM_PART(31, WP, SMALLCAPTIONSIZINGTEMPLATE, "SmallCaptionSizingTemplate")
    TM_PART(32, WP, FRAMELEFTSIZINGTEMPLATE, "FrameLeftSizingTemplate")
    TM_PART(33, WP, SMALLFRAMELEFTSIZINGTEMPLATE, "SmallFrameLeftSizingTemplate")
    TM_PART(34, WP, FRAMERIGHTSIZINGTEMPLATE, "FrameRightSizingTemplate")
    TM_PART(35, WP, SMALLFRAMERIGHTSIZINGTEMPLATE, "SmallFrameRightSizingTemplate")
    TM_PART(36, WP, FRAMEBOTTOMSIZINGTEMPLATE, "FrameBottomSizingTemplate")
    TM_PART(37, WP, SMALLFRAMEBOTTOMSIZINGTEMPLATE, "SmallFrameBottomSizingTemplate")
    // V4:
    TM_PART(38, WP, FRAME, "Frame")
    TM_PART(39, WP, BORDER, "Border")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(FRAME)
    TM_STATE(1, FS, ACTIVE, "Active")
    TM_STATE(2, FS, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CAPTION)
    TM_STATE(1, CS, ACTIVE, "Active")
    TM_STATE(2, CS, INACTIVE, "Inactive")
    TM_STATE(3, CS, DISABLED, "Disabled")
END_TM_PART_STATES()
    
BEGIN_TM_PART_STATES(MAXCAPTION)
    TM_STATE(1, MXCS, ACTIVE, "Active")
    TM_STATE(2, MXCS, INACTIVE, "Inactive")
    TM_STATE(3, MXCS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MINCAPTION)
    TM_STATE(1, MNCS, ACTIVE, "Active")
    TM_STATE(2, MNCS, INACTIVE, "Inactive")
    TM_STATE(3, MNCS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HORZSCROLL)
    TM_STATE(1, HSS, NORMAL, "Normal")
    TM_STATE(2, HSS, HOT, "Hot")
    TM_STATE(3, HSS, PUSHED, "Pushed")
    TM_STATE(4, HSS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HORZTHUMB)
    TM_STATE(1, HTS, NORMAL, "Normal")
    TM_STATE(2, HTS, HOT, "Hot")
    TM_STATE(3, HTS, PUSHED, "Pushed")
    TM_STATE(4, HTS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(VERTSCROLL)
    TM_STATE(1, VSS, NORMAL, "Normal")
    TM_STATE(2, VSS, HOT, "Hot")
    TM_STATE(3, VSS, PUSHED, "Pushed")
    TM_STATE(4, VSS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(VERTTHUMB)
    TM_STATE(1, VTS, NORMAL, "Normal")
    TM_STATE(2, VTS, HOT, "Hot")
    TM_STATE(3, VTS, PUSHED, "Pushed")
    TM_STATE(4, VTS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SYSBUTTON)
    TM_STATE(1, SBS, NORMAL, "Normal")
    TM_STATE(2, SBS, HOT, "Hot")
    TM_STATE(3, SBS, PUSHED, "Pushed")
    TM_STATE(4, SBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MINBUTTON)
    TM_STATE(1, MINBS, NORMAL, "Normal")
    TM_STATE(2, MINBS, HOT, "Hot")
    TM_STATE(3, MINBS, PUSHED, "Pushed")
    TM_STATE(4, MINBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MAXBUTTON)
    TM_STATE(1, MAXBS, NORMAL, "Normal")
    TM_STATE(2, MAXBS, HOT, "Hot")
    TM_STATE(3, MAXBS, PUSHED, "Pushed")
    TM_STATE(4, MAXBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(RESTOREBUTTON)
    TM_STATE(1, RBS, NORMAL, "Normal")
    TM_STATE(2, RBS, HOT, "Hot")
    TM_STATE(3, RBS, PUSHED, "Pushed")
    TM_STATE(4, RBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HELPBUTTON)
    TM_STATE(1, HBS, NORMAL, "Normal")
    TM_STATE(2, HBS, HOT, "Hot")
    TM_STATE(3, HBS, PUSHED, "Pushed")
    TM_STATE(4, HBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CLOSEBUTTON)
    TM_STATE(1, CBS, NORMAL, "Normal")
    TM_STATE(2, CBS, HOT, "Hot")
    TM_STATE(3, CBS, PUSHED, "Pushed")
    TM_STATE(4, CBS, DISABLED, "Disabled")
END_TM_PART_STATES()

// --- V4 ---

BEGIN_TM_PART_STATES(SMALLCLOSEBUTTON)
    TM_STATE(1, SCBS, NORMAL, "Normal")
    TM_STATE(2, SCBS, HOT, "Hot")
    TM_STATE(3, SCBS, PUSHED, "Pushed")
    TM_STATE(4, SCBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(FRAMEBOTTOM)
    TM_STATE(1, FRB, ACTIVE, "Active")
    TM_STATE(2, FRB, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(FRAMELEFT)
    TM_STATE(1, FRL, ACTIVE, "Active")
    TM_STATE(2, FRL, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(FRAMERIGHT)
    TM_STATE(1, FRR, ACTIVE, "Active")
    TM_STATE(2, FRR, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SMALLCAPTION)
    TM_STATE(1, SCS, ACTIVE, "Active")
    TM_STATE(2, SCS, INACTIVE, "Inactive")
    TM_STATE(3, SCS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SMALLFRAMEBOTTOM)
    TM_STATE(1, SFRB, ACTIVE, "Active")
    TM_STATE(2, SFRB, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SMALLFRAMELEFT)
    TM_STATE(1, SFRL, ACTIVE, "Active")
    TM_STATE(2, SFRL, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SMALLFRAMERIGHT)
    TM_STATE(1, SFRR, ACTIVE, "Active")
    TM_STATE(2, SFRR, INACTIVE, "Inactive")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MDICLOSEBUTTON)
    TM_STATE(1, MDCL, NORMAL, "Normal")
    TM_STATE(2, MDCL, HOT, "Hot")
    TM_STATE(3, MDCL, PUSHED, "Pushed")
    TM_STATE(4, MDCL, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MDIMINBUTTON)
    TM_STATE(1, MDMI, NORMAL, "Normal")
    TM_STATE(2, MDMI, HOT, "Hot")
    TM_STATE(3, MDMI, PUSHED, "Pushed")
    TM_STATE(4, MDMI, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MDIRESTOREBUTTON)
    TM_STATE(1, MDRE, NORMAL, "Normal")
    TM_STATE(2, MDRE, HOT, "Hot")
    TM_STATE(3, MDRE, PUSHED, "Pushed")
    TM_STATE(4, MDRE, DISABLED, "Disabled")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Button" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(BUTTON)
    TM_PART(1, BP, PUSHBUTTON, "PushButton")
    TM_PART(2, BP, RADIOBUTTON, "RadioButton")
    TM_PART(3, BP, CHECKBOX, "CheckBox")
    TM_PART(4, BP, GROUPBOX, "GroupBox")
    TM_PART(5, BP, USERBUTTON, "UserButton")
    // V4:
    TM_PART(6, BP, COMMANDLINK, "CommandLink")
    TM_PART(7, BP, COMMANDLINKGLYPH, "CommandLinkGlyph")
    // Added sometime after 14361 but before 19041.546
    TM_PART(8, BP, RADIOBUTTON_HCDISABLED, "RadioButton_HCDisabled")
    TM_PART(9, BP, CHECKBOX_HCDISABLED, "CheckBox_HCDisabled")
    TM_PART(10, BP, GROUPBOX_HCDISABLED, "GroupBox_HCDisabled")
    TM_PART(11, BP, PUSHBUTTONDROPDOWN, "PushButtonDropdown")
END_TM_CLASS_PARTS()
    
BEGIN_TM_PART_STATES(PUSHBUTTON)
    TM_STATE(1, PBS, NORMAL, "Normal")
    TM_STATE(2, PBS, HOT, "Hot")
    TM_STATE(3, PBS, PRESSED, "Pressed")
    TM_STATE(4, PBS, DISABLED, "Disabled")
    TM_STATE(5, PBS, DEFAULTED, "Defaulted")
    TM_STATE(6, PBS, DEFAULTED_ANIMATING, "Defaulted_Animating")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(RADIOBUTTON)
    TM_STATE(1, RBS, UNCHECKEDNORMAL, "UncheckedNormal")
    TM_STATE(2, RBS, UNCHECKEDHOT, "UncheckedHot")
    TM_STATE(3, RBS, UNCHECKEDPRESSED, "UncheckedPressed")
    TM_STATE(4, RBS, UNCHECKEDDISABLED, "UncheckedDisabled")
    TM_STATE(5, RBS, CHECKEDNORMAL, "CheckedNormal")
    TM_STATE(6, RBS, CHECKEDHOT, "CheckedHot")
    TM_STATE(7, RBS, CHECKEDPRESSED, "CheckedPressed")
    TM_STATE(8, RBS, CHECKEDDISABLED, "CheckedDisabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CHECKBOX)
    TM_STATE(1, CBS, UNCHECKEDNORMAL, "UncheckedNormal")
    TM_STATE(2, CBS, UNCHECKEDHOT, "UncheckedHot")
    TM_STATE(3, CBS, UNCHECKEDPRESSED, "UncheckedPressed")
    TM_STATE(4, CBS, UNCHECKEDDISABLED, "UncheckedDisabled")
    TM_STATE(5, CBS, CHECKEDNORMAL, "CheckedNormal")
    TM_STATE(6, CBS, CHECKEDHOT, "CheckedHot")
    TM_STATE(7, CBS, CHECKEDPRESSED, "CheckedPressed")
    TM_STATE(8, CBS, CHECKEDDISABLED, "CheckedDisabled")
    TM_STATE(9, CBS, MIXEDNORMAL, "MixedNormal")
    TM_STATE(10, CBS, MIXEDHOT, "MixedHot")
    TM_STATE(11, CBS, MIXEDPRESSED, "MixedPressed")
    TM_STATE(12, CBS, MIXEDDISABLED, "MixedDisabled")
    TM_STATE(13, CBS, IMPLICITNORMAL, "ImplicitNormal")
    TM_STATE(14, CBS, IMPLICITHOT, "ImplicitHot")
    TM_STATE(15, CBS, IMPLICITPRESSED, "ImplicitPressed")
    TM_STATE(16, CBS, IMPLICITDISABLED, "ImplicitDisabled")
    TM_STATE(17, CBS, EXCLUDEDNORMAL, "ExcludedNormal")
    TM_STATE(18, CBS, EXCLUDEDHOT, "ExcludedHot")
    TM_STATE(19, CBS, EXCLUDEDPRESSED, "ExcludedPressed")
    TM_STATE(20, CBS, EXCLUDEDDISABLED, "ExcludedDisabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(GROUPBOX)
    TM_STATE(1, GBS, NORMAL, "Normal")
    TM_STATE(2, GBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(COMMANDLINK)
    TM_STATE(1, CMDLS, NORMAL, "Normal")
    TM_STATE(2, CMDLS, HOT, "Hot")
    TM_STATE(3, CMDLS, PRESSED, "Pressed")
    TM_STATE(4, CMDLS, DISABLED, "Disabled")
    TM_STATE(5, CMDLS, DEFAULTED, "Defaulted")
    TM_STATE(6, CMDLS, DEFAULTED_ANIMATING, "Defaulted_Animating")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(COMMANDLINKGLPYH)
    TM_STATE(1, CMDLGS, NORMAL, "Normal")
    TM_STATE(2, CMDLGS, HOT, "Hot")
    TM_STATE(3, CMDLGS, PRESSED, "Pressed")
    TM_STATE(4, CMDLGS, DISABLED, "Disabled")
    TM_STATE(5, CMDLGS, DEFAULTED, "Defaulted")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Rebar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(REBAR)
    TM_PART(1, RP, GRIPPER, "Gripper")
    TM_PART(2, RP, GRIPPERVERT, "GripperVert")
    TM_PART(3, RP, BAND, "Band")
    TM_PART(4, RP, CHEVRON, "Chevron")
    TM_PART(5, RP, CHEVRONVERT, "ChevronVert")
    // V4:
    TM_PART(6, RP, BACKGROUND, "Background")
    TM_PART(7, RP, SPLITTER, "Splitter")
    TM_PART(8, RP, SPLITTERVERT, "SplitterVert")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(CHEVRON)
    TM_STATE(1, CHEVS, NORMAL, "Normal")
    TM_STATE(2, CHEVS, HOT, "Hot")
    TM_STATE(3, CHEVS, PRESSED, "Pressed")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Toolbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TOOLBAR)
    TM_PART(1, TP, BUTTON, "Button")
    TM_PART(2, TP, DROPDOWNBUTTON, "DropdownButton")
    TM_PART(3, TP, SPLITBUTTON, "SplitButton")
    TM_PART(4, TP, SPLITBUTTONDROPDOWN, "SplitButtonDropdown")
    TM_PART(5, TP, SEPARATOR, "Separator")
    TM_PART(6, TP, SEPARATORVERT, "SeparatorVert")
    // V4;
    TM_PART(7, TP, DROPDOWNBUTTONGLYPH, "DropdownButtonGlyph")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TOOLBAR)
    TM_STATE(1, TS, NORMAL, "Normal")
    TM_STATE(2, TS, HOT, "Hot")
    TM_STATE(3, TS, PRESSED, "Pressed")
    TM_STATE(4, TS, DISABLED, "Disabled")
    TM_STATE(5, TS, CHECKED, "Checked")
    TM_STATE(6, TS, HOTCHECKED, "HotChecked")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Status" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(STATUS)
    TM_PART(1, SP, PANE, "Pane")
    TM_PART(2, SP, GRIPPERPANE, "GripperPane")
    TM_PART(3, SP, GRIPPER, "Gripper")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MENU)
    TM_STATE(1, MS, NORMAL, "Normal")
    TM_STATE(2, MS, SELECTED, "Selected")
    TM_STATE(3, MS, DEMOTED, "Demoted")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "ListView" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(LISTVIEW)
    TM_PART(1, LVP, LISTITEM, "ListItem")
    TM_PART(2, LVP, LISTGROUP, "ListGroup")
    TM_PART(3, LVP, LISTDETAIL, "ListDetail")
    TM_PART(4, LVP, LISTSORTEDDETAIL, "ListSortedDetail")
    TM_PART(5, LVP, EMPTYTEXT, "EmptyText")
    // V4:
    TM_PART(6, LVP, GROUPHEADER, "GroupHeader")
    TM_PART(7, LVP, GROUPHEADERLINE, "GroupHeaderLine")
    TM_PART(8, LVP, EXPANDBUTTON, "ExpandButton")
    TM_PART(9, LVP, COLLAPSEBUTTON, "CollapseButton")
    TM_PART(10, LVP, COLUMNDETAIL, "ColumnDetail")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(COLLAPSEBUTTON)
    TM_STATE(1, LVCB, NORMAL, "Normal")
    TM_STATE(2, LVCB, HOVER, "Hover")
    TM_STATE(3, LVCB, PUSHED, "Pushed")
END_TM_PART_STATES()

    //
//  LIS prefix has been changed to LISS
//
#ifndef SCHEMA_VERIFY_VSSYM32

BEGIN_TM_PART_STATES(LISTITEM)
    TM_STATE(1, LIS, NORMAL, "Normal")
    TM_STATE(2, LIS, HOT, "Hot")
    TM_STATE(3, LIS, SELECTED, "Selected")
    TM_STATE(4, LIS, DISABLED, "Disabled")
    TM_STATE(5, LIS, SELECTEDNOTFOCUS, "SelectedNotFocus")
END_TM_PART_STATES()

#endif //#ifndef SCHEMA_VERIFY_VSSYM32

//---------------------------------------------------------------------------------------
//   "Header" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(HEADER)
    TM_PART(1, HP, HEADERITEM, "HeaderItem")
    TM_PART(2, HP, HEADERITEMLEFT, "HeaderItemLeft")
    TM_PART(3, HP, HEADERITEMRIGHT, "HeaderItemRight")
    TM_PART(4, HP, HEADERSORTARROW, "HeaderSortArrow")
    // V4:
    TM_PART(5, HP, HEADERDROPDOWN, "HeaderDropdown")
    TM_PART(6, HP, HEADERDROPDOWNFILTER, "HeaderDropdownFilter")
    TM_PART(7, HP, HEADEROVERFLOW, "HeaderOverflow")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(HEADERITEM)
    TM_STATE(1, HIS, NORMAL, "Normal")
    TM_STATE(2, HIS, HOT, "Hot")
    TM_STATE(3, HIS, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERITEMLEFT)
    TM_STATE(1, HILS, NORMAL, "Normal")
    TM_STATE(2, HILS, HOT, "Hot")
    TM_STATE(3, HILS, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERITEMRIGHT)
    TM_STATE(1, HIRS, NORMAL, "Normal")
    TM_STATE(2, HIRS, HOT, "Hot")
    TM_STATE(3, HIRS, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERSORTARROW)
    TM_STATE(1, HSAS, SORTEDUP, "SortedUp")
    TM_STATE(2, HSAS, SORTEDDOWN, "SortedDown")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Progress" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PROGRESS)
    TM_PART(1, PP, BAR, "Bar")
    TM_PART(2, PP, BARVERT, "BarVert")
    TM_PART(3, PP, CHUNK, "Chunk")
    TM_PART(4, PP, CHUNKVERT, "ChunkVert")
    // V4:
    TM_PART(5, PP, FILL, "Fill")
    TM_PART(6, PP, FILLVERT, "FillVert")
    TM_PART(7, PP, PULSEOVERLAY, "PulseOverlay")
    TM_PART(8, PP, MOVEOVERLAY, "MoveOverlay")
    TM_PART(9, PP, PULSEOVERLAYVERT, "PulseOverlayVert")
    TM_PART(10, PP, MOVEOVERLAYVERT, "MoveOverlayVert")
    TM_PART(11, PP, TRANSPARENTBAR, "TransparentBar")
    TM_PART(12, PP, TRANSPARENTBARVERT, "TransparentBarVert")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Tab" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TAB)
    TM_PART(1, TABP, TABITEM, "TabItem")
    TM_PART(2, TABP, TABITEMLEFTEDGE, "TabItemLeftEdge")
    TM_PART(3, TABP, TABITEMRIGHTEDGE, "TabItemRightEdge")
    TM_PART(4, TABP, TABITEMBOTHEDGE, "TabItemBothEdge")
    TM_PART(5, TABP, TOPTABITEM, "TopTabItem")
    TM_PART(6, TABP, TOPTABITEMLEFTEDGE, "TopTabItemLeftEdge")
    TM_PART(7, TABP, TOPTABITEMRIGHTEDGE, "TopTabItemRightEdge")
    TM_PART(8, TABP, TOPTABITEMBOTHEDGE, "TopTabItemBothEdge")
    TM_PART(9, TABP, PANE, "Pane")
    TM_PART(10, TABP, BODY, "Body")
    // V4:
    TM_PART(11, TABP, AEROWIZARDBODY, "AeroWizardBody")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TABITEM)
    TM_STATE(1, TIS, NORMAL, "Normal")
    TM_STATE(2, TIS, HOT, "Hot")
    TM_STATE(3, TIS, SELECTED, "Selected")
    TM_STATE(4, TIS, DISABLED, "Disabled")
    TM_STATE(5, TIS, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMLEFTEDGE)
    TM_STATE(1, TILES, NORMAL, "Normal")
    TM_STATE(2, TILES, HOT, "Hot")
    TM_STATE(3, TILES, SELECTED, "Selected")
    TM_STATE(4, TILES, DISABLED, "Disabled")
    TM_STATE(5, TILES, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMRIGHTEDGE)
    TM_STATE(1, TIRES, NORMAL, "Normal")
    TM_STATE(2, TIRES, HOT, "Hot")
    TM_STATE(3, TIRES, SELECTED, "Selected")
    TM_STATE(4, TIRES, DISABLED, "Disabled")
    TM_STATE(5, TIRES, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TABITEMBOTHEDGES)
    TM_STATE(1, TIBES, NORMAL, "Normal")
    TM_STATE(2, TIBES, HOT, "Hot")
    TM_STATE(3, TIBES, SELECTED, "Selected")
    TM_STATE(4, TIBES, DISABLED, "Disabled")
    TM_STATE(5, TIBES, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEM)
    TM_STATE(1, TTIS, NORMAL, "Normal")
    TM_STATE(2, TTIS, HOT, "Hot")
    TM_STATE(3, TTIS, SELECTED, "Selected")
    TM_STATE(4, TTIS, DISABLED, "Disabled")
    TM_STATE(5, TTIS, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMLEFTEDGE)
    TM_STATE(1, TTILES, NORMAL, "Normal")
    TM_STATE(2, TTILES, HOT, "Hot")
    TM_STATE(3, TTILES, SELECTED, "Selected")
    TM_STATE(4, TTILES, DISABLED, "Disabled")
    TM_STATE(5, TTILES, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMRIGHTEDGE)
    TM_STATE(1, TTIRES, NORMAL, "Normal")
    TM_STATE(2, TTIRES, HOT, "Hot")
    TM_STATE(3, TTIRES, SELECTED, "Selected")
    TM_STATE(4, TTIRES, DISABLED, "Disabled")
    TM_STATE(5, TTIRES, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TOPTABITEMBOTHEDGES)
    TM_STATE(1, TTIBES, NORMAL, "Normal")
    TM_STATE(2, TTIBES, HOT, "Hot")
    TM_STATE(3, TTIBES, SELECTED, "Selected")
    TM_STATE(4, TTIBES, DISABLED, "Disabled")
    TM_STATE(5, TTIBES, FOCUSED, "Focused")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Trackbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TRACKBAR)
    TM_PART(1, TKP, TRACK, "Track")
    TM_PART(2, TKP, TRACKVERT, "TrackVert")
    TM_PART(3, TKP, THUMB, "Thumb")
    TM_PART(4, TKP, THUMBBOTTOM, "ThumbBottom")
    TM_PART(5, TKP, THUMBTOP, "ThumbTop")
    TM_PART(6, TKP, THUMBVERT, "ThumbVert")
    TM_PART(7, TKP, THUMBLEFT, "ThumbLeft")
    TM_PART(8, TKP, THUMBRIGHT, "ThumbRight")
    TM_PART(9, TKP, TICS, "Tics")
    TM_PART(10, TKP, TICSVERT, "TicsVert")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TRACKBAR)
    TM_STATE(1, TKS, NORMAL, "Normal")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRACK)
    TM_STATE(1, TRS, NORMAL, "Normal")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRACKVERT)
    TM_STATE(1, TRVS, NORMAL, "Normal")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMB)
    TM_STATE(1, TUS, NORMAL, "Normal")
    TM_STATE(2, TUS, HOT, "Hot")
    TM_STATE(3, TUS, PRESSED, "Pressed")
    TM_STATE(4, TUS, FOCUSED, "Focused")
    TM_STATE(5, TUS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBBOTTOM)
    TM_STATE(1, TUBS, NORMAL, "Normal")
    TM_STATE(2, TUBS, HOT, "Hot")
    TM_STATE(3, TUBS, PRESSED, "Pressed")
    TM_STATE(4, TUBS, FOCUSED, "Focused")
    TM_STATE(5, TUBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBTOP)
    TM_STATE(1, TUTS, NORMAL, "Normal")
    TM_STATE(2, TUTS, HOT, "Hot")
    TM_STATE(3, TUTS, PRESSED, "Pressed")
    TM_STATE(4, TUTS, FOCUSED, "Focused")
    TM_STATE(5, TUTS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBVERT)
    TM_STATE(1, TUVS, NORMAL, "Normal")
    TM_STATE(2, TUVS, HOT, "Hot")
    TM_STATE(3, TUVS, PRESSED, "Pressed")
    TM_STATE(4, TUVS, FOCUSED, "Focused")
    TM_STATE(5, TUVS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBLEFT)
    TM_STATE(1, TUVLS, NORMAL, "Normal")
    TM_STATE(2, TUVLS, HOT, "Hot")
    TM_STATE(3, TUVLS, PRESSED, "Pressed")
    TM_STATE(4, TUVLS, FOCUSED, "Focused")
    TM_STATE(5, TUVLS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(THUMBRIGHT)
    TM_STATE(1, TUVRS, NORMAL, "Normal")
    TM_STATE(2, TUVRS, HOT, "Hot")
    TM_STATE(3, TUVRS, PRESSED, "Pressed")
    TM_STATE(4, TUVRS, FOCUSED, "Focused")
    TM_STATE(5, TUVRS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TICS)
    TM_STATE(1, TSS, NORMAL, "Normal")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TICSVERT)
    TM_STATE(1, TSVS, NORMAL, "Normal")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Tooltips" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TOOLTIP)
    TM_PART(1, TTP, STANDARD, "Standard")
    TM_PART(2, TTP, STANDARDTITLE, "StandardTitle")
    TM_PART(3, TTP, BALLOON, "Balloon")
    TM_PART(4, TTP, BALLOONTITLE, "BalloonTitle")
    TM_PART(5, TTP, CLOSE, "Close")
    // V4:
    TM_PART(6, TTP, BALLOONSTEM, "BalloonStem")
    TM_PART(7, TTP, WRENCH, "Wrench")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(STANDARD)
    TM_STATE(1, TTSS, NORMAL, "Normal")
    TM_STATE(2, TTSS, LINK, "Link")
END_TM_PART_STATES()
    
BEGIN_TM_PART_STATES(BALLOON)
    TM_STATE(1, TTBS, NORMAL, "Normal")
    TM_STATE(2, TTBS, LINK, "Link")
END_TM_PART_STATES()
    
BEGIN_TM_PART_STATES(CLOSE)
    TM_STATE(1, TTCS, NORMAL, "Normal")
    TM_STATE(2, TTCS, HOT, "Hot")
    TM_STATE(3, TTCS, PRESSED, "Pressed")
END_TM_PART_STATES()
    
BEGIN_TM_PART_STATES(BALLOONSTEM)
    TM_STATE(1, TTBSS, POINTINGUPLEFTWALL, "PointingUpLeftWall")
    TM_STATE(2, TTBSS, POINTINGUPCENTERED, "PointingUpCentered")
    TM_STATE(3, TTBSS, POINTINGUPRIGHTWALL, "PointingUpRightWall")
    TM_STATE(4, TTBSS, POINTINGDOWNRIGHTWALL, "PointingDownRightWall")
    TM_STATE(5, TTBSS, POINTINGDOWNCENTERED, "PointingDownCentered")
    TM_STATE(6, TTBSS, POINTINGDOWNLEFTWALL, "PointingDownLeftWall")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(WRENCH)
    TM_STATE(1, TTWS, NORMAL, "Normal")
    TM_STATE(2, TTWS, HOT, "Hot")
    TM_STATE(3, TTWS, PRESSED, "Pressed")
END_TM_PART_STATES(WRENCH)

//---------------------------------------------------------------------------------------
//   "TreeView" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TREEVIEW)
    TM_PART(1, TVP, TREEITEM, "TreeItem")
    TM_PART(2, TVP, GLYPH, "Glyph")
    TM_PART(3, TVP, BRANCH, "Branch")
    // V4:
    TM_PART(4, TVP, HOTGLYPH, "HotGlyph")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(TREEITEM)
    TM_STATE(1, TREIS, NORMAL, "Normal")
    TM_STATE(2, TREIS, HOT, "Hot")
    TM_STATE(3, TREIS, SELECTED, "Selected")
    TM_STATE(4, TREIS, DISABLED, "Disabled")
    TM_STATE(5, TREIS, SELECTEDNOTFOCUS, "SelectedNotFocus")
    // V4:
    TM_STATE(6, TREIS, HOTSELECTED, "HotSelected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(GLYPH)
    TM_STATE(1, GLPS, CLOSED, "Closed")
    TM_STATE(2, GLPS, OPENED, "Opened")
END_TM_PART_STATES()

// V4:
BEGIN_TM_PART_STATES(HOTGLYPH)
    TM_STATE(1, HGLPS, CLOSED, "Closed")
    TM_STATE(2, HGLPS, OPENED, "Opened")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Spin" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SPIN)
    TM_PART(1, SPNP, UP, "Up")
    TM_PART(2, SPNP, DOWN, "Down")
    TM_PART(3, SPNP, UPHORZ, "UpHorz")
    TM_PART(4, SPNP, DOWNHORZ, "DownHorz")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(UP)
    TM_STATE(1, UPS, NORMAL, "Normal")
    TM_STATE(2, UPS, HOT, "Hot")
    TM_STATE(3, UPS, PRESSED, "Pressed")
    TM_STATE(4, UPS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DOWN)
    TM_STATE(1, DNS, NORMAL, "Normal")
    TM_STATE(2, DNS, HOT, "Hot")
    TM_STATE(3, DNS, PRESSED, "Pressed")
    TM_STATE(4, DNS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(UPHORZ)
    TM_STATE(1, UPHZS, NORMAL, "Normal")
    TM_STATE(2, UPHZS, HOT, "Hot")
    TM_STATE(3, UPHZS, PRESSED, "Pressed")
    TM_STATE(4, UPHZS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DOWNHORZ)
    TM_STATE(1, DNHZS, NORMAL, "Normal")
    TM_STATE(2, DNHZS, HOT, "Hot")
    TM_STATE(3, DNHZS, PRESSED, "Pressed")
    TM_STATE(4, DNHZS, DISABLED, "Disabled")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Page" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PAGE)
    TM_PART(1, PGRP, UP, "Up")
    TM_PART(2, PGRP, DOWN, "Down")
    TM_PART(3, PGRP, UPHORZ, "UpHorz")
    TM_PART(4, PGRP, DOWNHORZ, "DownHorz")
END_TM_CLASS_PARTS()

//--- Pager uses same states as Spin ---

//---------------------------------------------------------------------------------------
//   "Scrollbar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SCROLLBAR)
    TM_PART(1, SBP, ARROWBTN, "ArrowBtn")
    TM_PART(2, SBP, THUMBBTNHORZ, "ThumbBtnHorz")
    TM_PART(3, SBP, THUMBBTNVERT, "ThumbBtnVert")
    TM_PART(4, SBP, LOWERTRACKHORZ, "LowerTrackHorz")
    TM_PART(5, SBP, UPPERTRACKHORZ, "UpperTrackHorz")
    TM_PART(6, SBP, LOWERTRACKVERT, "LowerTrackVert")
    TM_PART(7, SBP, UPPERTRACKVERT, "UpperTrackVert")
    TM_PART(8, SBP, GRIPPERHORZ, "GripperHorz")
    TM_PART(9, SBP, GRIPPERVERT, "GripperVert")
    TM_PART(10, SBP, SIZEBOX, "SizeBox")
    
    // V4:
    TM_PART(11, SBP, SIZEBOXBKGND, "SizeBoxBKGND") // Added after 14361 but technically a new V4 part?
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(ARROWBTN)
    TM_STATE(1, ABS, UPNORMAL, "UpNormal")
    TM_STATE(2, ABS, UPHOT, "UpHot")
    TM_STATE(3, ABS, UPPRESSED, "UpPressed")
    TM_STATE(4, ABS, UPDISABLED, "UpDisabled")
    TM_STATE(5, ABS, DOWNNORMAL, "DownNormal")
    TM_STATE(6, ABS, DOWNHOT, "DownHot")
    TM_STATE(7, ABS, DOWNPRESSED, "DownPressed")
    TM_STATE(8, ABS, DOWNDISABLED, "DownDisabled")
    TM_STATE(9, ABS, LEFTNORMAL, "LeftNormal")
    TM_STATE(10, ABS, LEFTHOT, "LeftHot")
    TM_STATE(11, ABS, LEFTPRESSED, "LeftPressed")
    TM_STATE(12, ABS, LEFTDISABLED, "LeftDisabled")
    TM_STATE(13, ABS, RIGHTNORMAL, "RightNormal")
    TM_STATE(14, ABS, RIGHTHOT, "RightHot")
    TM_STATE(15, ABS, RIGHTPRESSED, "RightPressed")
    TM_STATE(16, ABS, RIGHTDISABLED, "RightDisabled")
    TM_STATE(17, ABS, UPHOVER, "UpHover")
    TM_STATE(18, ABS, DOWNHOVER, "DownHover")
    TM_STATE(19, ABS, LEFTHOVER, "LeftHover")
    TM_STATE(20, ABS, RIGHTHOVER, "RightHover")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SCROLLBAR)
    TM_STATE(1, SCRBS, NORMAL, "Normal")
    TM_STATE(2, SCRBS, HOT, "Hot")
    TM_STATE(3, SCRBS, PRESSED, "Pressed")
    TM_STATE(4, SCRBS, DISABLED, "Disabled")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SIZEBOX)
    TM_STATE(1, SZB, RIGHTALIGN, "RightAlign")
    TM_STATE(2, SZB, LEFTALIGN, "LeftAlign")
    TM_STATE(3, SZB, TOPRIGHTALIGN, "TopRightAlign")
    TM_STATE(4, SZB, TOPLEFTALIGN, "TopLeftAlign")
    TM_STATE(5, SZB, HALFBOTTOMRIGHTALIGN, "HalfBottomRightAlign")
    TM_STATE(6, SZB, HALFBOTTOMLEFTALIGN, "HalfBottomLeftAlign")
    TM_STATE(7, SZB, HALFTOPRIGHTALIGN, "HalfTopRightAlign")
    TM_STATE(8, SZB, HALFTOPLEFTALIGN, "HalfTopLeftAlign")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Edit" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(EDIT)
    TM_PART(1, EP, EDITTEXT, "EditText")
    TM_PART(2, EP, CARET, "Caret")
    // V4:
    TM_PART(3, EP, BACKGROUND, "Background")
    TM_PART(4, EP, PASSWORD, "Password")
    TM_PART(5, EP, BACKGROUNDWITHBORDER, "BackgroundWithBorder")
    TM_PART(6, EP, EDITBORDER_NOSCROLL, "EditBorder_NoScroll")
    TM_PART(7, EP, EDITBORDER_HSCROLL, "EditBorder_HScroll")
    TM_PART(8, EP, EDITBORDER_VSCROLL, "EditBorder_VScroll")
    TM_PART(9, EP, EDITBORDER_HVSCROLL, "EditBorder_HVScroll")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(EDITTEXT)
    TM_STATE(1, ETS, NORMAL, "Normal")
    TM_STATE(2, ETS, HOT, "Hot")
    TM_STATE(3, ETS, SELECTED, "Selected")
    TM_STATE(4, ETS, DISABLED, "Disabled")
    TM_STATE(5, ETS, FOCUSED, "Focused")
    TM_STATE(6, ETS, READONLY, "ReadOnly")
    TM_STATE(7, ETS, ASSIST, "Assist")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "ComboBox" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(COMBOBOX)
    TM_PART(1, CP, DROPDOWNBUTTON, "DropDownButton")
    // V4:
    TM_PART(2, CP, BACKGROUND, "Background")
    TM_PART(3, CP, TRANSPARENTBACKGROUND, "TransparentBackground")
    TM_PART(4, CP, BORDER, "Border")
    TM_PART(5, CP, READONLY, "ReadOnly")
    TM_PART(6, CP, DROPDOWNBUTTONRIGHT, "DropdownButtonRight")
    TM_PART(7, CP, DROPDOWNBUTTONLEFT, "DropdownButtonLeft")
    TM_PART(8, CP, CUEBANNER, "CueBanner")

    // Added sometime after 14361 but before 19041.546
    TM_PART(9, CP, DROPDOWNITEM, "DropdownItem")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(COMBOBOX)
    TM_STATE(1, CBXS, NORMAL, "Normal")
    TM_STATE(2, CBXS, HOT, "Hot")
    TM_STATE(3, CBXS, PRESSED, "Pressed")
    TM_STATE(4, CBXS, DISABLED, "Disabled")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Taskbar Clock" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(CLOCK)
    TM_PART(1, CLP, TIME, "Time")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(CLOCK)
    TM_STATE(1, CLS, NORMAL, "Normal")
    TM_STATE(2, CLS, HOT, "Hot")
    TM_STATE(3, CLS, PRESSED, "Pressed")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Tray Notify" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TRAYNOTIFY)
    TM_PART(1, TNP, BACKGROUND, "Background")
    TM_PART(2, TNP, ANIMBACKGROUND, "AnimBackground")
END_TM_CLASS_PARTS()

BEGIN_TM_CLASS_PARTS(TRAYNOTIFYFLYOUT)
    TM_PART(1, TNF, TOP, "Top")
    TM_PART(2, TNF, BOTTOM, "Bottom")
    TM_PART(3, TNF, BUTTON, "Button")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBAR)
    TM_PART(1, TBP, BACKGROUNDBOTTOM, "BackgroundBottom")
    TM_PART(2, TBP, BACKGROUNDRIGHT, "BackgroundRight")
    TM_PART(3, TBP, BACKGROUNDTOP, "BackgroundTop")
    TM_PART(4, TBP, BACKGROUNDLEFT, "BackgroundLeft")
    TM_PART(5, TBP, SIZINGBARBOTTOM, "SizingBarBottom")
    TM_PART(6, TBP, SIZINGBARRIGHT, "SizingBarRight")
    TM_PART(7, TBP, SIZINGBARTOP, "SizingBarTop")
    TM_PART(8, TBP, SIZINGBARLEFT, "SizingBarLeft")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBar Pearl" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBARPEARL)
    TM_PART(1, TPRL, PEARLICON, "PearlIcon")
    TM_PART(2, TPRL, TABLETMODEPEARLICON, "TabletModePearlIcon")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBand" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBAND)
    TM_PART(1, TDP, GROUPCOUNT, "GroupCount")
    TM_PART(2, TDP, FLASHBUTTON, "FlashButton")
    TM_PART(3, TDP, FLASHBUTTONGROUPMENU, "FlashButtonGroupMenu")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBand Extended UI" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TaskbandExtendedUI) // Microsoft didn't use an all-caps name here.
    TM_PART(1, TBEUIP, BACKGROUND, "Background")
    TM_PART(2, TBEUIP, THUMBRECT, "ThumbRect")
    TM_PART(3, TBEUIP, WINDOWTEXT, "WindowText")
    TM_PART(4, TBEUIP, ACTIVERECT, "ActiveRect")
    TM_PART(5, TBEUIP, PREVACTIVERECT, "PrevActiveRect")
    TM_PART(6, TBEUIP, THUMBBARBUTTONSINGLE, "ThumbBarButtonSingle")
    TM_PART(7, TBEUIP, THUMBBARBUTTONLEFT, "ThumbBarButtonLeft")
    TM_PART(8, TBEUIP, THUMBBARBUTTONMIDDLE, "ThumbBarButtonMiddle")
    TM_PART(9, TBEUIP, THUMBBARBUTTONRIGHT, "ThumbBarButtonRight")
    TM_PART(10, TBEUIP, THUMBCLOSEBUTTON, "ThumbCloseButton")
    TM_PART(11, TBEUIP, ARROWBTN, "ArrowBtn")
    TM_PART(12, TBEUIP, THUMBSHADOWTOP, "ThumbShadowTop")
    TM_PART(13, TBEUIP, THUMBSHADOWLEFT, "ThumbShadowLeft")
    TM_PART(14, TBEUIP, THUMBSHADOWRIGHT, "ThumbShadowRight")
    TM_PART(15, TBEUIP, THUMBSHADOWBOTTOM, "ThumbShadowBottom")
    TM_PART(16, TBEUIP, WINDOWBORDER, "WindowBorder")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBand 2" Parts & States (Windows 7+ Superbar)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBAND2)
    TM_PART(1, TDP2, NEGATIVESPACEBOTTOM, "NegativeSpaceBottom")
    TM_PART(2, TDP2, NEGATIVESPACETOP, "NegativeSpaceTop")
    TM_PART(3, TDP2, NEGATIVESPACELEFT, "NegativeSpaceLeft")
    TM_PART(4, TDP2, NEGATIVESPACERIGHT, "NegativeSpaceRight")
    TM_PART(5, TDP2, SINGLEBUTTON, "SingleButton")
    TM_PART(6, TDP2, GLOMHORIZONTALACTIVE, "GlomHorizontalActive")
    TM_PART(7, TDP2, GLOMHORIZONTALINACTIVE, "GlomHorizontalInactive")
    TM_PART(8, TDP2, GLOMVERTICALACTIVE, "GlomVerticalActive")
    TM_PART(9, TDP2, GLOMVERTICALINACTIVE, "GlomVerticalInactive")
    TM_PART(10, TDP2, GLOMDIVIDER, "GlomDivider")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "TaskBar Show Desktop" Parts & States (Windows 7+ Superbar)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKBARSHOWDESKTOP)
    TM_PART(1, TSDT, SHOWDESKTOPTHEME, "ShowDesktopTheme")
    TM_PART(2, TSDT, DIVIDERLINE, "DividerLine")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "StartPanel" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(STARTPANEL)
    TM_PART(1, SPP, USERPANE, "UserPane")
    TM_PART(2, SPP, MOREPROGRAMS, "MorePrograms")
    TM_PART(3, SPP, MOREPROGRAMSARROW, "MoreProgramsArrow")
    TM_PART(4, SPP, PROGLIST, "ProgList")
    TM_PART(5, SPP, PROGLISTSEPARATOR, "ProgListSeparator")
    TM_PART(6, SPP, PLACESLIST, "PlacesList")
    TM_PART(7, SPP, PLACESLISTSEPARATOR, "PlacesListSeparator")
    TM_PART(8, SPP, LOGOFF, "Logoff")
    TM_PART(9, SPP, LOGOFFBUTTONS, "LogoffButtons")
    TM_PART(10, SPP, USERPICTURE, "UserPicture")
    TM_PART(11, SPP, PREVIEW, "Preview")
    // Vista+ properties:
    TM_PART(12, SPP, MOREPROGRAMSTAB, "MoreProgramsTab")
    TM_PART(13, SPP, NSCHOST, "NscHost")
    TM_PART(14, SPP, SOFTWAREEXPLORER, "SoftwareExplorer")
    TM_PART(15, SPP, OPENBOX, "OpenBox")
    TM_PART(16, SPP, SEARCHVIEW, "SearchView")
    TM_PART(17, SPP, MOREPROGRAMSARROWBACK, "MoreProgramsArrowBack")
    TM_PART(18, SPP, TOPMATCH, "TopMatch")
    TM_PART(19, SPP, LOGOFFSPLITBUTTONDROPDOWN, "LogOffSplitButtonDropdown")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MOREPROGRAMSARROW)
    TM_STATE(1, SPS, NORMAL, "Normal")
    TM_STATE(2, SPS, HOT, "Hot")
    TM_STATE(3, SPS, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(LOGOFFBUTTONS)
    TM_STATE(1, SPLS, NORMAL, "Normal")
    TM_STATE(2, SPLS, HOT, "Hot")
    TM_STATE(3, SPLS, PRESSED, "Pressed")
END_TM_PART_STATES()

// Vista+ part states definitions:
BEGIN_TM_PART_STATES(MOREPROGRAMSARROWBACK)
    TM_STATE(1, SPSB, NORMAL, "Normal")
    TM_STATE(2, SPSB, HOT, "Hot")
    TM_STATE(3, SPSB, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(MOREPROGRAMSTAB)
    TM_STATE(1, SPMPT, NORMAL, "Normal")
    TM_STATE(2, SPMPT, HOT, "Hot")
    TM_STATE(3, SPMPT, SELECTED, "Selected")
    TM_STATE(4, SPMPT, DISABLED, "Disabled")
    TM_STATE(5, SPMPT, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SOFTWAREEXPLORER)
    TM_STATE(1, SPSE, NORMAL, "Normal")
    TM_STATE(2, SPSE, HOT, "Hot")
    TM_STATE(3, SPSE, SELECTED, "Selected")
    TM_STATE(4, SPSE, DISABLED, "Disabled")
    TM_STATE(5, SPSE, FOCUSED, "Focused")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(OPENBOX)
    TM_STATE(1, SPOB, NORMAL, "Normal")
    TM_STATE(2, SPOB, HOT, "Hot")
    TM_STATE(3, SPOB, SELECTED, "Selected")
    TM_STATE(4, SPOB, DISABLED, "Disabled")
    TM_STATE(5, SPOB, FOCUSED, "Focused")
END_TM_PART_STATES()

// V4:
BEGIN_TM_CLASS_PARTS(STARTPANELPRIV)
    TM_PART(1, SPPP, MOREPROGRAMSTAB, "MoreProgramsTab")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "ExplorerBar" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(EXPLORERBAR)
    TM_PART(1, EBP, HEADERBACKGROUND, "HeaderBackground")
    TM_PART(2, EBP, HEADERCLOSE, "HeaderClose")
    TM_PART(3, EBP, HEADERPIN, "HeaderPin")
    TM_PART(4, EBP, IEBARMENU, "IEBarMenu")
    TM_PART(5, EBP, NORMALGROUPBACKGROUND, "NormalGroupBackground")
    TM_PART(6, EBP, NORMALGROUPCOLLAPSE, "NormalGroupCollapse")
    TM_PART(7, EBP, NORMALGROUPEXPAND, "NormalGroupExpand")
    TM_PART(8, EBP, NORMALGROUPHEAD, "NormalGroupHead")
    TM_PART(9, EBP, SPECIALGROUPBACKGROUND, "SpecialGroupBackground")
    TM_PART(10, EBP, SPECIALGROUPCOLLAPSE, "SpecialGroupCollapse")
    TM_PART(11, EBP, SPECIALGROUPEXPAND, "SpecialGroupExpand")
    TM_PART(12, EBP, SPECIALGROUPHEAD, "SpecialGroupHead")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(HEADERCLOSE)
    TM_STATE(1, EBHC, NORMAL, "Normal")
    TM_STATE(2, EBHC, HOT, "Hot")
    TM_STATE(3, EBHC, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HEADERPIN)
    TM_STATE(1, EBHP, NORMAL, "Normal")
    TM_STATE(2, EBHP, HOT, "Hot")
    TM_STATE(3, EBHP, PRESSED, "Pressed")
    TM_STATE(4, EBHP, SELECTEDNORMAL, "SelectedNormal")
    TM_STATE(5, EBHP, SELECTEDHOT, "SelectedHot")
    TM_STATE(6, EBHP, SELECTEDPRESSED, "SelectedPressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(IEBARMENU)
    TM_STATE(1, EBM, NORMAL, "Normal")
    TM_STATE(2, EBM, HOT, "Hot")
    TM_STATE(3, EBM, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(NORMALGROUPCOLLAPSE)
    TM_STATE(1, EBNGC, NORMAL, "Normal")
    TM_STATE(2, EBNGC, HOT, "Hot")
    TM_STATE(3, EBNGC, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(NORMALGROUPEXPAND)
    TM_STATE(1, EBNGE, NORMAL, "Normal")
    TM_STATE(2, EBNGE, HOT, "Hot")
    TM_STATE(3, EBNGE, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SPECIALGROUPCOLLAPSE)
    TM_STATE(1, EBSGC, NORMAL, "Normal")
    TM_STATE(2, EBSGC, HOT, "Hot")
    TM_STATE(3, EBSGC, PRESSED, "Pressed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SPECIALGROUPEXPAND)
    TM_STATE(1, EBSGE, NORMAL, "Normal")
    TM_STATE(2, EBSGE, HOT, "Hot")
    TM_STATE(3, EBSGE, PRESSED, "Pressed")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "TaskBand" Parts & States
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(MENUBAND)
    TM_PART(1, MDP, NEWAPPBUTTON, "NewAppButton")
    TM_PART(2, MDP, SEPERATOR, "Seperator") // Microsoft typo.
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(MENUBAND)
    TM_STATE(1, MDS, NORMAL, "Normal")
    TM_STATE(2, MDS, HOT, "Hot")
    TM_STATE(3, MDS, PRESSED, "Pressed")
    TM_STATE(4, MDS, DISABLED, "Disabled")
    TM_STATE(5, MDS, CHECKED, "Checked")
    TM_STATE(6, MDS, HOTCHECKED, "HotChecked")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Address Band" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(ADDRESSBAND)
    TM_STATE(1, ABP, ABBACKGROUND, "ABBackground")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(ADDRESSBAND)
    TM_STATE(1, ABBG, NORMAL, "Normal")
    TM_STATE(2, ABBG, HOT, "Hot")
    TM_STATE(3, ABBG, DISABLED, "Disabled")
    TM_STATE(4, ABBG, FOCUSED, "Focused")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Aero Wizard" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(AEROWIZARD)
    TM_PART(1, AW, TITLEBAR, "TitleBar")
    TM_PART(2, AW, HEADERAREA, "HeaderArea")
    TM_PART(3, AW, CONTENTAREA, "ContentArea")
    TM_PART(4, AW, COMMANDAREA, "CommandArea")
    TM_PART(5, AW, BUTTON, "Button")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Alt Tab" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(ALTTAB)
    TM_PART(1, ATP, WINDOWTEXT, "WindowText")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Barrier Page" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(BARRIERPAGE)
    TM_PART(1, BPG, PANEBACKGROUND, "PaneBackground")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Breadcrumb Bar" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(BREADCRUMBBAR)
    TM_PART(1, BCBP, BCBOVERFLOWCHEVRON, "BCBOverflowChevron")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Communications" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(COMMUNICATIONS)
    TM_PART(1, CSST, TAB, "Tab")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Control Panel" Parts & States (v4)
//--------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(CONTROLPANEL)
    TM_PART(1, CPANEL, NAVIGATIONPANE, "NavigationPane")
    TM_PART(2, CPANEL, CONTENTPANE, "ContentPane")
    TM_PART(3, CPANEL, NAVIGATIONPANELABEL, "NavigationPaneLabel")
    TM_PART(4, CPANEL, CONTENTPANELABEL, "ContentPaneLabel")
    TM_PART(5, CPANEL, TITLE, "Title")
    TM_PART(6, CPANEL, BODYTEXT, "BodyText")
    TM_PART(7, CPANEL, HELPLINK, "HelpLink")
    TM_PART(8, CPANEL, TASKLINK, "TaskLink")
    TM_PART(9, CPANEL, GROUPTEXT, "GroupText")
    TM_PART(10, CPANEL, CONTENTLINK, "ContentLink")
    TM_PART(11, CPANEL, SECTIONTITLELINK, "SectionTitleLink")
    TM_PART(12, CPANEL, LARGECOMMANDDATA, "LargeCommandData")
    TM_PART(13, CPANEL, SMALLCOMMANDDATA, "SmallCommandData")
    TM_PART(14, CPANEL, BUTTON, "Button")
    TM_PART(15, CPANEL, MESSAGETEXT, "MessageText")
    TM_PART(16, CPANEL, NAVIGATIONPANELINE, "NavigationPaneLine")
    TM_PART(17, CPANEL, CONTENTPANELINE, "ContentPaneLine")
    TM_PART(18, CPANEL, BANNERAREA, "BannerArea")
    TM_PART(19, CPANEL, BODYTITLE, "BodyTitle")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Copy Close" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(COPYCLOSE)
    TM_PART(1, CPYCLS, COPYCLOSEBTN, "CopyCloseBtn")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(COPYCLOSEBTN)
    TM_STATE(1, CPYCLS, NORMAL, "Normal")
    TM_STATE(2, CPYCLS, HOT, "Hot")
    TM_STATE(3, CPYCLS, PRESSED, "Pressed")
    TM_STATE(4, CPYCLS, DISABLEDPRESSED, "DisabledPressed")
    TM_STATE(5, CPYCLS, DISABLED, "Disabled")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Date Picker" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(DATEPICKER)
    TM_STATE(1, DP, DATETEXT, "DateText")
    TM_STATE(2, DP, DATEBORDER, "DateBorder")
    TM_STATE(3, DP, SHOWCALENDARBUTTONRIGHT, "ShowCalendarButtonRight")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Drag Drop" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(DRAGDROP)
    TM_PART(1, DD, COPY, "Copy")
    TM_PART(2, DD, MOVE, "Move")
    TM_PART(3, DD, UPDATEMETADATA, "UpdateMetadata")
    TM_PART(4, DD, CREATELINK, "CreateLink")
    TM_PART(5, DD, WARNING, "Warning")
    TM_PART(6, DD, NONE, "None")
    TM_PART(7, DD, IMAGEBG, "ImageBg")
    TM_PART(8, DD, TEXTBG, "TextBg")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Drop List" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(DROPLISTCONTROL)
    TM_PART(1, DLC, MENUBUTTON, "MenuButton")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Empty Markup" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(EMPTYMARKUP)
    TM_PART(1, EMP, MARKUPTEXT, "MarkupText")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Flyout" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(FLYOUT)
    TM_PART(1, FLYOUT, HEADER, "Header")
    TM_PART(2, FLYOUT, BODY, "Body")
    TM_PART(3, FLYOUT, LABEL, "Label")
    TM_PART(4, FLYOUT, LINK, "Link")
    TM_PART(5, FLYOUT, DIVIDER, "Divider")
    TM_PART(6, FLYOUT, WINDOW, "Window")
    TM_PART(7, FLYOUT, LINKAREA, "LinkArea")
    TM_PART(8, FLYOUT, LINKHEADER, "LinkHeader")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Help Searchbox" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(HELPSEARCHBOX)
    TM_PART(1, HSBP, HSBBACKGROUND, "HSBBackground")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Info Bar" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(INFOBAR)
    TM_PART(1, IBAR, BARBACKGROUND, "BarBackground")
    TM_PART(2, IBAR, BARFOREGROUND, "BarForeground")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(INFOBAR)
    TM_STATE(1, IBARS, NORMAL, "Normal")
    TM_STATE(2, IBARS, HOT, "Hot")
    TM_STATE(3, IBARS, PRESSED, "Pressed")
    TM_STATE(4, IBARS, KEYFOCUSED, "KeyFocused")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Items View" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(ITEMSVIEW)
    TM_PART(1, IV, SEARCHHIT, "SearchHit")
    TM_PART(2, IV, SUBSETBUTTON, "SubsetButton")
    TM_PART(3, IV, FOCUSRECT, "FocusRect")
    TM_PART(4, IV, PROPERTY, "Property")
    TM_PART(5, IV, EMPTYTEXT, "EmptyText")
    TM_PART(6, IV, FOCUSRECTINNER, "FocusRectInner")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "List Box" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(LISTBOX)
    TM_PART(1, LBCP, BORDER_HSCROLL, "Border_HScroll")
    TM_PART(2, LBCP, BORDER_HVSCROLL, "Border_HVScroll")
    TM_PART(3, LBCP, BORDER_NOSCROLL, "Border_NoScroll")
    TM_PART(4, LBCP, BORDER_VSCROLL, "Border_VScroll")
    TM_PART(5, LBCP, ITEM, "Item")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Link" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(LINK)
    TM_PART(1, LP, HYPERLINK, "HyperLink")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Menu" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(MENU)
    // The following section takes liberties on the names. The private names that Microsoft
    // use for these following symbols are suffixed with _TMSCHEMA. This is likely done as
    // a compiler hack, and should not affect parsing, so we will not include this pattern
    // in restyle. Notably, this section is what was added to TmSchema.h during Longhorn
    // development prior to its deprecation, which went on to be published in the Vista & 7
    // SDKs (without suffixes).
    TM_PART(1, MENU, MENUITEM, "MenuItem")
    TM_PART(2, MENU, MENUDROPDOWN, "MenuDropdown")
    TM_PART(3, MENU, MENUBARITEM, "MenuBarItem")
    TM_PART(4, MENU, MENUBARDROPDOWN, "MenuBarDropdown")
    TM_PART(5, MENU, CHEVRON, "Chevron")
    TM_PART(6, MENU, SEPARATOR, "Separator")
    // End _TMSCHEMA suffix section.
    TM_PART(7, MENU, BARBACKGROUND, "BarBackground")
    TM_PART(8, MENU, BARITEM, "BarItem")
    TM_PART(9, MENU, POPUPBACKGROUND, "PopupBackground")
    TM_PART(10, MENU, POPUPBORDERS, "PopupBorders")
    TM_PART(11, MENU, POPUPCHECK, "PopupCheck")
    TM_PART(12, MENU, POPUPCHECKBACKGROUND, "PopupCheckBackground")
    TM_PART(13, MENU, POPUPGUTTER, "PopupGutter")
    TM_PART(14, MENU, POPUPITEM, "PopupItem")
    TM_PART(15, MENU, POPUPSEPARATOR, "PopupSeparator")
    TM_PART(16, MENU, POPUPSUBMENU, "PopupSubmenu")
    TM_PART(17, MENU, SYSTEMCLOSE, "SystemClose")
    TM_PART(18, MENU, SYSTEMMAXIMIZE, "SystemMaximize")
    TM_PART(19, MENU, SYSTEMMINIMIZE, "SystemMinimize")
    TM_PART(20, MENU, SYSTEMRESTORE, "SystemRestore")
    // The following entries are very recent. They exist in the 10.0.22621 SDK, but don't
    // exist in 10.0.14361, nor are they acknowledged by other editors like msstyleEditor.
    TM_PART(21, MENU, POPUPSUBMENU_HCHOT, "PopupSubmenu_HCHot")
    TM_PART(22, MENU, SYSTEMCLOSE_HCHot, "SystemClose_HCHot")
    TM_PART(23, MENU, SYSTEMMAXIMIZE_HCHot, "SystemMaximize_HCHot")
    TM_PART(24, MENU, SYSTEMMINIMIZE_HCHot, "SystemMinimize_HCHot")
    TM_PART(25, MENU, SYSTEMRESTORE_HCHot, "SystemRestore_HCHot")
    TM_PART(26, MENU, POPUPITEMKBFOCUS, "PopupItemKBFocus")
    TM_PART(27, MENU, POPUPITEM_FOCUSABLE, "PopupItem_Focusable")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Navigation" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(NAVIGATION)
    TM_PART(1, NAV, BACKBUTTON, "BackButton")
    TM_PART(2, NAV, FORWARDBUTTON, "ForwardButton")
    TM_PART(3, NAV, MENUBUTTON, "MenuButton")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Month Calendar" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(MONTHCAL)
    TM_PART(1, MC, BACKGROUND, "Background")
    TM_PART(2, MC, BORDERS, "Borders")
    TM_PART(3, MC, GRIDBACKGROUND, "GridBackground")
    TM_PART(4, MC, COLHEADERSPLITTER, "ColHeaderSplitter")
    TM_PART(5, MC, GRIDCELLBACKGROUND, "GridCellBackground")
    TM_PART(6, MC, GRIDCELL, "GridCell")
    TM_PART(7, MC, GRIDCELLUPPER, "GridCellUpper")
    TM_PART(8, MC, TRAILINGGRIDCELL, "TrailingGridCell")
    TM_PART(9, MC, TRAILINGGRIDCELLUPPER, "TrailingGridCellUpper")
    TM_PART(10, MC, NAVNEXT, "NavNext")
    TM_PART(11, MC, NAVPREV, "NavPrev")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Pause" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PAUSE)
    TM_PART(1, PSE, PAUSEBTN, "PauseBtn")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Preview Pane" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PREVIEWPANE)
    TM_PART(1, PP, PREVIEWBACKGROUND, "PreviewBackground")
    TM_PART(2, PP, EDITPROPERTIES, "EditProperties")
    TM_PART(3, PP, NAVPANESIZER, "NavPaneSizer")
    TM_PART(4, PP, READINGPANESIZER, "ReadingPaneSizer")
    TM_PART(5, PP, TITLE, "Title")
    TM_PART(6, PP, LABEL, "Label")
    TM_PART(7, PP, VALUE, "Value")
    TM_PART(8, PP, LABELCID, "LabelCID")
    TM_PART(9, PP, VALUECID, "ValueCID")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Proper Tree" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(PROPERTREE)
    TM_PART(1, PTP, FOLDERSHEADER, "FoldersHeader")
    TM_PART(2, PTP, BACKGROUND, "Background")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Reading Pane" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(READINGPANE)
    TM_PART(1, RP, BACKGROUNDCOLORS, "BackgroundColors")
    TM_PART(2, RP, LABEL, "Label")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "SearchBox" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SEARCHBOX)
    TM_PART(1, SBP, SBBACKGROUND, "SBBackground")
    TM_PART(2, SBP, SBCLEARBUTTON, "SBClearButton")
    TM_PART(3, SBP, SBSEARCHBUTTON, "SBSearchButton")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Search Editbox" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SEARCHEDITBOX)
    TM_PART(1, SEBP, SEARCHEDITBOXTEXT, "SearchEditboxText")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Search Home" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SEARCHHOME)
    TM_PART(1, SH, BACKGROUND, "Background")
    TM_PART(2, SH, LINE, "Line")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Shell Status Bar Separator" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(SHELLSTATUSBARSEPARATOR)
    TM_PART(1, SSBS, BACKGROUND, "Background")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Static" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(STATIC)
    TM_PART(1, STAT, TEXT, "Text")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Task Dialog" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKDIALOG)
    TM_PART(1, TDLG, PRIMARYPANEL, "PrimaryPanel")
    TM_PART(2, TDLG, MAININSTRUCTIONPANEL, "MainInstructionPanel")
    TM_PART(3, TDLG, MAINICON, "MainIcon")
    TM_PART(4, TDLG, CONTENTPANE, "ContentPane")
    TM_PART(5, TDLG, CONTENTICON, "ContentIcon")
    TM_PART(6, TDLG, EXPANDEDCONTENT, "ExpandedContent")
    TM_PART(7, TDLG, COMMANDLINKPANE, "CommandLinkPane")
    TM_PART(8, TDLG, SECONDARYPANEL, "SecondaryPanel")
    TM_PART(9, TDLG, CONTROLPANE, "ControlPane")
    TM_PART(10, TDLG, BUTTONSECTION, "ButtonSection")
    TM_PART(11, TDLG, BUTTONWRAPPER, "ButtonWrapper")
    TM_PART(12, TDLG, EXPANDOTEXT, "ExpandoText")
    TM_PART(13, TDLG, EXPANDOBUTTON, "ExpandoButton")
    TM_PART(14, TDLG, VERIFICATIONTEXT, "VerificationText")
    TM_PART(15, TDLG, FOOTNOTEPANE, "FootnotePane")
    TM_PART(16, TDLG, FOOTNOTEAREA, "FootnoteArea")
    TM_PART(17, TDLG, FOOTNOTESEPARATOR, "FootnoteSeparator")
    TM_PART(18, TDLG, EXPANDEDFOOTERAREA, "ExpandedFooterArea")
    TM_PART(19, TDLG, PROGRESSBAR, "ProgressBar")
    TM_PART(20, TDLG, IMAGEALIGNMENT, "ImageAlignment")
    TM_PART(21, TDLG, RADIOBUTTONPANE, "RadioButtonPane")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Text Glow" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TEXTGLOW)
    TM_PART(1, TG, BACKGROUND, "Background")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Text Selection Gripper" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TEXTSELECTIONGRIPPER)
    TM_PART(1, TSGP, GRIPPER, "Gripper")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Text Style" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TEXTSTYLE)
    TM_PART(1, TEXT, MAININSTRUCTION, "MainInstruction")
    TM_PART(2, TEXT, INSTRUCTION, "Instruction")
    TM_PART(3, TEXT, BODYTITLE, "BodyTitle")
    TM_PART(4, TEXT, BODYTEXT, "BodyText")
    TM_PART(5, TEXT, SECONDARYTEXT, "SecondaryText")
    TM_PART(6, TEXT, HYPERLINKTEXT, "HyperlinkText")
    TM_PART(7, TEXT, EXPANDED, "Expanded")
    TM_PART(8, TEXT, LABEL, "Label")
    TM_PART(9, TEXT, CONTROLLABEL, "ControlLabel")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Try Harder" Parts & States (v4)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TRYHARDER)
    TM_PART(1, TH, BUTTON, "Button")
    TM_PART(2, TH, VERTICAL, "Vertical")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(BUTTON)
    TM_STATE(1, THB, NORMAL, "Normal")
    TM_STATE(2, THB, INACTIVE, "Inactive")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "User Tile" Parts & States (scrapped Windows 8 beta taskbar feature)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(USERTILE)
    TM_PART(1, UTP, STROKEBACKGROUND, "StrokeBackground")
    TM_PART(2, UTP, HOVERBACKGROUND, "HoverBackground")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "DWM Pen" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(DWMPEN)
    TM_PART(1, DWMPP, PENBARREL, "PenBarrel")
    TM_PART(2, DWMPP, PENHOLD, "PenHold")
    TM_PART(3, DWMPP, PENRIGHTTAP, "PenRightTap")
    TM_PART(4, DWMPP, PENTAP, "PenTap")
    TM_PART(5, DWMPP, PENDOUBLETAP, "PenDoubleTap")
    TM_PART(6, DWMPP, FLICKSCROLLUP, "FlickScrollUp")
    TM_PART(7, DWMPP, FLICKSCROLLDOWN, "FlickScrollDown")
    TM_PART(8, DWMPP, FLICKDRAGUP, "FlickDragUp")
    TM_PART(9, DWMPP, FLICKDRAGDOWN, "FlickDragDown")
    TM_PART(10, DWMPP, FLICKFORWARD, "FlickForward")
    TM_PART(11, DWMPP, FLICKBACKWARD, "FlickBackward")
    TM_PART(12, DWMPP, FLICKCUT, "FlickCut")
    TM_PART(13, DWMPP, FLICKCOPY, "FlickCopy")
    TM_PART(14, DWMPP, FLICKPASTE, "FlickPaste")
    TM_PART(15, DWMPP, FLICKUNDO, "FlickUndo")
    TM_PART(16, DWMPP, FLICKREDO, "FlickRedo")
    TM_PART(17, DWMPP, FLICKPRINT, "FlickPrint")
    TM_PART(18, DWMPP, FLICKDELETE, "FlickDelete")
    TM_PART(19, DWMPP, FLICKOPEN, "FlickOpen")
    TM_PART(20, DWMPP, FLICKSAVE, "FlickSave")
    TM_PART(21, DWMPP, FLICKMODIFIERKEY, "FlickModifierKey")
    TM_PART(22, DWMPP, FLICKSHIFTKEY, "FlickShiftKey")
    TM_PART(23, DWMPP, FLICKWINKEY, "FlickWinKey")
    TM_PART(24, DWMPP, FLICKGENERICKEY, "FlickGenericKey")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "DWM Touch" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(DWMTOUCH)
    TM_PART(1, DWMWP, TOUCHDRAG, "TouchDrag")
    TM_PART(2, DWMWP, TOUCHCONTACT, "TouchContact")
    TM_PART(3, DWMWP, TETHER, "Tether")
    TM_PART(4, DWMWP, TEXTHANDLEBLACK, "TextHandleBlack")
    TM_PART(5, DWMWP, TEXTHANDLEWHITE, "TextHandleWhite")
    TM_PART(6, DWMWP, TOUCHDOUBLECONTACT, "TouchDoubleContact")
    TM_PART(7, DWMWP, TOUCHCONTACTPRESENTATIONMODE, "TouchContactPresentationMode")
    TM_PART(8, DWMWP, INDIRECTTOUCH, "IndirectTouch")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "DWM Window" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS_FOR_OS(DWMWINDOW, ESupportedOS::WinVista)
    TM_PART(1, VISTA_DWMWP, BOTTOMFRAME, "BottomFrame")
    TM_PART(2, VISTA_DWMWP, BOTTOMSHADOW, "BottomShadow")
    TM_PART(3, VISTA_DWMWP, BUTTONACTIVECAPTION, "ButtonActiveCaption")
    TM_PART(4, VISTA_DWMWP, BUTTONINACTIVECAPTION, "ButtonInactiveCaption")
    TM_PART(5, VISTA_DWMWP, BUTTONACTIVECAPTIONEND, "ButtonActiveCaptionEnd")
    TM_PART(6, VISTA_DWMWP, BUTTONINACTIVECAPTIONEND, "ButtonInactiveCaptionEnd")
    TM_PART(7, VISTA_DWMWP, BUTTONACTIVECLOSE, "ButtonActiveClose")
    TM_PART(8, VISTA_DWMWP, BUTTONINACTIVECLOSE, "ButtonInactiveClose")
    TM_PART(9, VISTA_DWMWP, BUTTONACTIVECLOSEALONE, "ButtonActiveCloseAlone")
    TM_PART(10, VISTA_DWMWP, BUTTONINACTIVECLOSEALONE, "ButtonInactiveCloseAlone")
    TM_PART(11, VISTA_DWMWP, BUTTONCLOSEGLOW, "ButtonCloseGlow")
    TM_PART(12, VISTA_DWMWP, BUTTONCLOSEGLYPH96, "ButtonCloseGlyph96")
    TM_PART(13, VISTA_DWMWP, BUTTONCLOSEGLYPH120, "ButtonCloseGlyph120")
    TM_PART(14, VISTA_DWMWP, BUTTONCLOSEGLYPH144, "ButtonCloseGlyph144")
    TM_PART(15, VISTA_DWMWP, BUTTONCLOSEGLYPH192, "ButtonCloseGlyph192")
    TM_PART(16, VISTA_DWMWP, BUTTONGLOW, "ButtonGlow")
    TM_PART(17, VISTA_DWMWP, BUTTONHELPGLYPH96, "ButtonHelpGlyph96")
    TM_PART(18, VISTA_DWMWP, BUTTONHELPGLYPH120, "ButtonHelpGlyph120")
    TM_PART(19, VISTA_DWMWP, BUTTONHELPGLYPH144, "ButtonHelpGlyph144")
    TM_PART(20, VISTA_DWMWP, BUTTONHELPGLYPH192, "ButtonHelpGlyph192")
    TM_PART(21, VISTA_DWMWP, BUTTONMAXGLYPH96, "ButtonMaxGlyph96")
    TM_PART(22, VISTA_DWMWP, BUTTONMAXGLYPH120, "ButtonMaxGlyph120")
    TM_PART(23, VISTA_DWMWP, BUTTONMAXGLYPH144, "ButtonMaxGlyph144")
    TM_PART(24, VISTA_DWMWP, BUTTONMAXGLYPH192, "ButtonMaxGlyph192")
    TM_PART(25, VISTA_DWMWP, BUTTONMINGLYPH96, "ButtonMinGlyph96")
    TM_PART(26, VISTA_DWMWP, BUTTONMINGLYPH120, "ButtonMinGlyph120")
    TM_PART(27, VISTA_DWMWP, BUTTONMINGLYPH144, "ButtonMinGlyph144")
    TM_PART(28, VISTA_DWMWP, BUTTONMINGLYPH192, "ButtonMinGlyph192")
    TM_PART(29, VISTA_DWMWP, BUTTONRESTOREGLYPH96, "ButtonRestoreGlyph96")
    TM_PART(30, VISTA_DWMWP, BUTTONRESTOREGLYPH120, "ButtonRestoreGlyph120")
    TM_PART(31, VISTA_DWMWP, BUTTONRESTOREGLYPH144, "ButtonRestoreGlyph144")
    TM_PART(32, VISTA_DWMWP, BUTTONRESTOREGLYPH192, "ButtonRestoreGlyph192")
    TM_PART(33, VISTA_DWMWP, LEFTFRAME, "LeftFrame")
    TM_PART(34, VISTA_DWMWP, LEFTHIGHLIGHTCORNER, "LeftHighlightCorner")
    TM_PART(35, VISTA_DWMWP, LEFTSHADOW, "LeftShadow")
    TM_PART(36, VISTA_DWMWP, FLIP3DBOTTOMFRAME, "Flip3DBottomFrame")
    TM_PART(37, VISTA_DWMWP, FLIP3DLEFTFRAME, "Flip3DLeftFrame")
    TM_PART(38, VISTA_DWMWP, FLIP3DRIGHTFRAME, "Flip3DRightFrame")
    TM_PART(39, VISTA_DWMWP, FLIP3DTOPFRAME, "Flip3DTopFrame")
    TM_PART(40, VISTA_DWMWP, REFLECTIONMAP, "ReflectionMap")
    TM_PART(41, VISTA_DWMWP, RIGHTFRAME, "RightFrame")
    TM_PART(42, VISTA_DWMWP, RIGHTHIGHLIGHTCORNER, "RightHighlightCorner")
    TM_PART(43, VISTA_DWMWP, RIGHTSHADOW, "RightShadow")
    TM_PART(44, VISTA_DWMWP, SIDEHIGHLIGHT, "SideHighlight")
    TM_PART(45, VISTA_DWMWP, SMALLBOTTOMFRAME, "SmallBottomFrame")
    TM_PART(46, VISTA_DWMWP, SMALLBUTTONACTIVECLOSE, "SmallButtonActiveClose")
    TM_PART(47, VISTA_DWMWP, SMALLBUTTONINACTIVECLOSE, "SmallButtonInactiveClose")
    TM_PART(48, VISTA_DWMWP, SMALLBUTTONCLOSEGLOW, "SmallButtonCloseGlow")
    TM_PART(49, VISTA_DWMWP, SMALLBUTTONCLOSEGLYPH96, "SmallButtonCloseGlyph96")
    TM_PART(50, VISTA_DWMWP, SMALLBUTTONCLOSEGLYPH120, "SmallButtonCloseGlyph120")
    TM_PART(51, VISTA_DWMWP, SMALLBUTTONCLOSEGLYPH144, "SmallButtonCloseGlyph144")
    TM_PART(52, VISTA_DWMWP, SMALLBUTTONCLOSEGLYPH192, "SmallButtonCloseGlyph192")
    TM_PART(53, VISTA_DWMWP, SMALLLEFTFRAME, "SmallLeftFrame")
    TM_PART(54, VISTA_DWMWP, SMALLRIGHTFRAME, "SmallRightFrame")
    TM_PART(55, VISTA_DWMWP, SMALLTOPFRAME, "SmallTopFrame")
    TM_PART(56, VISTA_DWMWP, TEXTGLOW, "TextGlow")
    TM_PART(57, VISTA_DWMWP, TOPFRAME, "TopFrame")
    TM_PART(58, VISTA_DWMWP, TOPSHADOW, "TopShadow")
END_TM_CLASS_PARTS()

BEGIN_TM_CLASS_PARTS_FOR_OS(DWMWINDOW, ESupportedOS::Win7)
    TM_PART(1, W7_DWMWP, BOTTOMFRAME, "BottomFrame")
    TM_PART(2, W7_DWMWP, BOTTOMSHADOW, "BottomShadow")
    TM_PART(3, W7_DWMWP, BUTTONACTIVECAPTION, "ButtonActiveCaption")
    TM_PART(4, W7_DWMWP, BUTTONINACTIVECAPTION, "ButtonInactiveCaption")
    TM_PART(5, W7_DWMWP, BUTTONACTIVECAPTIONEND, "ButtonActiveCaptionEnd")
    TM_PART(6, W7_DWMWP, BUTTONINACTIVECAPTIONEND, "ButtonInactiveCaptionEnd")
    TM_PART(7, W7_DWMWP, BUTTONACTIVECLOSE, "ButtonActiveClose")
    TM_PART(8, W7_DWMWP, BUTTONINACTIVECLOSE, "ButtonInactiveClose")
    TM_PART(9, W7_DWMWP, BUTTONACTIVECLOSEALONE, "ButtonActiveCloseAlone")
    TM_PART(10, W7_DWMWP, BUTTONINACTIVECLOSEALONE, "ButtonInactiveCloseAlone")
    TM_PART(11, W7_DWMWP, BUTTONCLOSEGLOW, "ButtonCloseGlow")
    TM_PART(12, W7_DWMWP, BUTTONCLOSEGLYPH96, "ButtonCloseGlyph96")
    TM_PART(13, W7_DWMWP, BUTTONCLOSEGLYPH120, "ButtonCloseGlyph120")
    TM_PART(14, W7_DWMWP, BUTTONCLOSEGLYPH144, "ButtonCloseGlyph144")
    TM_PART(15, W7_DWMWP, BUTTONCLOSEGLYPH192, "ButtonCloseGlyph192")
    TM_PART(16, W7_DWMWP, BUTTONGLOW, "ButtonGlow")
    TM_PART(17, W7_DWMWP, BUTTONHELPGLYPH96, "ButtonHelpGlyph96")
    TM_PART(18, W7_DWMWP, BUTTONHELPGLYPH120, "ButtonHelpGlyph120")
    TM_PART(19, W7_DWMWP, BUTTONHELPGLYPH144, "ButtonHelpGlyph144")
    TM_PART(20, W7_DWMWP, BUTTONHELPGLYPH192, "ButtonHelpGlyph192")
    TM_PART(21, W7_DWMWP, BUTTONMAXGLYPH96, "ButtonMaxGlyph96")
    TM_PART(22, W7_DWMWP, BUTTONMAXGLYPH120, "ButtonMaxGlyph120")
    TM_PART(23, W7_DWMWP, BUTTONMAXGLYPH144, "ButtonMaxGlyph144")
    TM_PART(24, W7_DWMWP, BUTTONMAXGLYPH192, "ButtonMaxGlyph192")
    TM_PART(25, W7_DWMWP, BUTTONMINGLYPH96, "ButtonMinGlyph96")
    TM_PART(26, W7_DWMWP, BUTTONMINGLYPH120, "ButtonMinGlyph120")
    TM_PART(27, W7_DWMWP, BUTTONMINGLYPH144, "ButtonMinGlyph144")
    TM_PART(28, W7_DWMWP, BUTTONMINGLYPH192, "ButtonMinGlyph192")
    TM_PART(29, W7_DWMWP, BUTTONRESTOREGLYPH96, "ButtonRestoreGlyph96")
    TM_PART(30, W7_DWMWP, BUTTONRESTOREGLYPH120, "ButtonRestoreGlyph120")
    TM_PART(31, W7_DWMWP, BUTTONRESTOREGLYPH144, "ButtonRestoreGlyph144")
    TM_PART(32, W7_DWMWP, BUTTONRESTOREGLYPH192, "ButtonRestoreGlyph192")
    TM_PART(33, W7_DWMWP, LEFTFRAME, "LeftFrame")
    TM_PART(34, W7_DWMWP, LEFTHIGHLIGHTCORNER, "LeftHighlightCorner")
    TM_PART(35, W7_DWMWP, LEFTSHADOW, "LeftShadow")
    TM_PART(36, W7_DWMWP, FLIP3DBOTTOMFRAME, "Flip3DBottomFrame")
    TM_PART(37, W7_DWMWP, FLIP3DLEFTFRAME, "Flip3DLeftFrame")
    TM_PART(38, W7_DWMWP, FLIP3DRIGHTFRAME, "Flip3DRightFrame")
    TM_PART(39, W7_DWMWP, FLIP3DTOPFRAME, "Flip3DTopFrame")
    TM_PART(40, W7_DWMWP, REFLECTIONMAP, "ReflectionMap")
    TM_PART(41, W7_DWMWP, RIGHTFRAME, "RightFrame")
    TM_PART(42, W7_DWMWP, RIGHTSHADOW, "RightShadow")
    TM_PART(43, W7_DWMWP, SIDEHIGHLIGHT, "SideHighlight")
    TM_PART(44, W7_DWMWP, SMALLBOTTOMFRAME, "SmallBottomFrame")
    TM_PART(45, W7_DWMWP, SMALLBUTTONACTIVECLOSE, "SmallButtonActiveClose")
    TM_PART(46, W7_DWMWP, SMALLBUTTONINACTIVECLOSE, "SmallButtonInactiveClose")
    TM_PART(47, W7_DWMWP, SMALLBUTTONCLOSEGLOW, "SmallButtonCloseGlow")
    TM_PART(48, W7_DWMWP, SMALLBUTTONCLOSEGLYPH96, "SmallButtonCloseGlyph96")
    TM_PART(49, W7_DWMWP, SMALLBUTTONCLOSEGLYPH120, "SmallButtonCloseGlyph120")
    TM_PART(50, W7_DWMWP, SMALLBUTTONCLOSEGLYPH144, "SmallButtonCloseGlyph144")
    TM_PART(51, W7_DWMWP, SMALLBUTTONCLOSEGLYPH192, "SmallButtonCloseGlyph192")
    TM_PART(52, W7_DWMWP, SMALLLEFTFRAME, "SmallLeftFrame")
    TM_PART(53, W7_DWMWP, SMALLRIGHTFRAME, "SmallRightFrame")
    TM_PART(54, W7_DWMWP, SMALLTOPFRAME, "SmallTopFrame")
    TM_PART(55, W7_DWMWP, TEXTGLOW, "TextGlow")
    TM_PART(56, W7_DWMWP, TOPFRAME, "TopFrame")
    TM_PART(57, W7_DWMWP, TOPSHADOW, "TopShadow")
    TM_PART(58, W7_DWMWP, TOPFRAMENOSHADOW, "TopFrameNoShadow")
    TM_PART(59, W7_DWMWP, BOTTOMFRAMENOSHADOW, "BottomFrameNoShadow")
    TM_PART(60, W7_DWMWP, LEFTFRAMESQUEEGEE, "LeftFrameSqueegee")
    TM_PART(61, W7_DWMWP, RIGHTFRAMESQUEEGEE, "RightFrameSqueegee")
    TM_PART(62, W7_DWMWP, TOPFRAMESQUEEGEE, "TopFrameSqueegee")
    TM_PART(63, W7_DWMWP, BOTTOMFRAMESQUEEGEE, "BottomFrameSqueegee")
    TM_PART(64, W7_DWMWP, SMALLLEFTFRAMESQUEEGEE, "SmallLeftFrameSqueegee")
    TM_PART(65, W7_DWMWP, SMALLRIGHTFRAMESQUEEGEE, "SmallRightFrameSqueegee")
    TM_PART(66, W7_DWMWP, SMALLTOPFRAMESQUEEGEE, "SmallTopFrameSqueegee")
    TM_PART(67, W7_DWMWP, SMALLBOTTOMFRAMESQUEEGEE, "SmallBottomFrameSqueegee")
    TM_PART(68, W7_DWMWP, BITMAPPENDING, "BitmapPending")
    TM_PART(69, W7_DWMWP, RIPPLE, "Ripple")
    TM_PART(70, W7_DWMWP, TOPFRAMESQUEEGEENOSHADOW, "TopFrameSqueegeeNoShadow")
    TM_PART(71, W7_DWMWP, BOTTOMFRAMESQUEEGEENOSHADOW, "BottomFrameSqueegeeNoShadow")
END_TM_CLASS_PARTS()

BEGIN_TM_CLASS_PARTS_FOR_OS(DWMWINDOW, ESupportedOS::Win8 | ESupportedOS::Win81)
    TM_PART(1, W8_DWMWP, BOTTOMFRAME, "BottomFrame")
    TM_PART(2, W8_DWMWP, BOTTOMSHADOW, "BottomShadow")
    TM_PART(3, W8_DWMWP, BUTTONACTIVECAPTION, "ButtonActiveCaption")
    TM_PART(4, W8_DWMWP, BUTTONINACTIVECAPTION, "ButtonInactiveCaption")
    TM_PART(5, W8_DWMWP, BUTTONACTIVECAPTIONEND, "ButtonActiveCaptionEnd")
    TM_PART(6, W8_DWMWP, BUTTONINACTIVECAPTIONEND, "ButtonInactiveCaptionEnd")
    TM_PART(7, W8_DWMWP, BUTTONACTIVECLOSE, "ButtonActiveClose")
    TM_PART(8, W8_DWMWP, BUTTONINACTIVECLOSE, "ButtonInactiveClose")
    TM_PART(9, W8_DWMWP, BUTTONACTIVECLOSEALONE, "ButtonActiveCloseAlone")
    TM_PART(10, W8_DWMWP, BUTTONINACTIVECLOSEALONE, "ButtonInactiveCloseAlone")
    TM_PART(11, W8_DWMWP, BUTTONCLOSEGLOW, "ButtonCloseGlow")
    TM_PART(12, W8_DWMWP, BUTTONCLOSEGLYPH96, "ButtonCloseGlyph96")
    TM_PART(13, W8_DWMWP, BUTTONCLOSEGLYPH120, "ButtonCloseGlyph120")
    TM_PART(14, W8_DWMWP, BUTTONCLOSEGLYPH144, "ButtonCloseGlyph144")
    TM_PART(15, W8_DWMWP, BUTTONCLOSEGLYPH192, "ButtonCloseGlyph192")
    TM_PART(16, W8_DWMWP, BUTTONHELPGLYPH96, "ButtonHelpGlyph96")
    TM_PART(17, W8_DWMWP, BUTTONHELPGLYPH120, "ButtonHelpGlyph120")
    TM_PART(18, W8_DWMWP, BUTTONHELPGLYPH144, "ButtonHelpGlyph144")
    TM_PART(19, W8_DWMWP, BUTTONHELPGLYPH192, "ButtonHelpGlyph192")
    TM_PART(20, W8_DWMWP, BUTTONMAXGLYPH96, "ButtonMaxGlyph96")
    TM_PART(21, W8_DWMWP, BUTTONMAXGLYPH120, "ButtonMaxGlyph120")
    TM_PART(22, W8_DWMWP, BUTTONMAXGLYPH144, "ButtonMaxGlyph144")
    TM_PART(23, W8_DWMWP, BUTTONMAXGLYPH192, "ButtonMaxGlyph192")
    TM_PART(24, W8_DWMWP, BUTTONMINGLYPH96, "ButtonMinGlyph96")
    TM_PART(25, W8_DWMWP, BUTTONMINGLYPH120, "ButtonMinGlyph120")
    TM_PART(26, W8_DWMWP, BUTTONMINGLYPH144, "ButtonMinGlyph144")
    TM_PART(27, W8_DWMWP, BUTTONMINGLYPH192, "ButtonMinGlyph192")
    TM_PART(28, W8_DWMWP, BUTTONRESTOREGLYPH96, "ButtonRestoreGlyph96")
    TM_PART(29, W8_DWMWP, BUTTONRESTOREGLYPH120, "ButtonRestoreGlyph120")
    TM_PART(30, W8_DWMWP, BUTTONRESTOREGLYPH144, "ButtonRestoreGlyph144")
    TM_PART(31, W8_DWMWP, BUTTONRESTOREGLYPH192, "ButtonRestoreGlyph192")
    TM_PART(32, W8_DWMWP, LEFTFRAME, "LeftFrame")
    TM_PART(33, W8_DWMWP, LEFTSHADOW, "LeftShadow")
    TM_PART(34, W8_DWMWP, RIGHTFRAME, "RightFrame")
    TM_PART(35, W8_DWMWP, RIGHTSHADOW, "RightShadow")
    TM_PART(36, W8_DWMWP, SMALLBOTTOMFRAME, "SmallBottomFrame")
    TM_PART(37, W8_DWMWP, SMALLBUTTONACTIVECLOSE, "SmallButtonActiveClose")
    TM_PART(38, W8_DWMWP, SMALLBUTTONINACTIVECLOSE, "SmallButtonInactiveClose")
    TM_PART(39, W8_DWMWP, SMALLBUTTONCLOSEGLYPH96, "SmallButtonCloseGlyph96")
    TM_PART(40, W8_DWMWP, SMALLBUTTONCLOSEGLYPH120, "SmallButtonCloseGlyph120")
    TM_PART(41, W8_DWMWP, SMALLBUTTONCLOSEGLYPH144, "SmallButtonCloseGlyph144")
    TM_PART(42, W8_DWMWP, SMALLBUTTONCLOSEGLYPH192, "SmallButtonCloseGlyph192")
    TM_PART(43, W8_DWMWP, SMALLLEFTFRAME, "SmallLeftFrame")
    TM_PART(44, W8_DWMWP, SMALLRIGHTFRAME, "SmallRightFrame")
    TM_PART(45, W8_DWMWP, SMALLTOPFRAME, "SmallTopFrame")
    TM_PART(46, W8_DWMWP, TEXTGLOW, "TextGlow")
    TM_PART(47, W8_DWMWP, TOPFRAME, "TopFrame")
    TM_PART(48, W8_DWMWP, TOPSHADOW, "TopShadow")
    TM_PART(49, W8_DWMWP, TOPFRAMENOSHADOW, "TopFrameNoShadow")
    TM_PART(50, W8_DWMWP, BOTTOMFRAMENOSHADOW, "BottomFrameNoShadow")
    TM_PART(51, W8_DWMWP, LEFTFRAMESQUEEGEE, "LeftFrameSqueegee")
    TM_PART(52, W8_DWMWP, RIGHTFRAMESQUEEGEE, "RightFrameSqueegee")
    TM_PART(53, W8_DWMWP, TOPFRAMESQUEEGEE, "TopFrameSqueegee")
    TM_PART(54, W8_DWMWP, BOTTOMFRAMESQUEEGEE, "BottomFrameSqueegee")
    TM_PART(55, W8_DWMWP, SMALLLEFTFRAMESQUEEGEE, "SmallLeftFrameSqueegee")
    TM_PART(56, W8_DWMWP, SMALLRIGHTFRAMESQUEEGEE, "SmallRightFrameSqueegee")
    TM_PART(57, W8_DWMWP, SMALLTOPFRAMESQUEEGEE, "SmallTopFrameSqueegee")
    TM_PART(58, W8_DWMWP, SMALLBOTTOMFRAMESQUEEGEE, "SmallBottomFrameSqueegee")
    TM_PART(59, W8_DWMWP, BITMAPPENDING, "BitmapPending")
    TM_PART(60, W8_DWMWP, RIPPLE, "Ripple")
    TM_PART(61, W8_DWMWP, TOPFRAMESQUEEGEENOSHADOW, "TopFrameSqueegeeNoShadow")
    TM_PART(62, W8_DWMWP, BOTTOMFRAMESQUEEGEENOSHADOW, "BottomFrameSqueegeeNoShadow")
    TM_PART(63, W8_DWMWP, SQUEEGEEREFLECTIONMAP, "SqueegeeReflectionMap")
    TM_PART(64, W8_DWMWP, THUMBNAILBORDER, "ThumbnailBorder")
    // Windows 10 TH2 (10586)+:
    TM_PART(65, W8_DWMWP, BUTTONCLOSEGLYPH96DARK, "ButtonCloseGlyph96Dark")
    TM_PART(66, W8_DWMWP, BUTTONCLOSEGLYPH120DARK, "ButtonCloseGlyph120Dark")
    TM_PART(67, W8_DWMWP, BUTTONCLOSEGLYPH144DARK, "ButtonCloseGlyph144Dark")
    TM_PART(68, W8_DWMWP, BUTTONCLOSEGLYPH192DARK, "ButtonCloseGlyph192Dark")
    TM_PART(69, W8_DWMWP, BUTTONHELPGLYPH96DARK, "ButtonHelpGlyph96Dark")
    TM_PART(70, W8_DWMWP, BUTTONHELPGLYPH120DARK, "ButtonHelpGlyph120Dark")
    TM_PART(71, W8_DWMWP, BUTTONHELPGLYPH144DARK, "ButtonHelpGlyph144Dark")
    TM_PART(72, W8_DWMWP, BUTTONHELPGLYPH192DARK, "ButtonHelpGlyph192Dark")
    TM_PART(73, W8_DWMWP, BUTTONMAXGLYPH96DARK, "ButtonMaxGlyph96Dark")
    TM_PART(74, W8_DWMWP, BUTTONMAXGLYPH120DARK, "ButtonMaxGlyph120Dark")
    TM_PART(75, W8_DWMWP, BUTTONMAXGLYPH144DARK, "ButtonMaxGlyph144Dark")
    TM_PART(76, W8_DWMWP, BUTTONMAXGLYPH192DARK, "ButtonMaxGlyph192Dark")
    TM_PART(77, W8_DWMWP, BUTTONMINGLYPH96DARK, "ButtonMinGlyph96Dark")
    TM_PART(78, W8_DWMWP, BUTTONMINGLYPH120DARK, "ButtonMinGlyph120Dark")
    TM_PART(79, W8_DWMWP, BUTTONMINGLYPH144DARK, "ButtonMinGlyph144Dark")
    TM_PART(80, W8_DWMWP, BUTTONMINGLYPH192DARK, "ButtonMinGlyph192Dark")
    TM_PART(81, W8_DWMWP, BUTTONRESTOREGLYPH96DARK, "ButtonRestoreGlyph96Dark")
    TM_PART(82, W8_DWMWP, BUTTONRESTOREGLYPH120DARK, "ButtonRestoreGlyph120Dark")
    TM_PART(83, W8_DWMWP, BUTTONRESTOREGLYPH144DARK, "ButtonRestoreGlyph144Dark")
    TM_PART(84, W8_DWMWP, BUTTONRESTOREGLYPH192DARK, "ButtonRestoreGlyph192Dark")
    TM_PART(85, W8_DWMWP, SMALLBUTTONCLOSEGLYPH96DARK, "SmallButtonCloseGlyph96Dark")
    TM_PART(86, W8_DWMWP, SMALLBUTTONCLOSEGLYPH120DARK, "SmallButtonCloseGlyph120Dark")
    TM_PART(87, W8_DWMWP, SMALLBUTTONCLOSEGLYPH144DARK, "SmallButtonCloseGlyph144Dark")
    TM_PART(88, W8_DWMWP, SMALLBUTTONCLOSEGLYPH192DARK, "SmallButtonCloseGlyph192Dark")
    TM_PART(89, W8_DWMWP, BUTTONACTIVECAPTIONDARK, "ButtonActiveCaptionDark")
    TM_PART(90, W8_DWMWP, BUTTONINACTIVECAPTIONDARK, "ButtonInactiveCaptionDark")
    TM_PART(91, W8_DWMWP, BUTTONACTIVECAPTIONENDDARK, "ButtonActiveCaptionEndDark")
    TM_PART(92, W8_DWMWP, BUTTONINACTIVECAPTIONENDDARK, "ButtonInactiveCaptionEndDark")
END_TM_CLASS_PARTS()

BEGIN_TM_CLASS_PARTS(DWMWINDOW)
    TM_PART(1, DWMWP, BOTTOMFRAME, "BottomFrame")
    TM_PART(2, DWMWP, BOTTOMSHADOW, "BottomShadow")
    TM_PART(3, DWMWP, BUTTONACTIVECAPTION, "ButtonActiveCaption")
    TM_PART(4, DWMWP, BUTTONINACTIVECAPTION, "ButtonInactiveCaption")
    TM_PART(5, DWMWP, BUTTONACTIVECAPTIONEND, "ButtonActiveCaptionEnd")
    TM_PART(6, DWMWP, BUTTONINACTIVECAPTIONEND, "ButtonInactiveCaptionEnd")
    TM_PART(7, DWMWP, BUTTONACTIVECLOSE, "ButtonActiveClose")
    TM_PART(8, DWMWP, BUTTONINACTIVECLOSE, "ButtonInactiveClose")
    TM_PART(9, DWMWP, BUTTONACTIVECLOSEALONE, "ButtonActiveCloseAlone")
    TM_PART(10, DWMWP, BUTTONINACTIVECLOSEALONE, "ButtonInactiveCloseAlone")
    TM_PART(11, DWMWP, BUTTONCLOSEGLYPH96, "ButtonCloseGlyph96")
    TM_PART(12, DWMWP, BUTTONCLOSEGLYPH120, "ButtonCloseGlyph120")
    TM_PART(13, DWMWP, BUTTONCLOSEGLYPH144, "ButtonCloseGlyph144")
    TM_PART(14, DWMWP, BUTTONCLOSEGLYPH192, "ButtonCloseGlyph192")
    TM_PART(15, DWMWP, BUTTONHELPGLYPH96, "ButtonHelpGlyph96")
    TM_PART(16, DWMWP, BUTTONHELPGLYPH120, "ButtonHelpGlyph120")
    TM_PART(17, DWMWP, BUTTONHELPGLYPH144, "ButtonHelpGlyph144")
    TM_PART(18, DWMWP, BUTTONHELPGLYPH192, "ButtonHelpGlyph192")
    TM_PART(19, DWMWP, BUTTONMAXGLYPH96, "ButtonMaxGlyph96")
    TM_PART(20, DWMWP, BUTTONMAXGLYPH120, "ButtonMaxGlyph120")
    TM_PART(21, DWMWP, BUTTONMAXGLYPH144, "ButtonMaxGlyph144")
    TM_PART(22, DWMWP, BUTTONMAXGLYPH192, "ButtonMaxGlyph192")
    TM_PART(23, DWMWP, BUTTONMINGLYPH96, "ButtonMinGlyph96")
    TM_PART(24, DWMWP, BUTTONMINGLYPH120, "ButtonMinGlyph120")
    TM_PART(25, DWMWP, BUTTONMINGLYPH144, "ButtonMinGlyph144")
    TM_PART(26, DWMWP, BUTTONMINGLYPH192, "ButtonMinGlyph192")
    TM_PART(27, DWMWP, BUTTONRESTOREGLYPH96, "ButtonRestoreGlyph96")
    TM_PART(28, DWMWP, BUTTONRESTOREGLYPH120, "ButtonRestoreGlyph120")
    TM_PART(29, DWMWP, BUTTONRESTOREGLYPH144, "ButtonRestoreGlyph144")
    TM_PART(30, DWMWP, BUTTONRESTOREGLYPH192, "ButtonRestoreGlyph192")
    TM_PART(31, DWMWP, LEFTFRAME, "LeftFrame")
    TM_PART(32, DWMWP, LEFTSHADOW, "LeftShadow")
    TM_PART(33, DWMWP, RIGHTFRAME, "RightFrame")
    TM_PART(34, DWMWP, RIGHTSHADOW, "RightShadow")
    TM_PART(35, DWMWP, SMALLBOTTOMFRAME, "SmallBottomFrame")
    TM_PART(36, DWMWP, SMALLBUTTONACTIVECLOSE, "SmallButtonActiveClose")
    TM_PART(37, DWMWP, SMALLBUTTONINACTIVECLOSE, "SmallButtonInactiveClose")
    TM_PART(38, DWMWP, SMALLBUTTONCLOSEGLYPH96, "SmallButtonCloseGlyph96")
    TM_PART(39, DWMWP, SMALLBUTTONCLOSEGLYPH120, "SmallButtonCloseGlyph120")
    TM_PART(40, DWMWP, SMALLBUTTONCLOSEGLYPH144, "SmallButtonCloseGlyph144")
    TM_PART(41, DWMWP, SMALLBUTTONCLOSEGLYPH192, "SmallButtonCloseGlyph192")
    TM_PART(42, DWMWP, SMALLLEFTFRAME, "SmallLeftFrame")
    TM_PART(43, DWMWP, SMALLRIGHTFRAME, "SmallRightFrame")
    TM_PART(44, DWMWP, SMALLTOPFRAME, "SmallTopFrame")
    TM_PART(45, DWMWP, TEXTGLOW, "TextGlow")
    TM_PART(46, DWMWP, TOPFRAME, "TopFrame")
    TM_PART(47, DWMWP, TOPSHADOW, "TopShadow")
    TM_PART(48, DWMWP, TOPFRAMENOSHADOW, "TopFrameNoShadow")
    TM_PART(49, DWMWP, BOTTOMFRAMENOSHADOW, "BottomFrameNoShadow")
    TM_PART(50, DWMWP, LEFTFRAMESQUEEGEE, "LeftFrameSqueegee")
    TM_PART(51, DWMWP, RIGHTFRAMESQUEEGEE, "RightFrameSqueegee")
    TM_PART(52, DWMWP, TOPFRAMESQUEEGEE, "TopFrameSqueegee")
    TM_PART(53, DWMWP, BOTTOMFRAMESQUEEGEE, "BottomFrameSqueegee")
    TM_PART(54, DWMWP, SMALLLEFTFRAMESQUEEGEE, "SmallLeftFrameSqueegee")
    TM_PART(55, DWMWP, SMALLRIGHTFRAMESQUEEGEE, "SmallRightFrameSqueegee")
    TM_PART(56, DWMWP, SMALLTOPFRAMESQUEEGEE, "SmallTopFrameSqueegee")
    TM_PART(57, DWMWP, SMALLBOTTOMFRAMESQUEEGEE, "SmallBottomFrameSqueegee")
    TM_PART(58, DWMWP, BITMAPPENDING, "BitmapPending")
    TM_PART(59, DWMWP, RIPPLE, "Ripple")
    TM_PART(60, DWMWP, TOPFRAMESQUEEGEENOSHADOW, "TopFrameSqueegeeNoShadow")
    TM_PART(61, DWMWP, BOTTOMFRAMESQUEEGEENOSHADOW, "BottomFrameSqueegeeNoShadow")
    TM_PART(62, DWMWP, SQUEEGEEREFLECTIONMAP, "SqueegeeReflectionMap")
    TM_PART(63, DWMWP, THUMBNAILBORDER, "ThumbnailBorder")
    TM_PART(64, DWMWP, BUTTONCLOSEGLYPH96DARK, "ButtonCloseGlyph96Dark")
    TM_PART(65, DWMWP, BUTTONCLOSEGLYPH120DARK, "ButtonCloseGlyph120Dark")
    TM_PART(66, DWMWP, BUTTONCLOSEGLYPH144DARK, "ButtonCloseGlyph144Dark")
    TM_PART(67, DWMWP, BUTTONCLOSEGLYPH192DARK, "ButtonCloseGlyph192Dark")
    TM_PART(68, DWMWP, BUTTONHELPGLYPH96DARK, "ButtonHelpGlyph96Dark")
    TM_PART(69, DWMWP, BUTTONHELPGLYPH120DARK, "ButtonHelpGlyph120Dark")
    TM_PART(70, DWMWP, BUTTONHELPGLYPH144DARK, "ButtonHelpGlyph144Dark")
    TM_PART(71, DWMWP, BUTTONHELPGLYPH192DARK, "ButtonHelpGlyph192Dark")
    TM_PART(72, DWMWP, BUTTONMAXGLYPH96DARK, "ButtonMaxGlyph96Dark")
    TM_PART(73, DWMWP, BUTTONMAXGLYPH120DARK, "ButtonMaxGlyph120Dark")
    TM_PART(74, DWMWP, BUTTONMAXGLYPH144DARK, "ButtonMaxGlyph144Dark")
    TM_PART(75, DWMWP, BUTTONMAXGLYPH192DARK, "ButtonMaxGlyph192Dark")
    TM_PART(76, DWMWP, BUTTONMINGLYPH96DARK, "ButtonMinGlyph96Dark")
    TM_PART(77, DWMWP, BUTTONMINGLYPH120DARK, "ButtonMinGlyph120Dark")
    TM_PART(78, DWMWP, BUTTONMINGLYPH144DARK, "ButtonMinGlyph144Dark")
    TM_PART(79, DWMWP, BUTTONMINGLYPH192DARK, "ButtonMinGlyph192Dark")
    TM_PART(80, DWMWP, BUTTONRESTOREGLYPH96DARK, "ButtonRestoreGlyph96Dark")
    TM_PART(81, DWMWP, BUTTONRESTOREGLYPH120DARK, "ButtonRestoreGlyph120Dark")
    TM_PART(82, DWMWP, BUTTONRESTOREGLYPH144DARK, "ButtonRestoreGlyph144Dark")
    TM_PART(83, DWMWP, BUTTONRESTOREGLYPH192DARK, "ButtonRestoreGlyph192Dark")
    TM_PART(84, DWMWP, SMALLBUTTONCLOSEGLYPH96DARK, "SmallButtonCloseGlyph96Dark")
    TM_PART(85, DWMWP, SMALLBUTTONCLOSEGLYPH120DARK, "SmallButtonCloseGlyph120Dark")
    TM_PART(86, DWMWP, SMALLBUTTONCLOSEGLYPH144DARK, "SmallButtonCloseGlyph144Dark")
    TM_PART(87, DWMWP, SMALLBUTTONCLOSEGLYPH192DARK, "SmallButtonCloseGlyph192Dark")
    TM_PART(88, DWMWP, BUTTONACTIVECAPTIONDARK, "ButtonActiveCaptionDark")
    TM_PART(89, DWMWP, BUTTONINACTIVECAPTIONDARK, "ButtonInactiveCaptionDark")
    TM_PART(90, DWMWP, BUTTONACTIVECAPTIONENDDARK, "ButtonActiveCaptionEndDark")
    TM_PART(91, DWMWP, BUTTONINACTIVECAPTIONENDDARK, "ButtonInactiveCaptionEndDark")
END_TM_CLASS_PARTS()
//---------------------------------------------------------------------------------------
//   "Animations" Parts & States (Windows 8+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(ANIMATIONS)
    TM_PART(1, TAS, EXPAND, "Expand")
    TM_PART(2, TAS, COLLAPSE, "Collapse")
    TM_PART(3, TAS, REPOSITION, "Reposition")
    TM_PART(4, TAS, FADEIN, "FadeIn")
    TM_PART(5, TAS, FADEOUT, "FadeOut")
    TM_PART(6, TAS, ADDTOLIST, "AddToList")
    TM_PART(7, TAS, DELETEFROMLIST, "DeleteFromList")
    TM_PART(8, TAS, ADDTOGRID, "AddToGrid")
    TM_PART(9, TAS, DELETEFROMGRID, "DeleteFromGrid")
    TM_PART(10, TAS, ADDTOSEARCHGRID, "AddToSearchGrid")
    TM_PART(11, TAS, DELETEFROMSEARCHGRID, "DeleteFromSearchGrid")
    TM_PART(12, TAS, ADDTOSEARCHLIST, "AddToSearchList")
    TM_PART(13, TAS, DELETEFROMSEARCHLIST, "DeleteFromSearchList")
    TM_PART(14, TAS, SHOWEDGEUI, "ShowEdgeUI")
    TM_PART(15, TAS, SHOWPANEL, "ShowPanel")
    TM_PART(16, TAS, HIDEEDGEUI, "HideEdgeUI")
    TM_PART(17, TAS, HIDEPANEL, "HidePanel")
    TM_PART(18, TAS, SHOWPOPUP, "ShowPopup")
    TM_PART(19, TAS, HIDEPOPUP, "HidePopup")
    TM_PART(20, TAS, POINTERDOWN, "PointerDown")
    TM_PART(21, TAS, POINTERUP, "PointerUp")
    TM_PART(22, TAS, DRAGSOURCESTART, "DragSourceStart")
    TM_PART(23, TAS, DRAGSOURCEEND, "DragSourceEnd")
    TM_PART(24, TAS, TRANSITIONCONTENT, "TransitionContent")
    TM_PART(25, TAS, REVEAL, "Reveal")
    TM_PART(26, TAS, HIDE, "Hide")
    TM_PART(27, TAS, DRAGBETWEENENTER, "DragBetweenEnter")
    TM_PART(28, TAS, DRAGBETWEENLEAVE, "DragBetweenLeave")
    TM_PART(29, TAS, SWIPESELECT, "SwipeSelect")
    TM_PART(30, TAS, SWIPEDESELECT, "SwipeDeselect")
    TM_PART(31, TAS, SWIPEREVEAL, "SwipeReveal")
    TM_PART(32, TAS, ENTERPAGE, "EnterPage")
    TM_PART(33, TAS, TRANSITIONPAGE, "TransitionPage")
    TM_PART(34, TAS, CROSSFADE, "CrossFade")
    TM_PART(35, TAS, PEEK, "Peek")
    TM_PART(36, TAS, UPDATEBADGE, "UpdateBadge")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(EXPAND)
    TM_STATE(1, TA_EXPAND, AFFECTED, "Affected")
    TM_STATE(2, TA_EXPAND, REVEALED, "Revealed")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(COLLAPSE)
    TM_STATE(1, TA_COLLAPSE, HIDDEN, "Hidden")
    TM_STATE(2, TA_COLLAPSE, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(REPOSITION)
    TM_STATE(1, TA_REPOSITION, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(FADEIN)
    TM_STATE(1, TA_FADEIN, SHOWN, "Shown")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(FADEOUT)
    TM_STATE(1, TA_FADEOUT, HIDDEN, "Hidden")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(ADDTOLIST)
    TM_STATE(1, TA_ADDTOLIST, ADDED, "Added")
    TM_STATE(2, TA_ADDTOLIST, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DELETEFROMLIST)
    TM_STATE(1, TA_DELETEFROMLIST, DELETED, "Deleted")
    TM_STATE(2, TA_DELETEFROMLIST, REMAINING, "Remaining")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(ADDTOGRID)
    TM_STATE(1, TA_ADDTOGRID, ADDED, "Added")
    TM_STATE(2, TA_ADDTOGRID, AFFECTED, "Affected")
    TM_STATE(3, TA_ADDTOGRID, ROWOUT, "RowOut")
    TM_STATE(4, TA_ADDTOGRID, ROWIN, "RowIn")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DELETEFROMGRID)
    TM_STATE(1, TA_DELETEFROMGRID, DELETED, "Deleted")
    TM_STATE(2, TA_DELETEFROMGRID, REMAINING, "Remaining")
    TM_STATE(3, TA_DELETEFROMGRID, ROWOUT, "RowOut")
    TM_STATE(4, TA_DELETEFROMGRID, ROWIN, "RowIn")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(ADDTOSEARCHGRID)
    TM_STATE(1, TA_ADDTOSEARCHGRID, ADDED, "Added")
    TM_STATE(2, TA_ADDTOSEARCHGRID, AFFECTED, "Affected")
    TM_STATE(3, TA_ADDTOSEARCHGRID, ROWOUT, "RowOut")
    TM_STATE(4, TA_ADDTOSEARCHGRID, ROWIN, "RowIn")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DELETEFROMSEARCHGRID)
    TM_STATE(1, TA_DELETEFROMSEARCHGRID, DELETED, "Deleted")
    TM_STATE(2, TA_DELETEFROMSEARCHGRID, REMAINING, "Remaining")
    TM_STATE(3, TA_DELETEFROMSEARCHGRID, ROWOUT, "RowOut")
    TM_STATE(4, TA_DELETEFROMSEARCHGRID, ROWIN, "RowIn")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(ADDTOSEARCHLIST)
    TM_STATE(1, TA_ADDTOSEARCHLIST, ADDED, "Added")
    TM_STATE(2, TA_ADDTOSEARCHLIST, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DELETEFROMSEARCHLIST)
    TM_STATE(1, TA_DELETEFROMSEARCHLIST, DELETED, "Deleted")
    TM_STATE(2, TA_DELETEFROMSEARCHLIST, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SHOWEDGEUI)
    TM_STATE(1, TA_SHOWEDGEUI, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SHOWPANEL)
    TM_STATE(1, TA_SHOWPANEL, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HIDEEDGEUI)
    TM_STATE(1, TA_HIDEEDGEUI, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HIDEPANEL)
    TM_STATE(1, TA_HIDEPANEL, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SHOWPOPUP)
    TM_STATE(1, TA_SHOWPOPUP, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HIDEPOPUP)
    TM_STATE(1, TA_HIDEPOPUP, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(POINTERDOWN)
    TM_STATE(1, TA_POINTERDOWN, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(POINTERUP)
    TM_STATE(1, TA_POINTERUP, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DRAGSOURCESTART)
    TM_STATE(1, TA_DRAGSOURCESTART, DRAGSOURCE, "DragSource")
    TM_STATE(2, TA_DRAGSOURCESTART, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DRAGSOURCEEND)
    TM_STATE(1, TA_DRAGSOURCEEND, DRAGSOURCE, "DragSource")
    TM_STATE(2, TA_DRAGSOURCEEND, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRANSITIONCONTENT)
    TM_STATE(1, TA_TRANSITIONCONTENT, INCOMING, "Incoming")
    TM_STATE(2, TA_TRANSITIONCONTENT, OUTGOING, "Outgoing")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(REVEAL)
    TM_STATE(1, TA_REVEAL, BACKGROUND, "Background")
    TM_STATE(2, TA_REVEAL, CONTENT, "Content")
    TM_STATE(3, TA_REVEAL, OUTLINE, "Outline")
    TM_STATE(4, TA_REVEAL, TAPPED, "Tapped")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(HIDE)
    TM_STATE(1, TA_HIDE, BACKGROUND, "Background")
    TM_STATE(2, TA_HIDE, CONTENT, "Content")
    TM_STATE(3, TA_HIDE, OUTLINE, "Outline")
    TM_STATE(4, TA_HIDE, TAPPED, "Tapped")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DRAGBETWEENENTER)
    TM_STATE(1, TA_DRAGBETWEENENTER, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(DRAGBETWEENLEAVE)
    TM_STATE(1, TA_DRAGBETWEENLEAVE, AFFECTED, "Affected")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SWIPESELECT)
    TM_STATE(1, TA_SWIPESELECT, SELECTED, "Selected")
    TM_STATE(2, TA_SWIPESELECT, SELECTION, "Selection")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SWIPEDESELECT)
    TM_STATE(1, TA_SWIPEDESELECT, DESELECTED, "Deselected")
    TM_STATE(2, TA_SWIPEDESELECT, SELECTION, "Selection")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(SWIPEREVEAL)
    TM_STATE(1, TA_SWIPEREVEAL, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(ENTERPAGE)
    TM_STATE(1, TA_ENTERPAGE, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(TRANSITIONPAGE)
    TM_STATE(1, TA_TRANSITIONPAGE, INCOMING, "Incoming")
    TM_STATE(2, TA_TRANSITIONPAGE, OUTGOING, "Outgoing")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(CROSSFADE)
    TM_STATE(1, TA_CROSSFADE, INCOMING, "Incoming")
    TM_STATE(2, TA_CROSSFADE, OUTGOING, "Outgoing")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(PEEK)
    TM_STATE(1, TA_PEEK, TARGET, "Target")
END_TM_PART_STATES()

BEGIN_TM_PART_STATES(UPDATEBADGE)
    TM_STATE(1, TA_UPDATEBADGE, INCOMING, "Incoming")
    TM_STATE(2, TA_UPDATEBADGE, OUTGOING, "Outgoing")
END_TM_PART_STATES()

//---------------------------------------------------------------------------------------
//   "Advanced Task Manager" Parts & States (Windows 8+)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(TASKMANAGER)
    TM_PART(1, TM, ROW, "Row")
    TM_PART(2, TM, COLUMNDIVIDER, "ColumnDivider")
    TM_PART(3, TM, HEATMAP_COLOR0, "Heatmap_Color0")
    TM_PART(4, TM, HEATMAP_COLOR1, "Heatmap_Color1")
    TM_PART(5, TM, HEATMAP_COLOR2, "Heatmap_Color2")
    TM_PART(6, TM, HEATMAP_COLOR3, "Heatmap_Color3")
    TM_PART(7, TM, HEATMAP_COLOR4, "Heatmap_Color4")
    TM_PART(8, TM, HEATMAP_COLOR5, "Heatmap_Color5")
    TM_PART(9, TM, HEATMAP_COLOR6, "Heatmap_Color6")
    TM_PART(10, TM, HEATMAP_COLOR7, "Heatmap_Color7")
    TM_PART(11, TM, HEATMAP_COLOR8, "Heatmap_Color8")
    TM_PART(12, TM, HEATMAP_CONTENTIONMARKER, "Heatmap_ContentionMarker")
    TM_PART(13, TM, HEATMAP_TEXT, "Heatmap_Text")
    TM_PART(14, TM, RESOURCE_TITLE, "Resource_Title")
    TM_PART(15, TM, RESOURCE_SUBTITLE, "Resource_Subtitle")
    TM_PART(16, TM, GROUPLABEL, "GroupLabel")
    TM_PART(17, TM, STATICLABEL, "StaticLabel")
    TM_PART(18, TM, STATICDATA, "StaticData")
    TM_PART(19, TM, DYNAMICDATA, "DynamicData")
    TM_PART(20, TM, MEMORYBAR_AVAILABLE, "MemoryBar_Available")
    TM_PART(21, TM, MEMORYBAR_INUSE, "MemoryBar_InUse")
    TM_PART(22, TM, MEMORYBAR_MODIFIED, "MemoryBar_Modified")
    TM_PART(23, TM, MEMORYBAR_INUSE_SEPARATOR, "MemoryBar_InUse_Separator")
    TM_PART(24, TM, COLUMNHEADERTEXT, "ColumnHeaderText")
    TM_PART(25, TM, CPU_HEATMAP_TEXT, "CPU_Heatmap_Text")
    TM_PART(26, TM, CPU_HEATMAP_COLOR0, "CPU_Heatmap_Color0")
    TM_PART(27, TM, CPU_HEATMAP_COLOR1, "CPU_Heatmap_Color1")
    TM_PART(28, TM, CPU_HEATMAP_COLOR2, "CPU_Heatmap_Color2")
    TM_PART(29, TM, CPU_HEATMAP_COLOR3, "CPU_Heatmap_Color3")
    TM_PART(30, TM, CPU_HEATMAP_COLOR4, "CPU_Heatmap_Color4")
    TM_PART(31, TM, CPU_HEATMAP_COLOR5, "CPU_Heatmap_Color5")
    TM_PART(32, TM, CPU_HEATMAP_PARKED0, "CPU_Heatmap_Parked0")
    TM_PART(33, TM, CPU_HEATMAP_PARKED1, "CPU_Heatmap_Parked1")
    TM_PART(34, TM, CPU_HEATMAP_PARKED2, "CPU_Heatmap_Parked2")
    TM_PART(35, TM, CPU_HEATMAP_PARKED3, "CPU_Heatmap_Parked3")
    TM_PART(36, TM, CPU_HEATMAP_PARKED4, "CPU_Heatmap_Parked4")
    TM_PART(37, TM, CPU_HEATMAP_PARKED5, "CPU_Heatmap_Parked5")
    TM_PART(38, TM, CPU_HEATMAP_GRID_BORDER, "CPU_Heatmap_Grid_Border")
    TM_PART(39, TM, CPU_HEATMAP_BLOCK_BORDER_R, "CPU_Heatmap_Block_Border_R")
    TM_PART(40, TM, CPU_HEATMAP_BLOCK_BORDER_B, "CPU_Heatmap_Block_Border_B")
    TM_PART(41, TM, COLHEADER_DIVIDER_COLOR1, "ColHeader_Divider_Color1")
    TM_PART(42, TM, COLHEADER_DIVIDER_COLOR2, "ColHeader_Divider_Color2")
    TM_PART(43, TM, CONTENTION_COLUMN_HEADER, "Contention_Column_Header")
    TM_PART(44, TM, CHARTLEGEND, "ChartLegend")
    TM_PART(45, TM, COLUMNDIVIDERSELECTED, "ColumnDividerSelected")
    TM_PART(46, TM, COLUMNDIVIDERHOT, "ColumnDividerHot")
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(COLHEADER)
    TM_STATE(1, TM_COLHEADER, NORMAL, "Normal")
END_TM_PART_STATES()

BEGIN_TM_CLASS_PARTS(CHARTVIEW)
    TM_PART(1, CV, CPU_BG, "CPU_BG")
    TM_PART(2, CV, CPU_BORDER, "CPU_Border")
    TM_PART(3, CV, CPU_GRID, "CPU_Grid")
    TM_PART(4, CV, CPU_LINE1, "CPU_Line1")
    TM_PART(5, CV, CPU_LINE1FILL, "CPU_Line1Fill")
    TM_PART(6, CV, CPU_LINE2, "CPU_Line2")
    TM_PART(7, CV, CPU_LINE2FILL, "CPU_Line2Fill")
    TM_PART(8, CV, MEMORY_BG, "Memory_BG")
    TM_PART(9, CV, MEMORY_BORDER, "Memory_Border")
    TM_PART(10, CV, MEMORY_GRID, "Memory_Grid")
    TM_PART(11, CV, MEMORY_LINE1, "Memory_Line1")
    TM_PART(12, CV, MEMORY_LINE1FILL, "Memory_Line1Fill")
    TM_PART(13, CV, MEMORY_LINE2, "Memory_Line2")
    TM_PART(14, CV, MEMORY_LINE2FILL, "Memory_Line2Fill")
    TM_PART(15, CV, DISK_BG, "Disk_BG")
    TM_PART(16, CV, DISK_BORDER, "Disk_Border")
    TM_PART(17, CV, DISK_GRID, "Disk_Grid")
    TM_PART(18, CV, DISK_LINE1, "Disk_Line1")
    TM_PART(19, CV, DISK_LINE1FILL, "Disk_Line1Fill")
    TM_PART(20, CV, DISK_LINE2, "Disk_Line2")
    TM_PART(21, CV, DISK_LINE2FILL, "Disk_Line2Fill")
    TM_PART(22, CV, NETWORK_BG, "Network_BG")
    TM_PART(23, CV, NETWORK_BORDER, "Network_Border")
    TM_PART(24, CV, NETWORK_GRID, "Network_Grid")
    TM_PART(25, CV, NETWORK_LINE1, "Network_Line1")
    TM_PART(26, CV, NETWORK_LINE1FILL, "Network_Line1Fill")
    TM_PART(27, CV, NETWORK_LINE2, "Network_Line2")
    TM_PART(28, CV, NETWORK_LINE2FILL, "Network_Line2Fill")
    TM_PART(29, CV, COPY_BORDER, "Copy_Border")
    TM_PART(30, CV, COPY_GRID, "Copy_Grid")
    TM_PART(31, CV, COPY_LINE1, "Copy_Line1")
    TM_PART(32, CV, COPY_LINE1FILL, "Copy_Line1Fill")
    TM_PART(33, CV, COPY_LINE2, "Copy_Line2")
    TM_PART(34, CV, COPY_LINE2FILL, "Copy_Line2Fill")
    TM_PART(35, CV, CPU_SCALELINE, "CPU_ScaleLine")
    TM_PART(36, CV, CPU_SCALELINETEXT, "CPU_ScaleLineText")
    TM_PART(37, CV, MEMORY_SCALELINE, "Memory_ScaleLine")
    TM_PART(38, CV, MEMORY_SCALELINETEXT, "Memory_ScaleLineText")
    TM_PART(39, CV, DISK_SCALELINE, "Disk_ScaleLine")
    TM_PART(40, CV, DISK_SCALELINETEXT, "Disk_ScaleLineText")
    TM_PART(41, CV, NETWORK_SCALELINE, "Network_ScaleLine")
    TM_PART(42, CV, NETWORK_SCALELINETEXT, "Network_ScaleLineText")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Command Module" Parts & States (Windows Vista+)
//---------------------------------------------------------------------------------------

// Vista is same as 7 except for lacking ModuleBackgroundColor
BEGIN_TM_CLASS_PARTS_FOR_OS(COMMANDMODULE, ESupportedOS::WinVista)
    TM_PART(1, VISTA_CMOD, MODULEBACKGROUND, "ModuleBackground")
    TM_PART(2, VISTA_CMOD, TASKBUTTON, "TaskButton")
    TM_PART(3, VISTA_CMOD, SPLITBUTTONLEFT, "SplitButtonLeft")
    TM_PART(4, VISTA_CMOD, SPLITBUTTONRIGHT, "SplitButtonRight")
    TM_PART(5, VISTA_CMOD, MENUGLYPH, "MenuGlyph")
    TM_PART(6, VISTA_CMOD, OVERFLOWGLYPH, "OverflowGlyph")
    TM_PART(7, VISTA_CMOD, LIBRARYPANEMENUGLYPH, "LibraryPaneMenuGlyph")
    TM_PART(8, VISTA_CMOD, LIBRARYPANETOPVIEW, "LibraryPaneTopView")
    TM_PART(9, VISTA_CMOD, LIBRARYPANEIDENTIFIER, "LibraryPaneIdentifier")
    TM_PART(10, VISTA_CMOD, LIBRARYPANEBACKGROUND, "LibraryPaneBackground")
    TM_PART(11, VISTA_CMOD, LIBRARYPANEOVERLAY, "LibraryPaneOverlay")
END_TM_CLASS_PARTS()

// 7 is same as 8+ except for having LibraryPaneIdentifier and LibraryPaneOverlay
BEGIN_TM_CLASS_PARTS_FOR_OS(COMMANDMODULE, ESupportedOS::Win7)
    TM_PART(1, W7_CMOD, MODULEBACKGROUND, "ModuleBackground")
    TM_PART(2, W7_CMOD, MODULEBACKGROUNDCOLORS, "ModuleBackgroundColors")
    TM_PART(3, W7_CMOD, TASKBUTTON, "TaskButton")
    TM_PART(4, W7_CMOD, SPLITBUTTONLEFT, "SplitButtonLeft")
    TM_PART(5, W7_CMOD, SPLITBUTTONRIGHT, "SplitButtonRight")
    TM_PART(6, W7_CMOD, MENUGLYPH, "MenuGlyph")
    TM_PART(7, W7_CMOD, OVERFLOWGLYPH, "OverflowGlyph")
    TM_PART(8, W7_CMOD, LIBRARYPANEMENUGLYPH, "LibraryPaneMenuGlyph")
    TM_PART(9, W7_CMOD, LIBRARYPANETOPVIEW, "LibraryPaneTopView")
    TM_PART(10, W7_CMOD, LIBRARYPANEIDENTIFIER, "LibraryPaneIdentifier")
    TM_PART(11, W7_CMOD, LIBRARYPANEBACKGROUND, "LibraryPaneBackground")
    TM_PART(12, W7_CMOD, LIBRARYPANEOVERLAY, "LibraryPaneOverlay")
END_TM_CLASS_PARTS()

BEGIN_TM_CLASS_PARTS(COMMANDMODULE)
    TM_PART(1, CMOD, MODULEBACKGROUND, "ModuleBackground")
    TM_PART(2, CMOD, MODULEBACKGROUNDCOLORS, "ModuleBackgroundColors")
    TM_PART(3, CMOD, TASKBUTTON, "TaskButton")
    TM_PART(4, CMOD, SPLITBUTTONLEFT, "SplitButtonLeft")
    TM_PART(5, CMOD, SPLITBUTTONRIGHT, "SplitButtonRight")
    TM_PART(6, CMOD, MENUGLYPH, "MenuGlyph")
    TM_PART(7, CMOD, OVERFLOWGLYPH, "OverflowGlyph")
    TM_PART(8, CMOD, LIBRARYPANEMENUGLYPH, "LibraryPaneMenuGlyph")
    TM_PART(9, CMOD, LIBRARYPANETOPVIEW, "LibraryPaneTopView")
    TM_PART(10, CMOD, LIBRARYPANEBACKGROUND, "LibraryPaneBackground")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Common Items Dialog" Parts & States (Windows 10 ???)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(CommonItemsDialog)
    TM_PART(1, CID, POPUPGUTTER, "PopupGutter")
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "Common StatusBar" Parts & States (Windows 10 ???)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(ExplorerStatusBar)
    // Added sometime after 10.0.14361 but before 10.0.19041.546
    // Seems to be empty....?
    // Will look into it more eventually
END_TM_CLASS_PARTS()

//---------------------------------------------------------------------------------------
//   "File Explorer Banner Container" Parts & States (Windows 10 ???)
//---------------------------------------------------------------------------------------
BEGIN_TM_CLASS_PARTS(FileExplorerBannerContainer)
    TM_PART(1, FEBC, CLOSEGLYPH, "CloseGlyph")
END_TM_CLASS_PARTS()


//---------------------------------------------------------------------------
END_TM_SCHEMA(ThemeMgrSchema)
//---------------------------------------------------------------------------
} // namespace Restyle
#endif      // TMSCHEMA_H