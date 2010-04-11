#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include <ptexenc/ptexenc.h>
#include "makejvf.h"

FILE *vfp,*afp=NULL;
char *atfmname,*vtfmname,*afmname,*vfname,*kanatfm;
int kanatume=-1,chotai=0,baseshift=0,minute=0;

int main(int argc, char ** argv)
{
	int i,j;

	set_enc_string(NULL, "EUC");
	if (argc < 3) {
		usage();
		exit(0);
	}

	argv++;

	for (;**argv=='-';argv++) {
		switch ((*argv)[1]) {
		case 'k':
			if ((*argv)[2]!='\0') {
				kanatume = atoi(&(*argv)[2]);
			}
			else {
				kanatume = atoi(*(++argv));
			}
			break;
		case 'K':
			if ((*argv)[2]!='\0') {
				kanatfm = strdup(&(*argv)[2]);
			}
			else {
				kanatfm = strdup(*(++argv));
			}
			break;
		case 'C':
			chotai=1;
			break;
		case 'a':
			if ((*argv)[2]!='\0') {
				afmname = strdup(&(*argv)[2]);
			}
			else {
				afmname = strdup(*(++argv));
			}
			if ((afp = fopen(afmname,"r"))==NULL) {
				fprintf(stderr,"no AFM file, %s.\n",afmname);
				exit(-1);
			}
			break;
		case 'b':
			if ((*argv)[2]!='\0') {
				baseshift = atoi(&(*argv)[2]);
			}
			else {
				baseshift = atoi(*(++argv));
			}
			break;
		case 'm':
			minute=1;
			break;
		default:
			usage();
			exit(0);
		}
	}

	if (kanatume>=0 && !afp) {
		fprintf(stderr,"No AFM file for kanatume.\n");
		exit(-1);
	}

	atfmname = malloc(strlen(*argv)+4);
	strcpy(atfmname,*argv);

	vfname = malloc(strlen(*argv)+4);
	strcpy(vfname,*argv);
	for (i = strlen(vfname)-1 ; i >= 0 ; i--) {
		if (vfname[i] == '/') {
			vfname = &vfname[i+1];
			break;
		}
	}
	if (!strcmp(&vfname[strlen(vfname)-4],".tfm")) {
		vfname[strlen(vfname)-4] = '\0';
	}
	strcat(vfname,".vf");

	argv++;

	vtfmname = strdup(*argv);
	if (!strcmp(&vtfmname[strlen(vtfmname)-4],".tfm")) {
		vtfmname[strlen(vtfmname)-4] = '\0';
	}

	tfmget(atfmname);

	maketfm(vtfmname);

	if (kanatfm) {
		if (!strcmp(&kanatfm[strlen(kanatfm)-4],".tfm")) {
			kanatfm[strlen(kanatfm)-4] = '\0';
		}
		maketfm(kanatfm);
	}

	vfp = vfopen(vfname);

	for (i=0;i<94;i++)
		for (j=0;j<94;j++)
			writevf((0x21+i)*256+(0x21+j),vfp);

	vfclose(vfp);

	exit(0);
}

void usage(void)
{
	fputs2("MAKEJVF ver.1.1a -- make Japanese VF file.\n", stderr);
	fputs2("%% makejvf [<options>] <TFMfile> <PSfontTFM>\n", stderr);
	fputs2("options:\n", stderr);
	fputs2("-C           Ĺ�Υ⡼��\n", stderr);
	fputs2("-K <TFMfile> ��������Ѥ˺�������PS�ե����TFM̾\n", stderr);
	fputs2("-b <����>    �١����饤������\n", stderr);
	fputs2("             ʸ���ι⤵��1000�Ȥ��������ǻ���\n", stderr);
	fputs2("             �ץ饹��ʸ���������ꡢ�ޥ��ʥ���ʸ�����夬��\n", stderr);
	fputs2("-m           �Ľ񤭻��˥�������(�ǡ�)������˥ߥ˥塼��(���)�����\n", stderr);
	fputs2("-a <AFMfile> AFM�ե�����̾�ʤ��ʵͤ���˻��ѡ�\n", stderr);
	fputs2("-k <����>    ���ʵͤ�ޡ��������\n", stderr);
	fputs2("             ʸ������1000�Ȥ��������ǻ��ꡣ-a���ץ����ȶ��˻���\n", stderr);
}
