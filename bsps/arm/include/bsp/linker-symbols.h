/**
 * @file
 *
 * @ingroup arm_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (c) 2008, 2016 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup arm_linker Linker Support
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief Linker support.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

LINKER_SYMBOL(bsp_stack_fiq_size)
LINKER_SYMBOL(bsp_stack_abt_size)
LINKER_SYMBOL(bsp_stack_und_size)
LINKER_SYMBOL(bsp_stack_hyp_size)

LINKER_SYMBOL(bsp_section_start_begin)
LINKER_SYMBOL(bsp_section_start_end)
LINKER_SYMBOL(bsp_section_start_size)

LINKER_SYMBOL(bsp_section_vector_begin)
LINKER_SYMBOL(bsp_section_vector_end)
LINKER_SYMBOL(bsp_section_vector_size)

LINKER_SYMBOL(bsp_section_text_begin)
LINKER_SYMBOL(bsp_section_text_end)
LINKER_SYMBOL(bsp_section_text_size)
LINKER_SYMBOL(bsp_section_text_load_begin)
LINKER_SYMBOL(bsp_section_text_load_end)

LINKER_SYMBOL(bsp_section_rodata_begin)
LINKER_SYMBOL(bsp_section_rodata_end)
LINKER_SYMBOL(bsp_section_rodata_size)
LINKER_SYMBOL(bsp_section_rodata_load_begin)
LINKER_SYMBOL(bsp_section_rodata_load_end)

LINKER_SYMBOL(bsp_section_data_begin)
LINKER_SYMBOL(bsp_section_data_end)
LINKER_SYMBOL(bsp_section_data_size)
LINKER_SYMBOL(bsp_section_data_load_begin)
LINKER_SYMBOL(bsp_section_data_load_end)

LINKER_SYMBOL(bsp_section_fast_text_begin)
LINKER_SYMBOL(bsp_section_fast_text_end)
LINKER_SYMBOL(bsp_section_fast_text_size)
LINKER_SYMBOL(bsp_section_fast_text_load_begin)
LINKER_SYMBOL(bsp_section_fast_text_load_end)

LINKER_SYMBOL(bsp_section_fast_data_begin)
LINKER_SYMBOL(bsp_section_fast_data_end)
LINKER_SYMBOL(bsp_section_fast_data_size)
LINKER_SYMBOL(bsp_section_fast_data_load_begin)
LINKER_SYMBOL(bsp_section_fast_data_load_end)

LINKER_SYMBOL(bsp_section_bss_begin)
LINKER_SYMBOL(bsp_section_bss_end)
LINKER_SYMBOL(bsp_section_bss_size)

LINKER_SYMBOL(bsp_section_rtemsstack_begin)
LINKER_SYMBOL(bsp_section_rtemsstack_end)
LINKER_SYMBOL(bsp_section_rtemsstack_size)

LINKER_SYMBOL(bsp_section_noinit_begin)
LINKER_SYMBOL(bsp_section_noinit_end)
LINKER_SYMBOL(bsp_section_noinit_size)

LINKER_SYMBOL(bsp_section_work_begin)
LINKER_SYMBOL(bsp_section_work_end)
LINKER_SYMBOL(bsp_section_work_size)

LINKER_SYMBOL(bsp_section_stack_begin)
LINKER_SYMBOL(bsp_section_stack_end)
LINKER_SYMBOL(bsp_section_stack_size)

LINKER_SYMBOL(bsp_section_nocache_begin)
LINKER_SYMBOL(bsp_section_nocache_end)
LINKER_SYMBOL(bsp_section_nocache_size)
LINKER_SYMBOL(bsp_section_nocache_load_begin)
LINKER_SYMBOL(bsp_section_nocache_load_end)

LINKER_SYMBOL(bsp_section_nocachenoload_begin)
LINKER_SYMBOL(bsp_section_nocachenoload_end)
LINKER_SYMBOL(bsp_section_nocachenoload_size)

LINKER_SYMBOL(bsp_section_nocacheheap_begin)
LINKER_SYMBOL(bsp_section_nocacheheap_end)
LINKER_SYMBOL(bsp_section_nocacheheap_size)

LINKER_SYMBOL(bsp_vector_table_begin)
LINKER_SYMBOL(bsp_vector_table_end)
LINKER_SYMBOL(bsp_vector_table_size)

LINKER_SYMBOL(bsp_start_vector_table_begin)
LINKER_SYMBOL(bsp_start_vector_table_end)
LINKER_SYMBOL(bsp_start_vector_table_size)

LINKER_SYMBOL(bsp_translation_table_base)
LINKER_SYMBOL(bsp_translation_table_end)

#define BSP_FAST_TEXT_SECTION \
  RTEMS_SECTION(".bsp_fast_text")

#define BSP_FAST_DATA_SECTION \
  RTEMS_SECTION(".bsp_fast_data")

#define BSP_NOCACHE_SECTION \
  RTEMS_SECTION(".bsp_nocache")

#define BSP_NOCACHE_SUBSECTION(subsection) \
  RTEMS_SECTION(".bsp_nocache." # subsection)

#define BSP_NOCACHENOLOAD_SECTION \
  RTEMS_SECTION(".bsp_noload_nocache")

#define BSP_NOCACHENOLOAD_SUBSECTION(subsection) \
  RTEMS_SECTION(".bsp_noload_nocache." # subsection)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H */
