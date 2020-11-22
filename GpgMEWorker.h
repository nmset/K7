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
#include <gpgme++/gpgsetexpirytimeeditinteractor.h>
#include <gpgme++/gpgadduserideditinteractor.h>
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
     * @return : the fingerprint
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
    /**
     * Revoke UserID certifications.
     * \n Requires GnuPG >= 2.2.24
     * @param fprSigningKey
     * @param fprKeyToSign
     * @param userIDsToRevoke : vector of ::UserID
     * @param passphrase
     * @return 
     */
    const Error RevokeKeyCertifications(const char * fprSigningKey,
                           const char * fprKeyToSign,
                           vector<GpgME::UserID>& userIDsToRevoke,
                           const string& passphrase);
    /**
     * Sets the expiry time of a single subkey. Requires GPGME >= 1.15.0.
     * \n If no subkey is found (wrong fpr), the expiry time of key is set
     * instead.
     * @param keyFpr
     * @param subkeyFpr
     * @param passphrase
     * @param expires : seconds from now. Use 0 for no expiry.
     * @return 
     */
    const Error SetSubkeyExpiryTime(const char * keyFpr,
                                    const char * subkeyFpr,
                                    const string& passphrase,
                                    ulong expires = 63072000);
    /**
     * Set new expiry time of a secret key.
     * @param timeString
     * @return 
     */
    const Error SetExpiryTime(const char * keyFpr,
                              const string& passphrase,
                              const string& timeString = "0");
    /**
     * Adds a user identity to a key.
     * \n The email parameter must have a valid email address format here, else
     * GPGME will refuse to create the identity. This is not the case when
     * creating a key, where any string is accepted.
     * @param keyFpr
     * @param passphrase
     * @param name
     * @param email : string with a valid email format
     * @param comment
     * @return 
     */
    const Error AddUserID(const char * keyFpr, const string& passphrase,
                          const string& name, const string& email,
                          const string& comment);
    /**
     * Revoke a user identity. Does not remove the identity.
     * \n N.B. : if the uid is revoked a second time, Error::code is 0.
     * @param keyFpr
     * @param passphrase
     * @param name
     * @param email
     * @param comment
     * @return 
     */
    const Error RevokeUserID(const char * keyFpr, const string& passphrase,
                             const string& name, const string& email,
                             const string& comment);
    /**
     * Creates a pair of secret and public keys with the default engine
     * algorithms. Default expiry time is 2 * 365 days.
     * @param k : must be a null key
     * @param name
     * @param email
     * @param comment
     * @param passphrase
     * @param expires : seconds ahead of creation time. Use 0 for no expiry.
     * @return 
     */
    const Error CreateKeyWithEngineDefaultAlgo(GpgME::Key& k,
                                               const string& name,
                                               const string& email,
                                               const string& comment,
                                               const string& passphrase,
                                               ulong expires = 63072000);
    /**
     * Creates a secret key with passed in algorithm name. Default expiry time
     * is 2 * 365 days.
     * @param k : must be a null key
     * @param name
     * @param email
     * @param comment
     * @param algo : a valid algorithm name for a secret key
     * @param passphrase
     * @param expires : seconds ahead of creation time. Use 0 for no expiry.
     * @return 
     */
    const Error CreateKey(GpgME::Key& k,
                          const string& name,
                          const string& email,
                          const string& comment,
                          const char * algo,
                          const string& passphrase,
                          ulong expires = 63072000);
    /**
     * Creates a public key with passed in algorithm name and adds it to secret
     * key k. Default expiry time is 2 * 365 days.
     * @param k : must be a secret key
     * @param algo : : a valid algorithm name for a public key
     * @param passphrase
     * @param expires : seconds ahead of creation time. Use 0 for no expiry.
     * @return 
     */
    const Error CreateSubKey(GpgME::Key& k,
                             const char * algo,
                             const string& passphrase,
                             ulong expires = 63072000);

#ifdef DEVTIME
    /**
     * Status : testing
     * \n Result : fails to export a private key
     * \n Reason : loopback passphrase provider is never called
     * \n With default pinentry mode, the password is requested normally 
     * and the private key is exported. But this can't be done on a web server.
     * @param fpr
     * @param e
     * @return 
     */
    const Error ExportPrivateKey(const char * pattern, string& buffer,
                                 const string& passphrase = "");
#endif
    /**
     * Export a public key.
     * @param pattern : a key fingerprint
     * @param buffer : returns data in armor mode
     * @return 
     */
    const Error ExportPublicKey(const char * pattern, string& buffer);
private:
    Context * m_ctx;
    // GPG will fetch a password here.
    LoopbackPassphraseProvider * m_ppp;

    string MakeUidString(const string& name, const string& email,
                         const string& comment);
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

class SetExpiryTimeEditInteractor : public GpgSetExpiryTimeEditInteractor
{
public:

    SetExpiryTimeEditInteractor(const std::string& timeString = "0")
    : GpgSetExpiryTimeEditInteractor(timeString)
    {
    };

    virtual ~SetExpiryTimeEditInteractor()
    {
    };
};

class AddUserIDEditInteractor : public GpgAddUserIDEditInteractor
{
public:

    AddUserIDEditInteractor()
    : GpgAddUserIDEditInteractor()
    {
    }

    virtual ~AddUserIDEditInteractor()
    {
    };
};

#endif /* GPGMEWORKER_H */

