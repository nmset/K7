/* 
 * File:   AppConfig.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 9 octobre 2019, 20:23
 */

#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <Wt/WString.h>
#include <Wt/Json/Object.h>
#include <Wt/WSslCertificate.h>
#include <Wt/WText.h>
#include <vector>

using namespace Wt;
using namespace std;

/**
 * Json configuration file reader.
 */
class AppConfig {
public:
    AppConfig(WText * notifyWidget);
    virtual ~AppConfig();
    /**
     * Must be called by app. No autoloading of config file.
     * @return 
     */
    bool LoadConfig();
    /**
     * Can the user import keys ?
     * @return 
     */
    bool CanImport() const;
    /**
     * Can the user delete keys ? N.B. : he may delete private keys
     * only if he manages these keys.
     * @return 
     */
    bool CanDelete() const;
    /**
     * List of full private key identifiers. The user may delete these private keys.
     * Must be full keyid, short keyid or fingerprint.
     * @return 
     */
    vector<WString> PrivateKeyIds() const;
private:
    /**
     * To display error messages
     */
    WText * m_notifyWidget;
    /**
     * Root object of the config file
     */
    Json::Object m_RootObject;
    /**
     * User object, identified by the Subject Common Name of the X509 client certificate.
     */
    Json::Object m_SubjectCNObject;
    /**
     * Get an X509 client certificate attribute value
     * @param attrName
     * @return 
     */
    const WString GetSubjectDnAttribute(const WSslCertificate::DnAttributeName& attrName) const;
};

#endif /* APPCONFIG_H */

