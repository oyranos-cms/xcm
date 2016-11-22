/* gcc -Wall -g hextobin.c -o hextobin */
/* gcc -Wall -g hextobin.c -o hextobin && oyranos-monitor -clv 2>&1  | grep ^EDID: | sed s/EDID:0x// | ./hextobin | ./hextobin -r | ./hextobin | xcmedid --openicc */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h> /* uint64_t - a bit fragile */

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

int main(int argc, char ** argv)
{
  size_t size = 0;
  unsigned char * in = oyReadStdinToMem_( &size );
  unsigned int i,
               hex2bin = 1;

  if(argc == 2 && strcmp(argv[1],"-r") == 0)
    hex2bin = 0;
  else if(argc > 1)
  {
    printf("usage:\n$ cat hexadecimal | %s\n", argv[0]);
    printf("example:\n$ echo \"48656C6c6f20776f726C64210a\" | %s\n", argv[0]);
    return 0;
  }

  if(hex2bin)
  {
    fprintf( stderr, "hextobin:\n");
    for(i = 0; i < size; i += 2)
    {
      int c;
      if(!oyBigEndian())
        c = (hextobin( in[i] ) << 4) | hextobin( in[i+1] );
      else
        c = hextobin( in[i] ) | (hextobin( in[i+1] ) << 4);

      putchar( c );
    }
  }
  else
  {
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
