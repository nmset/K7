/* 
 * File:   PopupCertifyUserId.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on October 30, 2020, 7:50 PM
 */

#include "PopupCertifyUserId.h"
#include "global.h"
#include "Tools.h"
#include <Wt/WStandardItem.h>
#include <Wt/WStandardItemModel.h>
#include <iostream>

using namespace std;

PopupCertifyUserId::PopupCertifyUserId(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                                       const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_cmbPrivKeys = NULL;
    m_cbOptionExportable = NULL;
    m_cbOptionNonRevocable = NULL;
    // m_cbOptionTrust = NULL;
    m_lblPassphrase = NULL;
    m_lePassphrase = NULL;
    m_btnApply = NULL;
    m_certifyOptions = 0;
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupCertifyUserId::~PopupCertifyUserId()
{
}

void PopupCertifyUserId::Create(vector<WString>& privateKeys,
                                const WString& fprKeyToSign)
{
    m_fprKeyToSign = fprKeyToSign;
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    m_cwMain->setStyleClass("popup");

    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));

    m_cmbPrivKeys = new WComboBox();
    vblMain->addWidget(unique_ptr<WComboBox> (m_cmbPrivKeys));
    FillPrivateKeyComboBox(privateKeys);
    /*
     * Column 0 : user identities in checkboxes
     * Column 1 : signing options in checkboxes
     */
    m_hblPreferences = new WHBoxLayout();
    vblMain->addLayout(unique_ptr<WHBoxLayout> (m_hblPreferences));
    // Column 0
    m_vblEmail = new WVBoxLayout();
    m_hblPreferences->addLayout(unique_ptr<WVBoxLayout> (m_vblEmail));
    PresentEmail();
    // Column 1
    WVBoxLayout * vblOptions = new WVBoxLayout();
    m_hblPreferences->addLayout(unique_ptr<WVBoxLayout> (vblOptions));
    WText * lblOptions = new WText(TR("Options"));
    vblOptions->addWidget(unique_ptr<WText> (lblOptions));
    m_cbOptionExportable = new WCheckBox(TR("ExportableCertification"));
    m_cbOptionExportable->setToolTip(TR("OneWayHint"));
    vblOptions->addWidget(unique_ptr<WCheckBox> (m_cbOptionExportable));
    m_cbOptionNonRevocable = new WCheckBox(TR("NonRevocableCertification"));
    m_cbOptionNonRevocable->setToolTip(TR("OneWayHint"));
    vblOptions->addWidget(unique_ptr<WCheckBox> (m_cbOptionNonRevocable));
    /*m_cbOptionTrust = new WCheckBox(TR("TrustCertification"));
    vblOptions->addWidget(unique_ptr<WCheckBox> (m_cbOptionTrust));*/

    WHBoxLayout * hblPassphrase = new WHBoxLayout();
    m_lblPassphrase = new WText(TR("Passphrase"));
    hblPassphrase->addWidget(unique_ptr<WText> (m_lblPassphrase), 0);
    m_lePassphrase = new WLineEdit();
    m_lePassphrase->setEchoMode(EchoMode::Password);
    hblPassphrase->addWidget(unique_ptr<WLineEdit> (m_lePassphrase), 1);
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblPassphrase));

    m_btnApply = new WPushButton(TR("Apply"));
    vblMain->addWidget(unique_ptr<WPushButton> (m_btnApply));

    m_cmbPrivKeys->changed().connect(std::bind(&PopupCertifyUserId::ShowPassphrase, this, true));
    m_cbOptionExportable->checked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionChecked, this, 1));
    m_cbOptionNonRevocable->checked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionChecked, this, 2));
    // m_cbOptionTrust->checked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionChecked, this, 4));
    m_cbOptionExportable->unChecked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionUnChecked, this, 1));
    m_cbOptionNonRevocable->unChecked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionUnChecked, this, 2));
    // m_cbOptionTrust->unChecked().connect(std::bind(&PopupCertifyUserId::OnCertifyOptionUnChecked, this, 4));

}

