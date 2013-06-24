#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <time.h>

typedef int (BGD_STDCALL *FuncPtr)(gdImagePtr, int, int);

#define GET_PIXEL_FUNCTION(src)(src->trueColor?gdImageGetTrueColorPixel:gdImageGetPixel)
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)<(b)?(b):(a))

#ifdef _WIN32
# define GD_SCATTER_SEED() (unsigned int)(time(0) * GetCurrentProcessId())
#else
# define GD_SCATTER_SEED() (unsigned int)(time(0) * getpid())
#endif

BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus)
{
	gdScatter s;

	s.sub  = sub;
	s.plus = plus;
	s.num_colors = 0;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

BGD_DECLARE(int) gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors)
{
	gdScatter s;

	s.sub  = sub;
	s.plus = plus;
	s.colors = colors;
	s.num_colors = num_colors;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr scatter)
{
	register int x, y;
	int dest_x, dest_y;
	int pxl, new_pxl;
	unsigned int n;
	int sub = scatter->sub, plus = scatter->plus;

	if (plus == 0 && sub == 0) {
		return 1;
	} else if (sub >= plus) {
		return 0;
	}

	(void)srand(scatter->seed);

	if (scatter->num_colors) {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				dest_x = (int) (x + ((rand() % (plus - sub)) + sub));
				dest_y = (int) (y + ((rand() % (plus - sub)) + sub));

				if (!gdImageBoundsSafe(im, dest_x, dest_y)) {
					continue;
				}

				pxl = gdImageGetPixel(im, x, y);
				new_pxl = gdImageGetPixel(im, dest_x, dest_y);

				for (n = 0; n < scatter->num_colors; n++) {
					if (pxl == scatter->colors[n]) {
						gdImageSetPixel(im, dest_x, dest_y, pxl);
						gdImageSetPixel(im, x, y, new_pxl);
					}
				}
			}
		}
	} else {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				dest_x = (int) (x + ((rand() % (plus - sub)) + sub));
				dest_y = (int) (y + ((rand() % (plus - sub)) + sub));

				if (!gdImageBoundsSafe(im, dest_x, dest_y)) {
					continue;
				}

				pxl = gdImageGetPixel(im, x, y);
				new_pxl = gdImageGetPixel(im, dest_x, dest_y);

				gdImageSetPixel(im, dest_x, dest_y, pxl);
				gdImageSetPixel(im, x, y, new_pxl);
			}
		}
	}

	return 1;
}

BGD_DECLARE(int) gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode)
{
	int x, y;

	if (block_size <= 0) {
		return 0;
	} else if (block_size == 1) {
		return 1;
	}
	switch (mode) {
	case GD_PIXELATE_UPPERLEFT:
		for (y = 0; y < im->sy; y += block_size) {
			for (x = 0; x < im->sx; x += block_size) {
				if (gdImageBoundsSafe(im, x, y)) {
					int c = gdImageGetPixel(im, x, y);
					gdImageFilledRectangle(im, x, y, x + block_size - 1, y + block_size - 1, c);
				}
			}
		}
		break;
	case GD_PIXELATE_AVERAGE:
		for (y = 0; y < im->sy; y += block_size) {
			for (x = 0; x < im->sx; x += block_size) {
				int a, r, g, b, c;
				int total;
				int cx, cy;

				a = r = g = b = c = total = 0;
				/* sampling */
				for (cy = 0; cy < block_size; cy++) {
					for (cx = 0; cx < block_size; cx++) {
						if (!gdImageBoundsSafe(im, x + cx, y + cy)) {
							continue;
						}
						c = gdImageGetPixel(im, x + cx, y + cy);
						a += gdImageAlpha(im, c);
						r += gdImageRed(im, c);
						g += gdImageGreen(im, c);
						b += gdImageBlue(im, c);
						total++;
					}
				}
				/* drawing */
				if (total > 0) {
					c = gdImageColorResolveAlpha(im, r / total, g / total, b / total, a / total);
					gdImageFilledRectangle(im, x, y, x + block_size - 1, y + block_size - 1, c);
				}
			}
		}
		break;
	default:
		return 0;
	}
	return 1;
}

