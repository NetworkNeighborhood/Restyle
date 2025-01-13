/**
 * WRONG APPROACH JUST COPY STRINGS FROM HERE
 */

#include "restyle.h"
#include <vssym32.h>

template<typename T>
struct PreferredNameRenderingMap
{
	T eType;
	LPCSTR szRenderName;
};

const PreferredNameRenderingMap<BGTYPE> g_pnrBGTYPE[] = {
	{ BT_IMAGEFILE,  "ImageFile" },
	{ BT_BORDERFILL, "BorderFill" },
	{ BT_NONE,       "None" },
};

const PreferredNameRenderingMap<IMAGELAYOUT> g_pnrIMAGELAYOUT[] = {
	{ IL_VERTICAL,   "Vertical" },
	{ IL_HORIZONTAL, "Horizontal" },
};

const PreferredNameRenderingMap<BORDERTYPE> g_pnrBORDERTYPE[] = {
	{ BT_RECT,      "Rect" },
	{ BT_ROUNDRECT, "RoundRect" },
	{ BT_ELLIPSE,   "Ellipse" },
};

const PreferredNameRenderingMap<FILLTYPE> g_pnrFILLTYPE[] = {
	{ FT_SOLID,          "Solid" },
	{ FT_VERTGRADIENT,   "VertGradient" },
	{ FT_HORZGRADIENT,   "HorzGradient" },
	{ FT_RADIALGRADIENT, "RadialGradient" },
	{ FT_TILEIMAGE,      "TileImage" },
};

const PreferredNameRenderingMap<SIZINGTYPE> g_pnrSIZINGTYPE[] = {
	{ ST_TRUESIZE, "TrueSize" },
	{ ST_STRETCH,  "Stretch" },
	{ ST_TILE,     "Tile" },
};

const PreferredNameRenderingMap<HALIGN> g_pnrHALIGN[] = {
	{ HA_LEFT,   "Left" },
	{ HA_CENTER, "Center" },
	{ HA_RIGHT,  "Right" },
};

const PreferredNameRenderingMap<CONTENTALIGNMENT> g_pnrCONTENTALIGNMENT[] = {
	{ CA_LEFT,   "Left" },
	{ CA_CENTER, "Center" },
	{ CA_RIGHT,  "Right" },
};

const PreferredNameRenderingMap<VALIGN> g_pnrVALIGN[] = {
	{ VA_TOP,    "Top" },
	{ VA_CENTER, "Center" },
	{ VA_BOTTOM, "Bottom" },
};

const PreferredNameRenderingMap<OFFSETTYPE> g_pnrOFFSETTYPE[] = {
	{ OT_TOPLEFT,           "TopLeft" },
	{ OT_TOPRIGHT,          "TopRight" },
	{ OT_TOPMIDDLE,         "TopMiddle" },
	{ OT_BOTTOMLEFT,        "BottomLeft" },
	{ OT_BOTTOMRIGHT,       "BottomRight" },
	{ OT_BOTTOMMIDDLE,      "BottomMiddle" },
	{ OT_MIDDLELEFT,        "MiddleLeft" },
	{ OT_MIDDLERIGHT,       "MiddleRight" },
	{ OT_LEFTOFCAPTION,     "LeftOfCaption" },
	{ OT_RIGHTOFCAPTION,    "RightOfCaption" },
	{ OT_LEFTOFLASTBUTTON,  "LeftOfLastButton" },
	{ OT_RIGHTOFLASTBUTTON, "RightOfLastButton" },
	{ OT_ABOVELASTBUTTON,   "AboveLastButton" },
	{ OT_BELOWLASTBUTTON,   "BelowLastButton" },
};

const PreferredNameRenderingMap<ICONEFFECT> g_pnrICONEFFECT[] = {
	{ ICE_NONE,   "None" },
	{ ICE_GLOW,   "Glow" },
	{ ICE_SHADOW, "Shadow" },
	{ ICE_PULSE,  "Pulse" },
	{ ICE_ALPHA,  "Alpha" },
};

const PreferredNameRenderingMap<TEXTSHADOWTYPE> g_pnrTEXTSHADOWTYPE[] = {
	{ TST_NONE,       "None" },
	{ TST_SINGLE,     "Single" },
	{ TST_CONTINUOUS, "Continuous" },
};

