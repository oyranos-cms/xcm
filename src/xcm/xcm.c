/** xcm.c
 *
 *  A X Color Management specification compatible information tool for server 
 *  based per window color management.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  gcc -Wall -g -I../.. xcm.c -o xcm `pkg-config --cflags --libs x11 xcm oyranos`
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> /* sleep() */

#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>
#include <X11/extensions/Xfixes.h> /* XserverRegion */
#include <X11/Xcm/Xcm.h> /* XcolorRegion */

#include "xcm_version.h"
#include "xcm_macros.h"
#define OY_DBG_FORMAT_ "%s:%d %s() "
#define OY_DBG_ARGS_ __FILE__,__LINE__,strrchr(__func__,'/')?strrchr(__func__,'/')+1:__func__

int XcolorRegionFind(XcolorRegion * old_regions, unsigned long old_regions_n, Display * dpy, Window win, XRectangle * rectangle);

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
  fprintf( stderr, "  %s\n",               _("Set window region:"));
  fprintf( stderr, "      %s -r\n",argv[0]);
  fprintf( stderr, "        [-x pos_x -y pos_y --width width --height height|\n"
                   "         --geometry width_x_height_+_x_+_y]\n");
  fprintf( stderr, "        --id=window_id [--profile filename.icc]\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Delete window region:"));
  fprintf( stderr, "      %s -d\n",argv[0]);
  fprintf( stderr, "        [-x pos_x -y pos_y --width width --height height|\n"
                   "         --geometry width_x_height_+_x_+_y]\n");
  fprintf( stderr, "        --id=window_id\n");
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


int main(int argc, char ** argv)
{
  int id = 0;
  int print = 0;
  int print_window_name = 0;
  int list_windows = 0;
  int count = 0;
  const char * display = NULL;
  const char * geometry = NULL;

  Display * dpy = NULL;
  int screen = 0;
  Window win = 0,
         root;

  int place_region = 0;
  int delete_region = 0;
  int x = 0, y = 0, width = 0, height = 0;
  const char * profile_name = NULL;
  int verbose = 0;
  int result = 0;

  XserverRegion reg = 0;
  XcolorRegion region;
  int error;
  XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } };

#ifdef HAVE_OY
  char * blob = 0;
  size_t size = 0;
  oyProfile_s * p = 0;
  XcolorProfile * profile = 0;
#endif

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
              case 'd': delete_region = 1; break;
              case 'l': list_windows = 1; break;
              case 'p': print = 1; break;
              case 'r': place_region = 1; break;
              case 'v': verbose += 1; break;
              case 'x': OY_PARSE_INT_ARG( x ); break;
              case 'y': OY_PARSE_INT_ARG( y ); break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("id"))
                        { OY_PARSE_INT_ARG2( id, "id" ); break; }
                        else if(OY_IS_ARG("width"))
                        { OY_PARSE_INT_ARG2( width, "width" ); break; }
                        else if(OY_IS_ARG("height"))
                        { OY_PARSE_INT_ARG2( height, "height" ); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2( profile_name, "profile" ); break; }
                        else if(OY_IS_ARG("display"))
                        { OY_PARSE_STRING_ARG2( display, "display" ); break; }
                        else if(OY_IS_ARG("geometry"))
                        { OY_PARSE_STRING_ARG2( geometry, "geometry" ); break; }
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

#ifdef HAVE_OY
  XcmICCprofileFromMD5FuncSet( fromMD5 );
  XcmICCprofileGetNameFuncSet( getName );
