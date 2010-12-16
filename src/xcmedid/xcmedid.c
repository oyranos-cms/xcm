#include <X11/Xcm/XcmEdidParse.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
  XCM_EDID_ERROR_e err = XCM_EDID_WRONG_SIGNATURE;
  char * txt = 0;
  FILE * fp = 0;
  char * mem = 0;
  size_t size = 0;
  int s = 0;

  if(argc > 1)
  {
    fp = fopen(argv[1],"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      rewind(fp);
      mem = malloc(size);
      s = fread(mem, sizeof(char), size, fp);
    } else
    {
      fprintf(stderr, "Error: Could not open file - \"%s\"\n", argv[1]);
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
      err = XcmEdidPrintString( mem, &txt, malloc );
      if(err)
        fprintf(stderr, "Error: %s (%d)\n", XcmEdidErrorToString(err), size);
      else
      {
        fprintf(stdout, "%s\n", txt);
        free(txt);
      }
  }

  if(fp && fp != stdin)
    fclose (fp);

  if(err)
    fprintf(stderr, "\nUsage:\n\txcmedid EDID.bin\n\n");

  return err;
}
