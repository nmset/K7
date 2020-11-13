/* 
 * File:   PopupCreate.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on November 11, 2020, 10:20 PM
 */

#include "PopupCreate.h"
#include "global.h"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WText.h>

PopupCreate::PopupCreate(WWidget * anchorWidget,
                         TransientMessageWidget * txtMessage, const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_leName = NULL;
    m_leEmail = NULL;
    m_leComment = NULL;
    m_deExpiry = NULL;
    m_lePassphrase = NULL;
    m_leConfirm = NULL;
    m_cbDefaultAlgo = NULL;
    m_btnApply = NULL;
    m_arbitraryKeyAlgo = WString::Empty;
    m_arbitrarySubkeyAlgo = WString::Empty;
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupCreate::~PopupCreate()
{
}

void PopupCreate::Create()
{
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    m_cwMain->setStyleClass("popup");
    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));
    WGridLayout * grlOther = new WGridLayout();
    grlOther->setColumnStretch(1, 1);
    vblMain->addLayout(unique_ptr<WGridLayout> (grlOther));

    WText * lblName = new WText(TR("Name"));
    grlOther->addWidget(unique_ptr<WText> (lblName), 0, 0);
    m_leName = new WLineEdit();
    grlOther->addWidget(unique_ptr<WLineEdit> (m_leName), 0, 1);
    WText * lblEmail = new WText(TR("Email"));
    grlOther->addWidget(unique_ptr<WText> (lblEmail), 1, 0);
    m_leEmail = new WLineEdit();
    m_leEmail->setToolTip(TR("TTTEmailRecommended"));
    grlOther->addWidget(unique_ptr<WLineEdit> (m_leEmail), 1, 1);
    WText * lblComment = new WText(TR("Comment"));
    grlOther->addWidget(unique_ptr<WText> (lblComment), 2, 0);
    m_leComment = new WLineEdit();
    grlOther->addWidget(unique_ptr<WLineEdit> (m_leComment), 2, 1);
    WText * lblExpiry = new WText(TR("Expiration"));
    grlOther->addWidget(unique_ptr<WText> (lblExpiry), 3, 0);
    m_deExpiry = new WDateEdit();
    grlOther->addWidget(unique_ptr<WDateEdit> (m_deExpiry), 3, 1);
    m_cbDefaultAlgo = new WCheckBox(TR("DefaultAlgo"));
    grlOther->addWidget(unique_ptr<WCheckBox> (m_cbDefaultAlgo), 4, 1);

    /*
     * Key and subkey algorithms are grouped by category for clarity.
     * They are not constrained across categories, as kleopatra enforces.
     * Ex : an RSA subkey can be attached to a DSA secret key. Good? Bad?
     * There could have been only two comboboxes, one for key algorithm and one
     * for subkey algorithms.
     */
    WContainerWidget * cwAlgo = new WContainerWidget();
    cwAlgo->setToolTip(TR("TTTKeyAlgo"));
    WGridLayout * grlAlgo = new WGridLayout();
    cwAlgo->setLayout(unique_ptr<WGridLayout> (grlAlgo));
    grlAlgo->setColumnStretch(0, 1);
    grlAlgo->setColumnStretch(1, 1);
    vblMain->addWidget(unique_ptr<WContainerWidget> (cwAlgo));
    WText * lblKey = new WText(TR("Key"));
    grlAlgo->addWidget(unique_ptr<WText> (lblKey), 0, 0);
    WText * lblSubkey = new WText(TR("Subkey"));
    grlAlgo->addWidget(unique_ptr<WText> (lblSubkey), 0, 1);
    WComboBox * cmbKeyRSA = new WComboBox();
    cmbKeyRSA->setToolTip("RSA");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbKeyRSA), 1, 0);
    WComboBox * cmbSubkeyRSA = new WComboBox();
    cmbSubkeyRSA->setToolTip("RSA");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbSubkeyRSA), 1, 1);
    WComboBox * cmbKeyDSA = new WComboBox();
    cmbKeyDSA->setToolTip("DSA");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbKeyDSA), 2, 0);
    WComboBox * cmbSubkeyDSA = new WComboBox();
    cmbSubkeyDSA->setToolTip("Elgamal");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbSubkeyDSA), 2, 1);
    WComboBox * cmbKeyECDSA = new WComboBox();
    cmbKeyECDSA->setToolTip("ECDSA/EdDSA");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbKeyECDSA), 3, 0);
    WComboBox * cmbSubkeyECDSA = new WComboBox();
    cmbSubkeyECDSA->setToolTip("ECDH");
    grlAlgo->addWidget(unique_ptr<WComboBox> (cmbSubkeyECDSA), 3, 1);

    WGridLayout * grlPassphrase = new WGridLayout();
    grlPassphrase->setColumnStretch(1, 1);
    vblMain->addLayout(unique_ptr<WGridLayout> (grlPassphrase));
    WText * lblPassphrase = new WText(TR("Passphrase"));
    grlPassphrase->addWidget(unique_ptr<WText> (lblPassphrase), 0, 0);
    m_lePassphrase = new WLineEdit();
    m_lePassphrase->setEchoMode(EchoMode::Password);
    grlPassphrase->addWidget(unique_ptr<WLineEdit> (m_lePassphrase), 0, 1);
    WText * lblConfirm = new WText(TR("Confirm"));
    grlPassphrase->addWidget(unique_ptr<WText> (lblConfirm), 1, 0);
    m_leConfirm = new WLineEdit();
    m_leConfirm->setEchoMode(EchoMode::Password);
    grlPassphrase->addWidget(unique_ptr<WLineEdit> (m_leConfirm), 1, 1);

    WHBoxLayout * hblButtons = new WHBoxLayout();
    WPushButton * btnClose = new WPushButton(TR("Close"));
    hblButtons->addWidget(unique_ptr<WPushButton> (btnClose));
    m_btnApply = new WPushButton(TR("Apply"));
    hblButtons->addWidget(unique_ptr<WPushButton> (m_btnApply));
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblButtons));

    // All theses values come from kleopatra.
    cmbKeyRSA->addItem(WString::Empty);
    cmbKeyRSA->addItem("RSA2048");
    cmbKeyRSA->addItem("RSA3072");
    cmbKeyRSA->addItem("RSA4096");
    cmbSubkeyRSA->addItem(WString::Empty);
    cmbSubkeyRSA->addItem("RSA2048");
    cmbSubkeyRSA->addItem("RSA3072");
    cmbSubkeyRSA->addItem("RSA4096");
    cmbKeyDSA->addItem(WString::Empty);
    cmbKeyDSA->addItem("DSA2048");
    cmbSubkeyDSA->addItem(WString::Empty);
    cmbSubkeyDSA->addItem("ELG2048");
    cmbSubkeyDSA->addItem("ELG3072");
    cmbSubkeyDSA->addItem("ELG4096");
    cmbKeyECDSA->addItem(WString::Empty);
    cmbKeyECDSA->addItem("ed25519");
    cmbKeyECDSA->addItem("brainpoolP256r1");
    cmbKeyECDSA->addItem("brainpoolP384r1");
    cmbKeyECDSA->addItem("brainpoolP512r1");
    cmbKeyECDSA->addItem("NIST P-256");
    cmbKeyECDSA->addItem("NIST P-384");
    cmbKeyECDSA->addItem("NIST P-521");
    cmbSubkeyECDSA->addItem(WString::Empty);
    cmbSubkeyECDSA->addItem("cv25519");
    cmbSubkeyECDSA->addItem("brainpoolP256r1");
    cmbSubkeyECDSA->addItem("brainpoolP384r1");
    cmbSubkeyECDSA->addItem("brainpoolP512r1");
    cmbSubkeyECDSA->addItem("NIST P-256");
    cmbSubkeyECDSA->addItem("NIST P-384");
    cmbSubkeyECDSA->addItem("NIST P-521");

    // Group in lists for easy access by ::iterator.
    m_cmbKeyAlgo.push_back(cmbKeyRSA);
    m_cmbKeyAlgo.push_back(cmbKeyDSA);
    m_cmbKeyAlgo.push_back(cmbKeyECDSA);
    m_cmbSubkeyAlgo.push_back(cmbSubkeyRSA);
    m_cmbSubkeyAlgo.push_back(cmbSubkeyDSA);
    m_cmbSubkeyAlgo.push_back(cmbSubkeyECDSA);

    list<WComboBox*>::iterator it;
    for (it = m_cmbKeyAlgo.begin(); it != m_cmbKeyAlgo.end(); it++)
        (*it)->changed().connect(std::bind(&PopupCreate::OnComboKeyAlgoSelect, this, *it));
    for (it = m_cmbSubkeyAlgo.begin(); it != m_cmbSubkeyAlgo.end(); it++)
        (*it)->changed().connect(std::bind(&PopupCreate::OnComboSubkeyAlgoSelect, this, *it));

    m_cbDefaultAlgo->setChecked();
    cwAlgo->hide();
    m_cbDefaultAlgo->checked().connect(cwAlgo, &WContainerWidget::hide);
    m_cbDefaultAlgo->unChecked().connect(cwAlgo, &WContainerWidget::show);
    m_deExpiry->setDate(WDate::currentDate().addYears(2));
    btnClose->clicked().connect(this, &WPopupWidget::hide);

}

