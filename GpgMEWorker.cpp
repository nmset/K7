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
#include <gpgme++/keygenerationresult.h>
#include <locale>
#include <iostream>
#include <gpgme++/data.h>

using namespace std;

#define SPACE string(" ")
#define LESSTHAN string("<")
#define MORETHAN string(">")
#define LEFT_PARENTHESIS string("(")
#define RIGHT_PARENTHESIS string(")")
// From gpgme.h (C API), don't want to include it here for one const.
#define _CREATE_NOEXPIRE   (1 << 13)

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
    const string fpr = string(dKey.toKeys().at(0).primaryFingerprint()); // Must be done before import
    ImportResult rImportKey = m_ctx->importKeys(dKey);
    e = rImportKey.error();
    if (e.code() != 0)
    {
        fclose(kFp);
        return "";
    }

    fclose(kFp);
    return fpr;
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

const Error GpgMEWorker::SetExpiryTime(const char * keyFpr,
                                       const string& passphrase,
                                       const string& timeString)
{
    Error e;
    Key k = FindKey(keyFpr, e, true);
    if (e.code() != 0)
        return e;
    e = m_ctx->addSigningKey(k); // +++
    if (e.code() != 0)
        return e;

    m_ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    if (m_ppp == NULL)
        m_ppp = new LoopbackPassphraseProvider();
    m_ppp->SetPassphrase(passphrase);
    m_ctx->setPassphraseProvider(m_ppp);

    SetExpiryTimeEditInteractor * interactor
            = new SetExpiryTimeEditInteractor(timeString);
    GpgME::Data d;
    e = m_ctx->edit(k, std::unique_ptr<SetExpiryTimeEditInteractor> (interactor), d);
    m_ctx->clearSigningKeys();

    return e;
}

const Error GpgMEWorker::AddUserID(const char* keyFpr, const string& passphrase, 
                                   const string& name, const string& email,
                                   const string& comment)
{
    Error e;
    Key k = FindKey(keyFpr, e, true);
    if (e.code() != 0)
        return e;
    e = m_ctx->addSigningKey(k);
    if (e.code() != 0)
        return e;
    
    m_ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    if (m_ppp == NULL)
        m_ppp = new LoopbackPassphraseProvider();
    m_ppp->SetPassphrase(passphrase);
    m_ctx->setPassphraseProvider(m_ppp);
    
    AddUserIDEditInteractor * interactor = new AddUserIDEditInteractor();
    interactor->setNameUtf8(name);
    interactor->setEmailUtf8(email);
    interactor->setCommentUtf8(comment);
    GpgME::Data d;
    e = m_ctx->edit(k, std::unique_ptr<AddUserIDEditInteractor> (interactor), d);
    m_ctx->clearSigningKeys();

    return e;
}

const Error GpgMEWorker::RevokeUserID(const char* keyFpr,
                                      const string& passphrase,
                                      const string& name, const string& email,
                                      const string& comment)
{
    Error e;
    Key k = FindKey(keyFpr, e, true);
    if (e.code() != 0)
        return e;
    e = m_ctx->addSigningKey(k);
    if (e.code() != 0)
        return e;
    
    m_ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    if (m_ppp == NULL)
        m_ppp = new LoopbackPassphraseProvider();
    m_ppp->SetPassphrase(passphrase);
    m_ctx->setPassphraseProvider(m_ppp);
    
    const string uid = MakeUidString(name, email, comment);
    e = m_ctx->revUid(k, uid.c_str());
    k.update();
    m_ctx->clearSigningKeys();
    
    return e;
}

/*
 * Using a temporary context for key creation. It is altered after secret key
 * creation, and subkey creation fails thereafter. This is observational.
 */
