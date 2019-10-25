/* 
 * File:   AppConfig.cpp
 * Author: SET - nmset@netcourrier.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on 9 octobre 2019, 20:23
 */

#include "AppConfig.h"
#include "global.h"
#include <vector>
#include <fstream>
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WSslInfo.h>
#include <Wt/Json/Value.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Serializer.h>

using namespace std;

// Hard coded. File must be in WT_APP_ROOT
#define JSON_CONFIG_FILE "k7config.json"
/*
 {
    "sCommonName" : {
        "UserCommonName1" : {
            "canImport" : true,
            "canDelete" : true,
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