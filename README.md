# libvldmail

Your friendly e-mail address validation library.

## Why?

* Did you know that parentheses, spaces and - according to the [RFC 6531](https://tools.ietf.org/html/rfc6531) document - emojis can be a part of a valid e-mail address?
* Did you know that both IPv6 addresses and resources in your intranet are valid parts of the part after the "@", so requiring a TLD (*xxxx.yy*) is entirely wrong?

Nor do all of the existing (and more complex than "is there an @ character?") validators I've come across. So this is my approach.

## Features

* Written in C for a "good enough" interoperability with other languages.
* Does not depend on any non-standard library - no regex, no ICU, no other overhead.
* Can validate e-mail addresses according to RFC 6531 with a fallback to RFC 5321 ff.
* Will return both a "success" flag (`0` or `1`) and an error message. (See the **Usage** part for more information.)
* Could probably wash your dishes (after having adequate code and hardware extensions).

### A note on Unicode support

By default, `libvldmail` respects the latest internationalization standards, so Unicode characters are allowed in both the domain and the local part of the e-mail address you aim to have validated. If your service does not allow that, your service sucks and you should be ashamed. You can teach `libvldmail` to fall back to the good old ASCII days by defining the `NO_UNICODE_MAIL_PLEASE` preprocessor parameter.

### A note on deprecation inside the RFCs

Things change. E-mail addresses do not necessarily have to. By default, valid e-mail addresses are recognized as valid even if the standards say that you should not use them anymore. If you compile `libvldmail` with the `STRICT_VALIDATION` preprocessor parameter, however, the library will mark more "deprecated" addresses as invalid.

## Portability

You should be able to use `libvldmail` from inside Ruby, Python. Lisp etc. with the included [SWIG](http://www.swig.org/) template file (`contrib/libvldmail.i`).

## Usage

    #include <vldmail.h>
    
    int main(void) {
        /* ... your code ... */
        
        vldmail validator = validate_email("foo@bar.quux");
        if (0 == validator.success) {
            /* success == 0 means that something was wrong. */
            printf("Validating foo@bar.quux failed: %ls\n", validator.message);
        }
        
        /* ... more of your code ... */
    }

## Building

Use [CMake](https://cmake.org/) to create the `libvldmail` library, then link it into your application. And don't forget to point to the `vldmail.h` header.

## Versioning

`libvldmail` tries to follow the [Semantic Versioning](https://semver.org/) scheme. You can ask for the current version via the API:

    printf("libvldmail version %d", VLDMAIL_VERSION);

We use simple mathematics here:

    const int VLDMAIL_VERSION = 1;     // Version 0.0.1  (0 * 10^4 +  0 * 10^2 + 1 * 10^0)
    const int VLDMAIL_VERSION = 21209; // Version 2.12.9 (2 * 10^4 + 12 * 10^2 + 9 * 10^0)

This should be enough.

## Donations (optional)

`libvldmail` is free software by the terms of the [WTFPL](http://www.wtfpl.net/txt/copying/). As some people - including myself - like to contribute money for a good cause anyway, here are two possible options for you:

### Donate money to the nature:

Both [Kākāpō Recovery](http://kakaporecovery.org.nz/) and the [WWF](https://support.wwf.org.uk/adopt-a-panda) do a pretty good job at trying to keep species alive. You are invited to join their efforts.

### Donate money to me:

Yes, I like money as well.

[![Support via PayPal](https://cdn.rawgit.com/twolfson/paypal-github-button/1.0.0/dist/button.svg)](https://www.paypal.me/GebtmireuerGeld/)

## Help me, please?

I accept pull requests if they are related to adding RFC-compatibility. This library is (somewhat) working, but it is probably not quite perfect yet. You are invited to list yourself as a contributor below this paragraph if you need your merits:

### Contributors

* None, yet.
