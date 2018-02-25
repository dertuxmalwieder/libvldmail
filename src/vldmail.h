/* Copyright © 2018 Cthulhux <git_at_tuxproject_dot_de>
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details. */

#ifndef VLDMAIL_H
#define VLDMAIL_H

#include <wchar.h>

typedef struct {
    /*
      "success" will be 0 or 1 where 1 is what you'll want to get, mostly.
      "message" can contain additional deprecation warnings or validation errors.
    */
    int success;
    wchar_t message[256];
} vldmail;

extern const int VLDMAIL_VERSION;                          /* Contains the library version. */
extern vldmail validate_email(const wchar_t address[320]); /* Does all the work. */

#endif