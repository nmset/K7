/* 
 * File:   GpgMEWorker.cpp
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 * 
 * Created on 11 octobre 2019, 16:34
 */

#include "GpgMEWorker.h"
#include <gpgme++/keylistresult.h>
#include <gpgme++/importresult.h>
#include <locale>
#include <iostream>
#include <gpgme++/data.h>

GpgMEWorker::GpgMEWorker()
{
    m_ctx = Context::createForProtocol(Protocol::OpenPGP);
    // Allow to list key certifications
    m_ctx->setKeyListMode(GpgME::KeyListMode::Signatures
                    | GpgME::KeyListMode::Validate);
    m_ppp = NULL;
}

GpgMEWorker::~GpgMEWorker()
{
    delete m_ppp;
    delete m_ctx;
}

vector<GpgME::Key> GpgMEWorker::FindKeys(const char * pattern, bool hasSecret, Error& e) const
{
    vector<Key> klst;
    e = m_ctx->startKeyListing(pattern, hasSecret);
    if (e.code() != 0)
        return klst;
    KeyListResult klr = m_ctx->keyListResult();
    if (klr.error().code() != 0)
    {
        e = klr.error();
        return klst;
    }
    while (e.code() == 0)
    {
        Key k = m_ctx->nextKey(e);
        if (!k.isNull())
        { // What is that null key alongside ?
            klst.push_back(k);
        }
    }
    // e.code() != 0 here
    klr = m_ctx->endKeyListing();
    e = klr.error();
    return klst;
}

GpgME::Key GpgMEWorker::FindKey(const char * anyFullId, Error& e, bool secret) const
{
    return m_ctx->key(anyFullId, e, secret);
}

const string GpgMEWorker::ImportKey(const char * filePath, Error& e)
{
    // Should we use a mutex here ?
    FILE * kFp = fopen(filePath, "rb");
    GpgME::Data dKey(kFp);
    vector<GpgME::Key> keys = dKey.toKeys();
    if (keys.size() == 0)
    {
        fclose(kFp);
        return "";
    }
    const string keyid = string(dKey.toKeys().at(0).keyID()); // Must be done before import
    ImportResult rImportKey = m_ctx->importKeys(dKey);
    e = rImportKey.error();
    if (e.code() != 0)
    {
        fclose(kFp);
        return "";
    }

    fclose(kFp);
    return keyid;
}

const Error GpgMEWorker::EditOwnerTrust(const char* anyFullId, GpgME::Key::OwnerTrust trustLevel)
{
    Error e;
    Key k = FindKey(anyFullId, e, false);
    if (e.code() != 0)
        return e;
    SetOwnerTrustEditInteractor * interactor = new SetOwnerTrustEditInteractor(trustLevel);
    GpgME::Data d; // Internal processing data
    return m_ctx->edit(k, std::unique_ptr<SetOwnerTrustEditInteractor> (interactor), d);
}

const Error GpgMEWorker::CertifyKey(const char* fprSigningKey,
                                    const char * fprKeyToSign,
                                    vector<uint>& userIDsToSign, int options,
                                    const string& passphrase)
{
    Error e;
    Key signingKey = FindKey(fprSigningKey, e, true);
    if (e.code() != 0)
        return e;
    e = m_ctx->addSigningKey(signingKey); // +++
    if (e.code() != 0)
        return e;
    Key keyToSign = FindKey(fprKeyToSign, e, false);
    if (e.code() != 0)
        return e;
    
    // GPG engine will fetch for  passphrase in the custom provider.
    m_ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    if (m_ppp == NULL)
        m_ppp = new LoopbackPassphraseProvider();
    m_ppp->SetPassphrase(passphrase);
    m_ctx->setPassphraseProvider(m_ppp);
    
    SetSignKeyEditInteractor * interactor = new SetSignKeyEditInteractor();
    interactor->setKey(keyToSign);
    interactor->setUserIDsToSign(userIDsToSign);
    interactor->setSigningOptions(options);
    // What's that check level ?
    // interactor->setCheckLevel(2);
    GpgME::Data d;
    e = m_ctx->edit(keyToSign, std::unique_ptr<SetSignKeyEditInteractor> (interactor), d);
    m_ctx->clearSigningKeys();
    /*
     * On error, always : code = 1024 | asString = User defined error code 1
     * Can't distinguish between bad password or whatever cause.
     */
    return e;
}