const PreferredNameRenderingMap<GLYPHTYPE> g_pnrGLYPHTYPE[] = {
	{ GT_NONE,       "None" },
	{ GT_IMAGEGLYPH, "ImageGlyph" },
	{ GT_FONTGLYPH,  "FontGlyph" },
};

const PreferredNameRenderingMap<IMAGESELECTTYPE> g_pnrIMAGESELECTTYPE[] = {
	{ IST_NONE, "None" },
	{ IST_SIZE, "Size" },
	{ IST_DPI,  "DPI" },
};

const PreferredNameRenderingMap<TRUESIZESCALINGTYPE> g_pnrTRUESIZESCALINGTYPE[] = {
	{ TSST_NONE, "None" },
	{ TSST_SIZE, "Size" },
	{ TSST_DPI,  "DPI" },
};

const PreferredNameRenderingMap<GLYPHFONTSIZINGTYPE> g_pnrGLYPHFONTSIZINGTYPE[] = {
	{ GFST_NONE, "None" },
	{ GFST_SIZE, "Size" },
	{ GFST_DPI,  "DPI" },
};

// All TMT_* constants
const PreferredNameRenderingMap<int> g_pnrTMT[] = {
	{ TMT_RESERVEDLOW,             "ReservedLow" }, // special
	{ TMT_RESERVEDHIGH,            "ReservedHigh" }, // special
	{ TMT_DIBDATA,                 "DibData" }, // not in V3?
	{ TMT_GLYPHDIBDATA,            "GlyphDibData" }, // not in V3?
	{ TMT_ENUM,                    "Enum" }, // not in V3?
	{ TMT_STRING,                  "String" },
	{ TMT_INT,                     "Int" },
	{ TMT_BOOL,                    "Bool" },
	{ TMT_COLOR,                   "Color" },
	{ TMT_MARGINS,                 "Margins" },
	{ TMT_FILENAME,                "FileName" },
	{ TMT_SIZE,                    "Size" },
	{ TMT_POSITION,                "Position" },
	{ TMT_RECT,                    "Rect" },
	{ TMT_FONT,                    "Font" },
	{ TMT_INTLIST,                 "IntList" },
	{ TMT_HBITMAP,                 "HBITMAP" }, // not in V3?
	{ TMT_DISKSTREAM,              "DiskStream" }, // not in V3?
	{ TMT_STREAM,                  "Stream" },
	{ TMT_BITMAPREF,               "BitmapRef" },
	{ TMT_FLOAT,                   "Float" },
	{ TMT_FLOATLIST,               "FloatList" },
	{ TMT_COLORSCHEMES,            "ColorSchemes" },
	{ TMT_SIZES,                   "Sizes" },
	{ TMT_CHARSET,                 "Charset" },
	{ TMT_NAME,                    "Name" },
	{ TMT_DISPLAYNAME,             "DisplayName" },
	{ TMT_TOOLTIP,                 "ToolTip" },
	{ TMT_COMPANY,                 "Company" },
	{ TMT_AUTHOR,                  "Author" },
	{ TMT_COPYRIGHT,               "Copyright" },
	{ TMT_URL,                     "URL" },
	{ TMT_VERSION,                 "Version" },
	{ TMT_DESCRIPTION,             "Description" },
	{ TMT_CAPTIONFONT,             "CaptionFont" },
	{ TMT_SMALLCAPTIONFONT,        "SmallCaptionFont" },
	{ TMT_MENUFONT,                "MenuFont" },
	{ TMT_STATUSFONT,              "StatusFont" },
	{ TMT_MSGBOXFONT,              "MsgBoxFont" },
	{ TMT_ICONTITLEFONT,           "IconTitleFont" },
	// -- BEGIN V3 GAP --
	{ TMT_HEADING1FONT,            "Heading1Font" },
	{ TMT_HEADING2FONT,            "Heading2Font" },
	{ TMT_BODYFONT,                "BodyFont" },
	// -- END V3 GAP --
	{ TMT_FLATMENUS,               "FlatMenus" },
	{ TMT_SIZINGBORDERWIDTH,       "SizingBorderWidth" },
	{ TMT_SCROLLBARWIDTH,          "ScrollbarWidth" },
	{ TMT_SCROLLBARHEIGHT,         "ScrollbarHeight" },
	{ TMT_CAPTIONBARWIDTH,         "CaptionBarWidth" },
	{ TMT_CAPTIONBARHEIGHT,        "CaptionBarHeight" },
	{ TMT_SMCAPTIONBARWIDTH,       "SmallCaptionBarWidth" },
	{ TMT_SMCAPTIONBARHEIGHT,      "SmallCaptionBarHeight" },
	{ TMT_MENUBARWIDTH,            "MenuBarWidth" },
	{ TMT_MENUBARHEIGHT,           "MenuBarHeight" },
	{ TMT_PADDEDBORDERWIDTH,       "PaddedBorderWidth" }, // not in V3
	{ TMT_MINCOLORDEPTH,           "MinColorDepth" },
	{ TMT_CSSNAME,                 "CssName" },
	{ TMT_XMLNAME,                 "XmlName" },
	{ TMT_LASTUPDATED,             "LastUpdated" },
	{ TMT_ALIAS,                   "Alias" },
	{ TMT_SCROLLBAR,               "Scrollbar" },
	{ TMT_BACKGROUND,              "Background" },
	{ TMT_ACTIVECAPTION,           "ActiveCaption" },
	{ TMT_INACTIVECAPTION,         "InactiveCaption" },
	{ TMT_MENU,                    "Menu" },
	{ TMT_WINDOW,                  "Window" },
	{ TMT_WINDOWFRAME,             "WindowFrame" },
	{ TMT_MENUTEXT,                "MenuText" },
	{ TMT_WINDOWTEXT,              "WindowText" },
	{ TMT_CAPTIONTEXT,             "CaptionText" },
	{ TMT_ACTIVEBORDER,            "ActiveBorder" },
	{ TMT_INACTIVEBORDER,          "InactiveBorder" },
	{ TMT_APPWORKSPACE,            "AppWorkspace" },
	{ TMT_HIGHLIGHT,               "Highlight" },
	{ TMT_HIGHLIGHTTEXT,           "HighlightText" },
	{ TMT_BTNFACE,                 "Btnface" },
	{ TMT_BTNSHADOW,               "BtnShadow" },
	{ TMT_GRAYTEXT,                "GrayText" },
	{ TMT_BTNTEXT,                 "ButtonText" },
	{ TMT_INACTIVECAPTIONTEXT,     "InactiveCaptionText"},
	{ TMT_BTNHIGHLIGHT,            "BtnHighlight" },
	{ TMT_DKSHADOW3D,              "DkShadow3d" },
	{ TMT_LIGHT3D,                 "Light3d" },
	{ TMT_INFOTEXT,                "InfoText" },
	{ TMT_INFOBK,                  "InfoBk" },
	{ TMT_BUTTONALTERNATEFACE,     "ButtonAlternateFace" },
	{ TMT_HOTTRACKING,             "HotTracking" },
	{ TMT_GRADIENTACTIVECAPTION,   "GradientActiveCaption" },
	{ TMT_GRADIENTINACTIVECAPTION, "GradientInactiveCaption" },
	{ TMT_MENUHILIGHT,             "MenuHilight" },
	{ TMT_MENUBAR,                 "MenuBar" },
	{ TMT_FROMHUE1,                "FromHue1" },
	{ TMT_FROMHUE2,                "FromHue2" },
	{ TMT_FROMHUE3,                "FromHue3" },
	{ TMT_FROMHUE4,                "FromHue4" },
	{ TMT_FROMHUE5,                "FromHue5" },
	{ TMT_TOHUE1,                  "ToHue1" },
	{ TMT_TOHUE2,                  "ToHue2" },
	{ TMT_TOHUE3,                  "ToHue3" },
	{ TMT_TOHUE4,                  "ToHue4" },
	{ TMT_TOHUE5,                  "ToHue5" },
	{ TMT_FROMCOLOR1,              "FromColor1" },
	{ TMT_FROMCOLOR2,              "FromColor2" },
	{ TMT_FROMCOLOR3,              "FromColor3" },
	{ TMT_FROMCOLOR4,              "FromColor4" },
	{ TMT_FROMCOLOR5,              "FromColor5" },
	{ TMT_TOCOLOR1,                "ToColor1" },
	{ TMT_TOCOLOR2,                "ToColor2" },
	{ TMT_TOCOLOR3,                "ToColor3" },
	{ TMT_TOCOLOR4,                "ToColor4" },
	{ TMT_TOCOLOR5,                "ToColor5" },
	{ TMT_TRANSPARENT,             "Transparent" },
	{ TMT_AUTOSIZE,                "AutoSize" },
	{ TMT_BORDERONLY,              "BorderOnly" },
	{ TMT_COMPOSITED,              "Composited" }, // not in V3
	{ TMT_BGFILL,                  "BgFill" },
	{ TMT_GLYPHTRANSPARENT,        "GlyphTransparent" },
	{ TMT_GLYPHONLY,               "GlyphOnly"},
	{ TMT_ALWAYSSHOWSIZINGBAR,     "AlwaysShowSizingBar" },
	{ TMT_MIRRORIMAGE,             "MirrorImage" },
	{ TMT_UNIFORMSIZING,           "UniformSizing" },
	{ TMT_INTEGRALSIZING,          "IntegralSizing" },
	{ TMT_SOURCEGROW,              "SourceGrow" },
	{ TMT_SOURCESHRINK,            "SourceShrink" },
	// -- BEGIN V3 GAP --
	{ TMT_DRAWBORDERS,             "DrawBorders" },
	{ TMT_NOETCHEDEFFECT,          "NoEtchedEffect" },
	{ TMT_TEXTAPPLYOVERLAY,        "TextApplyOverlay" },
	{ TMT_TEXTGLOW,                "TextGlow" },
	{ TMT_TEXTITALIC,              "TextItalic" },
	{ TMT_COMPOSITEDOPAQUE,        "CompositedOpaque" },
	{ TMT_LOCALIZEDMIRRORIMAGE,    "LocalizedMirrorImage" },
	// -- END V3 GAP --
	{ TMT_IMAGECOUNT,              "ImageCount" },
	{ TMT_ALPHALEVEL,              "AlphaLevel" },
	{ TMT_BORDERSIZE,              "BorderSize" },
	{ TMT_ROUNDCORNERWIDTH,        "RoundCornerWidth" },
	{ TMT_ROUNDCORNERHEIGHT,       "RoundCornerHeight" },
	{ TMT_GRADIENTRATIO1,          "GradientRatio1" },
	{ TMT_GRADIENTRATIO2,          "GradientRatio2" },
	{ TMT_GRADIENTRATIO3,          "GradientRatio3" },
	{ TMT_GRADIENTRATIO4,          "GradientRatio4" },
	{ TMT_GRADIENTRATIO5,          "GradientRatio5" },
	{ TMT_PROGRESSCHUNKSIZE,       "ProgressChunkSize" },
	{ TMT_PROGRESSSPACESIZE,       "ProgressSpaceSize" },
	{ TMT_SATURATION,              "Saturation" },
	{ TMT_TEXTBORDERSIZE,          "TextBorderSize" },
	{ TMT_ALPHATHRESHOLD,          "AlphaThreshold" },
	{ TMT_WIDTH,                   "Width" },
	{ TMT_HEIGHT,                  "Height" },
	{ TMT_GLYPHINDEX,              "GlyphIndex" },
	{ TMT_TRUESIZESTRETCHMARK,     "TrueSizeStretchMark" },
	{ TMT_MINDPI1,                 "MinDpi1"},
	{ TMT_MINDPI2,                 "MinDpi2" },
	{ TMT_MINDPI3,                 "MinDpi3" },
	{ TMT_MINDPI4,                 "MinDpi4" },
	{ TMT_MINDPI5,                 "MinDpi5" },
	// -- BEGIN V3 GAP --
	{ TMT_TEXTGLOWSIZE,            "TextGlowSize" },
	{ TMT_FRAMESPERSECOND,         "FramesPerSecond" },
	{ TMT_PIXELSPERFRAME,          "PixelsPerFrame" },
	{ TMT_ANIMATIONDELAY,          "AnimationDelay" },
	{ TMT_GLOWINTENSITY,           "GlowIntensity" },
	{ TMT_OPACITY,                 "Opacity" },
	{ TMT_COLORIZATIONCOLOR,       "ColorizationColor" },
	{ TMT_COLORIZATIONOPACITY,     "ColorizationOpacity" },
	{ TMT_MINDPI6,                 "MinDpi6" },
	{ TMT_MINDPI7,                 "MinDpi7" },
	// -- END V3 GAP --
	{ TMT_GLYPHFONT,               "GlyphFont" },
	{ TMT_IMAGEFILE,               "ImageFile" },
	{ TMT_IMAGEFILE1,              "ImageFile1" },
	{ TMT_IMAGEFILE2,              "ImageFile2" },
	{ TMT_IMAGEFILE3,              "ImageFile3" },
	{ TMT_IMAGEFILE4,              "ImageFile4" },
	{ TMT_IMAGEFILE5,              "ImageFile5" },
	{ TMT_GLYPHIMAGEFILE,          "GlyphImageFile" },
	// -- BEGIN V3 GAP --
	{ TMT_IMAGEFILE6,              "ImageFile6" },
	{ TMT_IMAGEFILE7,              "ImageFile7" },
	// -- END V3 GAP --
	{ TMT_TEXT,                    "Text" },
	{ TMT_CLASSICVALUE,            "ClassicValue" }, // not in V3
	{ TMT_OFFSET,                  "Offset" },
	{ TMT_TEXTSHADOWOFFSET,        "TextShadowOffset" },
	{ TMT_MINSIZE,                 "MinSize" },
	{ TMT_MINSIZE1,                "MinSize1" },
	{ TMT_MINSIZE2,                "MinSize2" },
	{ TMT_MINSIZE3,                "MinSize3" },
	{ TMT_MINSIZE4,                "MinSize4" },
	{ TMT_MINSIZE5,                "MinSize5" },
	{ TMT_NORMALSIZE,              "NormalSize" },
	{ TMT_MINSIZE6,                "MinSize6" }, // not in V3
	{ TMT_MINSIZE7,                "MinSize7" }, // not in V3
	{ TMT_SIZINGMARGINS,           "SizingMargins" },
	{ TMT_CONTENTMARGINS,          "ContentMargins" },
	{ TMT_CAPTIONMARGINS,          "CaptionMargins" },
	{ TMT_BORDERCOLOR,             "BorderColor" },
	{ TMT_FILLCOLOR,               "FillColor" },
	{ TMT_TEXTCOLOR,               "TextColor" },
	{ TMT_EDGELIGHTCOLOR,          "EdgeLightColor" },
	{ TMT_EDGEHIGHLIGHTCOLOR,      "EdgeHighlightColor" },
	{ TMT_EDGESHADOWCOLOR,         "EdgeShadowColor" },
	{ TMT_EDGEDKSHADOWCOLOR,       "EdgeDkShadowColor" },
	{ TMT_EDGEFILLCOLOR,           "EdgeFillColor" },
	{ TMT_TRANSPARENTCOLOR,        "TransparentColor" },
	{ TMT_GRADIENTCOLOR1,          "GradientColor1" },
	{ TMT_GRADIENTCOLOR2,          "GradientColor2" },
	{ TMT_GRADIENTCOLOR3,          "GradientColor3" },
	{ TMT_GRADIENTCOLOR4,          "GradientColor4" },
	{ TMT_GRADIENTCOLOR5,          "GradientColor5" },
	{ TMT_SHADOWCOLOR,             "ShadowColor" },
	{ TMT_GLOWCOLOR,               "GlowColor" },
	{ TMT_TEXTBORDERCOLOR,         "TextBorderColor" },
	{ TMT_TEXTSHADOWCOLOR,         "TextShadowColor" },
	{ TMT_GLYPHTEXTCOLOR,          "GlyphTextColor" },
	{ TMT_GLYPHTRANSPARENTCOLOR,   "GlyphTransparentColor" },
	{ TMT_FILLCOLORHINT,           "FillColorHint" },
	{ TMT_BORDERCOLORHINT,         "BorderColorHint" },
	{ TMT_ACCENTCOLORHINT,         "AccentColorHint" },
	// -- BEGIN V3 GAP --
	{ TMT_TEXTCOLORHINT,           "TextColorHint" },
	{ TMT_HEADING1TEXTCOLOR,       "Heading1TextColor" },
	{ TMT_HEADING2TEXTCOLOR,       "Heading2TextColor" },
	{ TMT_BODYTEXTCOLOR,  	       "BodyTextColor" },
	// -- END V3 GAP --
	{ TMT_BGTYPE,         	       "BgType" },
	{ TMT_BORDERTYPE,     	       "BorderType" },
	{ TMT_FILLTYPE,       	       "FillType" },
	{ TMT_SIZINGTYPE,     	       "SizingType" },
	{ TMT_HALIGN,         	       "HAlign" },
	{ TMT_CONTENTALIGNMENT,        "ContentAlignment" },
	{ TMT_VALIGN,                  "VAlign" },
	{ TMT_OFFSETTYPE,              "OffsetType" },
	{ TMT_ICONEFFECT,              "IconEffect" },
	{ TMT_TEXTSHADOWTYPE,          "TextShadowType" },
	{ TMT_IMAGELAYOUT,             "ImageLayout" },
	{ TMT_GLYPHTYPE,               "GlyphType" },
	{ TMT_IMAGESELECTTYPE,         "ImageSelectType" },
	{ TMT_GLYPHFONTSIZINGTYPE,     "GlyphFontSizingType" },
	{ TMT_TRUESIZESCALINGTYPE,     "TrueSizeScalingType" },
	{ TMT_USERPICTURE,             "UserPicture" },
	{ TMT_DEFAULTPANESIZE,         "DefaultPaneSize" },
	{ TMT_BLENDCOLOR,              "BlendColor" },
	// -- BEGIN V3 GAP --
	{ TMT_CUSTOMSPLITRECT,         "CustomSplitRect" },
	{ TMT_ANIMATIONBUTTONRECT,     "AnimationButtonRect" },
	{ TMT_ANIMATIONDURATION,       "AnimationDuration" },
	{ TMT_TRANSITIONDURATIONS,     "TransitionDurations" },
	{ TMT_SCALEDBACKGROUND,        "ScaledBackground" },
	{ TMT_ATLASIMAGE,              "AtlasImage" },
	{ TMT_ATLASINPUTIMAGE,         "AtlasInputImage" },
	{ TMT_ATLASRECT,               "AtlasRect" },
	// -- END V3 GAP --
};

