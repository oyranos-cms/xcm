/** xcmevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#include <stdlib.h>
#include <string.h>                    /* strcmp() */
#include <X11/Xcm/XcmEvents.h>

int main(int argc, char *argv[])
{
  const char * display_name = getenv("DISPLAY");

  if(argc > 2 && strcmp(argv[1],"-display") == 0)
    display_name = argv[2];

  XcmeContext_s * c = XcmeContext_Create( display_name );

  for(;;)
  {
    XEvent event;
    XNextEvent( XcmeContext_DisplayGet( c ), &event);
    XcmeContext_InLoop( c, &event );
  }

  XcmeContext_Release( &c );

  return 0;
}

