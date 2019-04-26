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
    sub sp, sp, #0x18

    # save 5th arg
    ldr r6, [sp, #0x28]
    str r6, [sp]
    # save 6th arg
    ldr r6, [sp, #0x2c]
    str r6, [sp, #4]
	# save 7th arg
    ldr r6, [sp, #0x30]
    str r6, [sp, #8]
	# save 8th arg
    ldr r6, [sp, #0x34]
    str r6, [sp, #0xc]
	# save 9th arg
    ldr r6, [sp, #0x38]
    str r6, [sp, #0x10]
	# save 10th arg
    ldr r6, [sp, #0x3c]
    str r6, [sp, #0x14]

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
	add sp, sp, #0x18
	pop {r6-r8, pc}

FUNC_TABLE_2:
	.word 0x11111111

OLD_FUNC_2:

	#original instructions

	ldr.w pc, [pc, #0]
	.word 0x22222222




