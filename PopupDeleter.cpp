/* 
 * File:   PopupDeleter.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on 15 octobre 2019, 18:39
 */

#include "PopupDeleter.h"
#include "global.h"
#include <Wt/WVBoxLayout.h>
#include <iostream>

using namespace std;

Deleter::Deleter(WWidget * anchorWidget, TransientMessageWidget * txtMessage, const WLength& width) 
: WPopupWidget(cpp14::make_unique<WContainerWidget>()) {
    m_tmwMessage = txtMessage; m_cwMain = NULL; m_cbConfirm = NULL;
    m_cbReConfirm = NULL; m_btnDelete = NULL;
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

Deleter::~Deleter() {
}

void Deleter::Create() {
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    // White in css file, like default background color.
    m_cwMain->setStyleClass("popup");
    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));
    m_cbConfirm = new WCheckBox(TR("Confirm"));
    vblMain->addWidget(unique_ptr<WCheckBox> (m_cbConfirm));
    m_cbReConfirm = new WCheckBox(TR("ReConfirm"));
    vblMain->addWidget(unique_ptr<WCheckBox> (m_cbReConfirm));
    m_btnDelete = new WPushButton(TR("Delete"));
    vblMain->addWidget(unique_ptr<WPushButton> (m_btnDelete));
    m_cbReConfirm->hide();
    m_btnDelete->hide();
    m_cbConfirm->changed().connect(this, &Deleter::OnCbConfirm);
    m_cbReConfirm->changed().connect(this, &Deleter::OnCbReConfirm);
    this->hidden().connect(this, &Deleter::Reset);
}

void Deleter::Reset() {
    m_btnDelete->hide();
    m_cbReConfirm->setUnChecked();
    m_cbReConfirm->hide();
    m_cbConfirm->setUnChecked();
    m_cbConfirm->show();
}

void Deleter::OnCbConfirm() {
    m_cbReConfirm->setHidden(m_cbConfirm->checkState() != CheckState::Checked);
    m_cbReConfirm->setUnChecked();
    m_btnDelete->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}

void Deleter::OnCbReConfirm() {
    m_btnDelete->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}
