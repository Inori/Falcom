#	EDAO-Patch
#	Copyright (C) 2017, Asuka(Inori)
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.


#	Compile:
#		arm-vita-eabi-gcc -c hook_stub.s -o hook_stub.o
#
#   View assembly:
#		arm-vita-eabi-objdump -d hook_stub.o
#
#	Convert to binary:
#		arm-vita-eabi-objcopy -S -O binary hook_stub.o hook_stub.bin
#

	.syntax unified
	.section .text
	.thumb

	.global hook_stub

#hook_stub is a template, copy this function,
#modify 'original instructions' part
#nop to align
hook_stub:
	# No operation to support the ldr relativity
    nop
    
	#pushad
	stmfd sp!, {r0-r12}
	#pushfd
	mrs r7, cpsr
	str r7, [sp, #-4]!

	#load hook function address 
	ldr.w r7, [pc, #0x1c]
	ldr.w r7, [r7, #0]
	#call hook function
	blx r7
	#popfd
	ldr r7, [sp], #4
	msr cpsr, r7
	#popad
	ldmfd sp!, {r0-r12}

	#original instructions


	#jmp to original function
	ldr.w pc, [pc, #0]

	.word 0x11111111
	.word 0x22222222


jmp_stub:
	push {r6-r8, lr}

	# get function table address
	adr r6, FUNC_TABLE
	ldr r6, [r6]

	# get new function pointer
	ldr r7, [r6]

	# get old function pointer
	adr r8, OLD_FUNC

	# save old func pointer
	str r8, [r6, #4]

	# call our new function
	blx r7
	pop {r6-r8, pc}

FUNC_TABLE:
	.word 0x11111111

OLD_FUNC:

	#original instructions

	ldr.w pc, [pc, #0]
	.word 0x22222222



test_func:
	b test_label
	nop
	sub sp, sp, #0xBC
	movw            r2, #0x44F0
	movt.w          r2, #0x811F
	nop
	ldr.w pc, [pc, #0]
	.word 0x33333333
test_label:
	ldr.w pc, [pc, #0]
	.word 0x44444444




jmp_stub_stack_arg:
	push {r6-r8, lr}

    sub sp, sp, #8
    # save 5th arg
    ldr r6, [sp, #0x18]
    str r6, [sp]
    # save 6th arg
    ldr r6, [sp, #0x1c]
    str r6, [sp, #4]

	# get function table address
	adr r6, FUNC_TABLE_2
	ldr r6, [r6]

	# get new function pointer
	ldr r7, [r6]

	# get old function pointer
	adr r8, OLD_FUNC_2

	# save old func pointer
	str r8, [r6, #4]

	# call our new function
	blx r7
	add sp, sp, #8
	pop {r6-r8, pc}

FUNC_TABLE_2:
	.word 0x11111111

OLD_FUNC_2:

	#original instructions

	ldr.w pc, [pc, #0]
	.word 0x22222222

.arm

memcpy_neon:
		cmp	r2, #16
		bge	3f

		/* Do small memory copies (up to 15 bytes) using ARM */
		push	{r0, lr}
		subs	r2, r2, #2
		blt	2f
1:		ldrb	r3, [r1], #1
		ldrb	lr, [r1], #1
		subs	r2, r2, #2
		strb	r3, [r0], #1
		strb	lr, [r0], #1
		bge	1b
2:		cmp	r2, #-1
		ldrbeq	r3, [r1], #1
		strbeq	r3, [r0], #1
		pop	{r0, pc}
3:
		/* Do bigger memory copies using NEON instructions */
		mov	ip, r0
		tst	r0, #1
		beq	1f
		vld1.8	{d0[0]}, [r1]!
		vst1.8	{d0[0]}, [ip]!
		sub	r2, r2, #1
1:
		tst	ip, #2
		beq	1f
		vld2.8	{d0[0], d1[0]}, [r1]!
		vst2.8	{d0[0], d1[0]}, [ip]!
		sub	r2, r2, #2
1:
		tst	ip, #4
		beq	1f
		vld4.8	{d0[0], d1[0], d2[0], d3[0]}, [r1]!
		vst4.8	{d0[0], d1[0], d2[0], d3[0]}, [ip, :32]!
		sub	r2, r2, #4
1:
		tst	ip, #8
		beq	1f
		vld1.8	{d0}, [r1]!
		vst1.8	{d0}, [ip, :64]!
		sub	r2, r2, #8
1:
		subs	r2, r2, #32
		blt	3f
		mov	r3, #32
1:
		vld1.8	{d0-d3}, [r1]!
		cmp	r3, #(320 - 32)
		pld	[r1, r3]
		addle	r3, r3, #32
		sub	r2, r2, #32
		vst1.8	{d0-d3}, [ip, :128]!
		cmp	r2, r3
		bge	1b
		cmp	r2, #0
		blt	3f
1:
		vld1.8	{d0-d3}, [r1]!
		subs	r2, r2, #32
		vst1.8	{d0-d3}, [ip, :128]!
		bge	1b
3:
		tst	r2, #16
		beq	1f
		vld1.8	{d0, d1}, [r1]!
		vst1.8	{d0, d1}, [ip, :128]!
1:
		tst	r2, #8
		beq	1f
		vld1.8	{d0}, [r1]!
		vst1.8	{d0}, [ip, :64]!
1:
		tst	r2, #4
		beq	1f
		vld4.8	{d0[0], d1[0], d2[0], d3[0]}, [r1]!
		vst4.8	{d0[0], d1[0], d2[0], d3[0]}, [ip, :32]!
1:
		tst	r2, #2
		beq	1f
		vld2.8	{d0[0], d1[0]}, [r1]!
		vst2.8	{d0[0], d1[0]}, [ip]!
1:
		tst	r2, #1
		beq	1f
		vld1.8	{d0[0]}, [r1]!
		vst1.8	{d0[0]}, [ip]!
1:
		bx	lr




