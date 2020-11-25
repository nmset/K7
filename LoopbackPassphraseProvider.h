/* 
 * File:   LoopbackPassphraseProvider.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 2, 2020, 2:57 PM
 */

#ifndef LOOPBACKPASSPHRASEPROVIDER_H
#define LOOPBACKPASSPHRASEPROVIDER_H

#include <string>
#include <gpgme++/interfaces/passphraseprovider.h>

using namespace std;

/**
 * Replaces default pinentry on a web server.
 */
class LoopbackPassphraseProvider : public GpgME::PassphraseProvider
{
public:
    LoopbackPassphraseProvider();
    LoopbackPassphraseProvider(const string& passphrase);
    virtual ~LoopbackPassphraseProvider();
    /**
     * GPG engine gets its passphrase here.
     * \n The usefulness of the (out?) parameters needs to be better
     * understood (by me!).
     * @param useridHint : Signing keyid | key name | email 
     * @param description : Signing keyid | Signing keyid | 1 | 0
     * @param previousWasBad : Always 0, even with bad password
     * @param canceled : Always 0
     * @return 
     */
    virtual char *getPassphrase(const char *useridHint, const char *description,
                                bool previousWasBad, bool &canceled);
    /**
     * Application must do that.
     * @param passphrase
     */
    void SetPassphrase(const string& passphrase);
    /**
     * Application may need it, though it knows what it stored here.
     * @return 
     */
    const string GetPassphrase();

private:
    /*
     * Don't use char* m_passphrase. It is manipulated by gpg and exporting
     * secret keys become messy, to the point of crashing the application if
     * passphrase is bad.
     * See https://dev.gnupg.org/T5151#139421
    */
    string m_passphrase;

};

#endif /* LOOPBACKPASSPHRASEPROVIDER_H */

