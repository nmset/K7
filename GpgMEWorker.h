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

private:
    Context * m_ctx;
};

#endif /* GPGMEWORKER_H */

