/* Various Thresholds of MPFR, not exported.  -*- mode: C -*-

Copyright 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.

This file is part of the GNU MPFR Library.

The GNU MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFR Library; see the file COPYING.LESSER.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

/* Generated by MPFR's tuneup.c, 2011-07-31, gcc 4.4.5 */
/* gcc60.fsffrance.org (Madison) with gmp 5.0.2 */


#define MPFR_MULHIGH_TAB  \
 -1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, \
 -1,-1,14,16,16,17,18,19,20,20,22,22,20,21,22,23, \
 24,23,24,25,24,25,30,30,30,31,32,32,32,33,32,33, \
 32,33,34,37,36,37,38,35,36,40,36,39,40,41,40,43, \
 48,48,42,43,48,41,48,50,48,48,48,50,48,60,48,50, \
 60,64,60,60,60,60,60,66,64,64,64,63,64,65,68,64, \
 64,64,64,63,64,64,68,68,64,68,64,64,64,64,76,76, \
 80,76,76,72,72,80,76,76,76,82,80,80,80,80,80,80, \
 84,93,88,96,96,96,90,99,96,93,96,99,96,93,96,99, \
 96,96,102,105,108,99,108,105,108,105,108,111,108,111,108,111, \
 108,117,120,117,120,117,117,117,120,120,120,117,120,120,120,123, \
 120,117,120,123,120,120,126,141,120,141,141,141,140,141,144,141, \
 140,141,141,141,144,141,144,147,144,141,156,141,140,141,156,165, \
 164,165,156,165,164,165,165,165,164,165,164,165,164,165,165,165, \
 164,165,164,165,168,177,180,177,165,177,177,177,176,177,180,177, \
 180,177,180,177,176,177,180,165,168,189,180,189,192,189,186,189, \
 188,189,176,177,192,189,180,177,192,189,180,201,192,177,192,189, \
 192,189,189,189,188,213,212,213,192,213,200,201,192,213,200,201, \
 212,213,192,201,200,213,212,213,212,213,200,213,212,213,210,201, \
 212,213,236,212,212,213,212,213,216,213,236,213,236,235,236,252, \
 236,225,236,251,236,233,236,235,236,240,252,252,236,235,252,252, \
 236,249,252,251,252,252,252,251,256,256,256,251,252,251,252,251, \
 252,252,252,267,268,265,268,267,268,265,268,267,268,265,268,267, \
 268,267,268,283,268,281,284,283,284,281,268,283,284,281,284,283, \
 284,284,284,283,284,283,284,283,284,283,284,283,284,281,300,284, \
 284,284,300,300,300,316,284,315,284,313,300,315,316,284,316,315, \
 316,299,300,284,316,284,316,315,300,315,316,315,316,300,316,316, \
 316,313,316,315,316,313,316,315,316,316,316,315,320,316,316,315, \
 354,354,354,315,354,354,354,354,354,354,354,354,354,353,378,378, \
 354,354,354,378,354,354,354,354,354,377,378,354,354,377,378,378, \
 378,378,378,378,378,377,378,378,378,378,378,378,378,377,378,378, \
 378,378,378,378,378,377,378,378,378,378,378,378,378,378,378,378, \
 378,377,378,378,378,378,378,402,378,378,378,378,402,378,378,378, \
 402,426,426,426,426,402,426,426,402,402,426,426,402,402,426,426, \
 426,426,426,426,426,425,426,426,426,402,426,426,426,426,426,426, \
 426,426,426,426,426,426,426,426,472,472,426,426,472,426,472,472, \
 472,426,426,426,472,471,472,472,472,472,472,472,472,472,472,472, \
 472,472,472,472,472,472,472,472,472,472,472,472,472,472,472,472, \
 472,472,472,472,472,472,472,472,472,472,472,472,504,504,472,472, \
 504,504,504,472,472,472,472,504,472,472,472,472,504,472,504,504, \
 504,504,504,504,504,504,536,504,504,504,504,504,504,504,504,504, \
 504,504,536,535,536,504,536,536,536,504,536,536,504,504,504,504, \
 504,504,536,536,536,536,536,536,536,536,536,536,536,536,536,536, \
 536,536,536,536,536,536,536,536,536,536,536,536,536,536,536,536, \
 536,536,536,536,536,536,536,536,536,536,536,568,536,568,568,568, \
 568,568,568,568,568,568,568,568,568,568,568,568,568,568,568,568, \
 568,568,568,568,568,568,600,600,568,600,600,600,600,568,600,600, \
 600,600,600,600,600,600,600,600,600,600,599,600,600,600,600,600, \
 600,600,600,600,600,600,600,599,600,600,600,600,600,600,632,600, \
 600,600,600,600,632,600,600,600,632,600,632,632,632,632,632,632, \
 632,632,632,632,632,632,632,632,632,600,632,600,632,632,632,600, \
 664,632,664,664,632,632,664,664,664,664,632,664,664,632,664,664, \
 664,632,664,664,664,664,664,664,664,664,664,664,631,632,632,632, \
 664,632,664,664,664,664,663,664,664,664,736,664,736,664,664,664, \
 736,736,736,664,736,735,736,735,736,736,736,735,736,735,736,735, \
 736,736,664,735,736,736,736,736,736,735,736,735,736,735,736,736, \
 736,736,736,735,736,736,736,735,736,736,736,735,736,735,736,736, \
 736,736,760,760,736,760,760,760,760,760,784,760,760,783,784,760, \
 760,760,760,760,784,760,760,760,760,784,784,784,784,784,784,784, \
 736,760,784,784,784,784,784,783,784,783,784,783,784,783,784,784, \
 784,760,784,784,784,784,784,784,784,784,784,784,784,784,784,760, \
 784,784,784,784,784,783,784,831,784,784,784,784,784,784,784,831, \
 832,784,832,831,784,831,832,831,832,784,784,831,832,783,784,831, \
 832,784,832,831,832,784,856,856,856,856,856,856,856,856,856,856 \
  