void PopupCreate::OnComboKeyAlgoSelect(WComboBox * cmb)
{
    list<WComboBox*>::iterator it;
    for (it = m_cmbKeyAlgo.begin(); it != m_cmbKeyAlgo.end(); it++)
    {
        if (*it != cmb)
            (*it)->setCurrentIndex(0);
        else
            m_arbitraryKeyAlgo = (*it)->currentText();
    }
}

void PopupCreate::OnComboSubkeyAlgoSelect(WComboBox * cmb)
{
    list<WComboBox*>::iterator it;
    for (it = m_cmbSubkeyAlgo.begin(); it != m_cmbSubkeyAlgo.end(); it++)
    {
        if (*it != cmb)
            (*it)->setCurrentIndex(0);
        else
            m_arbitrarySubkeyAlgo = (*it)->currentText();
    }
}

bool PopupCreate::Validate() const
{
    if (m_leEmail->text().empty())
    {
        m_tmwMessage->SetText(TR("ValidateEmailMissing"));
        return false;
    }
    if (m_lePassphrase->text().empty())
    {
        m_tmwMessage->SetText(TR("ValidatePassphraseMissing"));
        return false;
    }
    if (m_lePassphrase->text() != m_leConfirm->text())
    {
        m_tmwMessage->SetText(TR("ValidatePassphraseNoMatch"));
        return false;
    }
    if (!m_cbDefaultAlgo->isChecked() && m_arbitraryKeyAlgo.empty())
    {
        m_tmwMessage->SetText(TR("ValidateKeyAlgoMissing"));
        return false;
    }
    return true;
}

const ulong PopupCreate::GetExpiry() const
{
    return ((WDate::currentDate().daysTo(m_deExpiry->date())) * 24 * 3600);
}

void PopupCreate::Reset(bool completely)
{
    m_leName->setText((WString::Empty));
    m_leEmail->setText((WString::Empty));
    m_leComment->setText((WString::Empty));
    m_lePassphrase->setText(WString::Empty);
    m_leConfirm->setText(WString::Empty);
    if (completely)
    {
        m_deExpiry->setDate(WDate::currentDate().addYears(2));
        m_cbDefaultAlgo->setChecked();
        list<WComboBox*>::iterator it;
        for (it = m_cmbKeyAlgo.begin(); it != m_cmbKeyAlgo.end(); it++)
            (*it)->setCurrentIndex(0);
        for (it = m_cmbSubkeyAlgo.begin(); it != m_cmbSubkeyAlgo.end(); it++)
            (*it)->setCurrentIndex(0);
        m_arbitraryKeyAlgo = WString::Empty;
        m_arbitrarySubkeyAlgo = WString::Empty;
    }
}
