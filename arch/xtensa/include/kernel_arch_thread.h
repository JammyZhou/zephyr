/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Per-arch thread definition
 *
 * This file contains defintions for
 *
 *  struct _thread_arch
 *  struct _callee_saved
 *  struct _caller_saved
 *
 * necessary to instantiate instances of struct k_thread.
 */

#ifndef _kernel_arch_thread__h_
#define _kernel_arch_thread__h_

#ifndef _ASMLANGUAGE
#include <zephyr/types.h>
#include <xtensa_context.h>

/*
 * The following structure defines the set of 'volatile' integer registers.
 * These registers need not be preserved by a called C function.  Given that
 * they are not preserved across function calls, they must be save/restored
 * (along with the struct _caller_saved) when a preemptive context switch
 * occurs.
 */

struct _caller_saved {

	/*
	 * The volatile registers area not included in the definition of
	 * 'tPreempReg' since the interrupt stubs (_IntEnt/_IntExit)
	 * and exception stubs (_ExcEnt/_ExcEnter) use the stack to save and
	 * restore the values of these registers in order to support interrupt
	 * nesting.  The stubs do _not_ copy the saved values from the stack
	 * into the k_thread.
	 */
};

typedef struct _caller_saved _caller_saved_t;

/*
 * The following structure defines the set of 'non-volatile' integer registers.
 * These registers must be preserved by a called C function.  These are the
 * only registers that need to be saved/restored when a cooperative context
 * switch occurs.
 */
struct _callee_saved {
	/*
	 * The following registers are considered non-volatile, i.e.
	 * callee-saved, but their values are pushed onto the stack rather than
	 * stored in the k_thread structure:
	 */
	u32_t retval; /* a2 */
	XtExcFrame *topOfStack; /* a1 = sp */
};

typedef struct _callee_saved _callee_saved_t;

/*
 * The following structure defines the set of 'non-volatile' x87 FPU/MMX/SSE
 * registers. These registers must be preserved by a called C function.
 * These are the only registers that need to be saved/restored when a
 * cooperative context switch occurs.
 */
typedef struct s_coopCoprocReg {

	/*
	 * This structure intentionally left blank. Coprocessor's registers are
	 * all 'volatile' and saved using the lazy context switch mechanism.
	 */

} tCoopCoprocReg;

/*
 * The following structure defines the set of 'volatile' x87 FPU/MMX/SSE
 * registers.  These registers need not be preserved by a called C function.
 * Given that they are not preserved across function calls, they must be
 * save/restored (along with s_coopCoprocReg) when a preemptive context switch
 * occurs.
 */
typedef struct s_preempCoprocReg {
	/*
	 * This structure intentionally left blank, as for now coprocessor's
	 * stack is positioned at top of the stack.
	 */
#if XCHAL_CP_NUM > 0
	char *cpStack;
#endif
} tPreempCoprocReg;

/*
 * The thread control stucture definition.  It contains the
 * various fields to manage a _single_ thread.
 */
struct _thread_arch {
	/*
	 * See the above flag definitions above for valid bit settings.  This
	 * field must remain near the start of struct k_thread, specifically
	 * before any #ifdef'ed fields since the host tools currently use a
	 * fixed offset to read the 'flags' field.
	 */
	u32_t flags;
#ifdef CONFIG_THREAD_CUSTOM_DATA
	void *custom_data;     /* available for custom use */
#endif
#if defined(CONFIG_THREAD_MONITOR)
	/* thread entry and parameters description */
	struct __thread_entry *entry;

	/* next item in list of ALL threads n*/
	struct k_thread *next_thread;
#endif
#ifdef CONFIG_ERRNO
	int errno_var;
#endif
	/*
	 * The location of all floating point related structures/fields MUST be
	 * located at the end of struct k_thread.  This way only the threads
	 * that actually utilize non-integer capabilities need to account for
	 * the increased memory required for storing FP state when sizing
	 * stacks.
	 *
	 * Given that stacks "grow down" on Xtensa, and the k_thread is located
	 * at the start of a thread's "workspace" memory, the stacks of threads
	 * that do not utilize floating point instruction can effectively
	 * consume the memory occupied by the 'tCoopCoprocReg' and
	 * 'tPreempCoprocReg' structures without ill effect.
	 *
	 * TODO: Move Xtensa coprocessor's stack here to get rid of extra
	 * indirection
	 */

	 /* non-volatile coprocessor's register storage */
	tCoopCoprocReg coopCoprocReg;

	/* volatile coprocessor's register storage */
	tPreempCoprocReg preempCoprocReg;
};

typedef struct _thread_arch _thread_arch_t;

#endif /* _ASMLANGUAGE */

#endif /* _kernel_arch_thread__h_ */

