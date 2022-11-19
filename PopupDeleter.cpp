/* 
 * File:   PopupDeleter.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 * 
 * Created on 15 octobre 2019, 18:39
 */

#include "PopupDeleter.h"
#include "global.h"
#include <Wt/WVBoxLayout.h>
#include <iostream>

using namespace std;

PopupDelete::PopupDelete(WWidget * anchorWidget, TransientMessageWidget * txtMessage, const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>())
{
    m_tmwMessage = txtMessage;
    m_cwMain = NULL;
    m_cbConfirm = NULL;
    m_cbReConfirm = NULL;
    m_btnDelete = NULL;
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

PopupDelete::~PopupDelete()
{
}

void PopupDelete::Create()
{
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
    m_cbConfirm->changed().connect(this, &PopupDelete::OnCbConfirm);
    m_cbReConfirm->changed().connect(this, &PopupDelete::OnCbReConfirm);
    this->hidden().connect(this, &PopupDelete::Reset);
}

void PopupDelete::Reset()
{
    m_btnDelete->hide();
    m_cbReConfirm->setUnChecked();
    m_cbReConfirm->hide();
    m_cbConfirm->setUnChecked();
    m_cbConfirm->show();
}

void PopupDelete::OnCbConfirm()
{
    m_cbReConfirm->setHidden(m_cbConfirm->checkState() != CheckState::Checked);
    m_cbReConfirm->setUnChecked();
    m_btnDelete->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}

void PopupDelete::OnCbReConfirm()
{
    m_btnDelete->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}
