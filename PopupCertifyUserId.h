/* 
 * File:   PopupCertifyUserId.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on October 30, 2020, 7:50 PM
 */

#ifndef POPUPCERTIFYUSERID_H
#define POPUPCERTIFYUSERID_H

#include <Wt/WPopupWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <vector>
#include "TransientMessageWidget.h"

using namespace Wt;

/**
 * A popup with required parameters to certify a key :
 * <ul>
 * <li>Signer's private keys</li>
 * <li>Target key user identities (email)</li>
 * <li>Signing options : Exportable and non-revocable</li>
 * <li>Passphrase for selected private key</li>
 * </ul>
 * The passphrase is cached by gpg-agent for default 10 mins.
 * \n The popup hides the passphrase widget until a certify op fails.
 * \n UserID::Validity:: lists many validity levels. How to selectively apply
 * an arbitrary level ? Before signing, it's 'Unknown'. After signing, it's
 * always 'Full'.
 */
class PopupCertifyUserId : public WPopupWidget
{
public:
    PopupCertifyUserId(WWidget * anchorWidget, TransientMessageWidget * txtMessage,
                       const WLength& width = 500);
    virtual ~PopupCertifyUserId();
    /**
     * Must be called on its own.
     * @param privateKeys
     * @param fprKeyToSign
     */
    void Create(vector<WString>& privateKeys, const WString& fprKeyToSign);
    /**
     * 
     * @return : Fingerprint of selected signing key
     */
    WString GetSelectedKey() const;
    /**
     * Controls visibility of passphrase widgets.
     * \n Need not be always visible as the passphrase is cached by gpg-agent.
     * \n During that caching period, a wrong input passphrase will not be looked
     * for by GPG engine, and may be confusing.
     * @param show
     */
    void ShowPassphrase(bool show = true);
    /**
     * Used to restore the state of the widgets when target key changes.
     * @return 
     */
    bool IsPasswordVisible() const
    {
        return m_lePassphrase->isVisible();
    }
    /**
     * Used to forward the passphrase to the loopback passphrase provider.
     * @return 
     */
    const string GetPassphrase()
    {
        return m_lePassphrase->text().toUTF8();
    }
    /**
     * Obviously.
     * @return 
     */
    const WString GetKeyToSign()
    {
        return m_fprKeyToSign;
    }
    /**
     * We can select what identity (email) to certify. GPG expects it as a
     * vector of indices.
     * @return 
     */
    vector<uint>& GetUidsToSign()
    {
        return m_uidsToSign;
    }
    /**
     * Sum of option values
     * <ul>
     * <li>Exportable : 1</li>
     * <li>Non revocable : 2</li>
     * </ul>
     * Trust(4) is not implemented as it always fails.
     * @return 
     */
    int GetCertifyOptions()
    {
        return m_certifyOptions;
    }
    /**
     * Caller binds its certify function here.
     * @return 
     */
    WPushButton* GetCertifyButton()
    {
        return m_btnApply;
    }
private:
    TransientMessageWidget * m_tmwMessage;
    WContainerWidget * m_cwMain;
    WString m_fprKeyToSign;

    WComboBox * m_cmbPrivKeys;
    WHBoxLayout * m_hblPreferences;
    WVBoxLayout * m_vblEmail;
    vector<WCheckBox*> m_lstUids;
    WCheckBox * m_cbOptionExportable;
    WCheckBox * m_cbOptionNonRevocable;
    // WCheckBox * m_cbOptionTrust; // Always fails
    WText * m_lblPassphrase;
    WLineEdit * m_lePassphrase;
    WPushButton * m_btnApply;

    vector<uint> m_uidsToSign;
    int m_certifyOptions;
    /**
     * Available private fingerprints in a combobox. The selected item is the 
     * signing key.
     * @param privateKeys
     */
    void FillPrivateKeyComboBox(vector<WString>& privateKeys);
    /**
     * Show user identities of key to sign. Can be independently selected
     * using checkboxes.
     */
    void PresentEmail();
    /**
     * Add the identity index in a vector.
     * @param cb
     */
    void OnEmailChecked(WCheckBox * cb);
    /**
     * Removes the identity index in a vector.
     * @param cb
     */
    void OnEmailUnChecked(WCheckBox * cb);
    /**
     * Adds the option value in m_certifyOptions.
     * @param id
     */
    void OnCertifyOptionChecked(int id);
    /**
     * Subtract the option value from m_certifyOptions.
     * @param id
     */
    void OnCertifyOptionUnChecked(int id);
};

#endif /* POPUPCERTIFYUSERID_H */