const PreferredNameRenderingMap<LINKPARTS> g_pnrLINKPARTS[] = {
	{ LP_HYPERLINK, "HyperLink" },
};

const PreferredNameRenderingMap<HYPERLINKSTATES> g_pnrHYPERLINKSTATES[] = {
	{ HLS_NORMALTEXT, "NormalText" },
	{ HLS_LINKTEXT,   "LinkText" },
};

const PreferredNameRenderingMap<EMPTYMARKUPPARTS> g_pnrEMPTYMARKUPPARTS[] = {
	{ EMP_MARKUPTEXT, "MarkupText" },
};

const PreferredNameRenderingMap<MARKUPTEXTSTATES> g_pnrMARKUPTEXTSTATES[] = {
	{ EMT_NORMALTEXT, "NormalText" },
	{ EMT_LINKTEXT,   "LinkText" },
};

const PreferredNameRenderingMap<STATICPARTS> g_pnrSTATICPARTS[] = {
	{ STAT_TEXT, "Text" },
};

const PreferredNameRenderingMap<PAGEPARTS> g_pnrPAGEPARTS[] = {
	{ PGRP_UP,       "Up" },
	{ PGRP_DOWN,     "Down" },
	{ PGRP_UPHORZ,   "UpHorz" },
	{ PGRP_DOWNHORZ, "DownHorz" },
};

