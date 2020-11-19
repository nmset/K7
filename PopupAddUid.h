/* 
 * File:   PopupAddUid.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 16, 2020, 3:59 PM
 */

#ifndef POPUPADDUID_H
#define POPUPADDUID_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include "TransientMessageWidget.h"

using namespace Wt;

class PopupAddUid : public WPopupWidget
{
public:

    enum What
    {
        Revoke = 0, Add
    };
    PopupAddUid(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                const WLength& width = 400);
    virtual ~PopupAddUid();
    void Create();

    const WString GetName() const
    {
        return m_leName->text();
    }

    /**
     * GPGME enforces a rightly formatted email address here.
     * @return 
     */
    const WString GetEmail() const
    {
        return m_leEmail->text();
    }

    const WString GetComment() const
    {
        return m_leComment->text();
    }
    /**
     * Add or revoke uid.
     * @return 
     */
    const What WhatToDo() const
    {
        return (What) m_bgWhat->checkedId();
    }
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
     * Caller binds its function here.
     * @return 
     */
    WPushButton* GetApplyButton()
    {
        return m_btnApply;
    }
    /**
     * Identity values from the tree table node. Should not be modified here
     * once assigned. Only KeyEdit::OnUidEmailClicked should do that.
     * @param name
     * @param email
     * @param comment
     */
    void SetNodeIdentity(const WString& name, const WString& email,
                         const WString& comment);
    /**
     * Confirmation is mandatory.
     * \n If adding a uid, a rightly formatted email address is required.
     * @return 
     */
    bool Validate() const;

private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WLineEdit * m_leName;
    WLineEdit * m_leEmail;
    WLineEdit * m_leComment;
    WText * m_lblPassphrase;
    WLineEdit * m_lePassphrase;
    shared_ptr<WButtonGroup> m_bgWhat;
    WCheckBox * m_cbConfirm;
    WPushButton * m_btnApply;

    // Identity values from the tree table node.
    WString m_nodeName, m_nodeEmail, m_nodeComment;
    
    void OnButtonGroupWhat(WRadioButton * btn);
};

#endif /* POPUPADDUID_H */

