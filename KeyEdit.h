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
#include "PopupExpiryTime.h"

using namespace Wt;

class K7Main;

/**
 * Some key editing functionalities are or will be implemented here. For now,
 * only owner trust level and key certification are implemented.
 * Is a pseudo-extension of K7Main. Both classes are friends to each other, and
 * everything is private here.
 * Does not manage keyring.
 */
class KeyEdit : public WObject
{
    friend class K7Main;
public:
    
private:
    KeyEdit(K7Main * owner);
    virtual ~KeyEdit();
    
    K7Main * m_owner;
    PopupCertifyUserId * m_popupUid;
    WString m_targetUidValidityKeyFpr;
    
    PopupExpiryTime * m_popupExpiryTime;
    WString m_expiryEditedKeyFpr;
    
    /**
     * Unknown is common.
     * \n If keyHasSecret is true, show only Ultimate level.
     * \n Else, show everything except Ultimate.
     * \n Undefined is not included.
     * @param cmb
     * @param keyHasSecret
     */
    void FillOwnerTrustCombo(WComboBox * cmb, bool keyHasSecret);
    void CertifyKey();
    void SetExpiryTime();
    
    /**
     * Shows a combobox with all trust levels
     * @param keyNode
     * @param keyHasSecret
     */
    void OnOwnerTrustDoubleClicked(WTreeTableNode * keyNode, bool keyHasSecret);
    /**
     * Saves any changes in trust level
     * @param keyNode
     * @param keyHasSecret
     */
    void OnOwnerTrustBlurred(WTreeTableNode * keyNode, bool keyHasSecret);
    /**
     * Shows a popup with parameters for key certification.
     * @param uidNode
     * @param privateKeys : A list of our private keys.
     * @param targetKeyFpr : The key to sign.
     */
    void OnUidValidityClicked(WTreeTableNode * uidNode, vector<WString>& privateKeys, const WString& targetKeyFpr);
    /**
     * Shows a popup with parameters to change expiry date.
     * @param subkeyNode
     * @param keyFpr
     */
    void OnExpiryClicked(WTreeTableNode * subkeyNode, const WString& keyFpr);
    
};

#endif /* KEYEDIT_H */