const PreferredNameRenderingMap<MONTHCALPARTS> g_pnrMONTHCALPARTS[] = {
	{ MC_BACKGROUND,            "Background" },
	{ MC_BORDERS,               "Borders" },
	{ MC_GRIDBACKGROUND,        "GridBackground" },
	{ MC_COLHEADERSPLITTER,     "ColHeaderSplitter" },
	{ MC_GRIDCELLBACKGROUND,    "GridCellBackground" },
	{ MC_GRIDCELL,              "GridCell" },
	{ MC_GRIDCELLUPPER,         "GridCellUpper" },
	{ MC_TRAILINGGRIDCELL,      "TrailingGridCell" },
	{ MC_TRAILINGGRIDCELLUPPER, "TrailingGridCellUpper" },
	{ MC_NAVNEXT,               "NavNext" },
	{ MC_NAVPREV,               "NavPrev" },
};

const PreferredNameRenderingMap<GRIDCELLBACKGROUNDSTATES> g_pnrGRIDCELLBACKGROUNDSTATES[] = {
	{ MCGCB_SELECTED,           "Selected" },
	{ MCGCB_HOT,                "Hot" },
	{ MCGCB_SELECTEDHOT,        "SelectedHot" },
	{ MCGCB_SELECTEDNOTFOCUSED, "SelectedNotFocused" },
	{ MCGCB_TODAY,              "Today" },
	{ MCGCB_TODAYSELECTED,      "TodaySelected" },
};

