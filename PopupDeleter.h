/* 
 * File:   PopupDeleter.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 15 octobre 2019, 18:39
 */

#ifndef POPUPDELETER_H
#define POPUPDELETER_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCheckBox.h>
#include <Wt/WPushButton.h>
#include "TransientMessageWidget.h"

using namespace Wt;
/**
 * A transient popup window with a forward confirmation 
 * before deleting a key.
 */
class PopupDelete : public WPopupWidget {
public:
    PopupDelete(WWidget * anchorWidget, TransientMessageWidget * txtMessage, const WLength& width = 350);
    virtual ~PopupDelete();
    void Create();
    /**
     * Caller can bind the delete function to this button.
     * @return 
     */
    WPushButton* GetDeleteButton() {return m_btnDelete;}
private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WCheckBox * m_cbConfirm;
    WCheckBox * m_cbReConfirm;
    WPushButton * m_btnDelete;
    
    // Visibility management
    void Reset();
    void OnCbConfirm();
    void OnCbReConfirm();
};

#endif /* POPUPDELETER_H */

