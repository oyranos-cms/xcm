#include <X11/Xcm/XcmEdidParse.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
  XCM_EDID_ERROR_e err = XCM_EDID_WRONG_SIGNATURE;
  char * txt = 0;
  FILE * fp = 0;
  void * mem = 0;
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

    if(fp)
    {
      err = XcmEdidPrintString( mem, &txt, malloc );
      if(err)
        fprintf(stderr, "Error: %s\n", XcmEdidErrorToString(err));
      else
      {
        fprintf(stdout, "%s\n", txt);
        free(txt);
      }
      fclose (fp);
    }
  }

  if(err)
    fprintf(stderr, "\nUsage:\n\txcmedid EDID.bin\n\n");

  return err;
}