#endif

  dpy = XOpenDisplay( display );
  if(!dpy)
  {
    fprintf( stderr, "%s %s\n", "unable to open display", display?display:"");
    exit(1);
  }

  if(geometry)
  {
    int matches = sscanf( geometry, "%ix%i+%i+%i",
                          &width, &height, &x, &y );
    if(matches !=  4 && strcmp(geometry,"0x0+0+0") != 0)
    {
      fprintf( stderr, "%s: --geometry WIDTHxHEIGHT+XPOS+YPOS (%s)\n",
               _("argument not recognised"), geometry);
      exit(1);
    }

    rec[0].x = x;
    rec[0].y = y;
    rec[0].width = width;
    rec[0].height = height;
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
  } else if(place_region)
  {
    int need_wait = 1;
#ifdef HAVE_OY
    /* Upload a ICC profile to X11 root window */
    if(profile_name)
    {
      p = oyProfile_FromFile( profile_name, 0,0 );
      if(p)
      {
        blob = oyProfile_GetMem( p, &size, 0,0 );

        if(blob && size)
        {
        /* Create a XcolorProfile object that will be uploaded to the display.*/
          profile = malloc(sizeof(XcolorProfile) + size);

          oyProfile_GetMD5(p, OY_FROM_PROFILE, (uint32_t*)profile->md5);

          profile->length = htonl(size);
          memcpy(profile + 1, blob, size);

          result = XcolorProfileUpload( dpy, profile );
          if(result)
            printf("XcolorProfileUpload: %d\n", result);
        }
        oyProfile_Release( &p );
      }
    }
#endif

    reg = XFixesCreateRegion( dpy, rec, 1);

    region.region = htonl(reg);
#ifdef HAVE_OY
    if(blob && size)
      memcpy(region.md5, profile->md5, 16);
    else
#endif
      memset( region.md5, 0, 16 );

    if(rec[0].x || rec[0].y || rec[0].width || rec[0].height)
    {

      /* upload the new or changed region to the X server */
      error = XcolorRegionInsert( dpy, win, 0, &region, 1 );
      if(error)
          printf( OY_DBG_FORMAT_
                   "XcolorRegionInsert failed %d\n",
                   OY_DBG_ARGS_, error );

    } else
    {
      unsigned long nRegions = 0;
      XcolorRegion * r = XcolorRegionFetch( dpy, win, &nRegions );
      need_wait = 0;
      if(nRegions && r)
      {
        error = XcolorRegionDelete( dpy, win, 0, nRegions );
        fprintf(stderr, "deleted %lu region%c\n", nRegions, nRegions==1?' ':'s');
        XFree( r ); r = 0;
      } else
      {
        fprintf(stderr, "no region to delete \n");
      }
    }

    XFlush( dpy );

    /** Closing the display object will destroy all XFixes regions automatically
     *  by Xorg. Therefore we loop here to keep the XFixes regions alive. */
    if(need_wait)
      while(1) sleep(2);

  } if(delete_region)
  {
    XcolorRegion *old_regions = 0;
    unsigned long old_regions_n = 0;
    int pos = -1;

    /* get old regions */
    old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
    /* remove specified region */
    pos = XcolorRegionFind( old_regions, old_regions_n, dpy, win, rec );
    XFree( old_regions );
    if(pos >= 0)
    {
      int undeleted_n = old_regions_n;
      XcolorRegionDelete( dpy, win, pos, 1 );
      old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
      if(undeleted_n - old_regions_n != 1)
        printf(  OY_DBG_FORMAT_"removed %d; have still %d\n", OY_DBG_ARGS_,
                 pos, (int)old_regions_n );
      else
        fprintf( stderr, "removed position %d\n", pos );
    } else
      printf( "region not found: %s in %lu\n", geometry, old_regions_n );

    XFlush( dpy );

  }

  XCloseDisplay( dpy );

  return result;
}

int XcolorRegionFind(XcolorRegion * old_regions, unsigned long old_regions_n, Display * dpy, Window win, XRectangle * rectangle)
{
  XRectangle * rect = 0;
  int nRect = 0;
  int pos = -1;
  int i, j;

  /* get old regions */
  old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
  /* search region */
  for(i = 0; i < old_regions_n; ++i)
  {

    if(!old_regions[i].region || pos >= 0)
      break;

    rect = XFixesFetchRegion( dpy, ntohl(old_regions[i].region),
                              &nRect );

    for(j = 0; j < nRect; ++j)
    {
#ifdef HAVE_OY
      if(oy_debug)
        printf( OY_DBG_FORMAT_
                 "reg[%d]: %dx%d+%d+%d %dx%d+%d+%d\n",
                   OY_DBG_ARGS_, i,
                   rectangle->width, rectangle->height,
                   rectangle->x, rectangle->y,
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y
                  );
#endif
      if(rectangle->x == rect[j].x &&
         rectangle->y == rect[j].y &&
         rectangle->width == rect[j].width &&
         rectangle->height == rect[j].height )
      {
        pos = i;
        break;
      }
    }
  }

  return pos;
}



#ifdef HAVE_OY
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
