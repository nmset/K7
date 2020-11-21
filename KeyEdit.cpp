/* 
 * File:   KeyEdit.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on October 25, 2020, 10:38 AM
 */

#include "KeyEdit.h"
#include <Wt/WText.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include "GpgMEWorker.h"
#include "Tools.h"

using namespace std;

KeyEdit::KeyEdit(K7Main * owner)
: WObject()
{
    m_owner = owner;
    m_popupUid = NULL;
    m_popupExpiryTime = NULL;
    m_popupAddUid = NULL;
    m_targetUidValidityKeyFpr = WString::Empty;
    m_expiryEditedKeyFpr = WString::Empty;
}

KeyEdit::~KeyEdit()
{
    delete m_popupUid;
}

void KeyEdit::OnOwnerTrustDoubleClicked(WTreeTableNode * keyNode, bool keyHasSecret)
{
    /*
     * A private key that a user does not manage will have its public part
     * listed in the public WTreeTableNode. The certification trust level must
     * not be editable by anyone.
     */
    WText * lblFpr = static_cast<WText*> (keyNode->columnWidget(3));
    vector<WString> ourKeys = m_owner->m_config->PrivateKeyIds();
    if (!Tools::IsOurKey(lblFpr->text(), ourKeys)
            && Tools::KeyHasSecret(lblFpr->text()))
    {
        m_owner->m_tmwMessage->SetText(TR("OwnerTrustReadOnly"));
        return;
    }
    /*
     * We leave a primary key with ultimate trust level for further change.
     * kleopatra does not do that.
     */
    WComboBox * cmbOwnerTrust = new WComboBox();
    FillOwnerTrustCombo(cmbOwnerTrust, keyHasSecret);
    cmbOwnerTrust->blurred().connect(std::bind(&KeyEdit::OnOwnerTrustBlurred, this, keyNode, keyHasSecret));
    WText * lblOwnerTrust = static_cast<WText*> (keyNode->columnWidget(2));
    cmbOwnerTrust->setCurrentIndex(cmbOwnerTrust->findText(lblOwnerTrust->text()));
    /*
     * Prepare to check for change in combobox item.
     * Change is detected by index, not value.
     */
    cmbOwnerTrust->setAttributeValue("previousTrustLevel", std::to_string(cmbOwnerTrust->currentIndex()));
    keyNode->setColumnWidget(2, unique_ptr<WComboBox> (cmbOwnerTrust));
    // +++
    cmbOwnerTrust->setFocus();
}

void KeyEdit::OnOwnerTrustBlurred(WTreeTableNode* keyNode, bool keyHasSecret)
{
    // Get new level (not index)
    WComboBox * cmbOwnerTrust = static_cast<WComboBox*> (keyNode->columnWidget(2));
    shared_ptr<WAbstractItemModel> aiModel = cmbOwnerTrust->model();
    WStandardItemModel * iModel = static_cast<WStandardItemModel*> (aiModel.get());
    WStandardItem * item = iModel->item(cmbOwnerTrust->currentIndex(), 0);
    int newLevel = Tools::ToInt(item->text().toUTF8());

    WText * lblOwnerTrust = new WText(cmbOwnerTrust->currentText());
    lblOwnerTrust->doubleClicked().connect(std::bind(&KeyEdit::OnOwnerTrustDoubleClicked, this, keyNode, keyHasSecret));
    const WText * lblFpr = static_cast<WText*> (keyNode->columnWidget(3));
    const uint newTrustLevel = cmbOwnerTrust->currentIndex();
    const WString previousTrustLevel = cmbOwnerTrust->attributeValue("previousTrustLevel");
    keyNode->setColumnWidget(2, unique_ptr<WText> (lblOwnerTrust));
    // If nothing was changed, don't go to engine.
    if (WString(std::to_string(newTrustLevel)) == previousTrustLevel)
        return;

    GpgMEWorker gpgWorker;
    GpgME::Error e = gpgWorker.EditOwnerTrust(lblFpr->text().toUTF8().c_str(), (GpgME::Key::OwnerTrust) newLevel);
    if (e.code() != 0)
    {
        lblOwnerTrust->setText(previousTrustLevel);
        m_owner->m_tmwMessage->SetText(TR("OwnerTrustFailure"));
        return;
    }
    m_owner->m_tmwMessage->SetText(TR("OwnerTrustSuccess"));
}

