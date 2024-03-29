/* 
 * File:   LoopbackPassphraseProvider.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 * 
 * Created on November 2, 2020, 2:57 PM
 */

#include "LoopbackPassphraseProvider.h"
#include <string.h>
#include <iostream>

LoopbackPassphraseProvider::LoopbackPassphraseProvider()
: PassphraseProvider()
{
    m_passphrase = "";
}

LoopbackPassphraseProvider::LoopbackPassphraseProvider(const string& passphrase)
: PassphraseProvider()
{
    m_passphrase = passphrase;
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
    /*
     * See https://dev.gnupg.org/T5151#139421
     */
    return strdup(m_passphrase.c_str());
}

void LoopbackPassphraseProvider::SetPassphrase(const string& passphrase)
{
    /*
     * Memory leak here ?
     */
    m_passphrase = passphrase;
}

const string LoopbackPassphraseProvider::GetPassphrase()
{
    return m_passphrase;
}
