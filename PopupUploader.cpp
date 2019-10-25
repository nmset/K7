/* 
 * File:   PopupUploader.cpp
 * Author: SET - nmset@netcourrier.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on 13 octobre 2019, 18:48
 */

#include "PopupUploader.h"
#include "global.h"
#include <Wt/WVBoxLayout.h>
#include <iostream>

using namespace std;

Uploader::Uploader(WWidget * anchorWidget, TransientMessageWidget * txtMessage, const WLength& width)
: WPopupWidget(cpp14::make_unique<WContainerWidget>()) {
    m_tmwMessage = txtMessage; m_upload = NULL; m_cwMain = NULL;
    m_cbConfirm = NULL; m_cbReConfirm = NULL; m_btnUpload = NULL;
    setTransient(true);
    setAnchorWidget(anchorWidget);
    setWidth(width);
}

Uploader::~Uploader() {
}

void Uploader::Create() {
    m_upload = new WFileUpload();
    m_upload->setFileTextSize(10240); // Is really approximate
    m_upload->setMultiple(false);
    m_cwMain = static_cast<WContainerWidget*> (implementation());
    WVBoxLayout * vblMain = new WVBoxLayout();
    m_cwMain->setLayout(unique_ptr<WVBoxLayout> (vblMain));
    vblMain->addWidget(unique_ptr<WFileUpload> (m_upload));
    m_cbConfirm = new WCheckBox(TR("Confirm"));
    vblMain->addWidget(unique_ptr<WCheckBox> (m_cbConfirm));
    m_cbReConfirm = new WCheckBox(TR("ReConfirm"));
    vblMain->addWidget(unique_ptr<WCheckBox> (m_cbReConfirm));
    m_btnUpload = new WPushButton(TR("Upload"));
    vblMain->addWidget(unique_ptr<WPushButton> (m_btnUpload));
    m_cbReConfirm->hide();
    m_cbConfirm->hide();
    m_btnUpload->hide();
    m_cbConfirm->changed().connect(this, &Uploader::OnCbConfirm);
    m_cbReConfirm->changed().connect(this, &Uploader::OnCbReConfirm);
    m_btnUpload->clicked().connect(this, &Uploader::DoUpload);
    m_upload->uploaded().connect(this, &Uploader::OnUploadDone);
    m_upload->fileTooLarge().connect(this, &Uploader::OnFileTooLarge);
    m_upload->changed().connect(this, &Uploader::Reset);
    this->hidden().connect(this, &Uploader::Reset);
}

void Uploader::Reset() {
    m_btnUpload->hide();
    m_cbReConfirm->setUnChecked();
    m_cbReConfirm->hide();
    m_cbConfirm->setUnChecked();
    m_cbConfirm->show();
    m_btnUpload->enable();
}
void Uploader::OnCbConfirm() {
    m_cbReConfirm->setHidden(m_cbConfirm->checkState() != CheckState::Checked);
    m_cbReConfirm->setUnChecked();
    m_btnUpload->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}

void Uploader::OnCbReConfirm() {
    m_btnUpload->setHidden(m_cbReConfirm->checkState() != CheckState::Checked);
}

void Uploader::DoUpload() {
    if (m_upload->canUpload()) {
        m_btnUpload->disable();
        m_upload->upload();
    } else {
        m_tmwMessage->SetText(TR("CantUpload"));
    }
}

void Uploader::OnUploadDone() {
    m_sigUploadDone.emit(m_upload->spoolFileName());
    m_btnUpload->enable();
}

void Uploader::OnFileTooLarge() {
    m_tmwMessage->SetText(TR("FileTooLarge"));
    m_btnUpload->enable();
}


