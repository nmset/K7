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
class AppConfig
{
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
     * Allows to edit trust in key owner. Users who don't manage private keys
     * can to that too.
     * @return 
     */
    bool CanEditOwnerTrust() const;
    /**
     * Allows to edit validity of user identity. 
     * Only users who manage private keys can do that.
     * @return 
     */
    bool CanEditUidValidity() const;
    /**
     * Allows to edit expiry time of a key. 
     * Only private keys are concerned.
     * @return 
     */
    bool CanEditExpiryTime() const;
    /**
     * Allows to create keys. At the application level, it means creating a pair
     * of secret and private keys.
     * @return 
     */
    bool CanCreateKeys() const;
    /**
     * Allows to add or revoke a user identity to a key. It deos not mean
     * deleting an identity.
     * @return 
     */
    bool CanAddRevokeUid() const;
    /**
     * Disown the user of the secret key, or grants him ownership. It just adds
     * or remove fpr from the private key array, without any further check. The
     * application must call here when relevant.
     * @param fpr
     * @param own : if true, adds the fpr to the private key array if found.
     * Else, removes the fpr if found.
     * @return 
     */
    bool UpdateSecretKeyOwnership(const WString& fpr, bool own);
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

    bool WriteTextFile(const WString& filePath, const WString& content);
};

#endif /* APPCONFIG_H */

