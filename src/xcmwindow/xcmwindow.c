/** xcmwindow.c
 *
 *  A net-color spec compatible information tool for server based per window color management.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  gcc -Wall -g -I../.. xcmwindow.c -o xcmwindow `pkg-config --cflags --libs x11 xcm`
 */

#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>

#include "xcm_version.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef USE_GETTEXT
#define _(text) text
#endif

void printfHelp(int argc, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a X11 color management client tool"));
  fprintf( stderr, "  Xcm v%s config: %s devel period: %s\n",
                  XCM_VERSION_NAME,
                  XCM_CONFIG_DATE, XCM_DATE );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List available windows:"));
  fprintf( stderr, "      %s -l [--window-name]\n",        argv[0]);
  fprintf( stderr, "        --window-name   %s\n", _("show window name"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print infos about a window:"));
  fprintf( stderr, "      %s -p --id=window_id\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        --display=:0.0  %s\n", _("X11 display name"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}

/* command line parsing macros */
/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_INT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atoi( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atoi( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) fprintf(stderr,#opt "=%d\n",opt)
#define OY_PARSE_INT_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = atoi(argv[pos+1]); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = atoi(&argv[pos][i+strlen(arg)+2]); \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) fprintf(stderr,arg "=%d\n",opt)
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) fprintf(stderr, #opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) fprintf(stderr,arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)

#define verbose oy_debug

int main(int argc, char ** argv)
{
  int id = 0;
  int print = 0;
  int print_window_name = 0;
  int list_windows = 0;
  int count = 0;
  const char * display = NULL;
  Display * dpy = NULL;
  int screen = 0;
  Window win = 0,
         root;
  int verbose = 0;
  int result = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'l': list_windows = 1; break;
              case 'p': print = 1; break;
              case 'v': verbose += 1; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("id"))
                        { OY_PARSE_INT_ARG2( id, "id" ); break; }
                        else if(OY_IS_ARG("display"))
                        { OY_PARSE_STRING_ARG2( display, "display" ); break; }
                        else if(OY_IS_ARG("window-name"))
                        { print_window_name = 1; i=100; break; }
                        }
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  dpy = XOpenDisplay( display );
  if(!dpy)
  {
    fprintf( stderr, "%s %s\n", "unable to open display", display?display:"");
    exit(1);
  }

  win = (Window) id;
  screen = DefaultScreen( dpy );
  root = XRootWindow( dpy, screen );

  if(list_windows)
  {
    Status status = 0;
    if(!verbose)
    {
      {
        Window root_return = 0,
               parent_return = 0, 
             * children_return = 0,
             * wins = 0;
        unsigned int nchildren_return = 0, wins_n = 0;
        int i;
        XWindowAttributes window_attributes_return;

        XSync( dpy, 0 );
        status = XQueryTree( dpy, root,
                         &root_return, &parent_return,
                         &children_return, &nchildren_return );
        wins = (Window*)malloc(sizeof(Window) * nchildren_return );
        memcpy( wins, children_return, sizeof(Window) * nchildren_return );
        XFree( children_return );
        children_return = wins; wins = 0;

        for(i = nchildren_return - 1; i >= 0; --i)
        {
          root_return = 0;
          status = XQueryTree( dpy, children_return[i],
                           &root_return, &parent_return,
                           &wins, &wins_n );
          status = XGetWindowAttributes( dpy, children_return[i],
                                     &window_attributes_return );
          if(window_attributes_return.map_state == IsViewable &&
             parent_return == root)
          {
            if(verbose)
              fprintf( stdout, "%s",
                       XcmePrintWindowRegions( dpy, children_return[i], 0 ));
            fprintf( stdout, "%d", (int)children_return[i] );
            if(print_window_name)
              fprintf( stdout, "  %s",
                       XcmePrintWindowName(dpy, children_return[i]) );
            fprintf( stdout, "\n" );
            ++count;
          }

          XFree( wins );
        }

        free( children_return );
      }
    } else
    {
      XcmeContext_s * c = XcmeContext_Create( display );
      XcmeContext_Release( &c );
    }

  } else if(print)
  {
    if(!win)
    {
      fprintf( stderr, "%s\n", _("The integer window ID is missed. Use option: --id 12345"));
      exit(1);
    }
    fprintf( stdout, "%s\n", XcmePrintWindowRegions( dpy, win, 1 ) );
  }

  return result;
}



