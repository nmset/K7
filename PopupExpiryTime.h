/* 
 * File:   PopupExpiryTime.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 7, 2020, 6:05 PM
 */

#ifndef POPUPEXPIRYTIME_H
#define POPUPEXPIRYTIME_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WDateEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include "TransientMessageWidget.h"

using namespace Wt;

class PopupExpiryTime : public WPopupWidget
{
public:
    PopupExpiryTime(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                       const WLength& width = 300);
    virtual ~PopupExpiryTime();
    void Create(const WString& keyFpr);
    /**
     * Controls visibility of passphrase widgets.
     * \n Need not be always visible as the passphrase is cached by gpg-agent.
     * \n During that caching period, a wrong input passphrase will not be looked
     * for by GPG engine, and may be confusing.
     * @param show
     */
    void ShowPassphrase(bool show = true);
    /**
     * Used to forward the passphrase to the loopback passphrase provider.
     * @return 
     */
    const string GetPassphrase()
    {
        return m_lePassphrase->text().toUTF8();
    }
    /**
     * Returns the new expiry date.
     */
    const std::string GetExpiryTime() const;
    /**
     * Caller binds its function here.
     * @return 
     */
    WPushButton* GetApplyButton()
    {
        return m_btnApply;
    }
    
private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WDateEdit * m_deExpiry;
    WLineEdit * m_lePassphrase;
    WPushButton * m_btnApply;
    WText * m_lblPassphrase;
    WString m_keyFpr;

};

#endif /* POPUPEXPIRYTIME_H */

