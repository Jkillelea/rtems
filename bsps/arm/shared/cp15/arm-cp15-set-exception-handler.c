/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libcpu/arm-cp15.h>

#include <bsp/linker-symbols.h>

void* arm_cp15_set_exception_handler(
  Arm_symbolic_exception_name exception,
  void (*handler)(void)
)
{
  uint32_t current_handler = 0;

  if ((unsigned) exception < MAX_EXCEPTIONS) {
    uint32_t *cpu_table = (uint32_t *) 0 + MAX_EXCEPTIONS;
    uint32_t *mirror_table = (uint32_t *) bsp_vector_table_begin + MAX_EXCEPTIONS;

    current_handler = mirror_table[exception];

    if (current_handler != (uint32_t) handler) {
      size_t table_size = MAX_EXCEPTIONS * sizeof(uint32_t);
      uint32_t cls = arm_cp15_get_min_cache_line_size();
      uint32_t ctrl;
      rtems_interrupt_level level;

      rtems_interrupt_local_disable(level);

      ctrl = arm_cp15_mmu_disable(cls);

      mirror_table[exception] = (uint32_t) handler;

      rtems_cache_flush_multiple_data_lines(mirror_table, table_size);

      /*
       * On ARMv7 processors with the Security Extension the mirror table might
       * be the actual table used by the processor.
       */
      rtems_cache_invalidate_multiple_instruction_lines(mirror_table, table_size);

      rtems_cache_invalidate_multiple_instruction_lines(cpu_table, table_size);

      arm_cp15_set_control(ctrl);

      rtems_interrupt_local_enable(level);
    }
  }

  return (void*) current_handler;
}
