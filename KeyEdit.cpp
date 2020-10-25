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
{
    m_owner = owner;
}

KeyEdit::~KeyEdit()
{
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
