/** xcmedid.c
 *
 *  A EEDID to key/value pair convertor.
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2009-2012 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xcm/XcmEdidParse.h>

#include "xcm_version.h"
#include "xcm_macros.h"

void printfHelp(int argc, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a EDID parsing tool"));
  fprintf( stderr, "  %s\n",
                  PACKAGE_STRING );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "      %s EDID.bin\n\n", argv[0]);
  fprintf( stderr, "\n");


  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Print EDID values:"));
  fprintf( stderr, "      %s EDID.bin\n\n", argv[0]);
  fprintf( stderr, "      cat EDID.bin | %s\n", argv[0]);
  fprintf( stderr, "        [--openicc|--ppmcie]\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        --openicc       %s\n", _("use JSON"));
  fprintf( stderr, "        --ppmcie        %s\n", _("ppcmcie compatible"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}

int main(int argc, char ** argv)
{
  XCM_EDID_ERROR_e err = XCM_EDID_WRONG_SIGNATURE;
  char * txt = 0;
  FILE * fp = 0;
  char * mem = 0;
  size_t size = 0, s = 0;
  int min_args = 1;
  int print_openicc_json = 0,
      print_ppmcie = 0;
  const char * file_name = NULL;
#ifndef HAVE_OY
  int verbose = 0;
#endif

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  if(argc > 1)
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
              case 'v': verbose += 1; break;
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("openicc"))
                        { print_openicc_json = 1; i=100; break; }
                        else if(OY_IS_ARG("ppmcie"))
                        { print_ppmcie = 1; i=100; break; }
                        }
                        printfHelp(argc, argv);
                        exit (0);
                        break;
              case '?':
              case 'h':
              default:
                        wrong_arg = argv[pos];
                        break;
            }
            break;
        default:
                        if(!file_name)
                        file_name = argv[pos];
      }
      if( wrong_arg )
      {
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }

  } 

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      rewind(fp);
      mem = malloc(size);
      s = fread(mem, sizeof(char), size, fp);
      if(s != size)
        fprintf(stderr, "Error: fread %lu but should read %lu\n", s, size);
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

  if(verbose)
  {
    fprintf(stderr, "JSON=%d\n", print_openicc_json);
  }

  if(mem && size)
  {
    if(print_openicc_json)
      err = XcmEdidPrintOpenIccJSON( mem, &txt, malloc );
    else if(print_ppmcie)
    {
      XcmEdidKeyValue_s * list = 0;
      int i = 0, count = 0;
      err = XcmEdidParse( mem, &list, &count );
      if(list && count)
      {
        fprintf(stderr, "ppmcie:");
        for(i = 0; i < count; ++i)
        {
          if(strcmp(list[i].key,XCM_EDID_KEY_REDx) == 0)
          { fprintf(stdout," -red %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_REDy) == 0)
          { fprintf(stdout," %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_GREENx) == 0)
          { fprintf(stdout," -green %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_GREENy) == 0)
          { fprintf(stdout," %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_BLUEx) == 0)
          { fprintf(stdout," -blue %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_BLUEy) == 0)
          { fprintf(stdout," %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_WHITEx) == 0)
          { fprintf(stdout," -white %g", list[i].value.dbl); fflush(stdout); }
          else if(strcmp(list[i].key,XCM_EDID_KEY_WHITEy) == 0)
          { fprintf(stdout," %g", list[i].value.dbl); fflush(stdout); }
        }
      }
    }
    else
      err = XcmEdidPrintString( mem, &txt, malloc );

      if(err)
        fprintf( stderr, "Error: %s (%d)\n",
                 XcmEdidErrorToString(err), (int)size);
      else if(txt)
      {
        fprintf(stdout, "%s\n", txt);
        free(txt);
      }
  }

  if(fp && fp != stdin)
    fclose (fp);

  return err;
}
