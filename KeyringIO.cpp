/* 
 * File:   KeyringIO.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on November 13, 2020, 6:46 PM
 */

#include "KeyringIO.h"
#include "GpgMEWorker.h"
#include "GpgMECWorker.h"
#include "Tools.h"
#include <Wt/WLink.h>
#include <strstream>

using namespace std;

KeyringIO::KeyringIO(K7Main * owner)
{
    m_owner = owner;
    m_popupUpload = NULL;
    m_popupDelete = NULL;
    m_popupCreate = NULL;
    m_config = m_owner->m_config;
    m_tmwMessage = m_owner->m_tmwMessage;
    m_btnUpload = m_owner->m_btnUpload;
    m_btnImport = m_owner->m_btnImport;
    m_btnDelete = m_owner->m_btnDelete;
    m_btnCreate = m_owner->m_btnCreate;
    m_btnExport = m_owner->m_btnExport;
    m_leSearch = m_owner->m_leSearch;

    if (m_config->CanImport())
    {
        m_btnUpload->clicked().connect(this, &KeyringIO::ShowPopupUpload);
        m_btnImport->clicked().connect(this, &KeyringIO::DoImportKey);
    }
    if (m_config->CanDelete())
        m_btnDelete->clicked().connect(this, &KeyringIO::ShowPopupDelete);
    if (m_config->CanCreateKeys())
        m_btnCreate->clicked().connect(this, &KeyringIO::ShowPopupCreate);
}

KeyringIO::~KeyringIO()
{
    delete m_popupUpload;
    delete m_popupDelete;
    delete m_popupCreate;
}

void KeyringIO::ShowPopupUpload()
{
    if (m_popupUpload == NULL)
    {
        m_popupUpload = new PopupUpload(m_btnUpload,
                                        m_tmwMessage);
        m_popupUpload->Create();
        m_popupUpload->UploadDone().connect(this, &KeyringIO::OnUploadCompleted);
    }
    m_popupUpload->show();
}

void KeyringIO::OnUploadCompleted(const WString& spool)
{
    // Buffer the spool file name in the import button
    m_btnImport->setAttributeValue("spool", spool);
    m_btnImport->show();
    m_popupUpload->hide();
}

void KeyringIO::DoImportKey()
{
    const WString spool = m_btnImport->attributeValue("spool");
    Error e;
    GpgMEWorker gpgw;
    const WString fpr = gpgw.ImportKey(spool.toUTF8().c_str(), e);
    m_btnImport->hide();
    m_btnImport->setAttributeValue("spool", "");
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    if (fpr.empty())
    {
        m_tmwMessage->SetText(TR("ImportError") + fpr);
        return;
    }
    // Show the imported key
    GpgME::Key k = gpgw.FindKey(fpr.toUTF8().c_str(), e, false); // A public is present anyway
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    m_tmwMessage->SetText(TR("ImportSuccess") + fpr + WString(" - ") + WString(k.userID(0).name()));
    m_leSearch->setText(fpr);
    if (Tools::KeyHasSecret(fpr))
        m_config->UpdateSecretKeyOwnership(fpr, true);
    m_owner->Search();
}

bool KeyringIO::CanKeyBeDeleted(const WString& fullKeyID)
{
    // Caller should check m_config->canDelete first. m_btnDelete is null if can't delete.
    Error e;
    GpgMEWorker gpgw;
    GpgME::Key k = gpgw.FindKey(fullKeyID.toUTF8().c_str(), e, true); // Look for a private key
    if (e.code() != 0 && e.code() != 16383)
    { // 16383 : end of file, when key is not private
        m_tmwMessage->SetText(e.asString());
        return false;
    }
    // k can now be secret or public
    if (k.isNull())
    {// Is a public key
        k = gpgw.FindKey(fullKeyID.toUTF8().c_str(), e, false);
        // Prepare actual delete
        m_btnDelete->setAttributeValue("keyid", k.keyID());
        m_btnDelete->setAttributeValue("hasSecret", "0");
        return true;
    }
    /* 
     * k is now secret
     * Is selected private key one of those that the current user manages ?
     */
    vector<WString> curUserPrivKeys = m_config->PrivateKeyIds();
    vector<WString>::iterator it;
    for (it = curUserPrivKeys.begin(); it != curUserPrivKeys.end(); it++)
    {
        if (Tools::ConfigKeyIdMatchesKey(k, *it))
        {
            m_btnDelete->setAttributeValue("keyid", k.keyID());
            m_btnDelete->setAttributeValue("hasSecret", "1");
            return true;
        }
    }
    return false;
}

void KeyringIO::ShowPopupDelete()
{
    if (m_popupDelete == NULL)
    {
        m_popupDelete = new PopupDelete(m_btnDelete,
                                        m_tmwMessage);
        m_popupDelete->Create();
        m_popupDelete->GetDeleteButton()->clicked().connect(this, &KeyringIO::DoDeleteKey);
    }
    m_popupDelete->show();
}

