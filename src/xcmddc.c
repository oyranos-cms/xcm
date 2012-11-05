/** xcmddc.c
 *
 *  A EEDID from i2c fetcher.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xcm/XcmDDC.h>

#include "xcm_version.h"
#include "xcm_macros.h"

void printfHelp(int argc, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a EDID from i2c tool"));
  fprintf( stderr, "  %s\n",
                  PACKAGE_STRING );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List available i2c->monitor devices:"));
  fprintf( stderr, "      %s -l\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Dump EDID from i2c:"));
  fprintf( stderr, "      %s -d positional_number [--force-output] [--identify]\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Dump EDID from i2c:"));
  fprintf( stderr, "      %s --i2c node_name [--force-output] [--identify]\n",    argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      %s\n",           _("-v verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}


int main(int argc, char ** argv)
{
  XCM_DDC_ERROR_e error = 0;
  int i;
  char * data = 0;
  size_t size;
  int list_devices = 0;
  int device = -1;
  char ** devices = NULL;
  int force_output = 0;
  int identify = 0;
  int verbose = 0;
  int n = 0;
  const char * i2c_node_name = NULL;
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
              case 'l': list_devices = 1; break;
              case 'd': OY_PARSE_INT_ARG( device ); break;
              case 'v': verbose += 1; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("i2c"))
                        { OY_PARSE_STRING_ARG2(i2c_node_name, "i2c"); break; }
                        else if(OY_IS_ARG("force-output"))
                        { force_output = 1; i=100; break; }
                        else if(OY_IS_ARG("identify"))
                        { identify = 1; i=100; break; }
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

  if(list_devices || !i2c_node_name)
    error = XcmDDClist( &devices, &n );

  if(list_devices)
  {
    if(!verbose)
    {
      fprintf( stderr, "number of i2c->monitor nodes:\n");
      fprintf( stdout, "%d", n);
      fflush( stdout );
      fprintf( stderr, "\n");
    } else
    {
      for(i = 0; i < n; ++i)
      {
        puts(devices[i]);
        fflush( stdout );
      }
    }
  }

  if(device != -1 || i2c_node_name)
  {
    if(!i2c_node_name &&
       (device <0 || n <= device))
    {
      fprintf( stderr, "device out of range: %d; select >= 0 && < %d\n",
               device, n );
    } else
    {
      if(!i2c_node_name)
        i2c_node_name = devices[device];
      error = XcmDDCgetEDID( i2c_node_name, &data, &size );
      if(error)
      {
        result = error;

        if(verbose)
          fprintf(stderr, "ERROR[%s]: %s\n", i2c_node_name,
                                             XcmDDCErrorToString(error) );
      }

      if(error == XCM_DDC_OK || force_output)
      {
        if(verbose)
          fprintf(stderr, "%s\n", i2c_node_name );
        if(data)
        {
          if(!identify)
            fwrite( data, sizeof(char), size, stdout );
          free(data); data = 0; size = 0;
        }
        fflush( stdout );
      }

      if(identify != 0)
      {
        printf( "%s\n", XcmDDCErrorToString(error));
        return error;
      }
    }
  }

  if(devices && n)
    for(i = 0; i < n; ++i)
      if(devices[i])
        free(devices[i]);

  if(devices)
    free(devices);

  return result;
}



