/*
POSIX getopt for Windows

AT&T Public License

Code given out at the 1985 UNIFORUM conference in Dallas.
*/

#ifndef _MY_GETOPT_H_
#define _MY_GETOPT_H_

#ifdef __GNUC__
#include <getopt.h>
#endif

#ifndef __GNUC__

extern int opterr;
extern int optind;
extern int optopt;
extern char* optarg;
extern int getopt(int argc, char** argv, char* opts);

#endif  /* __GNUC__ */
#endif  /* _GETOPT_H_ */
