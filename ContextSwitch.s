	.file	"ContextSwitch.s"
	.text
	.align	2
	.global	Create
	.type	Create, %function
Create:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#0

	.size	Create, .-Create
	.align	2
	.global	MyTid
	.type	MyTid, %function
MyTid:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#1

	.size	MyTid, .-MyTid
	.align	2
	.global	MyParentTid
	.type	MyParentTid, %function
MyParentTid:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#2

	.size	MyParentTid, .-MyParentTid
	.align	2
	.global	Pass
	.type	Pass, %function
Pass:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#3

	.size	Pass, .-Pass
	.align	2
	.global	Exit
	.type	Exit, %function
Exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#4

	.size	Exit, .-Exit
	.text
	.align	2
	.global	Send
	.type	Send, %function

Send:
	@ args = 4, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#5

	.size	Send, .-Send
	.text
	.align	2
	.global	Receive
	.type	Receive, %function
Receive:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#6

	.size	Receive, .-Receive
	.align	2
	.global	Reply
	.type	Reply, %function
Reply:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#7

	.size	Reply, .-Reply
	.align	2
	.global	AwaitEvent
	.type	AwaitEvent, %function
AwaitEvent:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#8

	.size	AwaitEvent, .-AwaitEvent
	.align	2
	.global	SystemShutdown
	.type	SystemShutdown, %function
SystemShutdown:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0

	swi	#9

	.size	SystemShutdown, .-SystemShutdown
        .align	2
	.global	context_switch_to_task
	.type	context_switch_to_task, %function
context_switch_to_task:
	@ args = 0, pretend = 0, frame = 4
	@ frame_needed = 1, uses_anonymous_args = 0

	# Save everything onto the stack for the kernel
	stmfd	sp, {r0-r14}
	sub		sp, sp, #60

	# Restore the SPSR and then go into user mode
	# bic     r1, r1, #0x00000080
	msr     spsr, r1
	nop

	# Restore the context of user mode
	add		lr, r0, #60
	ldmfd   r0, {r0-r14}^
	ldmfd	lr, {pc}^
	.size	context_switch_to_task, .-context_switch_to_task
	.align	2
	.global	kernel_entry
	.type	kernel_entry, %function
kernel_entry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0

	# Save registers r0 to r3 on stack
	stmfd	sp, {r0-r4, lr}
	sub		sp, sp, #24
	mov		r4, sp

	# Save SPSR
	mrs		r0, spsr
	bl		save_spsr(PLT)
	####################### System Mode ######################
	mrs		r0, cpsr
	orr		r0, r0, #0x0000000F
	msr		cpsr, r0

	# Restore r0 to r3 and Store all registers on user stack
	ldmfd	r4, {r0-r4}
	stmfd   sp, {r0-r15}
	str		lr, [sp, #-4]
	sub		sp, sp, #64

	# Save SP
	mov		r0, sp
	bl		save_stack_pointer(PLT)

	# Go back to SuperVisor Mode
	mrs		ip, cpsr
	bic		ip, ip, #0x0000000C
	msr		cpsr, ip

	###################### SuperVisor Mode ####################
	add		sp, sp, #24
	ldr 	lr, [sp, #-4]
	# Retreive value of swi and put it in r0
	ldr		r0, [lr, #-4]
	bic		r0, r0, #0xFF000000
	str		r0, [sp, #0]

	# Load the kernels context from its stack
	ldmfd   sp, {r0-r13, r15}
	.size	kernel_entry, .-kernel_entry
	.ident	"GCC: (GNU) 4.0.2"
	.global	interrupt_entry
	.type	interrupt_entry, %function
interrupt_entry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp, {r0-r2, r14}
	sub		sp, sp, #16

	mrs		r0, spsr
	bl		save_spsr(PLT)
	bl		handle_interrupt(PLT)

	mov		r1, sp
	ldr		lr, [sp, #12]
	sub		r2, lr, #4
	add		sp, sp, #16

	# Go to System Mode
	mrs		r0, cpsr
	orr		r0, r0, #0x0000000F
	msr		cpsr, r0

	# Setup stack frame for recovery
	mov		r0, sp
	stmfd	r0!, {r2}
	stmfd	r0!, {r3-r14}

	mov		sp, r0
	ldmfd	r1, {r0-r2}
	stmfd	sp!, {r0-r2}

	mov		r0, sp
	bl		save_stack_pointer(PLT)

	# Go back to SuperVisor Mode
	mrs		ip, cpsr
	bic		ip, ip, #0x0000000C
	msr		cpsr, ip

	###################### SuperVisor Mode ####################
	# Fake value of swi and put it in r0
	mov		r0, #255
	str		r0, [sp, #0]

	# Load the kernels context from its stack
	ldmfd   sp, {r0-r13, r15}
	.size	interrupt_entry, .-interrupt_entry
	.ident	"GCC: (GNU) 4.0.2"
	.global	dump_registers
	.type	dump_registers, %function
dump_registers:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp, {r0-r14}
	sub		sp, sp, #60

	mov		r0, sp
	bl		print_registers(PLT)
	# Load the kernels context from its stack
	ldmfd   sp, {r0-r13, r15}
	.size	dump_registers, .-dump_registers
	.ident	"GCC: (GNU) 4.0.2"
