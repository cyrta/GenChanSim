/* random number generator functions header file

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: seed.h,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: seed.h,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#ifndef _SEED_H
#define _SEED_H

#define		SEEDFILE	"/misc/seed.dat"	/* r.n.g. seed file */

/* functions */
void SaveSeedTable(unsigned long int seed[]);
extern long GetSeed(void);
extern void SaveSeed(long seed);

#endif
