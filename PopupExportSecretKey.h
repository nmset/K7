/* 
 * File:   PopupExportSecretKey.h
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License : GPL v2
 * Copyright Saleem Edah-Tally - © 2019
 *
 * Created on November 19, 2020, 9:01 PM
 */

#ifndef POPUPEXPORTSECRETKEY_H
#define POPUPEXPORTSECRETKEY_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include "TransientMessageWidget.h"

using namespace Wt;

class PopupExportSecretKey : public WPopupWidget
{
public:
    PopupExportSecretKey(WWidget * anchorWidget,
                         TransientMessageWidget * txtMessage,
                         const WLength& width = 300);
    virtual ~PopupExportSecretKey();
    void Create();
    /**
     * Used to forward the passphrase to the loopback passphrase provider.
     * @return 
     */
    const string GetPassphrase()
    {
        return m_lePassphrase->text().toUTF8();
    }
    /**
     * Caller binds its function here to add a link to the apply button.
     * @return 
     */
    WPushButton* GetPreApplyButton()
    {
        return m_btnPreApply;
    }
    /**
     * Expects a link with a shared resource that generates the secret key
     * and forwards it to the browser.
     * @return 
     */
    WPushButton* GetApplyButton()
    {
        return m_btnApply;
    }
private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WLineEdit * m_lePassphrase;
    WPushButton * m_btnPreApply;
    WPushButton * m_btnApply;
    WText * m_lblPassphrase;
};

#endif /* POPUPEXPORTSECRETKEY_H */

