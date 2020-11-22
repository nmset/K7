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
#include <Wt/WGroupBox.h>
#include <Wt/WButtonGroup.h>
#include <vector>
#include <gpgme++/key.h>
#include "TransientMessageWidget.h"

using namespace Wt;

/**
 * A popup with required parameters to certify or revoke a key uid :
 * <ul>
 * <li>Signer's private keys</li>
 * <li>Target key user identities (email)</li>
 * <li>Signing options : Exportable and non-revocable</li>
 * <li>Passphrase for selected private key</li>
 * </ul>
 * \n For revocation, GnuPG >= 2.2.24 is required.
 */
class PopupCertifyUserId : public WPopupWidget
{
public:

    enum What
    {
        RevokeUidCertification = 0, CertifyUid
    };
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
        // isVisible is always false when the popup is hidden !
        return !(m_lePassphrase->isHidden());
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
     * Get the certifications to revoke.
     * \n GPGME >= 1.15.0 is required. It expects
     * it as a vector of GpgME::UserID, instead of a vector of indices used in
     * GetUidsToSign().
     * @return 
     */
    vector<GpgME::UserID>& GetUidsToRevokeCertification()
    {
        return m_uidsToRevokeCertification;
    }

    /**
     * Certify selected user identities or revoke certifications.
     * @return 
     */
    const What WhatToDo() const
    {
        return (What) m_bgWhat->checkedId();
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
    vector<GpgME::UserID> m_uidsToRevokeCertification;
    int m_certifyOptions;
    WGroupBox * m_gbOptions;
    shared_ptr<WButtonGroup> m_bgWhat;
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
     * Add the identity index in a vector for certification.
     * \n Add the ::UserID in a vector for signature revocation.
     * @param cb
     */
    void OnEmailChecked(WCheckBox * cb, GpgME::UserID& uid);
    /**
     * Removes the identity index from a vector for certification.
     * \n Removes the ::UserID from a vector for signature revocation.
     * @param cb
     */
    void OnEmailUnChecked(WCheckBox * cb, GpgME::UserID& uid);
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
    /**
     * Show certification options if certifying, else hide them.
     * @param btn
     */
    void OnButtonGroupWhat(WRadioButton * btn);
};

#endif /* POPUPCERTIFYUSERID_H */

