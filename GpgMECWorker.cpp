/* 
 * File:   GpgMECWorker.cpp
 * Author: SET - nmset@yandex.com
 * License : LGPL v2.1
 * Copyright SET - © 2019
 * 
 * Created on 14 octobre 2019, 15:22
 */

#include "GpgMECWorker.h"

#ifdef DEVTIME

/**
 * This callback is never called when exporting private keys.
 * From gpgme-1.14.0/tests/run-support.h
 */
gpgme_error_t passphrase_cb(void *opaque, const char *uid_hint, const char *passphrase_info,
                            int last_was_bad, int fd)
{
    int res;
    char pass[] = "test_pwd\n";
    int passlen = strlen(pass);
    int off = 0;

    (void) opaque;
    (void) uid_hint;
    (void) passphrase_info;
    (void) last_was_bad;

    do
    {
        res = gpgme_io_write(fd, &pass[off], passlen - off);
        if (res > 0)
            off += res;
    }
    while (res > 0 && off != passlen);

    return off == passlen ? 0 : gpgme_error_from_errno(errno);
}
#endif

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

#ifdef DEVTIME

/**
 * Much code here is obtained from gpgme-1.14.0/tests/run-export.c
 * @param fpr
 * @param e
 * @return 
 */
bool GpgMECWorker::ExportPrivateKey(const char* fpr, GpgME::Error& e)
{
    gpgme_key_t c_key;
    gpgme_data_t out;
    gpgme_set_armor(c_ctx, 1);

    gpgme_error_t c_err = gpgme_get_key(c_ctx, fpr, &c_key, true);
    if (c_key == NULL)
    {
        e = GpgME::Error::fromCode(c_err);
        return false;
    }

    FILE * kFp = fopen("/tmp/data", "wb");
    if (kFp == NULL)
        return false;
    c_err = gpgme_data_new_from_stream(&out, kFp);
    if (c_err != 0)
    {
        fclose(kFp);
        e = GpgME::Error::fromCode(c_err);
        return false;
    }

    gpgme_key_t aKeys[2];
    aKeys[0] = c_key;
    aKeys[1] = NULL;

    c_err = gpgme_set_pinentry_mode(c_ctx, GPGME_PINENTRY_MODE_LOOPBACK);
    if (c_err != 0)
    {
        fclose(kFp);
        e = GpgME::Error::fromCode(c_err);
        return false;
    }
    gpgme_set_passphrase_cb(c_ctx, passphrase_cb, NULL);

    gpgme_export_mode_t flags = GPGME_EXPORT_MODE_SECRET;
    c_err = gpgme_op_export_keys(c_ctx, aKeys, flags, out);
    if (c_err != 0)
    {
        fclose(kFp);
        gpgme_data_release(out);
        e = GpgME::Error::fromCode(c_err);
        return false;
    }
    fflush(NULL);
    fclose(kFp);

    gpgme_data_release(out);

    return true;
}
#endif
