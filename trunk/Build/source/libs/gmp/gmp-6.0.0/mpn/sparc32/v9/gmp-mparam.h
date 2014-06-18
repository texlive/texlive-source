/* SPARC v9 32-bit gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999-2002, 2004, 2009-2011, 2014 Free Software
Foundation, Inc.

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

/* 1593 MHz ultrasparc3 running Solaris 10 (swift.nada.kth.se) */
/* FFT tuning limit = 25000000 */
/* Generated by tuneup.c, 2014-03-16, gcc 3.4 */

#define DIVREM_1_NORM_THRESHOLD              3
#define DIVREM_1_UNNORM_THRESHOLD            4
#define MOD_1_1P_METHOD                      2
#define MOD_1_NORM_THRESHOLD                 3
#define MOD_1_UNNORM_THRESHOLD               4
#define MOD_1N_TO_MOD_1_1_THRESHOLD         13
#define MOD_1U_TO_MOD_1_1_THRESHOLD         12
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0  /* never mpn_mod_1_1p */
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        22
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     32
#define USE_PREINV_DIVREM_1                  1
#define DIV_QR_1N_PI1_METHOD                 1
#define DIV_QR_1_NORM_THRESHOLD              4
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD        MP_SIZE_T_MAX  /* never */

#define MUL_TOOM22_THRESHOLD                28
#define MUL_TOOM33_THRESHOLD                43
#define MUL_TOOM44_THRESHOLD               126
#define MUL_TOOM6H_THRESHOLD               161
#define MUL_TOOM8H_THRESHOLD               208

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD      80
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      85
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      55
#define MUL_TOOM43_TO_TOOM54_THRESHOLD      72

#define SQR_BASECASE_THRESHOLD               4
#define SQR_TOOM2_THRESHOLD                 64
#define SQR_TOOM3_THRESHOLD                 85
#define SQR_TOOM4_THRESHOLD                152
#define SQR_TOOM6_THRESHOLD                185
#define SQR_TOOM8_THRESHOLD                324

#define MULMID_TOOM42_THRESHOLD             64

#define MULMOD_BNM1_THRESHOLD               12
#define SQRMOD_BNM1_THRESHOLD               16

#define MUL_FFT_MODF_THRESHOLD             288  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    288, 5}, {      9, 4}, {     19, 5}, {     11, 6}, \
    {      6, 5}, {     14, 6}, {      8, 5}, {     17, 6}, \
    {      9, 5}, {     20, 6}, {     13, 7}, {      7, 6}, \
    {     16, 7}, {      9, 6}, {     19, 7}, {     11, 6}, \
    {     23, 7}, {     13, 8}, {      7, 7}, {     15, 6}, \
    {     31, 7}, {     19, 8}, {     11, 7}, {     23, 9}, \
    {      7, 8}, {     15, 7}, {     31, 8}, {     19, 7}, \
    {     39, 8}, {     27, 9}, {     15, 8}, {     31, 7}, \
    {     63, 8}, {     39, 9}, {     23, 8}, {     47,10}, \
    {     15, 9}, {     31, 8}, {     67, 9}, {     39, 8}, \
    {     79, 9}, {     47,10}, {     31, 9}, {     71, 8}, \
    {    143, 9}, {     79,10}, {     47, 9}, {     95,11}, \
    {     31,10}, {     63, 9}, {    135, 8}, {    271, 9}, \
    {    143, 8}, {    287,10}, {     79, 9}, {    175,10}, \
    {     95, 9}, {    191, 8}, {    383,10}, {    111,11}, \
    {     63,10}, {    143, 9}, {    287, 8}, {    575,10}, \
    {    175,11}, {     95,10}, {    191, 9}, {    415, 8}, \
    {    831,12}, {     63,11}, {    127,10}, {    287, 9}, \
    {    575,11}, {    159,10}, {    351, 9}, {    703,11}, \
    {    191,10}, {    415, 9}, {    831,11}, {    223,10}, \
    {    447, 9}, {    895, 8}, {   1791,12}, {    127,11}, \
    {    287,10}, {    607, 9}, {   1215, 8}, {   2431,11}, \
    {    319, 9}, {   1279,11}, {    351,12}, {    191,11}, \
    {    415,10}, {    831,11}, {    447,10}, {    895, 9}, \
    {   1791,11}, {    479,13}, {    127,12}, {    255,11}, \
    {    575,10}, {   1151,11}, {    607,12}, {    319,11}, \
    {    703,12}, {    383,11}, {    831,12}, {    447,11}, \
    {    895,10}, {   1791,11}, {    959,13}, {    255,12}, \
    {    575,11}, {   1215,10}, {   2431,12}, {    703,13}, \
    {    383,12}, {    959,14}, {    255,13}, {    511,12}, \
    {   1087,11}, {   2175,12}, {   1215,11}, {   2431,13}, \
    {    639,12}, {   1407,11}, {   2943,13}, {    895,12}, \
    {   1919,14}, {    511,13}, {   1151,12}, {   2431,13}, \
    {   1407,14}, {    767,13}, {   1791,15}, {    511,14}, \
    {   1023,13}, {   2431,14}, {   1279,13}, {   2943,12}, \
    {   5887,14}, {  16384,15}, {  32768,16} }
