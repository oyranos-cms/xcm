/** xcm_macros.h
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011 - Kai-Uwe Behrmann <ku.b@gmx.de>
 */

#ifndef USE_GETTEXT
#define _(text) text
#endif

#include "config.h"

#ifdef HAVE_OY
#include <alpha/oyranos_alpha.h>
void * fromMD5                       ( const void        * md5_hash,
                                       size_t            * size,
                                       void              *(allocate_func)(size_t) );
char * getName                       ( const void        * data,
                                       size_t              size,
                                       void              *(allocate_func)(size_t),
                                       int                 file_name );
#define verbose oy_debug
#else
#define oy_debug verbose
#endif

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