const PreferredNameRenderingMap<GRIDCELLSTATES> g_pnrGRIDCELLSTATES[] = {
	{ MCGC_HOT,           "Hot" },
	{ MCGC_HASSTATE,      "HasState" },
	{ MCGC_HASSTATEHOT,   "HasStateHot" },
	{ MCGC_TODAY,         "Today" },
	{ MCGC_TODAYSELECTED, "TodaySelected" },
	{ MCGC_SELECTED,      "Selected" },
	{ MCGC_SELECTEDHOT,   "SelectedHot" },
};

const PreferredNameRenderingMap<GRIDCELLUPPERSTATES> g_pnrGRIDCELLUPPERSTATES[] = {
	{ MCGCU_HOT,           "Hot" },
	{ MCGCU_HASSTATE,      "HasState" },
	{ MCGCU_HASSTATEHOT,   "HasStateHot" },
	{ MCGCU_SELECTED,      "Selected" },
	{ MCGCU_SELECTEDHOT,   "SelectedHot" },
};

const PreferredNameRenderingMap<TRAILINGGRIDCELLSTATES> g_pnrTRAILINGGRIDCELLSTATES[] = {
	{ MCTGC_HOT,           "Hot" },
	{ MCTGC_HASSTATE,      "HasState" },
	{ MCTGC_HASSTATEHOT,   "HasStateHot" },
	{ MCTGC_TODAY,         "Today" },
	{ MCTGC_TODAYSELECTED, "TodaySelected" },
	{ MCTGC_SELECTED,      "Selected" },
	{ MCTGC_SELECTEDHOT,   "SelectedHot" },
};

