/* 
 * File:   Tools.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 1, 2020, 8:31 PM
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <Wt/WString.h>
#include "GpgMEWorker.h"

using namespace Wt;

/**
 * A few general usage functions.
 */
class Tools {
public:
    Tools();
    virtual ~Tools();
    
    /**
     * We want key identifier in config file to be real and complete.
     * @param k
     * @param configKeyId key identifier as entered in configuration file
     * @return 
     */
    static bool ConfigKeyIdMatchesKey(const GpgME::Key& k,
                                const WString& configKeyId);
    /**
     * Integer value of a string.
     * @param s
     * @return 
     */
    static int ToInt(const string& s);
    
    static WString TexttualBool(bool value);
    
    static WString GetKeyStatus(const GpgME::Key& k);
    static WString GetUidStatus(const GpgME::UserID& uid);
    static WString GetSigStatus(const GpgME::UserID::Signature& sig);
    static bool KeyHasSecret(const WString& fpr);
    /**
     * If the fingerprint is that of a private key we manage, returns true.
     * @param fpr
     * @param ourPrivKeys
     * @return 
     */
    static bool IsOurKey(const WString& fpr, vector<WString>& ourPrivKeys);
    
private:
    static bool IsFound(const WString& item, vector<WString>& items);
};

#endif /* TOOLS_H */

