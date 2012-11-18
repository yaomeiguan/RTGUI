#ifndef RTCONFIG_H
#define RTCONFIG_H

/*
 * As all the RT-Thread kernel is compiled unconditionally in this simulator,
 * the only thing we need to configure is the options for RTGUI.
 */

/* name length of RTGUI object */
#define RTGUI_NAME_MAX      12
/* support 16 weight font */
#define RTGUI_USING_FONT16
/* support Chinese font */
#define RTGUI_USING_FONTHZ
/* support FreeType TTF font */
//#define RTGUI_USING_TTF
/* use small size in RTGUI */
#define RTGUI_USING_SMALL_SIZE
/* use mouse cursor */
/* #define RTGUI_USING_MOUSE_CURSOR */
/* default font size in RTGUI */
#define RTGUI_DEFAULT_FONT_SIZE 12

#define RTGUI_USING_STDIO_FILERW
#define RTGUI_IMAGE_CONTAINER
#define RTGUI_IMAGE_XPM
#define RTGUI_IMAGE_BMP
#define RTGUI_IMAGE_PNG
// #define RTGUI_IMAGE_JPEG
// #define RTGUI_IMAGE_TJPGD
#define RTGUI_USING_FONT12
#define RTGUI_USING_HZ_BMP
#define RTGUI_MEM_TRACE
#define RTGUI_USING_WINMOVE
#define RTGUI_USING_NOTEBOOK_IMAGE
// #define RTGUI_USING_HZ_FILE
//#define RTGUI_USING_FONT_COMPACT

// RT_USING_DFS is needed by RTGUI_USING_DFS_FILERW
#define RT_USING_DFS
#define RTGUI_USING_DFS_FILERW

#define _WIN32_NATIVE

#endif /* end of include guard: RTCONFIG_H */
