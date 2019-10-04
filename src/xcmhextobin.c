/* gcc -Wall -g xcmhextobin.c -o xcmhextobin */
/* gcc -Wall -g xcmhextobin.c -o xcmhextobin && oyranos-monitor -clv 2>&1  | grep ^EDID: | sed s/EDID:0x// | ./hextobin | ./hextobin -r | ./hextobin | xcmedid --openicc */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h> /* uint64_t - a bit fragile */

#include "xcm_version.h"
#include "xcm_macros.h"

unsigned char * oyReadStdinToMem_             ( size_t            * size )
{
  unsigned char * text = 0;
  {
    int text_size = 0, buf_size = 0; 
    int c;
    unsigned char * tmp = 0;

    while(((c = getc(stdin)) != EOF))
    {
      if(text_size >= buf_size-1)
      {
        buf_size = text_size + 65536;
        tmp = calloc( sizeof(unsigned char), buf_size );
        if(text_size)
          memcpy(tmp, text, text_size);
        free(text);
        text = tmp; tmp = 0;
      }
      text[text_size++] = c;
    }
    if(text)
      text[text_size] = '\000';

    if(size)
      *size = text_size;
  }

  return text;
}

int hextobin( unsigned char c )
{
  return c < 58 ? c - 48 : tolower(c) - 87;
}
int                oyBigEndian       ( void )
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}

void printfHelp(int argc, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a pipe converter"));
  fprintf( stderr, "  %s\n",
                  PACKAGE_STRING );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "      | %s\n\n", argv[0]);
  fprintf( stderr, "\n");


  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Convert from Hex to Binary:"));
  fprintf( stderr, "      cat EDID.txt | %s\n", argv[0]);
  fprintf( stderr, "        [-s] [-f %s] [-d %s] [-l %s]\n", _("PATTERN"), _("NUMBER"), _("MAX_LENGTH"));
  fprintf( stderr, "\n");
  fprintf( stderr, "      echo \"48656C6c6f20776f726C64210a\" | %s\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "      xrandr --props | %s -sf 00ffffffffffff00 -l 128 | xcmedid --openicc\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "      cat ~/.config/color.jcnf | %s -sf 00ffffffffffff00 -l 128 | xcmedid --openicc\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "      oyranos-monitor -lvcd 0 2>&1 | grep EDID: | %s -sf 00ffffffffffff00 -l 128 | xcmedid --openicc\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Convert from Binary to Hex:"));
  fprintf( stderr, "      cat EDID.bin | %s -r\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        -r              %s\n", _("reverse : binary to hexadecimal"));
  fprintf( stderr, "        -s              %s\n", _("skip white space"));
  fprintf( stderr, "        -f              %s\n", _("search for pattern in hexadecimal input"));
  fprintf( stderr, "        -d              %s\n", _("search for pattern in hexadecimal input n+1 times"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}

int main(int argc, char ** argv)
{
  size_t size = 0;
  unsigned char * in;
  unsigned int i,
               hex2bin = 1;
  int skip_space = 0,
      verbose = 0,
      len = 0;
  const char * file_name = NULL,
             * hex_filter = NULL;
  int hex_filter_pos = 0;

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
              case 'd': OY_PARSE_INT_ARG( hex_filter_pos ); break;
              case 'f': OY_PARSE_STRING_ARG( hex_filter ); break;
              case 'l': OY_PARSE_INT_ARG( len ); break;
              case 'r': hex2bin = 0; break;
              case 's': skip_space = 1; break;
              case 'v': verbose += 1; break;
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("reverse"))
                        { hex2bin = 0; i=100; break; }
                        else if(OY_IS_ARG("skip-space"))
                        { skip_space = 1; i=100; break; }
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
    FILE * fp = fopen(file_name,"rb");
    size_t s;
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      rewind(fp);
      in = malloc(size);
      s = fread(in, sizeof(char), size, fp);
      if(s != size)
        fprintf(stderr, "Error: fread %lu but should read %lu\n", s, size);
    } else
    {
      fprintf(stderr, "Error: Could not open file - \"%s\"\n", file_name);
    }
  } else
  {
    in = oyReadStdinToMem_( &size );
  }



  if(hex2bin)
  {
    char * edid = (char *) in;
    int count = 0;

    if(verbose)
      fprintf( stderr, "hextobin:\n");

    if(hex_filter)
    {
      char * lower_hex_filter = strdup(hex_filter);
      char * lower_in = strdup( (char*)  in );

      i = 0;
      while(lower_hex_filter[i]) { lower_hex_filter[i] = tolower(lower_hex_filter[i]); ++i; }
      i = 0;
      while(lower_in[i]) { lower_in[i] = tolower(lower_in[i]); ++i; }

      edid = strstr( (char*)lower_in, lower_hex_filter );
      for(i = 0; i < hex_filter_pos && edid; ++i)
        edid = strstr( (char*)edid+1, lower_hex_filter );
      if(verbose && edid)
        fprintf( stderr, "filter:%s\n", edid);

      if(edid)
        in = (unsigned char*) edid;
    }

    for(i = 0; i < size; ++i)
    {
      int c;

      if(skip_space)
        while( i < size && isspace(in[i]) ) ++i;

      if(!oyBigEndian())
        c = (hextobin( in[i] ) << 4) | hextobin( in[i+1] );
      else
        c = hextobin( in[i] ) | (hextobin( in[i+1] ) << 4);
      ++i;

      putchar( c );

      if(len && ++count >= len)
        break;
    }
  }
  else
  {
    if(verbose)
      fprintf( stderr, "bintohex:\n");

    for(i = 0; i < size; ++i)
    {
      char hex[4] = {0,0,0,0};
      sprintf( hex, "%02X", in[i] );
      printf( "%s", hex );
    }
  }

  return 0;
}
