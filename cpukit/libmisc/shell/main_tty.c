/**
 * @file
 * 
 * @brief TTY Shell Command Implmentation
 */

/*
 * Copyright (c) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_tty(
  int   argc RTEMS_UNUSED,
  char *argv[] RTEMS_UNUSED
)
{
  printf("%s\n", ttyname(fileno(stdin)));
  return 0;
}

rtems_shell_cmd_t rtems_shell_TTY_Command = {
  "tty",                                      /* name */
  "show ttyname",                             /* usage */
  "misc",                                     /* topic */
  rtems_shell_main_tty,                       /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};
