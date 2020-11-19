/* 
 * File:   PopupAddUid.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on November 16, 2020, 3:59 PM
 */

#include "PopupAddUid.h"
#include "global.h"
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WRadioButton.h>
#include <Wt/WRegExpValidator.h>

PopupAddUid::PopupAddUid(WWidget * anchorWidget,
                         TransientMessageWidget * txtMessage,
                         const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_leName = NULL;
    m_leEmail = NULL;
    m_leComment = NULL;
    m_lePassphrase = NULL;
    m_btnApply = NULL;

    m_nodeName = WString::Empty;
    m_nodeEmail = WString::Empty;
    m_nodeComment = WString::Empty;

    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupAddUid::~PopupAddUid()
{
}

void PopupAddUid::Create()
{
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    m_cwMain->setStyleClass("popup");
    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));
    WGridLayout * grlMain = new WGridLayout();
    grlMain->setColumnStretch(1, 1);
    vblMain->addLayout(unique_ptr<WGridLayout> (grlMain));

    WText * lblName = new WText(TR("Name"));
    grlMain->addWidget(unique_ptr<WText> (lblName), 0, 0);
    m_leName = new WLineEdit();
    grlMain->addWidget(unique_ptr<WLineEdit> (m_leName), 0, 1);
    WText * lblEmail = new WText(TR("Email"));
    grlMain->addWidget(unique_ptr<WText> (lblEmail), 1, 0);
    m_leEmail = new WLineEdit();
    grlMain->addWidget(unique_ptr<WLineEdit> (m_leEmail), 1, 1);
    WText * lblComment = new WText(TR("Comment"));
    grlMain->addWidget(unique_ptr<WText> (lblComment), 2, 0);
    m_leComment = new WLineEdit();
    grlMain->addWidget(unique_ptr<WLineEdit> (m_leComment), 2, 1);
    m_lblPassphrase = new WText(TR("Passphrase"));
    grlMain->addWidget(unique_ptr<WText> (m_lblPassphrase), 3, 0);
    m_lePassphrase = new WLineEdit();
    m_lePassphrase->setEchoMode(EchoMode::Password);
    grlMain->addWidget(unique_ptr<WLineEdit> (m_lePassphrase), 3, 1);

    WHBoxLayout * hblWhat = new WHBoxLayout();
    WRadioButton * rbAdd = new WRadioButton(WString(TR("AddUid")));
    hblWhat->addWidget(unique_ptr<WRadioButton> (rbAdd));
    WRadioButton * rbRevoke = new WRadioButton(WString(TR("RevokeUid")));
    hblWhat->addWidget(unique_ptr<WRadioButton> (rbRevoke));
    m_bgWhat = make_shared<WButtonGroup>();
    m_bgWhat->addButton(rbAdd, What::Add);
    m_bgWhat->addButton(rbRevoke, What::Revoke);
    m_bgWhat->setCheckedButton(rbAdd);
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblWhat));

    m_cbConfirm = new WCheckBox(TR("Confirm"));
    vblMain->addWidget(unique_ptr<WCheckBox> (m_cbConfirm));

    WHBoxLayout * hblButtons = new WHBoxLayout();
    WPushButton * btnClose = new WPushButton(TR("Close"));
    hblButtons->addWidget(unique_ptr<WPushButton> (btnClose));
    m_btnApply = new WPushButton(TR("Apply"));
    hblButtons->addWidget(unique_ptr<WPushButton> (m_btnApply));
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblButtons));

    m_bgWhat->checkedChanged().connect(this, &PopupAddUid::OnButtonGroupWhat);
    btnClose->clicked().connect(this, &WPopupWidget::hide);
    this->hidden().connect(m_cbConfirm, &WCheckBox::setUnChecked);

    // From WRegExpValidator docs
    shared_ptr<WRegExpValidator> validator
            = make_shared<WRegExpValidator>
            ("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    validator->setMandatory(true);
    m_leEmail->setValidator(validator);
}

void PopupAddUid::ShowPassphrase(bool show)
{
    // See comments in PopupCertifyUserId::ShowPassphrase
    if (show)
    {
        m_lblPassphrase->show();
        m_lePassphrase->show();
        m_lePassphrase->setText(WString::Empty);
    }
    else
    {
        m_lblPassphrase->hide();
        m_lePassphrase->hide();
    }
}

void PopupAddUid::OnButtonGroupWhat(WRadioButton * btn)
{
    m_leName->setDisabled(m_bgWhat->checkedId() == What::Revoke);
    m_leEmail->setDisabled(m_bgWhat->checkedId() == What::Revoke);
    m_leComment->setDisabled(m_bgWhat->checkedId() == What::Revoke);

    m_leName->setText(m_nodeName);
    if (m_bgWhat->checkedId() == What::Revoke)
    {
        m_leEmail->setText(m_nodeEmail);
        m_leComment->setText(m_nodeComment);
    }
    else
    {
        m_leEmail->setText(WString::Empty);
        m_leComment->setText(WString::Empty);
    }
    m_cbConfirm->setUnChecked();
}

void PopupAddUid::SetNodeIdentity(const WString& name, const WString& email,
                                  const WString& comment)
{
    m_nodeName = name;
    m_nodeEmail = email;
    m_nodeComment = comment;
    OnButtonGroupWhat(m_bgWhat->checkedButton());
}

bool PopupAddUid::Validate() const
{
    /*
     * It's pointless  to check if a passphrase is provided as the loopback
     * passphrase provider won't ever be called as long as gpg-agent holds a
     * valid passphrase from a previous transaction.
     */
    if (m_bgWhat->checkedId() == What::Revoke)
        return (m_cbConfirm->isChecked());
    return (m_cbConfirm->isChecked()
            && (m_leEmail->validate() == ValidationState::Valid));
}
