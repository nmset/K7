/* 
 * File:   PopupExportSecretKey.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 * 
 * Created on November 19, 2020, 9:01 PM
 */

#include "PopupExportSecretKey.h"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include "global.h"

PopupExportSecretKey::PopupExportSecretKey(WWidget * anchorWidget,
                                           TransientMessageWidget * txtMessage,
                                           const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_lblPassphrase = NULL;
    m_lePassphrase = NULL;
    m_btnPreApply = NULL;
    m_btnApply = NULL;
    
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupExportSecretKey::~PopupExportSecretKey()
{
}

void PopupExportSecretKey::Create()
{
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    m_cwMain->setStyleClass("popup");

    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));
    WHBoxLayout * hblPassphrase = new WHBoxLayout();
    m_lblPassphrase = new WText(TR("Passphrase"));
    hblPassphrase->addWidget(unique_ptr<WText> (m_lblPassphrase));
    m_lePassphrase = new WLineEdit();
    m_lePassphrase->setEchoMode(EchoMode::Password);
    hblPassphrase->addWidget(unique_ptr<WLineEdit> (m_lePassphrase));
    vblMain->addLayout(unique_ptr<WHBoxLayout> (hblPassphrase));
    
    m_btnPreApply = new WPushButton(TR("PreExportSecretKey"));
    vblMain->addWidget(unique_ptr<WPushButton> (m_btnPreApply));
    m_btnApply = new WPushButton(TR("ExportSecretKey"));
    m_btnApply->setToolTip(TR("TTTExportSecretKey"));
    vblMain->addWidget(unique_ptr<WPushButton> (m_btnApply));
    
    // With keyPressed(), backspace is ignored.
    m_lePassphrase->keyWentDown().connect(m_btnApply, &WPushButton::disable);
}
