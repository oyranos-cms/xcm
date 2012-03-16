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

#include "config.h"
#include "xcm_version.h"
#include "xcm_macros.h"


int main(int argc, char *argv[])
{
  const char * display_name = getenv("DISPLAY");

  if(argc > 2 && strcmp(argv[1],"-display") == 0)
    display_name = argv[2];

#ifdef XCM_HAVE_OY
  XcmICCprofileFromMD5FuncSet( fromMD5 );
  XcmICCprofileGetNameFuncSet( getName );
#endif

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

#ifdef XCM_HAVE_OY
void * fromMD5                       ( const void        * md5_hash,
                                       size_t            * size,
                                       void              *(allocate_func)(size_t) )
{
  void * data = 0;
  oyProfile_s * p = oyProfile_FromMD5( (uint32_t*)md5_hash, 0 );
  data = oyProfile_GetMem( p, size, 0, allocate_func );
  oyProfile_Release( &p );
  return data;
}

char * getName                       ( const void        * data,
                                       size_t              size,
                                       void              *(allocate_func)(size_t),
                                       int                 file_name )
{
  char * text = 0;
  const char * t = 0;
  oyProfile_s * p = oyProfile_FromMem( size, (void*)data, 0, 0 );
  if(file_name)
    t = oyProfile_GetFileName( p, -1 );
  else
    t = oyProfile_GetText( p, oyNAME_DESCRIPTION );

  if(t && t[0])
  {
    text = (char*)allocate_func( strlen(t) + 1 );
    strcpy( text, t );
  }

  oyProfile_Release( &p );
  return text;
}
#endif
