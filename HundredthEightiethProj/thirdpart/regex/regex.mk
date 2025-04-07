gcc -c -o regcomp.o -g -O0 -mthreads  -DHAVE_W32API_H -DNDEBUG -I..\ -I..\ -D__WXMSW__  -D_UNICODE   -MTregcomp.o -MFregcomp.o.d -MD -MP regcomp.c
gcc -c -o regexec.o -g -O0 -mthreads  -DHAVE_W32API_H -DNDEBUG -I..\ -I..\ -D__WXMSW__  -D_UNICODE   -MTregexec.o -MFregexec.o.d -MD -MP regexec.c
gcc -c -o regerror.o -g -O0 -mthreads  -DHAVE_W32API_H -DNDEBUG -I..\ -I..\ -D__WXMSW__  -D_UNICODE   -MTregerror.o -MFregerror.o.d -MD -MP regerror.c
gcc -c -o regfree.o -g -O0 -mthreads  -DHAVE_W32API_H -DNDEBUG -I..\ -I..\ -D__WXMSW__  -D_UNICODE   -MTregfree.o -MFregfree.o.d -MD -MP regfree.c