#define MPFR_SQRHIGH_TAB  \
 -1,0,0,0,0,0,-1,-1,-1,6,-1,8,8,8,10,10, \
 10,12,11,11,12,12,14,14,14,14,16,15,16,16,18,19, \
 18,18,19,20,20,20,22,22,22,22,24,24,24,28,26,30, \
 28,32,30,28,28,28,30,30,30,30,32,34,34,34,36,36, \
 36,38,38,40,36,38,38,40,38,42,42,44,44,44,42,44, \
 44,46,46,48,44,50,46,52,48,54,50,52,48,54,50,52, \
 54,50,56,52,56,52,58,56,56,56,58,58,60,60,62,62, \
 60,64,62,62,68,68,66,66,64,68,66,70,72,72,66,68, \
 68,70,70,70,72,74,74,74,80,78,78,76,80,80,82,84, \
 84,82,86,80,82,82,84,84,86,86,88,92,88,90,86,92, \
 84,94,86,90,88,92,90,94,86,86,88,98,92,88,90,90, \
 94,92,92,94,96,92,94,94,96,100,98,98,96,100,102,102, \
 100,98,102,100,100,102,102,102,104,106,106,108,108,104,106,110, \
 108,106,114,108,108,124,110,110,116,110,114,116,112,114,114,128, \
 128,114,118,120,128,128,118,120,132,132,132,120,120,128,124,124, \
 128,144,124,128,128,128,140,136,132,132,128,136,136,128,136,140, \
 132,140,166,136,160,161,140,140,136,140,166,154,144,168,166,154, \
 154,155,142,142,160,167,166,166,148,166,178,167,160,154,154,179, \
 172,167,167,154,156,148,156,160,172,172,166,166,167,166,154,155, \
 168,167,166,162,172,168,167,178,166,178,167,178,180,174,173,184, \
 180,190,178,172,178,178,190,178,178,190,191,184,178,190,190,172, \
 191,184,178,192,180,180,191,184,192,190,190,190,184,190,190,190, \
 192,190,214,203,204,202,190,214,202,190,190,190,204,191,190,208, \
 204,216,196,203,216,208,215,212,209,214,214,216,215,220,215,216, \
 214,216,216,214,220,216,214,216,220,214,214,240,228,226,226,232, \
 226,234,228,238,233,238,226,226,232,220,238,240,244,239,239,214, \
 240,232,238,250,227,232,250,250,233,238,240,226,232,234,238,226, \
 252,232,238,244,239,240,238,238,239,240,238,262,246,256,250,246, \
 252,244,262,252,252,262,251,262,262,264,275,262,264,258,233,262, \
 268,234,263,236,263,240,239,270,238,244,239,244,240,240,249,244, \
 243,244,245,252,249,244,251,250,257,258,255,250,262,264,257,258, \
 250,262,262,258,257,262,262,274,274,262,273,262,263,274,275,274, \
 275,264,263,276,274,268,275,288,281,286,288,288,264,274,281,274, \
 295,274,296,300,293,274,275,296,295,288,292,280,311,312,300,304, \
 307,302,300,308,312,304,315,314,299,316,320,316,303,320,318,320, \
 315,312,320,320,318,320,307,336,316,334,313,332,327,336,312,334, \
 316,320,313,340,316,320,315,344,332,315,332,315,316,328,335,318, \
 332,320,331,308,332,331,326,315,316,331,345,332,347,344,347,347, \
 315,329,335,331,348,345,327,320,331,334,348,352,378,378,334,378, \
 351,390,332,378,348,390,390,378,377,390,377,378,390,377,329,402, \
 356,390,402,378,378,390,378,390,378,390,401,378,378,390,390,414, \
 388,426,401,426,378,401,378,426,389,390,402,426,402,402,426,414, \
 402,414,402,426,425,426,437,414,402,438,402,426,426,401,426,426, \
 426,390,426,390,426,425,426,414,426,426,402,426,426,426,426,426, \
 426,438,426,426,426,426,438,414,402,438,450,426,426,402,426,438, \
 426,426,426,426,437,438,426,426,438,438,450,426,426,438,426,426, \
 437,438,426,426,425,426,438,460,474,438,449,438,426,474,426,474, \
 425,438,426,425,486,462,438,474,438,462,450,474,426,426,426,474, \
 438,474,462,474,450,474,438,474,473,438,474,474,474,536,426,474, \
 486,486,474,486,474,474,536,474,474,536,474,474,474,536,462,536, \
 486,536,474,536,536,536,536,536,536,536,536,536,474,536,536,536, \
 535,536,535,536,536,536,536,536,536,536,536,536,536,536,536,536, \
 536,536,535,535,536,536,536,535,536,536,535,535,536,536,536,536, \
 536,536,536,536,536,552,536,535,536,535,536,535,536,536,536,535, \
 536,536,536,536,536,584,536,536,536,535,536,535,536,536,568,535, \
 552,536,568,536,536,584,536,535,536,536,536,536,536,536,600,535, \
 536,536,536,584,536,584,536,535,536,536,536,535,536,600,600,535, \
 536,600,568,536,568,568,536,584,536,536,536,536,600,584,567,584, \
 600,584,583,599,584,584,600,584,600,568,600,599,600,600,600,584, \
 616,600,600,600,600,600,600,599,600,600,600,600,600,600,600,599, \
 600,600,615,599,616,616,584,599,600,616,600,600,600,600,600,599, \
 616,616,600,600,616,616,600,600,600,616,600,599,600,600,600,599, \
 600,616,616,599,616,616,616,599,616,616,616,599,616,616,600,599, \
 616,616,600,616,616,616,632,648,664,648,616,648,600,600,632,664 \
  
