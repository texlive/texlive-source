/* x86/k10 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2011, 2014 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#define GMP_LIMB_BITS 32
#define GMP_LIMB_BYTES 4

/* 2400 MHz K10 Barcelona */
/* FFT tuning limit = 25000000 */
/* Generated by tuneup.c, 2014-03-13, gcc 4.5 */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD         12
#define MOD_1U_TO_MOD_1_1_THRESHOLD          7
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         9
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        12
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     15
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1N_PI1_METHOD                 1
#define DIV_QR_1_NORM_THRESHOLD              1
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           32

#define MUL_TOOM22_THRESHOLD                24
#define MUL_TOOM33_THRESHOLD                81
#define MUL_TOOM44_THRESHOLD               130
#define MUL_TOOM6H_THRESHOLD               189
#define MUL_TOOM8H_THRESHOLD               430

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      81
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      91
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      82
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      90
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     112

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 38
#define SQR_TOOM3_THRESHOLD                 77
#define SQR_TOOM4_THRESHOLD                184
#define SQR_TOOM6_THRESHOLD                262
#define SQR_TOOM8_THRESHOLD                369

#define MULMID_TOOM42_THRESHOLD             56

#define MULMOD_BNM1_THRESHOLD               17
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_MODF_THRESHOLD             765  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    765, 5}, {     25, 6}, {     13, 5}, {     27, 6}, \
    {     25, 7}, {     13, 6}, {     27, 7}, {     15, 6}, \
    {     31, 7}, {     17, 6}, {     35, 7}, {     19, 6}, \
    {     39, 7}, {     23, 6}, {     47, 7}, {     27, 8}, \
    {     15, 7}, {     35, 8}, {     19, 7}, {     41, 8}, \
    {     23, 7}, {     47, 8}, {     27, 9}, {     15, 8}, \
    {     31, 7}, {     63, 8}, {     39, 9}, {     23, 8}, \
    {     51, 9}, {     31, 8}, {     67, 9}, {     39, 8}, \
    {     79, 9}, {     47, 8}, {     95,10}, {     31, 9}, \
    {     63, 8}, {    127, 9}, {     79,10}, {     47, 9}, \
    {    103,11}, {     31,10}, {     63, 9}, {    135,10}, \
    {     79, 9}, {    159,10}, {     95, 9}, {    199,10}, \
    {    111,11}, {     63,10}, {    127, 9}, {    263,10}, \
    {    175,11}, {     95,10}, {    207,12}, {     63,11}, \
    {    127,10}, {    255, 9}, {    543, 8}, {   1087, 9}, \
    {    575,11}, {    159,10}, {    319, 9}, {    671, 8}, \
    {   1343, 9}, {    735,11}, {    191, 9}, {    799, 8}, \
    {   1599,10}, {    415, 9}, {    863,11}, {    223,12}, \
    {    127,11}, {    255,10}, {    543, 9}, {   1087,10}, \
    {    607, 9}, {   1215, 8}, {   2431,11}, {    319,10}, \
    {    671, 9}, {   1343,10}, {    735,12}, {    191,11}, \
    {    383,10}, {    799, 9}, {   1599,11}, {    415,10}, \
    {    863, 9}, {   1727,13}, {    127,12}, {    255,11}, \
    {    543,10}, {   1087,11}, {    607,10}, {   1215, 9}, \
    {   2431,12}, {    319,11}, {    671,10}, {   1343,11}, \
    {    735,10}, {   1471, 9}, {   2943, 8}, {   5887,12}, \
    {    383,11}, {    799,10}, {   1599,11}, {    863,10}, \
    {   1727,12}, {    447,11}, {    959,10}, {   1919,11}, \
    {    991,10}, {   1983,13}, {    255,12}, {    511,11}, \
    {   1087,12}, {    575,11}, {   1215,10}, {   2431,12}, \
    {    639,11}, {   1343,12}, {    703,11}, {   1471,10}, \
    {   2943, 9}, {   5887,13}, {    383,12}, {    767,11}, \
    {   1599,12}, {    831,11}, {   1727,10}, {   3455,12}, \
    {    959,11}, {   1983,14}, {    255,13}, {    511,12}, \
    {   1087,11}, {   2239,12}, {   1215,11}, {   2431,13}, \
    {    639,12}, {   1471,11}, {   2943,10}, {   5887,13}, \
    {    767,12}, {   1727,11}, {   3455,13}, {    895,12}, \
    {   1983,14}, {    511,13}, {   1023,12}, {   2239,13}, \
    {   1151,12}, {   2495,13}, {   1407,12}, {   2943,11}, \
    {   5887,14}, {    767,13}, {   1663,12}, {   3455,13}, \
    {   1919,12}, {   3839,15}, {    511,14}, {   1023,13}, \
    {   2175,12}, {   4351,13}, {   2431,14}, {   1279,13}, \
    {   2943,12}, {   5887,14}, {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 172
#define MUL_FFT_THRESHOLD                 6784

#define SQR_FFT_MODF_THRESHOLD             555  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    555, 5}, {     21, 6}, {     11, 5}, {     25, 6}, \
    {     13, 5}, {     27, 6}, {     27, 7}, {     15, 6}, \
    {     32, 7}, {     17, 6}, {     35, 7}, {     19, 6}, \
    {     39, 7}, {     27, 8}, {     15, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     23, 7}, {     47, 8}, \
    {     27, 9}, {     15, 8}, {     31, 7}, {     63, 8}, \
    {     39, 9}, {     23, 8}, {     51,10}, {     15, 9}, \
    {     31, 8}, {     67, 9}, {     39, 8}, {     79, 9}, \
    {     47,10}, {     31, 9}, {     79,10}, {     47, 9}, \
    {     95,11}, {     31,10}, {     63, 9}, {    127,10}, \
    {     79, 9}, {    167,10}, {     95, 9}, {    191,10}, \
    {    111,11}, {     63,10}, {    143, 9}, {    287, 8}, \
    {    575,10}, {    159,11}, {     95,10}, {    191,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    543, 8}, \
    {   1087,10}, {    287, 9}, {    607,11}, {    159,10}, \
    {    319, 9}, {    671, 8}, {   1343,10}, {    351, 9}, \
    {    735, 8}, {   1471,11}, {    191,10}, {    383, 9}, \
    {    767,10}, {    399, 9}, {    799, 8}, {   1599,10}, \
    {    415, 9}, {    863,11}, {    223,10}, {    479,12}, \
    {    127,11}, {    255,10}, {    543, 9}, {   1087,11}, \
    {    287,10}, {    607, 9}, {   1215, 8}, {   2431,11}, \
    {    319,10}, {    671, 9}, {   1343,11}, {    351,10}, \
    {    735, 9}, {   1471,12}, {    191,11}, {    383,10}, \
    {    799, 9}, {   1599,11}, {    415,10}, {    863, 9}, \
    {   1727,11}, {    479,13}, {    127,12}, {    255,11}, \
    {    511,10}, {   1023,11}, {    543,10}, {   1087,11}, \
    {    607,10}, {   1215, 9}, {   2431,12}, {    319,11}, \
    {    671,10}, {   1343,11}, {    735,10}, {   1471, 9}, \
    {   2943,12}, {    383,11}, {    799,10}, {   1599,11}, \
    {    863,10}, {   1727,12}, {    447,11}, {    959,10}, \
    {   1919,11}, {    991,10}, {   1983,12}, {    511,11}, \
    {   1087,12}, {    575,11}, {   1215,10}, {   2431,12}, \
    {    639,11}, {   1343,12}, {    703,11}, {   1471,10}, \
    {   2943,13}, {    383,12}, {    767,11}, {   1599,12}, \
    {    831,11}, {   1727,10}, {   3455,12}, {    959,11}, \
    {   1983,13}, {    511,12}, {   1215,11}, {   2431,13}, \
    {    639,12}, {   1471,11}, {   2943,13}, {    767,12}, \
    {   1727,11}, {   3455,13}, {    895,12}, {   1983,14}, \
    {    511,13}, {   1023,12}, {   2111,13}, {   1151,12}, \
    {   2431,13}, {   1407,12}, {   2943,14}, {    767,13}, \
    {   1663,12}, {   3455,13}, {   1919,12}, {   3839,15}, \
    {    511,14}, {   1023,13}, {   2431,14}, {   1279,13}, \
    {   2943,12}, {   5887,14}, {  16384,15}, {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 172
#define SQR_FFT_THRESHOLD                 5504

#define MULLO_BASECASE_THRESHOLD             7
#define MULLO_DC_THRESHOLD                  40
#define MULLO_MUL_N_THRESHOLD            13463

#define DC_DIV_QR_THRESHOLD                 59
#define DC_DIVAPPR_Q_THRESHOLD             270
#define DC_BDIV_QR_THRESHOLD                55
#define DC_BDIV_Q_THRESHOLD                206

#define INV_MULMOD_BNM1_THRESHOLD           62
#define INV_NEWTON_THRESHOLD               254
#define INV_APPR_THRESHOLD                 252

#define BINV_NEWTON_THRESHOLD              274
#define REDC_1_TO_REDC_N_THRESHOLD          74

#define MU_DIV_QR_THRESHOLD               1589
#define MU_DIVAPPR_Q_THRESHOLD            1589
#define MUPI_DIV_QR_THRESHOLD              106
#define MU_BDIV_QR_THRESHOLD              1470
#define MU_BDIV_Q_THRESHOLD               1558

#define POWM_SEC_TABLE  1,16,114,428,1240

#define MATRIX22_STRASSEN_THRESHOLD         19
#define HGCD_THRESHOLD                     136
#define HGCD_APPR_THRESHOLD                175
#define HGCD_REDUCE_THRESHOLD             3389
#define GCD_DC_THRESHOLD                   595
#define GCDEXT_DC_THRESHOLD                424
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                15
#define GET_STR_PRECOMPUTE_THRESHOLD        28
#define SET_STR_DC_THRESHOLD               100
#define SET_STR_PRECOMPUTE_THRESHOLD      1360

#define FAC_DSC_THRESHOLD                  224
#define FAC_ODD_THRESHOLD                   29
