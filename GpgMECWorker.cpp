/* 
 * File:   GpgMECWorker.cpp
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 * 
 * Created on 14 octobre 2019, 15:22
 */

#include "GpgMECWorker.h"

GpgMECWorker::GpgMECWorker()
{
    gpgme_error_t c_err = gpgme_new(&c_ctx);
}

GpgMECWorker::~GpgMECWorker()
{
    gpgme_release(c_ctx);
}

bool GpgMECWorker::DeleteKey(const char * fpr, bool secret, GpgME::Error& e)
{
    gpgme_key_t c_key;
    gpgme_error_t c_err = gpgme_get_key(c_ctx, fpr, &c_key, secret);
    if (c_key == NULL)
    {
        e = GpgME::Error::fromCode(c_err);
        return false;
    }
    int flags = secret ? GPGME_DELETE_ALLOW_SECRET | GPGME_DELETE_FORCE : GPGME_DELETE_FORCE;
    c_err = gpgme_op_delete_ext(c_ctx, c_key, flags);
    if (c_err != 0)
    {
        e = GpgME::Error::fromCode(c_err);
        return false;
    }
    return true;
}
