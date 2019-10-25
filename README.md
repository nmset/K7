K7 is an application for minimal remote management of GPG keys on a web server.
It allows to view, import and delete keys.
It does not intend nor need to be a full blown key manager.
Key generation and editing is not implemented, and it probably cannot be done this way.

K7 is developed as a NetBeans project on the [WebToolkit](https://www.webtoolkit.eu/)(Wt) libraries.
It is released under the GPL version 2 license.

The intent is solely to add and delete keys in the default GPG keyring of the running user.
These keys can then be used by other Wt applications, or applications based on other libraries, to encrypt and sign data. As such, it suits my personal needs.

BUILDING

As a NetBeans project, a Makefile exists in the project root.

cd /path/to/K7

make CONF=Debug #linked to wthttp here

make CONF=Release #linked to wtfcgi

INSTALLING AND RUNNING

Please see Wt's manuals about the modes of installing and running Wt applications.
In particular, K7 needs the environment variable WT_APP_ROOT that should point to the directory containing configuration and translation files. In the project's tree, it's WTAPPROOT/K7.

MAIN CONFIGURATION FILE

This file is hard coded as k7config.json, and must be found in WT_APP_ROOT.
It controls access to the application. All users must authenticate by sending a client X509 certificate, and access is allowed if the subject's common name is listed as a JSON key in the configuration file.
An allowed user can always view keys. He can optionally import any GPG key, and delete GPG public keys.
An allowed user can optionally delete private GPG keys. For this, a full key identifier must be declared in a JSON array in the configuration file. This should be a normal key identifier, a short key identifier or a fingerprint, but always a full identifier.

TRANSLATION FILES

K7.xml contains user facing strings for English default language. K7_fr.xml is translated in French .

DISCLAIMER

As usual, use at your own risks or don't use. I don't claim that K7 is fit for any purpose. Programming is just my hobby.
