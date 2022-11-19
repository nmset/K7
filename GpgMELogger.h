/* 
 * File:   GpgMELogger.h
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 *
 * Created on November 23, 2020, 4:00 PM
 */

#ifndef GPGMELOGGER_H
#define GPGMELOGGER_H

#include <Wt/WLogger.h>
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WSslCertificate.h>
#include <Wt/WSslInfo.h>
#include <gpgme++/error.h>
#include <mutex>

using namespace Wt;

static std::mutex gs_logGpgMEError;
#define LGE(e) LogGpgMEError(e)

static void LogGpgMEError(const GpgME::Error& e)
{
    if (e.code() == 0)
        return;
    std::unique_lock<std::mutex> lock(gs_logGpgMEError);

    const vector<WSslCertificate::DnAttribute> * dnAttr
            = &(WApplication::instance()->environment().sslInfo()
            ->clientCertificate().subjectDn());
    WString subjectCommonName = WString::Empty;
    for (uint i = 0; i < dnAttr->size(); i++)
    {
        if (dnAttr->at(i).name() == WSslCertificate::DnAttributeName::CommonName)
        {
            subjectCommonName = dnAttr->at(i).value();
            break;
        }
    }
    

    const string logfile = WApplication::appRoot() + string("gpgme.log");
    WLogger logger;
    logger.setFile(logfile);
    logger.entry("") << WLogger::timestamp
            << WLogger::sep << WString("[") << subjectCommonName << WString("]")
            << WLogger::sep << WApplication::instance()->environment().userAgent()
            << WLogger::sep << std::to_string(e.code())
            << WLogger::sep << e.asString();
}

#endif /* GPGMELOGGER_H */

