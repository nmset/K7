/* 
 * File:   GpgMELogger.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 23, 2020, 4:00 PM
 */

#ifndef GPGMELOGGER_H
#define GPGMELOGGER_H

#include <Wt/WLogger.h>
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
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
    const string logfile = WApplication::appRoot() + string("gpgme.log");
    WLogger logger;
    logger.setFile(logfile);
    logger.entry("") << WLogger::timestamp
            << WLogger::sep << WApplication::instance()->environment().userAgent()
            << WLogger::sep << std::to_string(e.code())
            << WLogger::sep << e.asString();
}

#endif /* GPGMELOGGER_H */

