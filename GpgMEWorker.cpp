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
}

GpgMEWorker::~GpgMEWorker()
{
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