const PreferredNameRenderingMap<TRAILINGGRIDCELLUPPERSTATES> g_pnrTRAILINGGRIDCELLUPPERSTATES[] = {
	{ MCTGCU_HOT,           "Hot" },
	{ MCTGCU_HASSTATE,      "HasState" },
	{ MCTGCU_HASSTATEHOT,   "HasStateHot" },
	{ MCTGCU_SELECTED,      "Selected" },
	{ MCTGCU_SELECTEDHOT,   "SelectedHot" },
};

const PreferredNameRenderingMap<NAVNEXTSTATES> g_pnrNAVNEXTSTATES[] = {
	{ MCNN_NORMAL,   "Normal" },
	{ MCNN_HOT,      "Hot" },
	{ MCNN_PRESSED,  "Pressed" },
	{ MCNN_DISABLED, "Disabled" },
};

const PreferredNameRenderingMap<NAVPREVSTATES> g_pnrNAVPREVSTATES[] = {
	{ MCNP_NORMAL,   "Normal" },
	{ MCNP_HOT,      "Hot" },
	{ MCNP_PRESSED,  "Pressed" },
	{ MCNP_DISABLED, "Disabled" },
};

const PreferredNameRenderingMap<CLOCKPARTS> g_pnrCLOCKPARTS[] = {
	{ CLP_TIME, "Time" },
};

