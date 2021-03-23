/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: qromb.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: qromb.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
#include <math.h>
#include <malloc.h>

/* origignal values
#define EPS 1.0e-6
#define JMAX 20
#define JMAXP JMAX+1
#define K 5
 */
#define EPS 1.0e-6
#define JMAX 20
#define JMAXP JMAX+1
#define K 5

static char rcsid[] = "$Id: qromb.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

float qromb(func,a,b)
float a,b;
/* non-ANSI
float (*func)();
 */
float (*func)(float);
{
	float ss,dss,trapzd();
	float s[JMAXP+1],h[JMAXP+1];
	int j;
	void polint(),nrerror();

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=trapzd(func,a,b,j);
		if (j >= K) {
			polint(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
			if (fabs(dss) < EPS*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	nrerror("Too many steps in routine QROMB");
	return -1;
}

#undef EPS
#undef JMAX
#undef JMAXP
#undef K


#include <math.h>

void polint(xa,ya,n,x,y,dy)
float xa[],ya[],x,*y,*dy;
int n;
{
	int i,m,ns=1;
	float den,dif,dift,ho,hp,w;
	float *c,*d,*vector();
	void nrerror(),free_vector();

	dif=fabs(x-xa[1]);
	c=vector(1,n);
	d=vector(1,n);
	for (i=1;i<=n;i++) {
		if ( (dift=fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	*y=ya[ns--];
	for (m=1;m<n;m++) {
		for (i=1;i<=n-m;i++) {
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if ( (den=ho-hp) == 0.0) nrerror("Error in routine POLINT");
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	free_vector(d,1,n);
	free_vector(c,1,n);
}


#define FUNC(x) ((*func)(x))

float trapzd(func,a,b,n)
float a,b;
/* non-ANSI
float (*func)();
 */
float (*func)(float);
int n;
{
	float x,tnm,sum,del;
	static float s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		return (s=0.5*(b-a)*(FUNC(a)+FUNC(b)));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FUNC(x);
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}

float *vector(int i, int n) {
	float *mem;

	mem = (float *)calloc(n-i+2, sizeof(float));
	return(mem);
}

void free_vector(float *vector, int i, int n) {
	free(vector);
}

void nrerror(char *fmt) {
	printf(fmt);
	exit(1);
}
