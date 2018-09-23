/* This is a test file. I should add a header here to set my copyright
   and stuff, but seriously: why would you even want to break the
   copyright for a test file?
   Note that I assume Unicode here. No tests for ASCII frogs!
*/

#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <vldmail.h>

int test_counter = 0;

static void note(const wchar_t *message) {
    printf("# %ls\n", message);
}

static void diag(const wchar_t *message) {
    fprintf(stderr, "# %ls\n", message);
}

static void done_testing() {
    printf("1..%d\n", test_counter);
}

static void test(const wchar_t *address, int success) {
    vldmail validator;
    validator = validate_email(address);
    int passed = validator.success == success;
    if (!passed) printf("not ");
    printf("ok '%ls' should %s\n", address, success ? "pass" : "fail");
    if (validator.success == 0) {
        if (passed) diag(L"EXPECTED MESSAGE:");
        diag(validator.message);
    }
    test_counter++;
}

int main(void) {
    setlocale(LC_ALL, "");

    note(L"Basic tests.");

    test(L"foo@bar.quux", 1);
    test(L"hügo@müller.berlin", 1);
    test(L"admin@localhost", 0); /* Tricky, but the RFC suggests that this is invalid. */
    test(L"🎃@emojiguy", 1); /* Valid if inside the local network ... */

    /* Tests from Wikipedia et al.: */
    note(L"Advanced tests.");

    test(L"foo@[192.168.0.1]", 1);
    test(L"\"very.(),:;<>[]\\\".VERY.\\\"very@\\\\ \\\"very\\\".unusual\"@strange.example.com", 1); /* Valid thanks to quoting. */
    test(L"\" \"@provider.tld", 1); /* Seems to be valid according to the RFCs. Wikipedia says otherwise. But there is no obvious reason for that. */

    done_testing();

    return 0;
}
