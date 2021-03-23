#include <stdio.h>
void print_func();
main()
{
char name[4][100];

          sscanf("abcd","%s",name[0]);
          sscanf("efgh","%s",name[1]);
          sscanf("ijkl","%s",name[2]);
          sscanf("mnop","%s",name[3]);
		/*  printf("name 1 %s\n name2 %s\n name3 %s\n name4 %s\n",
				 name[0],name[1],name[2],name[3]);*/

   print_func(name);
}
void print_func(nm)
char *nm;
{
printf("name 1 %s\n name2 %s\n name3 %s\n name4 %s\n",
				 nm,nm+100,nm+200,nm+300);
}
