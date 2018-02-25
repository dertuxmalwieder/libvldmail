/* This is a test file. I should add a header here to set my copyright
   and stuff, but seriously: why would you even want to break the
   copyright for a test file?
   Note that I assume Unicode here. No tests for ASCII frogs!
*/
#include <stdio.h>
#include <vldmail.h>

int main(void) {
    vldmail validator;

    printf("Basic tests.\n");
    printf("\n");

    printf("  foo@bar.quux:         ");
    validator = validate_email(L"foo@bar.quux");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    printf("  hügö@müller.berlin:   ");
    validator = validate_email(L"hügö@müller.berlin");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    printf("  admin@localhost:      "); /* Tricky, but the RFC suggests that this is invalid. */
    validator = validate_email(L"admin@localhost");
    printf(validator.success == 0 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    printf("  🎃@emojiguy:          "); /* Valid if inside the local network ... */
    validator = validate_email(L"🎃@emojiguy");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    /* Tests from Wikipedia et al.: */
    printf("\n");
    printf("Advanced tests.\n");
    printf("\n");

    printf("  foo@[192.168.0.1]:                                                      ");
    validator = validate_email(L"foo@[192.168.0.1]");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    printf("  \"very.(),:;<>[]\\\".VERY.\\\"very@\\\\ \\\"very\\\".unusual\"@strange.example.com: "); /* Valid thanks to quoting. */
    validator = validate_email(L"\"very.(),:;<>[]\\\".VERY.\\\"very@\\\\ \\\"very\\\".unusual\"@strange.example.com");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    printf("  \" \"@provider.tld:                                                       "); /* Seems to be valid according to the RFCs. */
    /* (Wikipedia says otherwise. But there is no obvious reason for that.) */
    validator = validate_email(L"\" \"@provider.tld");
    printf(validator.success == 1 ? "passed" : "failed");
    if (validator.success == 0) printf("\n    error: %ls\n", validator.message); else printf("\n");

    return 0;
}
