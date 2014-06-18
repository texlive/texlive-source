dnl  Intel Atom mpn_lshiftc -- mpn left shift with complement.

dnl  Copyright 2011 Free Software Foundation, Inc.

dnl  Contributed to the GNU project by Torbjorn Granlund and Marco Bodrato.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of either:
dnl
dnl    * the GNU Lesser General Public License as published by the Free
dnl      Software Foundation; either version 3 of the License, or (at your
dnl      option) any later version.
dnl
dnl  or
dnl
dnl    * the GNU General Public License as published by the Free Software
dnl      Foundation; either version 2 of the License, or (at your option) any
dnl      later version.
dnl
dnl  or both in parallel, as here.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl  for more details.
dnl
dnl  You should have received copies of the GNU General Public License and the
dnl  GNU Lesser General Public License along with the GNU MP Library.  If not,
dnl  see https://www.gnu.org/licenses/.

include(`../config.m4')

C mp_limb_t mpn_lshiftc (mp_ptr dst, mp_srcptr src, mp_size_t size,
C			 unsigned cnt);

C				cycles/limb
C P5
C P6 model 0-8,10-12
C P6 model 9  (Banias)
C P6 model 13 (Dothan)
C P4 model 0  (Willamette)
C P4 model 1  (?)
C P4 model 2  (Northwood)
C P4 model 3  (Prescott)
C P4 model 4  (Nocona)
C Intel Atom			 5.5
C AMD K6
C AMD K7
C AMD K8
C AMD K10

defframe(PARAM_CNT, 16)
defframe(PARAM_SIZE,12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)

dnl  re-use parameter space
define(SAVE_UP,`PARAM_CNT')
define(VAR_COUNT,`PARAM_SIZE')
define(SAVE_EBX,`PARAM_SRC')
define(SAVE_EBP,`PARAM_DST')

define(`rp',  `%edi')
define(`up',  `%esi')
define(`cnt',  `%ecx')

ASM_START()
	TEXT

PROLOGUE(mpn_lshiftc)
deflit(`FRAME',0)
	mov	PARAM_CNT, cnt
	mov	PARAM_SIZE, %edx
	mov	up, SAVE_UP
	mov	PARAM_SRC, up
	push	rp			FRAME_pushl()
	mov	PARAM_DST, rp

	lea	-4(up,%edx,4), up
	mov	%ebx, SAVE_EBX
	lea	-4(rp,%edx,4), rp

	shr	%edx
	mov	(up), %eax
	mov	%edx, VAR_COUNT
	jnc	L(evn)

	mov	%eax, %ebx
	shl	%cl, %ebx
	neg	cnt
	shr	%cl, %eax
	test	%edx, %edx
	jnz	L(gt1)
	not	%ebx
	mov	%ebx, (rp)
	jmp	L(quit)

L(gt1):	mov	%ebp, SAVE_EBP
	push	%eax
	mov	-4(up), %eax
	mov	%eax, %ebp
	shr	%cl, %eax
	jmp	L(lo1)

L(evn):	mov	%ebp, SAVE_EBP
	neg	cnt
	mov	%eax, %ebp
	mov	-4(up), %edx
	shr	%cl, %eax
	mov	%edx, %ebx
	shr	%cl, %edx
	neg	cnt
	decl	VAR_COUNT
	lea	4(rp), rp
	lea	-4(up), up
	jz	L(end)
	push	%eax			FRAME_pushl()

L(top):	shl	%cl, %ebp
	or	%ebp, %edx
	shl	%cl, %ebx
	neg	cnt
	not	%edx
	mov	-4(up), %eax
	mov	%eax, %ebp
	mov	%edx, -4(rp)
	shr	%cl, %eax
	lea	-8(rp), rp
L(lo1):	mov	-8(up), %edx
	or	%ebx, %eax
	mov	%edx, %ebx
	shr	%cl, %edx
	not	%eax
	lea	-8(up), up
	neg	cnt
	mov	%eax, (rp)
	decl	VAR_COUNT
	jg	L(top)

	pop	%eax			FRAME_popl()
L(end):
	shl	%cl, %ebp
	shl	%cl, %ebx
	or	%ebp, %edx
	mov	SAVE_EBP, %ebp
	not	%edx
	not	%ebx
	mov	%edx, -4(rp)
	mov	%ebx, -8(rp)

L(quit):
	mov	SAVE_UP, up
	mov	SAVE_EBX, %ebx
	pop	rp			FRAME_popl()
	ret
EPILOGUE()
ASM_END()