const PreferredNameRenderingMap<CLOCKSTATES> g_pnrCLOCKSTATES[] = {
	{ CLS_NORMAL,   "Normal" },
	{ CLS_HOT,      "Hot" },
	{ CLS_PRESSED,  "Pressed" },
};

const PreferredNameRenderingMap<TRAYNOTIFYPARTS> g_pnrTRAYNOTIFYPARTS[] = {
	{ TNP_BACKGROUND,     "Background" },
	{ TNP_ANIMBACKGROUND, "AnimBackground" },
};

const PreferredNameRenderingMap<TASKBARPARTS> g_pnrTASKBARPARTS[] = {
	{ TBP_BACKGROUNDBOTTOM, "BackgroundBottom" },
	{ TBP_BACKGROUNDRIGHT,  "BackgroundRight" },
	{ TBP_BACKGROUNDTOP,    "BackgroundTop" },
	{ TBP_BACKGROUNDLEFT,   "BackgroundLeft" },
	{ TBP_SIZINGBARBOTTOM,  "SizingBarBottom" },
	{ TBP_SIZINGBARRIGHT,   "SizingBarRight" },
	{ TBP_SIZINGBARTOP,     "SizingBarTop" },
	{ TBP_SIZINGBARLEFT,    "SizingBarLeft" },
};

const PreferredNameRenderingMap<TASKBANDPARTS> g_pnrTASKBANDPARTS[] = {
	{ TDP_GROUPCOUNT,           "GroupCount" },
	{ TDP_FLASHBUTTON,          "FlashButton" },
	{ TDP_FLASHBUTTONGROUPMENU, "FlashButtonGroupMenu" },
};