void PopupCertifyUserId::FillPrivateKeyComboBox(vector<WString>& privateKeys)
{
    if (m_cmbPrivKeys == NULL)
        return;
    vector<WString>::iterator it;
    GpgMEWorker gpgw;
    GpgME::Error e;
    shared_ptr<WStandardItemModel> iModel =
            make_shared<WStandardItemModel> (0, 2);
    for (it = privateKeys.begin(); it != privateKeys.end(); it++)
    {
        vector<GpgME::Key> lst = gpgw.FindKeys((*it).toUTF8().c_str(), true, e);
        if (e.code() != 0)
        {
            m_tmwMessage->SetText(e.asString());
            return;
        }
        /*
         * A fingerprint manually added in the config file may be missing in
         * the keyring.
         */
        if (lst.size() == 0)
            continue;
        // We are expecting one single key from a full fpr
        const GpgME::Key k = lst.at(0);
        if (!Tools::ConfigKeyIdMatchesKey(k, (*it)))
        {
            m_tmwMessage->SetText((*it) + TR("BadConfigKeyId"));
            return;
        }
        /* Limit to first email. name should be the same
         * for all UIDs of the key.
         */
        const WString displayed = WString(k.userID(0).name())
                + _SPACE_
                + _ANGLE_BRACKET_OPEN_ + k.userID(0).email()
                + _ANGLE_BRACKET_CLOSE_
                + _SPACE_
                + _BRACKET_OPEN_ + k.shortKeyID() + _BRACKET_CLOSE_;
        iModel->appendRow(cpp14::make_unique<WStandardItem> (*it));
        iModel->setItem(iModel->rowCount() - 1, 1, cpp14::make_unique<WStandardItem> (displayed));
    }
    m_cmbPrivKeys->setModel(iModel);
    m_cmbPrivKeys->setModelColumn(1);
}

void PopupCertifyUserId::PresentEmail()
{
    WText * lblEmail = new WText(TR("Email"));
    m_vblEmail->addWidget(unique_ptr<WText> (lblEmail));

    GpgMEWorker gpgw;
    GpgME::Error e;
    vector<GpgME::Key> lst = gpgw.FindKeys(m_fprKeyToSign.toUTF8().c_str(), false, e);
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    if (lst.size() != 1)
    {
        m_tmwMessage->SetText(m_fprKeyToSign + TR("BadKeyCount"));
        return;
    }
    const GpgME::Key k = lst.at(0);

    vector<GpgME::UserID> uids = k.userIDs();
    vector<GpgME::UserID>::iterator it;
    uint id = 0;
    for (it = uids.begin(); it != uids.end(); it++)
    {
        WCheckBox * cbEmail = new WCheckBox(it->email());
        m_vblEmail->addWidget(unique_ptr<WCheckBox> (cbEmail));
        cbEmail->setId(std::to_string(id));
        cbEmail->checked().connect(std::bind(&PopupCertifyUserId::OnEmailChecked, this, cbEmail));
        cbEmail->unChecked().connect(std::bind(&PopupCertifyUserId::OnEmailUnChecked, this, cbEmail));
        id++;
    }
}

WString PopupCertifyUserId::GetSelectedKey() const
{
    if (!m_cmbPrivKeys->count())
        return WString::Empty;
    shared_ptr<WAbstractItemModel> aiModel = m_cmbPrivKeys->model();
    WStandardItemModel * iModel = static_cast<WStandardItemModel*> (aiModel.get());
    if (!iModel) return WString::Empty;
    WStandardItem * item = iModel->item(m_cmbPrivKeys->currentIndex(), 0);
    if (!item) return WString::Empty;
    return item->text();
}

void PopupCertifyUserId::ShowPassphrase(bool show)
{
    if (show)
    {
        m_lblPassphrase->show();
        m_lePassphrase->show();
        /*
         * We cannot know the reason of a certify failure.
         * Empty the passphrase widget in any case.
         */
        m_lePassphrase->setText(WString::Empty);
    }
    else
    {
        m_lblPassphrase->hide();
        m_lePassphrase->hide();
        /*
         * The passphrase widget is not cleared.
         * gpg-agent will not fetch it from the loopback passphrase provider
         * as long as its caching timeout is not reached.
         * To better mimic the behavior of the default pinentry mechanism,
         * we should probably clear m_lePassphrase AND the passphrase stored
         * in the loopback passphrase provider.
         */
    }
}

void PopupCertifyUserId::OnEmailChecked(WCheckBox* cb)
{
    int id = Tools::ToInt(cb->id());
    m_uidsToSign.push_back(id);
}

void PopupCertifyUserId::OnEmailUnChecked(WCheckBox* cb)
{
    // Any tip to locate a known value in a vector without iterating over?
    const uint id = Tools::ToInt(cb->id());
    vector<uint>::iterator it;
    for (it = m_uidsToSign.begin(); it != m_uidsToSign.end(); it++)
    {
        if ((*it) == id)
        {
            m_uidsToSign.erase(it);
            return;
        }
    }
}

void PopupCertifyUserId::OnCertifyOptionChecked(int id)
{
    m_certifyOptions += id;
}

void PopupCertifyUserId::OnCertifyOptionUnChecked(int id)
{
    m_certifyOptions -= id;
}