/* invert src image */
BGD_DECLARE(int) gdImageNegate(gdImagePtr src)
{
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			new_pxl = gdImageColorAllocateAlpha(src, 255-r, 255-g, 255-b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, 255-r, 255-g, 255-b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/* Convert the image src to a grayscale image */
BGD_DECLARE(int) gdImageGrayScale(gdImagePtr src)
{
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);
			r = g = b = (int) (.299 * r + .587 * g + .114 * b);

			new_pxl = gdImageColorAllocateAlpha(src, r, g, b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, r, g, b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

/* Set the brightness level <level> for the image src */
BGD_DECLARE(int) gdImageBrightness(gdImagePtr src, int brightness)
{
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	FuncPtr f;

	if (src==NULL || (brightness < -255 || brightness > 255)) {
		return 0;
	}

	if (brightness==0) {
		return 1;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);

			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			r = r + brightness;
			g = g + brightness;
			b = b + brightness;

			r = (r > 255)? 255 : ((r < 0)? 0:r);
			g = (g > 255)? 255 : ((g < 0)? 0:g);
			b = (b > 255)? 255 : ((b < 0)? 0:b);

			new_pxl = gdImageColorAllocateAlpha(src, (int)r, (int)g, (int)b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)r, (int)g, (int)b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}


BGD_DECLARE(int) gdImageContrast(gdImagePtr src, double contrast)
{
	int x, y;
	int r,g,b,a;
	double rf,gf,bf;
	int new_pxl, pxl;

	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	contrast = (double)(100.0-contrast)/100.0;
	contrast = contrast*contrast;

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f(src, x, y);

			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			rf = (double)r/255.0;
			rf = rf-0.5;
			rf = rf*contrast;
			rf = rf+0.5;
			rf = rf*255.0;

			bf = (double)b/255.0;
			bf = bf-0.5;
			bf = bf*contrast;
			bf = bf+0.5;
			bf = bf*255.0;

			gf = (double)g/255.0;
			gf = gf-0.5;
			gf = gf*contrast;
			gf = gf+0.5;
			gf = gf*255.0;

			rf = (rf > 255.0)? 255.0 : ((rf < 0.0)? 0.0:rf);
			gf = (gf > 255.0)? 255.0 : ((gf < 0.0)? 0.0:gf);
			bf = (bf > 255.0)? 255.0 : ((bf < 0.0)? 0.0:bf);

			new_pxl = gdImageColorAllocateAlpha(src, (int)rf, (int)gf, (int)bf, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)rf, (int)gf, (int)bf, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}


BGD_DECLARE(int) gdImageColor(gdImagePtr src, const int red, const int green, const int blue, const int alpha)
{
	int x, y;
	int new_pxl, pxl;
	FuncPtr f;

	if (src == NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			int r,g,b,a;

			pxl = f(src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			r = r + red;
			g = g + green;
			b = b + blue;
			a = a + alpha;

			r = (r > 255)? 255 : ((r < 0)? 0 : r);
			g = (g > 255)? 255 : ((g < 0)? 0 : g);
			b = (b > 255)? 255 : ((b < 0)? 0 : b);
			a = (a > 127)? 127 : ((a < 0)? 0 : a);

			new_pxl = gdImageColorAllocateAlpha(src, r, g, b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, r, g, b, a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	return 1;
}

BGD_DECLARE(int) gdImageConvolution(gdImagePtr src, float filter[3][3], float filter_div, float offset)
{
	int         x, y, i, j, new_a;
	float       new_r, new_g, new_b;
	int         new_pxl, pxl=0;
	gdImagePtr  srcback;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	/* We need the orinal image with each safe neoghb. pixel */
	srcback = gdImageCreateTrueColor (src->sx, src->sy);
	if (srcback==NULL) {
		return 0;
	}

	gdImageSaveAlpha(srcback, 1);
	new_pxl = gdImageColorAllocateAlpha(srcback, 0, 0, 0, 127);
	gdImageFill(srcback, 0, 0, new_pxl);

	gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);

	f = GET_PIXEL_FUNCTION(src);

	for ( y=0; y<src->sy; y++) {
		for(x=0; x<src->sx; x++) {
			new_r = new_g = new_b = 0;
			new_a = gdImageAlpha(srcback, pxl);

			for (j=0; j<3; j++) {
				int yv = MIN(MAX(y - 1 + j, 0), src->sy - 1);
				for (i=0; i<3; i++) {
				        pxl = f(srcback, MIN(MAX(x - 1 + i, 0), src->sx - 1), yv);
					new_r += (float)gdImageRed(srcback, pxl) * filter[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * filter[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * filter[j][i];
				}
			}

			new_r = (new_r/filter_div)+offset;
			new_g = (new_g/filter_div)+offset;
			new_b = (new_b/filter_div)+offset;

			new_r = (new_r > 255.0f)? 255.0f : ((new_r < 0.0f)? 0.0f:new_r);
			new_g = (new_g > 255.0f)? 255.0f : ((new_g < 0.0f)? 0.0f:new_g);
			new_b = (new_b > 255.0f)? 255.0f : ((new_b < 0.0f)? 0.0f:new_b);

			new_pxl = gdImageColorAllocateAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	gdImageDestroy(srcback);
	return 1;
}

BGD_DECLARE(int) gdImageSelectiveBlur( gdImagePtr src)
{
	int         x, y, i, j;
	float       new_r, new_g, new_b;
	int         new_pxl, cpxl, pxl, new_a=0;
	float flt_r [3][3];
	float flt_g [3][3];
	float flt_b [3][3];
	float flt_r_sum, flt_g_sum, flt_b_sum;

	gdImagePtr srcback;
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	/* We need the orinal image with each safe neoghb. pixel */
	srcback = gdImageCreateTrueColor (src->sx, src->sy);
	if (srcback==NULL) {
		return 0;
	}
	gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);

	f = GET_PIXEL_FUNCTION(src);

	for(y = 0; y<src->sy; y++) {
		for (x=0; x<src->sx; x++) {
		      flt_r_sum = flt_g_sum = flt_b_sum = 0.0;
			cpxl = f(src, x, y);

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if ((j == 1) && (i == 1)) {
						flt_r[1][1] = flt_g[1][1] = flt_b[1][1] = 0.5;
					} else {
						pxl = f(src, x-(3>>1)+i, y-(3>>1)+j);
						new_a = gdImageAlpha(srcback, pxl);

						new_r = ((float)gdImageRed(srcback, cpxl)) - ((float)gdImageRed (srcback, pxl));

						if (new_r < 0.0f) {
							new_r = -new_r;
						}
						if (new_r != 0) {
							flt_r[j][i] = 1.0f/new_r;
						} else {
							flt_r[j][i] = 1.0f;
						}

						new_g = ((float)gdImageGreen(srcback, cpxl)) - ((float)gdImageGreen(srcback, pxl));

						if (new_g < 0.0f) {
							new_g = -new_g;
						}
						if (new_g != 0) {
							flt_g[j][i] = 1.0f/new_g;
						} else {
							flt_g[j][i] = 1.0f;
						}

						new_b = ((float)gdImageBlue(srcback, cpxl)) - ((float)gdImageBlue(srcback, pxl));

						if (new_b < 0.0f) {
							new_b = -new_b;
						}
						if (new_b != 0) {
							flt_b[j][i] = 1.0f/new_b;
						} else {
							flt_b[j][i] = 1.0f;
						}
					}

					flt_r_sum += flt_r[j][i];
					flt_g_sum += flt_g[j][i];
					flt_b_sum += flt_b [j][i];
				}
			}

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if (flt_r_sum != 0.0) {
						flt_r[j][i] /= flt_r_sum;
					}
					if (flt_g_sum != 0.0) {
						flt_g[j][i] /= flt_g_sum;
					}
					if (flt_b_sum != 0.0) {
						flt_b [j][i] /= flt_b_sum;
					}
				}
			}

			new_r = new_g = new_b = 0.0;

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					pxl = f(src, x-(3>>1)+i, y-(3>>1)+j);
					new_r += (float)gdImageRed(srcback, pxl) * flt_r[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * flt_g[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * flt_b[j][i];
				}
			}

			new_r = (new_r > 255.0f)? 255.0f : ((new_r < 0.0f)? 0.0f:new_r);
			new_g = (new_g > 255.0f)? 255.0f : ((new_g < 0.0f)? 0.0f:new_g);
			new_b = (new_b > 255.0f)? 255.0f : ((new_b < 0.0f)? 0.0f:new_b);
			new_pxl = gdImageColorAllocateAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			}
			gdImageSetPixel (src, x, y, new_pxl);
		}
	}
	gdImageDestroy(srcback);
	return 1;
}

BGD_DECLARE(int) gdImageEdgeDetectQuick(gdImagePtr src)
{
	float filter[3][3] =	{{-1.0,0.0,-1.0},
				{0.0,4.0,0.0},
				{-1.0,0.0,-1.0}};

	return gdImageConvolution(src, filter, 1, 127);
}

BGD_DECLARE(int) gdImageGaussianBlur(gdImagePtr im)
{
	float filter[3][3] =	{{1.0,2.0,1.0},
				{2.0,4.0,2.0},
				{1.0,2.0,1.0}};

	return gdImageConvolution(im, filter, 16, 0);
}

BGD_DECLARE(int) gdImageEmboss(gdImagePtr im)
{
/*
	float filter[3][3] =	{{1.0,1.0,1.0},
				{0.0,0.0,0.0},
				{-1.0,-1.0,-1.0}};
*/
	float filter[3][3] =	{{ 1.5, 0.0, 0.0},
				 { 0.0, 0.0, 0.0},
				 { 0.0, 0.0,-1.5}};

	return gdImageConvolution(im, filter, 1, 127);
}

BGD_DECLARE(int) gdImageMeanRemoval(gdImagePtr im)
{
	float filter[3][3] =	{{-1.0,-1.0,-1.0},
				{-1.0,9.0,-1.0},
				{-1.0,-1.0,-1.0}};

	return gdImageConvolution(im, filter, 1, 0);
}

BGD_DECLARE(int) gdImageSmooth(gdImagePtr im, float weight)
{
	float filter[3][3] =	{{1.0,1.0,1.0},
				{1.0,0.0,1.0},
				{1.0,1.0,1.0}};

	filter[1][1] = weight;

	return gdImageConvolution(im, filter, weight+8, 0);
}