#define MPFR_DIVHIGH_TAB  \
 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,13, \
 16,17,18,19,18,21,22,23,22,25,17,17,20,23,25,22, \
 24,25,21,21,24,29,29,31,28,28,26,31,30,28,29,30, \
 31,37,38,37,36,40,37,37,40,37,41,37,36,39,38,46, \
 42,43,42,43,40,37,42,43,44,49,42,43,40,45,46,47, \
 49,49,50,43,44,45,50,46,54,53,53,55,50,51,50,50, \
 54,56,54,52,55,53,57,55,56,57,58,58,56,62,66,59, \
 62,65,64,59,60,65,66,77,76,73,70,73,68,65,72,73, \
 76,69,72,71,70,81,72,85,76,77,74,77,80,81,76,77, \
 84,85,84,79,82,81,84,77,84,83,80,85,82,81,96,83, \
 96,85,85,91,96,84,92,95,92,100,92,96,88,98,96,99, \
 96,96,100,100,96,95,100,100,102,96,100,95,96,98,100,99, \
 109,100,100,100,104,112,104,117,104,109,120,117,108,110,124,109, \
 112,114,112,115,120,120,112,117,118,116,116,115,112,120,124,127, \
 128,127,120,116,120,127,125,129,120,123,120,131,131,129,132,136, \
 132,125,136,131,136,125,144,144,136,144,144,147,144,128,152,144, \
 148,149,132,144,150,145,147,145,152,151,144,143,152,164,152,152, \
 152,152,152,149,152,159,152,152,152,159,144,155,160,144,164,160, \
 152,152,168,163,148,160,168,160,155,164,170,152,152,160,160,164, \
 168,160,166,168,168,170,168,162,164,168,164,168,168,160,163,176, \
 168,166,176,167,168,168,169,176,168,190,186,176,192,171,184,192, \
 192,190,192,186,176,172,176,176,176,192,192,186,192,192,198,191, \
 192,198,192,198,192,192,192,216,192,192,192,191,192,191,216,198, \
 198,198,192,192,192,198,197,192,192,192,198,198,208,224,208,198, \
 198,216,198,198,216,208,216,208,216,222,216,215,208,209,234,224, \
 240,221,234,216,216,240,232,233,216,222,233,232,216,239,240,218, \
 224,234,240,240,240,240,224,232,240,240,216,240,224,233,230,240, \
 224,233,240,234,240,234,224,232,240,233,233,240,230,230,234,240, \
 240,232,240,240,240,228,240,240,235,240,240,232,240,238,240,240, \
 240,240,240,240,240,244,246,240,256,240,240,240,244,288,250,250, \
 256,288,288,256,246,282,288,288,256,288,256,288,248,293,254,288, \
 288,256,264,256,257,265,288,257,255,288,288,288,256,288,288,281, \
 292,280,288,292,288,287,280,288,287,280,288,282,288,294,288,288, \
 288,288,288,288,288,288,288,288,294,304,288,304,288,288,288,292, \
 294,288,288,288,280,294,292,294,292,328,288,282,328,288,288,291, \
 288,288,288,288,300,288,317,304,288,329,304,292,304,320,304,318, \
 327,325,324,326,304,312,336,304,329,320,328,330,328,330,336,320, \
 328,327,330,318,316,319,330,336,328,336,352,336,320,326,352,320, \
 336,325,326,324,342,329,327,328,336,336,336,336,336,328,352,330, \
 320,328,336,326,320,335,335,326,352,352,342,336,352,326,336,336, \
 336,352,329,328,342,328,342,336,326,336,384,352,384,329,352,336, \
 384,352,384,352,336,336,334,384,384,384,384,384,384,352,351,384, \
 384,384,383,372,384,372,384,384,352,368,384,384,382,396,384,372, \
 384,384,352,384,384,383,384,384,384,384,384,384,384,384,384,384, \
 372,384,384,396,384,384,384,384,384,384,384,384,384,384,384,384, \
 384,396,384,384,384,384,384,372,384,384,384,396,384,384,384,432, \
 384,383,384,384,384,396,384,384,384,396,382,384,384,396,394,384, \
 384,384,384,383,392,396,400,384,384,401,384,384,396,432,396,432, \
 392,408,432,396,396,396,416,396,416,432,448,428,424,420,394,395, \
 432,432,432,396,432,432,432,432,432,426,432,432,448,448,444,464, \
 416,440,432,425,432,430,448,432,432,432,432,432,432,432,442,432, \
 432,432,432,447,472,432,432,480,432,468,480,480,448,465,450,432, \
 480,465,448,464,480,448,472,467,468,480,438,479,464,465,468,472, \
 468,469,480,448,480,480,464,480,468,469,464,467,468,432,472,480, \
 464,480,472,468,480,468,472,472,468,477,480,471,472,467,448,480, \
 448,496,488,464,470,480,480,480,480,469,472,448,480,479,480,512, \
 492,477,480,469,480,464,480,480,464,480,500,480,472,480,472,479, \
 488,468,500,480,512,472,504,467,480,472,480,480,464,469,480,480, \
 480,472,496,500,480,480,480,480,472,480,480,496,480,576,480,480, \
 480,497,576,500,480,516,504,504,498,480,504,480,504,480,504,576, \
 512,496,512,508,576,529,512,500,576,534,534,504,512,515,528,576, \
 560,528,576,512,528,513,512,528,528,576,576,496,528,576,576,511, \
 512,504,576,576,560,512,576,504,576,576,576,576,564,576,564,576, \
 576,562,576,576,576,576,576,560,576,576,564,528,532,576,576,576 \
  
#define MPFR_MUL_THRESHOLD 26 /* limbs */
#define MPFR_SQR_THRESHOLD 19 /* limbs */
#define MPFR_DIV_THRESHOLD 44 /* limbs */
#define MPFR_EXP_2_THRESHOLD 1092 /* bits */
#define MPFR_EXP_THRESHOLD 5435 /* bits */
#define MPFR_SINCOS_THRESHOLD 24855 /* bits */
#define MPFR_AI_THRESHOLD1 -9637 /* threshold for negative input of mpfr_ai */
#define MPFR_AI_THRESHOLD2 922
#define MPFR_AI_THRESHOLD3 16031
/* Tuneup completed successfully, took 1058 seconds */
