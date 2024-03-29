/* 
 * File:   GpgMECWorker.h
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright Saleem Edah-Tally - © 2019
 *
 * Created on 14 octobre 2019, 15:22
 */

#ifndef GPGMECWORKER_H
#define GPGMECWORKER_H

#include <gpgme.h>
#include <gpgme++/error.h>

class GpgMECWorker
{
public:
    GpgMECWorker();
    virtual ~GpgMECWorker();

    /**
     * Deleting keys must be done with the C API because 
     * gpgmepp does not provide a way to use GPGME_DELETE_FORCE,
     * resulting in a confirmation dialog triggered by GPG.
     * This does not fit use on a web server.
     * @param fpr
     * @param secret delete secret key ?
     * @param e
     * @return 
     */
    bool DeleteKey(const char * fpr, bool secret, GpgME::Error& e);

private:
    gpgme_ctx_t c_ctx;
};

#endif /* GPGMECWORKER_H */

