**K7** is an application for minimal remote management of GPG keys on a web server.

It is developed in C++ as a NetBeans project on the [WebToolkit](https://www.webtoolkit.eu/)(Wt) libraries.

It allows to view, import, export, create, delete keys. Certification trust level, secret key expiry date can also be changed, and user identities can be added, revoked and certified.
Export concerns public keys only; secret keys cannot be technically exported on a web server.

Available keys can then be used by other Wt applications, or web applications based on other libraries, to encrypt and sign data. As such, it suits my personal needs.

K7 is released under the GPL version 2 license. It does not intend nor need to be a full blown key manager.

**BUILDING**

As a NetBeans project, a Makefile exists in the project root.

cd /path/to/K7

make CONF=Debug

make CONF=Release

**INSTALLING AND RUNNING**

Please see Wt's manuals about the *many* modes of installing and running Wt applications.
In particular, K7 needs the environment variable WT_APP_ROOT that should point to the directory containing configuration and translation files. In the project's tree, it's WTAPPROOT/K7.

**MAIN CONFIGURATION FILE**

This file is hard coded as k7config.json, and must be found in WT_APP_ROOT.
It controls access to the application. All users must authenticate by sending a client X509 certificate, and access is allowed if the subject's common name is listed as a JSON key in the configuration file.
An allowed user can always view keys. He can optionally import any GPG key, and delete GPG public keys. Key editing is also controlled in the configuration file.
An allowed user can optionally delete private GPG keys that he controls.

**TRANSLATION FILES**

K7.xml contains user facing strings for English default language. K7_fr.xml is translated in French.

*DISCLAIMER*

As usual, use at your own risks or don't use. I don't claim that K7 is fit for any purpose. Programming is just my hobby.
