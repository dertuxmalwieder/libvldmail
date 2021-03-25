/* This is a test file. I should add a header here to set my copyright
   and stuff, but seriously: why would you even want to break the
   copyright for a test file?
   Note that I assume Unicode here. No tests for ASCII frogs!
*/

#include "src/vldmail.h"
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define INPUTFILE "autotest-cases"
#define MAXLINE 500 /* arbitrary */
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

static void chomp(const wchar_t *wcs) {
    wchar_t *newlineptr = wcschr(wcs, L'\n');
    if (newlineptr) *newlineptr = 0;
}

static void test(const wchar_t *address, int success) {
    valid_mail_t validator;
    validator = validate_email(address);
    int passed = validator.success == success;
    if (!passed) printf("not ");
    printf("ok '%ls' should %s\n", address, success ? "pass" : "fail");
    if (validator.success == 0) {
        if (passed) diag(L"EXPECTED MESSAGE:");
        chomp(validator.message);
        diag(validator.message);
    }
    test_counter++;
}

int main(void) {
    setlocale(LC_ALL, "");

    FILE *inputs = fopen(INPUTFILE, "r");
    if (!inputs) {
        perror(INPUTFILE);
        exit(1);
    }
    wchar_t line[MAXLINE];
    while (fgetws(line, MAXLINE, inputs)) {
        if (wcscmp(line, L"\n") == 0) continue; /* skip empty lines */
        if (wcschr(line, L'#') == line) {
            chomp(line);
            note(line+1);
            continue;
        }
        if (wcschr(line, L'1') == line) {
            chomp(line);
            test(line+2, 1);
            continue;
        }
        if (wcschr(line, L'0') == line) {
            chomp(line);
            test(line+2, 0);
            continue;
        }
        diag(L"IGNORING unknown line:");
        diag(line);
    }

    done_testing();

    return 0;
}
