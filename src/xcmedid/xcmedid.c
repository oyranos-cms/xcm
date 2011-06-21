/** xcmedid.c
 *
 *  A EEDID to key/value pair convertor.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */
#include <X11/Xcm/XcmEdidParse.h>
#include "xcm_version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef USE_GETTEXT
#define _(text) text
#endif

int main(int argc, char ** argv)
{
  XCM_EDID_ERROR_e err = XCM_EDID_WRONG_SIGNATURE;
  char * txt = 0;
  FILE * fp = 0;
  char * mem = 0;
  size_t size = 0;
  int s = 0;
  int min_args = 1;
  int print_openicc_json = 0;

  if(argc > min_args)
  {
    if(strcmp( argv[min_args], "--openicc" ) == 0)
    {
      print_openicc_json = 1;
      ++min_args;
    }

    fp = fopen(argv[min_args],"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      rewind(fp);
      mem = malloc(size);
      s = fread(mem, sizeof(char), size, fp);
    } else
    {
      fprintf(stderr, "Error: Could not open file - \"%s\"\n", argv[min_args]);
    }
  } else
  {
    int c;

    mem = malloc(65535);
    fp = stdin;
    while(((c = getc(stdin)) != EOF) &&
          size < 65535)
      mem[size++] = c;
  }

  if(mem && size)
  {
    if(print_openicc_json)
      err = XcmEdidPrintOpenIccJSON( mem, &txt, malloc );
    else
      err = XcmEdidPrintString( mem, &txt, malloc );

      if(err)
        fprintf( stderr, "Error: %s (%d)\n",
                 XcmEdidErrorToString(err), (int)size);
      else
      {
        fprintf(stdout, "%s\n", txt);
        free(txt);
      }
  }

  if(fp && fp != stdin)
    fclose (fp);

  if(err)
  {
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a EDID parsing tool"));
  fprintf( stderr, "  Xcm v%s config: %s devel period: %s\n",
                  XCM_VERSION_NAME,
                  XCM_CONFIG_DATE, XCM_DATE );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf(stderr, "      %s EDID.bin\n\n", argv[0]);
  }

  return err;
}