void KeyEdit::FillOwnerTrustCombo(WComboBox * cmb, bool keyHasSecret)
{
    shared_ptr<WStandardItemModel> siModel = make_shared<WStandardItemModel> ();
    OwnerTrustMap OwnerTrustLevel = m_owner->OwnerTrustLevel;
    vector<unique_ptr < WStandardItem>> colIndex;
    vector<unique_ptr < WStandardItem>> colText;
    colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string(UserID::Validity::Unknown)));
    colText.push_back(cpp14::make_unique<WStandardItem> (TR("UidUnknown")));
    if (keyHasSecret)
    {
        colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string(UserID::Validity::Ultimate)));
        colText.push_back(cpp14::make_unique<WStandardItem> (TR("UidUltimate")));
    }
    else
    {
        colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string(UserID::Validity::Never)));
        colText.push_back(cpp14::make_unique<WStandardItem> (TR("UidNever")));
        colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string(UserID::Validity::Marginal)));
        colText.push_back(cpp14::make_unique<WStandardItem> (TR("UidMarginal")));
        colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string(UserID::Validity::Full)));
        colText.push_back(cpp14::make_unique<WStandardItem> (TR("UidFull")));
    }
    siModel->appendColumn(std::move(colIndex));
    siModel->appendColumn(std::move(colText));
    cmb->clear();
    cmb->setModel(siModel);
    cmb->setModelColumn(1);
}

void KeyEdit::OnUidValidityClicked(WTreeTableNode* uidNode, vector<WString>& privateKeys, const WString& targetKeyFpr)
{
    if (targetKeyFpr != m_targetUidValidityKeyFpr)
    {
        bool passwordVisibility = true;
        if (m_popupUid)
            passwordVisibility = m_popupUid->IsPasswordVisible();
        delete m_popupUid;
        WText * lblUidValidity = static_cast<WText*> (uidNode->columnWidget(2));
        m_popupUid = new PopupCertifyUserId(lblUidValidity, m_owner->m_tmwMessage);
        m_popupUid->Create(privateKeys, targetKeyFpr);
        m_popupUid->ShowPassphrase(passwordVisibility);
        m_targetUidValidityKeyFpr = targetKeyFpr;
        m_popupUid->GetCertifyButton()->clicked().connect(this, &KeyEdit::CertifyKey);
    }
    m_popupUid->show();
}

void KeyEdit::CertifyKey()
{
    vector<uint>& uidsToSign = m_popupUid->GetUidsToSign();
    if (uidsToSign.size() == 0)
    {
        m_owner->m_tmwMessage->SetText(TR("NoUidSelected"));
        return;
    }
    const WString signingKey = m_popupUid->GetSelectedKey();
    const WString keyToSign = m_popupUid->GetKeyToSign();
    int options = m_popupUid->GetCertifyOptions();
    GpgMEWorker gpgWorker;
    GpgME::Error e = gpgWorker.CertifyKey(signingKey.toUTF8().c_str(),
                                          keyToSign.toUTF8().c_str(),
                                          uidsToSign, options,
                                          m_popupUid->GetPassphrase());
    if (e.code() != 0)
    {
        m_owner->m_tmwMessage->SetText(TR("CertificationFailure"));
        m_popupUid->ShowPassphrase(true);
        return;
    }
    m_owner->m_tmwMessage->SetText(TR("CertificationSuccess"));
    m_popupUid->ShowPassphrase(false);
    m_owner->DisplayUids(keyToSign);
}

