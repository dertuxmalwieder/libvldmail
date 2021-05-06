/* Copyright © 2018-2021 Cthulhux <git_at_tuxproject_dot_de>
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the MIT No Attribution license. See the file COPYING for
 * details.
 */

#include <wchar.h>
#include <locale.h>
#include <wctype.h>

#include "vldmail.h"


#ifndef NO_UNICODE_MAIL_PLEASE
/* According to RFC 3629, UTF-8 ends at 4 bytes = 2,097,152 max. characters. */
# define MAX_CODEPOINT    2097152
#else
/* ASCII only. */
# define MAX_CODEPOINT    128
#endif


/* Export the version number: */
const int VLDMAIL_VERSION = 10100; // 1.1.0


/* Loop leaving macro when a check fails: */
#define BREAK_LOOP_FAIL(msg)  \
    ret.success = 0;          \
    wcscat(ret.message, msg); \
    break;


#define ARRAY_SIZE(arr)    (sizeof(arr) / sizeof((arr)[0]))


VLDMAIL_EXPORT
valid_mail_t validate_email(const wchar_t address[320]) {
    setlocale(LC_ALL, "");

    valid_mail_t ret;

    /* Init.: */
    ret.success = 1;
    wcscpy(ret.message, L"");

    unsigned char has_at     = 0;       /* 1 after the first unmasked "@" - now the domain part starts */
    unsigned char masked     = 0;       /* 1 after a "\" */
    unsigned char in_quote   = 0;       /* 1 inside quotation marks */
    unsigned char in_comment = 0;       /* 1 between "(" and ")" */
    unsigned char has_deprecation_warning = 0;

    /* There can be exactly one comment on either end of the local or domain part,
     * starting with "(" and ending with ")". Set checkmarks so we know where we are. */
    unsigned char comment_local_end  = 0;
    unsigned char comment_domain_end = 0;

    unsigned short len_local  = 0;      /* Length of the local part (max. 64). */
    unsigned short len_domain = 0;      /* Length of the domain part (max. 255). */

    unsigned char domain_is_ip     = 0; /* 1 if IPv4, 2 if IPv6 */
    unsigned char domain_ip_octets = 0; /* the number of octets found if domain_is_ip > 0 */

    wchar_t domain[320] = L"";          /* Can hold the domain part so we can, like, parse it a second time. */


    /* List of allowed ASCII characters in the local part
     * (outside quotation marks) unless used illegally as
     * checked before this point. UTF-8 characters above
     * U+007F are generally allowed unless compiled other-
     * wise. */
    const int allowed_local_ascii[] = {
        /* Numbers: */
        48,   49,  50,  51,  52,  53,  54,  55,  56,  57,

        /* Alphabet: */
        65,   66,  67,  68,  69,  70,  71,  72,  73,  74,
        75,   76,  77,  78,  79,  80,  81,  82,  83,  84,
        85,   86,  87,  88,  89,  90,
        97,   98,  99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
        117, 118, 119, 120, 121, 122,

        /* Characters with a special function: */
        34,   40,  41,  43,  46,

        /* Other allowed special characters: */
        33,   35,  36,  37,  38,  39,  42,  45,  47,  61,63,
        94,   95,  96, 123, 124, 125, 126
    };

    /* List of allowed ASCII characters in the local part
     * (inside quotation marks), extending the list above
     * (no duplicates). */
    const int allowed_local_quoted_ascii[] = {
        /* Note that \ (92) and " (34) MUST be masked when
         * used inside a quoted string. */
        32, 44, 58, 59, 60, 62, 64, 91, 92, 93
    };

    /* List of allowed ASCII characters in the domain part. */
    const int allowed_domain_ascii[] = {
        /* Numbers: */
        48,   49,  50,  51,  52,  53,  54,  55,  56,  57,

        /* Alphabet: */
        65,   66,  67,  68,  69,  70,  71,  72,  73,  74,
        75,   76,  77,  78,  79,  80,  81,  82,  83,  84,
        85,   86,  87,  88,  89,  90,
        97,   98,  99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
        117, 118, 119, 120, 121, 122,

        /* Special characters (for IPs): */
        46,   58,  91,  93,

        /* Special characters (otherwise): */
        45
    };

    for (size_t i = 0; i < wcslen(address); i++) {
        int prev_codepoint = (i > 0 ? (int)address[i - 1] : -1);
        int codepoint      = (int)address[i];
        int next_codepoint = (i < wcslen(address) - 1 ? (int)address[i + 1] : -1);

        /* ------------------------------------- */
        /* Common checks for both address parts: */

        /* Check for ASCII (or UTF-8) overflow: */
        if (codepoint > MAX_CODEPOINT) {
            BREAK_LOOP_FAIL(L"One or more characters in your e-mail address are too high.\n");
        }

        /* Check for leading or trailing spaces or dots: */
        if ((codepoint == 46 || codepoint == 32) && (prev_codepoint == -1 || next_codepoint == -1 || (!in_quote && !in_comment && next_codepoint == 64))) {
            BREAK_LOOP_FAIL(L"Leading and trailing spaces and dots are invalid in an e-mail address.\n");
        }

        /* Check for double dots: */
        if (codepoint == 46 && next_codepoint == 46 && !in_quote) {
            BREAK_LOOP_FAIL(L"Double dots are not allowed outside quotation marks.\n");
        }

        /* Check for comments: */
        if (codepoint == 40) {
            if (in_comment && !masked) {
                /* (( ... */
                BREAK_LOOP_FAIL(L"Unmasked opening parenthesis inside a comment.\n");
            }

            /* Set the comment position: */
            if (!has_at && len_local > 0) {
                comment_local_end = 1;
            }
            else if (has_at && wcslen(domain) > 0) {
                comment_domain_end = 1;
            }

            if (!in_quote && !in_comment) {
                /* Start a comment. */
                in_comment = 1;
                continue;
            }
        }

        if (codepoint == 41) {
            if (in_comment) {
                /* Close the comment. */
                in_comment = 0;

                if ((comment_local_end && next_codepoint != 64) || (comment_domain_end && next_codepoint != -1)) {
                    /* On end comments, we assume an "@" (local) or "nothing" (domain) after the closing parenthesis.
                     * Something else is here. */
                    BREAK_LOOP_FAIL(L"Wrong comment syntax: unexpected character after the closing parenthesis.\n");
                }
            }
            else if (!in_quote) {
                BREAK_LOOP_FAIL(L"Unmatched closing parenthesis outside a comment.\n");
            }
        }

        /* ------------------------------------ */
        /* Validating the two parts separately: */

        if (has_at) {
            /* Domain part validation. */

            if (codepoint == 64) {
                /* A new "at" inside the domain part... */
                BREAK_LOOP_FAIL(L"One @ is more than enough.\n");
            }

            if (len_domain == 0 && codepoint == 91) {
                /* This domain is assumed to be an IP. If it is not, it is broken. */
                domain_ip_octets++;
            }

            len_domain++;
            if (len_domain > 255) {
                /* Domain part too long. */
                BREAK_LOOP_FAIL(L"This address's domain part exceeds 255 characters.\n");
            }

            if (domain_ip_octets > 0) {
                /* This domain is assumed to be an IP address with less than 256 characters.
                   But which kind of IP address? */
                if (codepoint == 46) {
                    /* IPv4. */
                    if (domain_is_ip == 0) domain_is_ip = 1;
                    domain_ip_octets++;
                    continue;
                }

                if (codepoint == 58) {
                    /* IPv6. */
                    if (domain_is_ip == 0) domain_is_ip = 2;
                    domain_ip_octets++;
                    continue;
                }
            }

            if (codepoint == 93 && domain_is_ip > 0) {
                /* This could be the end of an IP domain. */
                if (next_codepoint != -1) {
                    /* Well, it is not. */
                    BREAK_LOOP_FAIL(L"Unexpected characters after the end of an IP block.\n");
                }
                else {
                    /* IP validation:
                     * We have a copy of the whole [...] part in <domain> by this
                     * point. We know from <domain_is_ip> that we have either an
                     * IPv4 or an IPv6. Parse them accordingly. */
                    int ip_is_valid   = 0;
                    int current_block = 0;

                    switch (domain_is_ip) {
                    case 1:
                        /* IPv4. */
                        if (domain_ip_octets == 4) {
                            /* Correct number of octets found. Parse... */
                            wchar_t *buffer;
                            wchar_t *token = wcstok(domain, L".", &buffer);
                            while (token) {
                                current_block++;
                                if (current_block > 4) {
                                    /* Somehow, this IP has too many blocks. */
                                    goto switchend;
                                }

                                /* Check every single block for anything non-numeric. */
                                for (size_t j = 0; j < wcslen(token); j++) {
                                    if (!iswdigit(token[j]) && (int)token[j] != 91 && (int)token[j] != 93) {
                                        /* NaN and no []. */
                                        goto switchend;
                                    }

                                    wchar_t *pEnd;
                                    long int blockparse = wcstol(token, &pEnd, 10);
                                    if (blockparse > 255) {
                                        /* We can't have IPv4 parts > 255. */
                                        goto switchend;
                                    }
                                }
                                token = wcstok(NULL, L".", &buffer);
                            }
                        }
                        break;

                    case 2:
                        if (domain_ip_octets == 5) {
                            /* Correct number of octets found. Parse... */
                            wchar_t *buffer;
                            wchar_t *token = wcstok(domain, L":", &buffer);
                            while (token) {
                                current_block++;
                                if (current_block > 5) {
                                    /* Somehow, this IP has too many blocks. */
                                    goto switchend;
                                }

                                /* Check every single block for anything non-hex. */
                                int current_block_length = 1;
                                for (size_t j = 0; j < wcslen(token); j++) {
                                    if (!iswxdigit(token[j]) && (int)token[j] != 91 && (int)token[j] != 93) {
                                        /* NaN and no []. */
                                        goto switchend;
                                    }

                                    if (current_block_length > 4) {
                                        /* Not more than 4 characters please. */
                                        goto switchend;
                                    }

                                    current_block_length++;
                                }

                                token = wcstok(NULL, L".", &buffer);
                            }
                        }
                        break;
                    }

                    ip_is_valid = 1; /* No errors had happened up to this point. */

switchend:
                    if (!ip_is_valid) {
                        BREAK_LOOP_FAIL(L"Erroneous IP address found - try again.\n");
                    }
                }
            }

            /* Check if this codepoint - if ASCII - is actually allowed: */
            if (codepoint <= 128) {
                int is_allowed = 0;
                for (size_t j = 0; j < ARRAY_SIZE(allowed_domain_ascii); j++) {
                    if (allowed_domain_ascii[j] == codepoint) {
                        is_allowed = 1;
                        break;
                    }
                }

                if (!is_allowed) {
                    /* Nope. */
                    BREAK_LOOP_FAIL(L"Invalid character found outside a quotation.\n");
                }
            }
        }
        else {
            /* Local part validation. */

            /* Check for "at": */
            if (codepoint == 64 && !masked && !in_quote) {
                if (in_comment) {
                    /* Unmasked @ where it does not belong. */
                    BREAK_LOOP_FAIL(L"Unmasked @ found in an unexpected place.\n");
                }
                has_at = 1;

                /* The part after this is most likely the domain now. */
                if (next_codepoint > -1) {
                    wcscat(domain, &address[i + 1]);
                }

                continue;
            }

            len_local++;
            if (len_local > 64) {
                /* Local part too long. */
                BREAK_LOOP_FAIL(L"This address's local part exceeds 64 characters.\n");
            }

           /* Check for masking: */
           if (codepoint == 92 && !masked) {
               /* Backslash detected, mask the next character. */
               masked = 1;
               continue;
           }

            /* Check if we are inside a quotation or the quotation starts or ends: */
            if (codepoint == 34 && !masked) {
                if (in_quote) {
                    /* Ending a quote -- or do we? */
                    if (next_codepoint != 64) {
                        if (next_codepoint != 40 && next_codepoint != 43 && next_codepoint != 46) {
                            /* The next character must be either of [.(+@]. It is not. */
                            BREAK_LOOP_FAIL(L"Wrong quotation (end).\n");
                        }
                        else {
                            /* The mix of dot strings and quoted strings is deprecated for new e-mail
                             * addresses. Mark it as such. */
                            if (!has_deprecation_warning) {
                                wcscat(ret.message, L"Mixing dot strings and quoted strings is deprecated.\n");
                                has_deprecation_warning = 1;
                            }
#ifdef STRICT_VALIDATION
                            /* The person who compiled libvldmail decided that we shouldn't allow that
                             * at all. */
                            ret.success = 0;
                            break;
#endif
                        }
                    }
                    in_quote = 0;
                }
                else {
                    /* Starting a quote -- or do we? */
                    if (prev_codepoint != -1) {
                        if (prev_codepoint != 41 && prev_codepoint != 46) {
                            /* The previous character must be either of [.)]. It is not. */
                            BREAK_LOOP_FAIL(L"Wrong quotation (start).\n");
                        }
                        else {
                            /* The mix of dot strings and quoted strings is deprecated for new e-mail
                             * addresses. Mark it as such. */
                            if (!has_deprecation_warning) {
                                wcscat(ret.message, L"Mixing dot strings and quoted strings is deprecated.\n");
                                has_deprecation_warning = 1;
                            }
#ifdef STRICT_VALIDATION
                            /* The person who compiled libvldmail decided that we shouldn't allow that
                             * at all. */
                            ret.success = 0;
                            break;
#endif
                       }
                   }
                   in_quote = 1;
               }
           }

           /* Check if this codepoint - if ASCII - is actually allowed: */
           if (codepoint <= 128) {
               int is_allowed = 0;
               for (size_t j = 0; j < ARRAY_SIZE(allowed_local_ascii); j++) {
                   if (allowed_local_ascii[j] == codepoint) {
                       is_allowed = 1;
                       break;
                   }
               }
               if (!is_allowed && in_quote) {
                   /* Inside a quote, additional characters are allowed. */
                   for (size_t j = 0; j < ARRAY_SIZE(allowed_local_quoted_ascii); j++) {
                       if (allowed_local_quoted_ascii[j] == codepoint && (masked || (codepoint != 34 && codepoint != 92))) {
                           /* 34 and 92 have to be masked inside a quotation. */
                           is_allowed = 1;
                           break;
                       }
                   }
               }
               if (!is_allowed) {
                   /* Nope. */
                   BREAK_LOOP_FAIL(L"Invalid character found outside a quotation.\n");
               }
           }

            /* Close the mask after we're done processing the current character: */
            if (masked) {
                masked = 0;
            }
        }
    }

    /* ---------------- */
    /* Finishing tests: */

    if (wcslen(domain) > 0 && wcsncmp(domain, L"localhost", wcslen(domain)) == 0 && ret.success) {
        ret.success = 0;
        wcscat(ret.message, L"'localhost' is actually not a valid hostname - sorry.\n");
    }

    if (in_comment && ret.success) {
        ret.success = 0;
        wcscat(ret.message, L"Address comment lacks closing parenthesis.\n");
    }

    if (in_quote && ret.success) {
        ret.success = 0;
        wcscat(ret.message, L"Quotation end tag is missing.\n");
    }

    if (!has_at && ret.success) {
        /* No domain? :-( */
        ret.success = 0;
        wcscat(ret.message, L"This e-mail address does not seem to have a domain.\n");
    }

    return(ret);
}