const Error GpgMEWorker::CreateKeyWithEngineDefaultAlgo(GpgME::Key& k,
                                                        const string& name,
                                                        const string& email,
                                                        const string& comment,
                                                        const string& passphrase,
                                                        ulong expires)
{
    Error e;
    Context * ctx = Context::createForProtocol(Protocol::OpenPGP);
    LoopbackPassphraseProvider * ppp = new LoopbackPassphraseProvider(passphrase);
    ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    ctx->setPassphraseProvider(ppp);

    const string uid =MakeUidString(name, email, comment);
    uint flags = expires
            ? 0 : _CREATE_NOEXPIRE;

    KeyGenerationResult kgr = ctx->createKeyEx(uid.c_str(), "default",
                                               0, expires, k, flags);
    delete ppp;
    delete ctx;
    if (kgr.error().code() == 0)
        // Why is k not assigned the newly created key ?!
        k = FindKey(kgr.fingerprint(), e, true);
    return kgr.error();
}

const Error GpgMEWorker::CreateKey(GpgME::Key& k,
                                   const string& name,
                                   const string& email,
                                   const string& comment,
                                   const char* algo,
                                   const string& passphrase,
                                   ulong expires)
{
    Error e;
    Context * ctx = Context::createForProtocol(Protocol::OpenPGP);
    LoopbackPassphraseProvider * ppp = new LoopbackPassphraseProvider(passphrase);
    ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    ctx->setPassphraseProvider(ppp);

    const string uid = MakeUidString(name, email, comment);
    uint flags = expires
            ? 0 : _CREATE_NOEXPIRE;

    KeyGenerationResult kgr = ctx->createKeyEx(uid.c_str(), algo,
                                               0, expires, k, flags);
    delete ppp;
    delete ctx;
    if (kgr.error().code() == 0)
        // Why is k not assigned the newly created key ?!
        k = FindKey(kgr.fingerprint(), e, true);
    return kgr.error();
}

const Error GpgMEWorker::CreateSubKey(GpgME::Key& k,
                                      const char* algo,
                                      const string& passphrase,
                                      ulong expires)
{
    Error e;
    Context * ctx = Context::createForProtocol(Protocol::OpenPGP);
    LoopbackPassphraseProvider * ppp = new LoopbackPassphraseProvider(passphrase);
    ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    ctx->setPassphraseProvider(ppp);

    uint flags = expires
            ? 0 : _CREATE_NOEXPIRE;

    e = ctx->createSubkey(k, algo, 0, expires, flags);
    k.update();
    delete ppp;
    delete ctx;
    return e;
}

#ifdef DEVTIME

const Error GpgMEWorker::ExportPrivateKey(const char * pattern, string& buffer,
                                          const string& passphrase)
{
    GpgME::Data kData;
    Context * ctx = Context::createForProtocol(Protocol::OpenPGP);
    LoopbackPassphraseProvider * ppp = new LoopbackPassphraseProvider();
    ppp->SetPassphrase(passphrase);
    ctx->setPinentryMode(Context::PinentryMode::PinentryLoopback);
    ctx->setPassphraseProvider(ppp);

    ctx->setArmor(true);
    uint flags = Context::ExportSecret;

    Error e = ctx->exportPublicKeys(pattern, kData, flags);
    buffer = kData.toString(); // Empty

    delete ppp;
    delete ctx;

    return e;
}
#endif

const Error GpgMEWorker::ExportPublicKey(const char* pattern, string& buffer)
{
    GpgME::Data kData;
    Context * ctx = Context::createForProtocol(Protocol::OpenPGP);
    ctx->setArmor(true);
    uint flags = Context::ExportDefault;

    Error e = ctx->exportPublicKeys(pattern, kData, flags);
    buffer = kData.toString();

    delete ctx;

    return e;
}

string GpgMEWorker::MakeUidString(const string& name, const string& email, const string& comment)
{
    string uid = name;
    if (!comment.empty())
        uid += SPACE + LEFT_PARENTHESIS + comment + RIGHT_PARENTHESIS;
    uid += SPACE
            + LESSTHAN + email + MORETHAN;
    return uid;
}
