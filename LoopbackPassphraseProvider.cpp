/* 
 * File:   LoopbackPassphraseProvider.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on November 2, 2020, 2:57 PM
 */

#include "LoopbackPassphraseProvider.h"
#include <string.h>
#include <iostream>

LoopbackPassphraseProvider::LoopbackPassphraseProvider()
: PassphraseProvider()
{
    m_passphrase = strdup("");
}

LoopbackPassphraseProvider::LoopbackPassphraseProvider(const string& passphrase)
: PassphraseProvider()
{
    m_passphrase = strdup(passphrase.c_str());
}

LoopbackPassphraseProvider::~LoopbackPassphraseProvider()
{
}

/*
 * Called by GPG engine.
 * Is not called if gpg-agent already has a valid password.
 */
char* LoopbackPassphraseProvider::getPassphrase(const char* useridHint,
                                                const char* description,
                                                bool previousWasBad,
                                                bool& canceled)
{
    /*
    cout << useridHint << endl; // <Signing keyid> <key name> <email>
    cout << description << endl; // <Signing keyid> <Signing keyid> 1 0
    cout << previousWasBad << endl; // Always 0, even with bad password
    cout << canceled << endl; // Always 0
     */
    return m_passphrase;
}

void LoopbackPassphraseProvider::SetPassphrase(const string& passphrase)
{
    /*
     * Memory leak here ?
     */
    m_passphrase = strdup(passphrase.c_str());
}

const string LoopbackPassphraseProvider::GetPassphrase()
{
    return m_passphrase;
}
