#define banner  \
"extconv (CJK ver. 4.7.0)" \

/*2:*/
#line 94 "/home/wl/cvs/cjk/cjk-4.7.0/utils/extconv/extconv.w"

#include <stdio.h> 
#include <stdlib.h> 


int main(argc,argv)
int argc;
char*argv[];

{int ch;


fprintf(stdout,"\\def\\CJKpreproc{%s}",banner);

ch= fgetc(stdin);

while(!feof(stdin))
{if(ch>=0x81&&ch<=0xFE)
{fprintf(stdout,"\177%c\177",ch);

ch= fgetc(stdin);
if(!feof(stdin))
fprintf(stdout,"%d\177",ch);
}
else
fputc(ch,stdout);

ch= fgetc(stdin);
}
exit(EXIT_SUCCESS);
return 0;
}/*:2*/
