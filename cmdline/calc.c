/* example calculator to demonstrate some 'cmdline' functions */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: calc.c,v 2.2 1994/07/25 18:18:32 jjb Exp $
   $Log: calc.c,v $
 * Revision 2.2  1994/07/25  18:18:32  jjb
 * Added copyright notice
 *

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Cmdline.h>

#define	NO	0
#define	YES	1

typedef struct Calc {
	float		s[4];					/* calculator stack */
	float		value;					/* value entered or operation result */
	enum {Result, Entered, Incomplete} display;	/* type of value */
	int	decimal;						/* number of decimal places */
} Calc;

extern void Multiply(), Divide(), Add(), Subtract(), enter(), dec(),
	n0(), n1(), n2(), n3(), n4(), n5(), n6(), n7(), n8(), n9(), push(), pop();
extern void number(Calc *, float);

static char rcsid[] = "$Id: calc.c,v 2.2 1994/07/25 18:18:32 jjb Exp $";

void main(int argc, char **argv) {
	Calc		calc = {0, 0, 0, 0, 0, Entered, 0};		/* calc data */

	CmdFloat("-Stack", &calc.s[0], cmdDef);
	CmdFloat("-v Value", &calc.value, cmdDef);
	CmdColumn();
	CmdFunc("-n7 7", &n7, &calc);
	CmdFunc("-n4 4", &n4, &calc);
	CmdFunc("-n1 1", &n1, &calc);
	CmdFunc("-dec .", &dec, &calc);
	CmdColumn();
	CmdFunc("-n8 8", &n8, &calc);
	CmdFunc("-n5 5", &n5, &calc);
	CmdFunc("-n2 2", &n2, &calc);
	CmdFunc("-n0 0", &n0, &calc);
	CmdColumn();
	CmdFunc("-n9 9", &n9, &calc);
	CmdFunc("-n6 6", &n6, &calc);
	CmdFunc("-n3 3", &n3, &calc);
	CmdFunc("-e Ent", &enter, &calc);
	CmdColumn();
	CmdFunc("-m x", &Multiply, &calc);
	CmdFunc("-d /", &Divide, &calc);
	CmdFunc("-a +", &Add, &calc);
	CmdFunc("-s -", &Subtract, &calc);
	CmdNoOk();
	CmdArgs(argc, argv);

	printf("value = %f\n", calc.value);
}

/* some buttons */
void n0(Calc *c) { number(c, 0.0); }
void n1(Calc *c) { number(c, 1.0); }
void n2(Calc *c) { number(c, 2.0); }
void n3(Calc *c) { number(c, 3.0); }
void n4(Calc *c) { number(c, 4.0); }
void n5(Calc *c) { number(c, 5.0); }
void n6(Calc *c) { number(c, 6.0); }
void n7(Calc *c) { number(c, 7.0); }
void n8(Calc *c) { number(c, 8.0); }
void n9(Calc *c) { number(c, 9.0); }
void dec(Calc *c) { number(c, 0.0); c->decimal = 1; }

/* enter button */
void enter(Calc *c) {
	c->display = Entered;
	push(c);
}

/* multiply and adjust stack */
void Multiply(Calc *c) {
	c->display = Result;
	c->value = c->s[0] * c->value;
	pop(c);
}

/* divide and adjust stack */
void Divide(Calc *c) {
	c->display = Result;
	c->value = c->s[0] / c->value;
	pop(c);
}

/* add and adjust stack */
void Add(Calc *c) {
	c->display = Result;
	c->value = c->s[0] + c->value;
	pop(c);
}

/* subtract and adjust stack */
void Subtract(Calc *c) {
	c->display = Result;
	c->value = c->s[0] - c->value;
	pop(c);
}

/* handle numbers entered */
void number(Calc *c, float number) {
	if(c->display == Result) {
		push(c);
		c->value = 0.0;
		c->decimal = 0;
	} else if(c->display == Entered) {
		c->value = 0.0;
		c->decimal = 0;
	}
	c->display = Incomplete;
	if(c->decimal) {
		c->value += number * pow(10.0, -(float)c->decimal);
		c->decimal++;
	} else {
		c->value = c->value * 10 + number;
	}
}

/* push value onto stack */
void push(Calc *c) {
	c->s[3] = c->s[2]; c->s[2] = c->s[1]; c->s[1] = c->s[0]; c->s[0] = c->value;
}

/* pop value off stack */
void pop(Calc *c) {
	c->s[0] = c->s[1]; c->s[1] = c->s[2]; c->s[2] = c->s[3];
}
