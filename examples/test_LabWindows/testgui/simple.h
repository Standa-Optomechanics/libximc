/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_NUMERIC                    2       /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON              3       /* control type: command, callback function: Button1 */
#define  PANEL_TEXTMSG                    4       /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  5       /* control type: textMsg, callback function: (none) */
#define  PANEL_QUITBUTTON                 6       /* control type: command, callback function: QuitCallback */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Button1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