void KeyEdit::OnExpiryClicked(WTreeTableNode* subkeyNode, const WString& keyFpr,
                              const WString& subkeyFpr)
{
    if (keyFpr != m_expiryEditedKeyFpr)
    {
        delete m_popupExpiryTime;
        WText * lblExpiry = static_cast<WText*> (subkeyNode->columnWidget(2));
        m_popupExpiryTime = new PopupExpiryTime(lblExpiry, m_owner->m_tmwMessage);
        m_popupExpiryTime->Create(keyFpr);
        m_expiryEditedKeyFpr = keyFpr;
        m_popupExpiryTime->GetApplyButton()->clicked().connect(this, &KeyEdit::SetExpiryTime);
    }
    m_popupExpiryTime->SetSubkeyFpr(subkeyFpr);
    m_popupExpiryTime->show();
}

void KeyEdit::SetExpiryTime()
{
    GpgMEWorker gpgWorker;
    GpgME::Error e;
    const WString keyFpr = m_popupExpiryTime->GetKeyFpr();
    const WString subkeyFpr = m_popupExpiryTime->GetSubkeyFpr();
    if (keyFpr == subkeyFpr)
    {
        e = gpgWorker.SetExpiryTime(keyFpr.toUTF8().c_str(),
                                    m_popupExpiryTime->GetPassphrase(),
                                    m_popupExpiryTime->GetExpiryTime());
    }
    else
    {
        e = gpgWorker.SetSubkeyExpiryTime(keyFpr.toUTF8().c_str(),
                                          subkeyFpr.toUTF8().c_str(),
                                          m_popupExpiryTime->GetPassphrase(),
                                          m_popupExpiryTime->GetExpiry());
    }
    if (e.code() != 0)
    {
        m_owner->m_tmwMessage->SetText(TR("SetExpirationTimeFailure"));
        m_popupExpiryTime->ShowPassphrase(true);
        return;
    }
    m_owner->m_tmwMessage->SetText(TR("SetExpirationTimeSuccess"));
    m_popupExpiryTime->ShowPassphrase(false);
    m_owner->DisplaySubKeys(m_expiryEditedKeyFpr, true);
}

void KeyEdit::OnUidEmailClicked(WTreeTableNode* uidNode, const WString& keyFpr)
{
    WText * lblEmail = static_cast<WText*> (uidNode->columnWidget(1));
    if (keyFpr != m_addUidKeyFpr)
    {
        delete m_popupAddUid;
        m_popupAddUid = new PopupAddUid(lblEmail, m_owner->m_tmwMessage);
        m_popupAddUid->Create();
        m_addUidKeyFpr = keyFpr;
        m_popupAddUid->GetApplyButton()->clicked().connect(this, &KeyEdit::AddOrRevokeUid);
    }
    WText * lblName = uidNode->label();
    WText * lblComment = static_cast<WText*> (uidNode->columnWidget(3));
    m_popupAddUid->SetNodeIdentity(lblName->text(), lblEmail->text(),
                                   lblComment->text());
    m_popupAddUid->show();
}

void KeyEdit::AddOrRevokeUid()
{
    if (!m_popupAddUid->Validate())
    {
        m_owner->m_tmwMessage->SetText(TR("InvalidInput"));
        return;
    }
    const WString name = m_popupAddUid->GetName();
    const WString email = m_popupAddUid->GetEmail();
    const WString comment = m_popupAddUid->GetComment();
    const WString passphrase = m_popupAddUid->GetPassphrase();

    Error e;
    GpgMEWorker gpgw;
    if (m_popupAddUid->WhatToDo() == PopupAddUid::What::Revoke)
    {
        e = gpgw.RevokeUserID(m_addUidKeyFpr.toUTF8().c_str(),
                              passphrase.toUTF8(),
                              name.toUTF8(), email.toUTF8(), comment.toUTF8());
    }
    else
    {
        e = gpgw.AddUserID(m_addUidKeyFpr.toUTF8().c_str(),
                           passphrase.toUTF8(),
                           name.toUTF8(), email.toUTF8(), comment.toUTF8());
    }
    m_popupAddUid->hide();
    if (e.code() != 0)
    {
        m_popupAddUid->ShowPassphrase(true);
        m_owner->m_tmwMessage->SetText(e.asString());
    }
    else
    {
        m_popupAddUid->ShowPassphrase(false);
    }
    // Key certifications are not listed on this refresh !
    m_owner->DisplayUids(m_addUidKeyFpr, true);
}
