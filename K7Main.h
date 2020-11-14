/* 
 * File:   K7Main.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 7 octobre 2019, 21:29
 */

#ifndef K7MAIN_H
#define K7MAIN_H

#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WGridLayout.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTreeTable.h>
#include <Wt/WTreeTableNode.h>
#include <Wt/WAnchor.h>
#include <gpgme++/key.h>
#include <map>
#include "AppConfig.h"
#include "TransientMessageWidget.h"
#include "KeyEdit.h"
#include "KeyringIO.h"
#include "global.h"

using namespace Wt;

class KeyEdit;
class KeyringIO;

class K7Main : public WApplication
{
    friend class KeyEdit;
    friend class KeyringIO;
public:
    K7Main(const WEnvironment& env);
    virtual ~K7Main();
    void Create();

private:
    /**
     * Translate UID trusts to string
     */
    UidValidityMap UidValidities;
    OwnerTrustMap OwnerTrustLevel;
    AppConfig * m_config;
    WGridLayout * m_grlMain;
    WContainerWidget * m_cwMain;
    TransientMessageWidget * m_tmwMessage;
    WLineEdit * m_leSearch;
    WPushButton * m_btnUpload;
    WPushButton * m_btnImport;
    WPushButton * m_btnDelete;
    WPushButton * m_btnCreate;
    WTreeTable * m_ttbKeys;
    WTreeTable * m_ttbUids;
    WTreeTable * m_ttbSubKeys;
    KeyEdit * m_keyEdit;
    KeyringIO * m_keyringIO;
    /**
     * Finds public keys as per criteria,
     * and private keys if any is declared in config file for current client.
     * \nShows the keys in tree table.
     */
    void Search();
    /**
     * Show keys in tree table
     */
    void DisplayKeys(const vector<GpgME::Key>& kList, const WString& grpLabel, bool expand);
    /**
     * Show key details
     * @param source
     */
    void OnKeyAnchorClicked(WAnchor * source);
    /**
     * Show UID details for a key
     * @param fullKeyID
     * @param secret
     */
    void DisplayUids(const WString& fullKeyID, bool secret = false);
    /**
     * Show subkey details for a key
     * @param fullKeyID
     * @param secret
     */
    void DisplaySubKeys(const WString& fullKeyID, bool secret = false);
    /**
     * Translates unit time to readable date.
     */
    WString MakeDateTimeLabel(time_t ticks);
};

#endif /* K7MAIN_H */

