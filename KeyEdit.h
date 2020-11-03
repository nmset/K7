/* 
 * File:   KeyEdit.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on October 25, 2020, 10:38 AM
 */

#ifndef KEYEDIT_H
#define KEYEDIT_H

#include <Wt/WTreeTableNode.h>
#include "K7Main.h"
#include <Wt/WComboBox.h>
#include "PopupCertifyUserId.h"

using namespace Wt;

class K7Main;

/**
 * Some key editing functionalities are or will be implemented here. For now,
 * only owner trust level and key certification are implemented.
 */
class KeyEdit : public WObject
{
public:
    KeyEdit(K7Main * owner);
    virtual ~KeyEdit();
    /**
     * Shows a combobox with all trust levels
     * @param keyNode
     */
    void OnOwnerTrustDoubleClicked(WTreeTableNode * keyNode);
    /**
     * Saves any changes in trust level
     * @param keyNode
     */
    void OnOwnerTrustBlurred(WTreeTableNode * keyNode);
    /**
     * If the fingerprint is that of a private key we manage, returns true.
     * @param fpr
     * @return 
     */
    bool IsOurKey(const WString& fpr);
    /**
     * Shows a popup with parameters for key certification.
     * @param uidNode
     * @param privateKeys : A list of our private keys.
     * @param targetKeyFpr : The key to sign.
     */
    void OnUidValidityClicked(WTreeTableNode * uidNode, vector<WString>& privateKeys, const WString& targetKeyFpr);
    
private:
    K7Main * m_owner;
    PopupCertifyUserId * m_popupUid;
    WString m_targetKeyFpr;

    void FillOwnerTrustCombo(WComboBox * cmb);
    void CertifyKey();
};

#endif /* KEYEDIT_H */
