/* 
 * File:   AppConfig.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on 9 octobre 2019, 20:23
 */

#include "AppConfig.h"
#include "global.h"
#include <vector>
#include <fstream>
#include <mutex>
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WSslInfo.h>
#include <Wt/Json/Value.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Serializer.h>
#include <algorithm>

using namespace std;

mutex gs_fileWriteMutex;

// Hard coded. File must be in WT_APP_ROOT
#define JSON_CONFIG_FILE "k7config.json"

/*
 {
    "sCommonName" : {
        "UserCommonName1" : {
            "canImport" : true,
            "canDelete" : true,
            "canEditOwnerTrust" : true,
            "canEditUidValidity" : true,
            "canEditExpiryTime" : true, 
            "canCreateKeys" : true,
            "privKeyIds" : [
                "fullKeyId1",
                "fullKeyId2"
            ]
        },
        "UserCommonName2" : {
                ...
        }
    }
}
 */

AppConfig::AppConfig(WText * notifyWidget)
{
    m_notifyWidget = notifyWidget;
}

AppConfig::~AppConfig()
{
}

bool AppConfig::LoadConfig()
{
    // Read config file
    ifstream jsonFile;
    const WString f(WApplication::appRoot() + WString(JSON_CONFIG_FILE));
    jsonFile.open(f.toUTF8().c_str());
    if (!jsonFile.is_open())
    {
        m_notifyWidget->setText(TR("CantLoadConfig"));
        return false;
    }
    string line;
    WString configData;
    while (getline(jsonFile, line))
    {
        configData += WString(line);
    }
    jsonFile.close();

    Json::ParseError pe;
    Json::parse(configData.toUTF8(), m_RootObject, pe, false);
    if (!string(pe.what()).empty())
    {
        // pe.what() dumps all Json data
        m_notifyWidget->setText(TR("CantParseJson"));
    }

    if (!m_RootObject.contains("sCommonName"))
    {
        m_notifyWidget->setText(TR("CNObjectMissing"));
        return false;
    }
    m_SubjectCNObject = m_RootObject.get("sCommonName");
    // If the client cert's subject common name is not listed, deny access.
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
    {
        m_notifyWidget->setText(TR("AccessDenied"));
        return false;
    }
    return true;
}

bool AppConfig::CanImport() const
{
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8())) // Should not happen, see above
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canImport"))
        return false;
    return cnObject.get("canImport");
}

bool AppConfig::CanDelete() const
{
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canDelete"))
        return false;
    return cnObject.get("canDelete");
}

bool AppConfig::CanEditOwnerTrust() const
{
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canEditOwnerTrust"))
        return false;
    return cnObject.get("canEditOwnerTrust");
}

bool AppConfig::CanEditUidValidity() const
{
    if (PrivateKeyIds().size() == 0)
        return false;
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canEditUidValidity"))
        return false;
    return cnObject.get("canEditUidValidity");
}

bool AppConfig::CanEditExpiryTime() const
{
    if (PrivateKeyIds().size() == 0)
        return false;
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canEditExpiryTime"))
        return false;
    return cnObject.get("canEditExpiryTime");
}

bool AppConfig::CanCreateKeys() const
{
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("canCreateKeys"))
        return false;
    return cnObject.get("canCreateKeys");
}

bool AppConfig::UpdateSecretKeyOwnership(const WString& fpr, bool own)
{
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return false;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    Json::Array aKeyId;
    if (cnObject.contains("privKeyIds"))
    {
        aKeyId = cnObject.get("privKeyIds");
        cnObject.erase("privKeyIds");
    }
    Json::Array::iterator it = std::find(aKeyId.begin(), aKeyId.end(), Json::Value(fpr));
    if (it == aKeyId.end())
    {
        if (own) 
            aKeyId.push_back(Json::Value(fpr));
        else
            return true; // We don't own it.
    }
    else
    {
        if (not own)
            aKeyId.erase(it);
        else
            return true; // We already own it.
    }
    
    // TODO : Do this **better**, without replacing sequentially up to root.
    cnObject.insert(make_pair("privKeyIds", aKeyId));
    m_SubjectCNObject.erase(commonName.toUTF8());
    m_SubjectCNObject.insert(make_pair(commonName.toUTF8(), cnObject));
    m_RootObject.erase("sCommonName");
    m_RootObject.insert(make_pair("sCommonName", m_SubjectCNObject));

    if (WriteTextFile(JSON_CONFIG_FILE, Json::serialize(m_RootObject)))
        return LoadConfig();
    return false;
}

vector<WString> AppConfig::PrivateKeyIds() const
{
    // List private key identifiers.
    vector<WString> pKeyIds;
    const WString commonName = GetSubjectDnAttribute(WSslCertificate::DnAttributeName::CommonName);
    if (!m_SubjectCNObject.contains(commonName.toUTF8()))
        return pKeyIds;
    Json::Object cnObject = m_SubjectCNObject.get(commonName.toUTF8());
    if (!cnObject.contains("privKeyIds"))
        return pKeyIds;
    Json::Array aKeyId = cnObject.get("privKeyIds");
    for (uint i = 0; i < aKeyId.size(); i++)
    {
        pKeyIds.push_back(aKeyId.at(i));
    }
    return pKeyIds;
}

const WString AppConfig::GetSubjectDnAttribute(const WSslCertificate::DnAttributeName& attrName) const
{
    // Get an X509 client certificate attribute value
    const vector<WSslCertificate::DnAttribute> * dnAttr
            = &(WApplication::instance()->environment().sslInfo()->clientCertificate().subjectDn());
    for (uint i = 0; i < dnAttr->size(); i++)
    {
        if (dnAttr->at(i).name() == attrName)
            return dnAttr->at(i).value();
    }
    return WString::Empty;
}

bool AppConfig::WriteTextFile(const WString& filePath, const WString& content)
{
    gs_fileWriteMutex.lock();
    ofstream osFile;
    const WString f(WApplication::appRoot() + WString(filePath));
    osFile.open(f.toUTF8().c_str());
    if (!osFile.is_open())
    {
        gs_fileWriteMutex.unlock();
        return false;
    }
    osFile << content.toUTF8();
    osFile.flush();
    osFile.close();
    gs_fileWriteMutex.unlock();
    return osFile.bad();
}