const PreferredNameRenderingMap<STARTPANELPARTS> g_pnrSTARTPANELPARTS[] = {
	{ SPP_USERPANE,                  "UserPane" },
	{ SPP_MOREPROGRAMS,              "MorePrograms" },
	{ SPP_MOREPROGRAMSARROW,         "MoreProgramsArrow" },
	{ SPP_PROGLIST,                  "ProgList" },
	{ SPP_PROGLISTSEPARATOR,         "ProgListSeparator" },
	{ SPP_PLACESLIST,                "PlacesList" },
	{ SPP_PLACESLISTSEPARATOR,       "PlacesListSeparator" },
	{ SPP_LOGOFF,                    "Logoff" },
	{ SPP_LOGOFFBUTTONS,             "LogoffButtons" },
	{ SPP_USERPICTURE,               "UserPicture" },
	{ SPP_PREVIEW,                   "Preview" },
	{ SPP_MOREPROGRAMSTAB,           "MoreProgramsTab" },
	{ SPP_NSCHOST,                   "NscHost" },
	{ SPP_SOFTWAREEXPLORER,          "SoftwareExplorer" },
	{ SPP_OPENBOX,                   "OpenBox" },
	{ SPP_SEARCHVIEW,                "SearchView" },
	{ SPP_MOREPROGRAMSARROWBACK,     "MoreProgramsArrowBack" },
	{ SPP_TOPMATCH,                  "TopMatch" },
	{ SPP_LOGOFFSPLITBUTTONDROPDOWN, "LogoffSplitButtonDropdown" },
};

const PreferredNameRenderingMap<MOREPROGRAMSTABSTATES> g_pnrMOREPROGRAMSTABSTATES[] = {
	{ SPMPT_NORMAL,    "Normal" },
	{ SPMPT_HOT,       "Hot" },
	{ SPMPT_SELECTED,  "Selected" },
	{ SPMPT_DISABLED,  "Disabled" },
	{ SPMPT_FOCUSED,   "Focused" },
};

const PreferredNameRenderingMap<SOFTWAREEXPLORERSTATES> g_pnrSOFTWAREEXPLORERSTATES[] = {
	{ SPSE_NORMAL,    "Normal" },
	{ SPSE_HOT,       "Hot" },
	{ SPSE_SELECTED,  "Selected" },
	{ SPSE_DISABLED,  "Disabled" },
	{ SPSE_FOCUSED,   "Focused" },
};

const PreferredNameRenderingMap<OPENBOXSTATES> g_pnrOPENBOXSTATES[] = {
	{ SPOB_NORMAL,    "Normal" },
	{ SPOB_HOT,       "Hot" },
	{ SPOB_SELECTED,  "Selected" },
	{ SPOB_DISABLED,  "Disabled" },
	{ SPOB_FOCUSED,   "Focused" },
};

const PreferredNameRenderingMap<MOREPROGRAMSARROWSTATES> g_pnrMOREPROGRAMSARROWSTATES[] = {
	{ SPS_NORMAL,   "Normal" },
	{ SPS_HOT,      "Hot" },
	{ SPS_PRESSED,  "Pressed" },
};

const PreferredNameRenderingMap<MOREPROGRAMSARROWBACKSTATES> g_pnrMOREPROGRAMSARROWBACKSTATES[] = {
	{ SPSB_NORMAL,   "Normal" },
	{ SPSB_HOT,      "Hot" },
	{ SPSB_PRESSED,  "Pressed" },
};

const PreferredNameRenderingMap<LOGOFFBUTTONSSTATES> g_pnrLOGOFFBUTTONSSTATES[] = {
	{ SPLS_NORMAL,   "Normal" },
	{ SPLS_HOT,      "Hot" },
	{ SPLS_PRESSED,  "Pressed" },
};

const PreferredNameRenderingMap<MENUBANDPARTS> g_pnrMENUBANDPARTS[] = {
	{ MDP_NEWAPPBUTTON, "NewAppButton" },
	{ MDP_SEPERATOR,    "Separator" },
};

const PreferredNameRenderingMap<MENUBANDSTATES> g_pnrMENUBANDSTATES[] = {
	{ MDS_NORMAL,       "Normal" },
	{ MDS_HOT,          "Hot" },
	{ MDS_PRESSED,      "Pressed" },
	{ MDS_DISABLED,     "Disabled" },
	{ MDS_CHECKED,      "Checked" },
	{ MDS_HOTCHECKED,   "HotChecked" },
};