void KeyringIO::DoDeleteKey()
{
    // Deleting keys requires the GPGME C API
    Error c_e, e;
    GpgMECWorker gpgcw;
    GpgMEWorker gpgw;
    const WString fullKeyID = m_btnDelete->attributeValue("keyid");
    const WString hasSecret = m_btnDelete->attributeValue("hasSecret");
    bool secret = true;
    if (hasSecret == WString("0"))
        secret = false;
    // Get the key before deletion, to show its ID on success
    GpgME::Key k = gpgw.FindKey(fullKeyID.toUTF8().c_str(), e, secret);
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    // Delete the key using the C API
    const WString fpr(k.primaryFingerprint());
    bool res = gpgcw.DeleteKey(k.primaryFingerprint(), secret, c_e);
    if (c_e.code() != 0)
    {
        m_tmwMessage->SetText(c_e.asString());
    }
    else
    {
        m_tmwMessage->SetText(TR("DeleteSuccess") + fpr + WString(" - ") + WString(k.userID(0).name()));
    }
    m_btnDelete->hide();
    m_popupDelete->hide();
    if (secret)
        m_config->UpdateSecretKeyOwnership(fpr, false);
    // Show that the key is no longer available
    m_leSearch->setText(fpr);
    m_owner->Search();
}

void KeyringIO::ShowPopupCreate()
{
    if (m_popupCreate == NULL)
    {
        m_popupCreate = new PopupCreate(m_btnCreate, m_tmwMessage);
        m_popupCreate->Create();
        m_popupCreate->GetApplyButton()->clicked().connect(this, &KeyringIO::DoCreateKey);
    }
    m_popupCreate->show();
}

void KeyringIO::DoCreateKey()
{
    if (!m_popupCreate->Validate())
        return;
    Error e;
    GpgME::Key k;
    GpgMEWorker gpgw;
    if (m_popupCreate->UseDefaultEngineAlgorithms())
    {
        e = gpgw.CreateKeyWithEngineDefaultAlgo(k, m_popupCreate->GetName().toUTF8(),
                                                m_popupCreate->GetEmail().toUTF8(),
                                                m_popupCreate->GetComment().toUTF8(),
                                                m_popupCreate->GetPassphrase().toUTF8(),
                                                m_popupCreate->GetExpiry());
    }
    else
    {
        e = gpgw.CreateKey(k, m_popupCreate->GetName().toUTF8(),
                           m_popupCreate->GetEmail().toUTF8(),
                           m_popupCreate->GetComment().toUTF8(),
                           m_popupCreate->GetArbitraryKeyAlgo().toUTF8().c_str(),
                           m_popupCreate->GetPassphrase().toUTF8(),
                           m_popupCreate->GetExpiry());
        // GPGME accepts a missing subkey.
        if (e.code() == 0 && !m_popupCreate->GetArbitrarySubkeyAlgo().empty())
            e = gpgw.CreateSubKey(k,
                                  m_popupCreate->GetArbitrarySubkeyAlgo().toUTF8().c_str(),
                                  m_popupCreate->GetPassphrase().toUTF8(),
                                  m_popupCreate->GetExpiry());
    }
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
    }
    else
    {
        const WString fpr(k.primaryFingerprint());
        m_tmwMessage->SetText(TR("CreateSuccess")
                              + fpr + WString(" - ") + WString(k.userID(0).name()));
        // Add the key fingerprint to the list of keys managed by the user.
        m_config->UpdateSecretKeyOwnership(fpr, true);
        m_popupCreate->hide();
#ifndef DEVTIME
        m_popupCreate->Reset();
#endif
        m_leSearch->setText(fpr);
        m_owner->Search();
    }
}

void KeyringIO::PrepareExport(const WString& fpr, bool isSecret)
{
    WLink link;
    shared_ptr<ExportKeyStreamResource> shResource =
            make_shared<ExportKeyStreamResource>
            (fpr, isSecret, "appliation/pgp-keys", m_tmwMessage);
    link.setResource(shResource);
    m_btnExport->setLink(link);
    if (isSecret)
        m_btnExport->hide();
    else
        m_btnExport->show();
}

ExportKeyStreamResource::ExportKeyStreamResource(const WString& fpr,
                                                 bool isSecret,
                                                 TransientMessageWidget * tmw)
: WStreamResource()
{
    m_fpr = fpr;
    m_isSecret = isSecret;
    m_tmwMessage = tmw;
}

ExportKeyStreamResource::ExportKeyStreamResource(const WString& fpr,
                                                 bool isSecret,
                                                 const string& mimeType,
                                                 TransientMessageWidget * tmw)
: WStreamResource(mimeType)
{
    m_fpr = fpr;
    m_isSecret = isSecret;
    m_tmwMessage = tmw;
}

ExportKeyStreamResource::~ExportKeyStreamResource()
{
    beingDeleted();
}

void ExportKeyStreamResource::handleRequest(const Http::Request& request,
                                            Http::Response& response)
{
    /*
     * Private keys cannot be exported with loopback pinentry.
     * Let's hope it gets better someday.
     */
    if (m_isSecret)
    {
        m_tmwMessage->SetText(TR("TTTExport"));
        return;
    }
    string buffer;
    if (!request.continuation()) // Needed for WStreamResource ?
    {
        Error e;
        GpgMEWorker gpgw;
        e = gpgw.ExportPublicKey(m_fpr.toUTF8().c_str(), buffer);
        if (e.code() != 0)
        {
            m_tmwMessage->SetText(e.asString());
            return;
        }
        suggestFileName(m_fpr + WString(".asc"), ContentDisposition::Attachment);
    }

    istrstream bufStream(buffer.c_str());
    handleRequestPiecewise(request, response, bufStream);
}