#define MUL_FFT_TABLE3_SIZE 143
#define MUL_FFT_THRESHOLD                 2240

#define SQR_FFT_MODF_THRESHOLD             244  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    244, 5}, {      8, 4}, {     17, 5}, {     17, 6}, \
    {      9, 5}, {     19, 6}, {     17, 7}, {      9, 6}, \
    {     20, 7}, {     11, 6}, {     23, 7}, {     13, 8}, \
    {      7, 7}, {     19, 8}, {     11, 7}, {     25, 9}, \
    {      7, 8}, {     15, 7}, {     33, 8}, {     19, 7}, \
    {     39, 8}, {     23, 9}, {     15, 8}, {     39, 9}, \
    {     23,10}, {     15, 9}, {     31, 8}, {     63, 9}, \
    {     47,10}, {     31, 9}, {     63, 8}, {    127, 9}, \
    {     71, 8}, {    143, 7}, {    287, 9}, {     79,10}, \
    {     47,11}, {     31,10}, {     63, 9}, {    127, 8}, \
    {    255, 9}, {    143, 8}, {    287,10}, {     79, 9}, \
    {    159, 8}, {    319, 9}, {    175, 8}, {    351, 7}, \
    {    703,10}, {     95, 9}, {    191, 8}, {    383, 9}, \
    {    207, 8}, {    415, 9}, {    223,11}, {     63,10}, \
    {    127, 9}, {    271,10}, {    143, 9}, {    287, 8}, \
    {    575,10}, {    159, 9}, {    319,10}, {    175, 9}, \
    {    351, 8}, {    703,11}, {     95,10}, {    191, 9}, \
    {    383,10}, {    207, 9}, {    415, 8}, {    831,10}, \
    {    223,12}, {     63,11}, {    127,10}, {    271, 9}, \
    {    543,10}, {    287, 9}, {    575,11}, {    159,10}, \
    {    319, 9}, {    639,10}, {    351, 9}, {    703, 8}, \
    {   1407,11}, {    191,10}, {    415, 9}, {    831,11}, \
    {    223,10}, {    447, 9}, {    895,10}, {    479,12}, \
    {    127,11}, {    255,10}, {    543,11}, {    287,10}, \
    {    575,11}, {    319,10}, {    639,11}, {    351,10}, \
    {    703,12}, {    191,11}, {    415,10}, {    831,11}, \
    {    447,10}, {    895, 9}, {   1791,13}, {    127,12}, \
    {    255,11}, {    575,12}, {    319,11}, {    703,10}, \
    {   1407,12}, {    383,11}, {    831,12}, {    447,11}, \
    {    959,10}, {   1919, 9}, {   3839,13}, {    255,12}, \
    {    575,11}, {   1151,12}, {    703,11}, {   1407,13}, \
    {    383,12}, {    959,14}, {    255,13}, {    511,12}, \
    {   1215,11}, {   2431,13}, {    639,12}, {   1407,13}, \
    {    767,12}, {   1599,13}, {    895,12}, {   1919,14}, \
    {    511,13}, {   1151,12}, {   2431,13}, {   1407,12}, \
    {   2815,14}, {    767,13}, {   1535,12}, {   3071,13}, \
    {   1919,15}, {    511,14}, {   1023,13}, {   2431,14}, \
    {   1279,13}, {   2943,12}, {   5887,14}, {  16384,15}, \
    {  32768,16} }
#define SQR_FFT_TABLE3_SIZE 153
#define SQR_FFT_THRESHOLD                 2112

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 144
#define MULLO_MUL_N_THRESHOLD             4292

#define DC_DIV_QR_THRESHOLD                 74
#define DC_DIVAPPR_Q_THRESHOLD             406
#define DC_BDIV_QR_THRESHOLD                63
#define DC_BDIV_Q_THRESHOLD                363

#define INV_MULMOD_BNM1_THRESHOLD          108
#define INV_NEWTON_THRESHOLD               351
#define INV_APPR_THRESHOLD                 303

#define BINV_NEWTON_THRESHOLD              354
#define REDC_1_TO_REDC_N_THRESHOLD          61

#define MU_DIV_QR_THRESHOLD                998
#define MU_DIVAPPR_Q_THRESHOLD            1099
#define MUPI_DIV_QR_THRESHOLD              118
#define MU_BDIV_QR_THRESHOLD               807
#define MU_BDIV_Q_THRESHOLD                979

#define POWM_SEC_TABLE  3,22,127,624,779,2351

#define MATRIX22_STRASSEN_THRESHOLD          7
#define HGCD_THRESHOLD                      90
#define HGCD_APPR_THRESHOLD                123
#define HGCD_REDUCE_THRESHOLD             1494
#define GCD_DC_THRESHOLD                   283
#define GCDEXT_DC_THRESHOLD                192
#define JACOBI_BASE_METHOD                   4

#define GET_STR_DC_THRESHOLD                12
#define GET_STR_PRECOMPUTE_THRESHOLD        27
#define SET_STR_DC_THRESHOLD               290
#define SET_STR_PRECOMPUTE_THRESHOLD       634

#define FAC_DSC_THRESHOLD                  156
#define FAC_ODD_THRESHOLD                   25
