/* 
 * File:   GpgMEWorker.h
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 *
 * Created on 11 octobre 2019, 16:34
 */

#ifndef GPGMEWORKER_H
#define GPGMEWORKER_H

#include <gpgme++/context.h>
#include <gpgme++/error.h>
#include <gpgme++/key.h>
#include <gpgme++/gpgsetownertrusteditinteractor.h>
#include <gpgme++/gpgsignkeyeditinteractor.h>
#include <vector>
#include "LoopbackPassphraseProvider.h"

using namespace std;
using namespace GpgME;

/**
 * Main class for GPGME OP, except deleting keys.
 */
class GpgMEWorker
{
public:
    GpgMEWorker();
    virtual ~GpgMEWorker();

    /**
     * Find keys corresponding to pattern. GPGME searches in many fields :
     * keyid, fingerprint, name, email...
     * @param pattern
     * @param hasSecret
     * @param e
     * @return 
     */
    vector<GpgME::Key> FindKeys(const char * pattern, bool hasSecret, Error& e) const;
    /**
     * Search for a single key fully identified : keyid, short keyid, fingerprint.
     * @param anyFullId
     * @param e
     * @param secret
     * @return 
     */
    GpgME::Key FindKey(const char * anyFullId, Error& e, bool secret = false) const;
    /**
     * Import a key from file.
     * @param filePath
     * @param e
     * @return the keyid
     */
    const string ImportKey(const char * filePath, Error& e);
    /**
     * Inplace editing of owner trust if allowed
     * @param anyFullId
     * @param trustLevel : New trust level in key owner
     * @return 
     */
    const Error EditOwnerTrust(const char * anyFullId, GpgME::Key::OwnerTrust trustLevel);
    /**
     * Certify (sign) selected key.
     * @param fprSigningKey
     * @param fprKeyToSign
     * @param userIDsToSign : index of each user identity in a vector.
     * @param options : Exportable (1), Non revocable (2).
     * \nTrust(4) is not implemented.
     * @param passphrase
     * @return 
     */
    const Error CertifyKey(const char * fprSigningKey,
                           const char * fprKeyToSign,
                           vector<uint>& userIDsToSign, int options,
                           const string& passphrase);

private:
    Context * m_ctx;
    // GPG will fetch a password here.
    LoopbackPassphraseProvider * m_ppp;
};

/**
 * Edit trust in key owner
 */
class SetOwnerTrustEditInteractor : public GpgSetOwnerTrustEditInteractor
{
public:

    /**
     * 
     * @param ownerTrust : New trust level
     */
    SetOwnerTrustEditInteractor(GpgME::Key::OwnerTrust ownerTrust)
    : GpgSetOwnerTrustEditInteractor(ownerTrust)
    {
    }

    virtual ~SetOwnerTrustEditInteractor()
    {
    }

};

/**
 * Passed to GPG engine to certify (sign) a key.
 */
class SetSignKeyEditInteractor : public GpgSignKeyEditInteractor
{
public:

    SetSignKeyEditInteractor() : GpgSignKeyEditInteractor()
    {
    };

    virtual ~SetSignKeyEditInteractor()
    {
    };

};

#endif /* GPGMEWORKER_H */

