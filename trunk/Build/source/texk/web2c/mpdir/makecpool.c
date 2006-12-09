#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *filename, *headername, data[1024];
  FILE *fh;
  if (argc!=3) {
	puts("Need exactly two arguments: pool_name and header_name");
    exit(1);
  }
  filename = argv[1];
  headername = argv[2];
  fh = fopen(filename,"r");
  printf(
    "/* This file is auto-generated by makecpool */\n"
    "\n"
    "#include <stdio.h>\n"
    "#include \"%s\"\n"
    "\n"
    "static char *poolfilearr[] = {\n",headername);
  while (fgets(data,1024,fh)) {
    int len = strlen(data);
    int o = 0; // skip first o characters
    int i;
    if (data[len-1]=='\n') { // chomp;
      data[len-1] = 0;
      len--;
    }
    if (data[0]=='*') break; // last if /^\*/;
    if (data[0]>='0' && data[0]<='9' && data[1]>='0' && data[1]<='9') {
      o=2; // $data =~ s/^\d\d//;
    }
    printf("  \"");
    for (i=o; i<len; i++) {
      if (data[i]=='"' || data[i]=='\\') putchar('\\');
	  if (data[i]=='?') printf("\" \""); /* suppress trigraphs */
      putchar(data[i]);
    } // $data =~ s/(["\\])/\\$1/g;
    printf("\",\n");
  }
  fclose(fh);
  printf("  NULL };\n"
    "int loadpoolstrings (integer spare_size) {\n"
    "  strnumber g=0;\n"
    "  int i=0,j=0;\n"
    "  char *s;\n"
    "  while ((s = poolfilearr[j++])) {\n"
    "    int l = strlen (s);\n"
    "    i += l;\n"
    "    if (i>=spare_size) return 0;\n"
    "    while (l-- > 0) strpool[poolptr++] = *s++;\n"
    "    g = makestring();\n"
    "    strref[g]= 127;\n"
    "  }\n"
    "  return g;\n"
    "}\n");
  return 0;
}
