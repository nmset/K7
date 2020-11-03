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

using namespace std;

KeyEdit::KeyEdit(K7Main * owner)
:WObject()
{
    m_owner = owner;
    m_popupUid = NULL;
    m_targetKeyFpr = WString::Empty;
}

KeyEdit::~KeyEdit()
{
    delete m_popupUid;
}

void KeyEdit::OnOwnerTrustDoubleClicked(WTreeTableNode * keyNode)
{
    // We ought to trust our own keys
    const WText * lblFpr = static_cast<WText*> (keyNode->columnWidget(3));
    if (IsOurKey(lblFpr->text()))
        return;
    WComboBox * cmbOwnerTrust = new WComboBox();
    FillOwnerTrustCombo(cmbOwnerTrust);
    cmbOwnerTrust->blurred().connect(std::bind(&KeyEdit::OnOwnerTrustBlurred, this, keyNode));
    WText * lblOwnerTrust = static_cast<WText*> (keyNode->columnWidget(2));
    cmbOwnerTrust->setCurrentIndex(cmbOwnerTrust->findText(lblOwnerTrust->text()));
    // If nothing gets changed, don't go to engine.
    cmbOwnerTrust->setAttributeValue("previousTrustLevel", std::to_string(cmbOwnerTrust->currentIndex()));
    keyNode->setColumnWidget(2, unique_ptr<WComboBox> (cmbOwnerTrust));
    // +++
    cmbOwnerTrust->setFocus();
}

void KeyEdit::OnOwnerTrustBlurred(WTreeTableNode* keyNode)
{
    WComboBox * cmbOwnerTrust = static_cast<WComboBox*> (keyNode->columnWidget(2));
    WText * lblOwnerTrust = new WText(cmbOwnerTrust->currentText());
    lblOwnerTrust->doubleClicked().connect(std::bind(&KeyEdit::OnOwnerTrustDoubleClicked, this, keyNode));
    const WText * lblFpr = static_cast<WText*> (keyNode->columnWidget(3));
    const uint newTrustLevel = cmbOwnerTrust->currentIndex();
    const WString previousTrustLevel = cmbOwnerTrust->attributeValue("previousTrustLevel");
    keyNode->setColumnWidget(2, unique_ptr<WText> (lblOwnerTrust));
    // If nothing was changed, don't go to engine.
    if (WString(std::to_string(newTrustLevel)) == previousTrustLevel)
        return;

    GpgMEWorker gpgWorker;
    GpgME::Error e = gpgWorker.EditOwnerTrust(lblFpr->text().toUTF8().c_str(), (GpgME::Key::OwnerTrust) newTrustLevel);
    if (e.code() != 0)
    {
        lblOwnerTrust->setText(previousTrustLevel);
        m_owner->m_tmwMessage->SetText(TR("OwnerTrustFailure"));
        return;
    }
    m_owner->m_tmwMessage->SetText(TR("OwnerTrustSuccess"));
}

void KeyEdit::FillOwnerTrustCombo(WComboBox * cmb)
{
    /*
     * We should perhaps exclude OwnerTrust::Ultimate.
     * kleopatra doesn't do that.
     */
    shared_ptr<WStandardItemModel> siModel = make_shared<WStandardItemModel> ();
    OwnerTrustMap OwnerTrustLevel = m_owner->OwnerTrustLevel;
    vector<unique_ptr < WStandardItem>> colIndex;
    vector<unique_ptr < WStandardItem>> colText;
    OwnerTrustMap::iterator it;
    for (it = OwnerTrustLevel.begin(); it != OwnerTrustLevel.end(); it++)
    {
        colIndex.push_back(cpp14::make_unique<WStandardItem> (std::to_string((*it).first)));
        colText.push_back(cpp14::make_unique<WStandardItem> ((*it).second));
    }
    siModel->appendColumn(std::move(colIndex));
    siModel->appendColumn(std::move(colText));
    cmb->clear();
    cmb->setModel(siModel);
    cmb->setModelColumn(1);
}

bool KeyEdit::IsOurKey(const WString& fpr)
{
    vector<WString> ourKeys = m_owner->m_config->PrivateKeyIds();
    vector<WString> ::iterator it;
    for (it = ourKeys.begin(); it != ourKeys.end(); it++)
    {
        if (*it == fpr)
            return true;
    }
    return false;
}

void KeyEdit::OnUidValidityClicked(WTreeTableNode* uidNode, vector<WString>& privateKeys, const WString& targetKeyFpr)
{
    if (targetKeyFpr != m_targetKeyFpr) {
        bool passwordVisibility = true;
        if (m_popupUid)
            passwordVisibility = m_popupUid->IsPasswordVisible();
        delete m_popupUid;
        WText * lblUidValidity = static_cast<WText*> (uidNode->columnWidget(2));
        m_popupUid = new PopupCertifyUserId(lblUidValidity, m_owner->m_tmwMessage);
        m_popupUid->Create(privateKeys, targetKeyFpr);
        m_popupUid->ShowPassphrase(passwordVisibility);
        m_targetKeyFpr = targetKeyFpr;
        m_popupUid->GetCertifyButton()->clicked().connect(this, &KeyEdit::CertifyKey);
    }
    m_popupUid->show();
}

void KeyEdit::CertifyKey()
{
    vector<uint>& uidsToSign = m_popupUid->GetUidsToSign();
    if (uidsToSign.size() == 0) {
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
