/* 
 * File:   PopupExpiryTime.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 * 
 * Created on November 7, 2020, 6:05 PM
 */

#include "PopupExpiryTime.h"
#include "global.h"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>

using namespace std;

PopupExpiryTime::PopupExpiryTime(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                                 const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_keyFpr = WString::Empty;
    m_subkeyFpr = WString::Empty;
    /*
     * Trade-off.
     * When the calendar of WDateEdit is clicked, this popup gets hidden,
     * forcing to show it again.
     * If setTransient is false, this popup will obviously persist, until
     * explicitly hidden with the Close button. Not satisfactory to change key
     * selection while it is visible, nor to do other tasks like changing
     * certification trust level, certifying uids.... whatever.
     * Forcing its persistence while playing with many events has not been
     * successful, and quite ugly.
     */
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupExpiryTime::~PopupExpiryTime()
{
}

void PopupExpiryTime::Create(const WString& keyFpr)
{
    m_keyFpr = keyFpr;
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    m_cwMain->setStyleClass("popup");

    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));

    WHBoxLayout * hblExpiry = new WHBoxLayout();
    WText * lblExpiry = new WText(TR("ExpiryDate"));
    hblExpiry->addWidget(unique_ptr<WText> (lblExpiry));
    m_deExpiry = new WDateEdit();
    hblExpiry->addWidget(unique_ptr<WDateEdit> (m_deExpiry), 1);
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblExpiry));

    WHBoxLayout * hblPassphrase = new WHBoxLayout();
    m_lblPassphrase = new WText(TR("Passphrase"));
    hblPassphrase->addWidget(unique_ptr<WText> (m_lblPassphrase));
    m_lePassphrase = new WLineEdit();
    m_lePassphrase->setEchoMode(EchoMode::Password);
    hblPassphrase->addWidget(unique_ptr<WLineEdit> (m_lePassphrase), 1);
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblPassphrase));

    WHBoxLayout * hblButtons = new WHBoxLayout();
    WPushButton * btnClose = new WPushButton(TR("Close"));
    btnClose->clicked().connect(this, &WPopupWidget::hide);
    hblButtons->addWidget(unique_ptr<WPushButton> (btnClose));
    m_btnApply = new WPushButton(TR("Apply"));
    hblButtons->addWidget(unique_ptr<WPushButton> (m_btnApply));
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblButtons));
}

const string PopupExpiryTime::GetExpiryTime() const
{
    if (m_deExpiry->text().empty())
        return "0";
    return m_deExpiry->text().toUTF8();
}

const ulong PopupExpiryTime::GetExpiry() const
{
    return ((WDate::currentDate().daysTo(m_deExpiry->date())) * 24 * 3600);
}

void PopupExpiryTime::ShowPassphrase(bool show)
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
