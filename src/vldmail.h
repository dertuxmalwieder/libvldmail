/* Copyright © 2018-2021 Cthulhux <git_at_tuxproject_dot_de>
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the MIT No Attribution license. See the file COPYING for
 * details.
 */

#ifndef VLDMAIL_H
#define VLDMAIL_H

#include <wchar.h>


#if defined (_MSC_VER) && !defined (__clang__)
# define VLDMAIL_EXPORT    __declspec(dllexport)
#else
# define VLDMAIL_EXPORT
#endif


VLDMAIL_EXPORT
typedef struct {
    /*
     * "success" will be 0 or 1 where 1 is what you'll want to get, mostly.
     * "message" can contain additional deprecation warnings or validation errors.
     */
    int     success;
    wchar_t message[256];
} valid_mail_t;

extern VLDMAIL_EXPORT const int VLDMAIL_VERSION;                          /* Contains the library version. */
extern VLDMAIL_EXPORT valid_mail_t validate_email(const wchar_t address[320]); /* Does all the work. */

#endif
