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

	push {r6, r7}
	adr r6, OLD_FUNC
	ldr r7, [pc, #4]
	ldr pc, [r7]
	#bx r7
	.word 0x11111111
OLD_FUNC:
	pop {r6, r7}
	#original instructions

	ldr.w pc, [pc, #0]
	.word 0x22222222
	


