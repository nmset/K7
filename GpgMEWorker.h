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
#include <vector>

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

private:
    Context * m_ctx;
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
    : GpgSetOwnerTrustEditInteractor(ownerTrust) {}

    virtual ~SetOwnerTrustEditInteractor() {}

};

#endif /* GPGMEWORKER_H */

