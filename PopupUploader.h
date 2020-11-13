/* 
 * File:   PopupUploader.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 13 octobre 2019, 18:48
 */

#ifndef POPUPUPLOADER_H
#define POPUPUPLOADER_H

#include <Wt/WPopupWidget.h>
#include <Wt/WFileUpload.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCheckBox.h>
#include <Wt/WPushButton.h>
#include <Wt/Signals/signals.hpp>
#include "TransientMessageWidget.h"

using namespace Wt;
/**
 * A transient popup window to upload a key
 * that will next be imported.
 * @param anchorWidget
 * @param txtMessage
 * @param width
 */
class PopupUpload : public WPopupWidget
{
public:
    PopupUpload(WWidget * anchorWidget, TransientMessageWidget * txtMessage, const WLength& width = 350);
    virtual ~PopupUpload();
    void Create();

    /**
     * Forward WFileUpload::uploaded() to caller 
     * @return 
     */
    Signal<WString>& UploadDone()
    {
        return m_sigUploadDone;
    }
private:
    TransientMessageWidget * m_tmwMessage;
    WFileUpload * m_upload;
    WContainerWidget * m_cwMain;
    WCheckBox * m_cbConfirm;
    WCheckBox * m_cbReConfirm;
    WPushButton * m_btnUpload;
    Signal<WString> m_sigUploadDone;

    void Reset();
    void OnCbConfirm();
    void OnCbReConfirm();
    void DoUpload();
    void OnUploadDone();
    void OnFileTooLarge();
    
};

#endif /* POPUPUPLOADER_H */

