/* 
 * File:   PopupCreate.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 11, 2020, 10:20 PM
 */

#ifndef POPUPCREATE_H
#define POPUPCREATE_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include "TransientMessageWidget.h"

using namespace Wt;

/**
 * A popup with all parameters to create keys. A secret key must be created, a
 * subkey is optional (GPGME allows this). Key algorithm identifiers are poked
 * from kleopatra.
 * @param anchorWidget
 * @param txtMessage
 * @param width
 */
class PopupCreate : public WPopupWidget
{
public:
    PopupCreate(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                const WLength& width = 400);
    virtual ~PopupCreate();
    void Create();

    const WString GetName() const
    {
        return m_leName->text();
    }

    /**
     * GPGME accepts anything here, it may not be an email address.
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
     * Number of seconds from now. Default is 2 years.
     * \n NB : with default engine algorithms, GPGME does not set an expiry time
     * for the subkey.
     * @return 
     */
    const ulong GetExpiry() const;

    bool UseDefaultEngineAlgorithms() const
    {
        return m_cbDefaultAlgo->isChecked();
    }

    /**
     * Selected by the user.
     * @return 
     */
    const WString GetArbitraryKeyAlgo() const
    {
        return m_arbitraryKeyAlgo;
    }

    /**
     * Selected by the user.
     * @return 
     */
    const WString GetArbitrarySubkeyAlgo() const
    {
        return m_arbitrarySubkeyAlgo;
    }

    const WString GetPassphrase() const
    {
        return m_lePassphrase->text();
    }

    /**
     * For caller to bind its key creation function.
     * @return 
     */
    WPushButton * GetApplyButton() const
    {
        return m_btnApply;
    }
    /**
     * There must be
     * <ul>
     * <li>an email address</li>
     * <li>a passphrase</li>
     * <li>a passphrase confirmation</li>
     * <li>a key algorithm</li>
     * </ul>
     * @return 
     */
    bool Validate() const;
    /**
     * If completely is false :
     * <ul>
     * <li>name</li>
     * <li>email</li>
     * <li>comment</li>
     * <li>password fields</li>
     * </ul>
     * are set to default values.
     * \n If completely is true, all parameters are reset to default.
     * @param completely
     */
    void Reset(bool completely = false);

private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WLineEdit * m_leName;
    WLineEdit * m_leEmail;
    WLineEdit * m_leComment;
    WDateEdit * m_deExpiry;
    WLineEdit * m_lePassphrase;
    WLineEdit * m_leConfirm;
    WCheckBox * m_cbDefaultAlgo;
    WPushButton * m_btnApply;

    list<WComboBox*> m_cmbKeyAlgo;
    list<WComboBox*> m_cmbSubkeyAlgo;
    WString m_arbitraryKeyAlgo;
    WString m_arbitrarySubkeyAlgo;

    /**
     * Sets the other comboboxes in the list to empty values.
     * \n Assigns the selected algorithm to m_arbitraryKeyAlgo.
     * @param cmb
     */
    void OnComboKeyAlgoSelect(WComboBox * cmb);
    /**
     * Sets the other comboboxes in the list to empty values.
     * \n Assigns the selected algorithm to m_arbitrarySubkeyAlgo.
     * @param cmb
     */
    void OnComboSubkeyAlgoSelect(WComboBox * cmb);

};

#endif /* POPUPCREATE_H */

