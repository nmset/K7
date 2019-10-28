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
#include "PopupUploader.h"
#include "PopupDeleter.h"
#include "TransientMessageWidget.h"

using namespace Wt;

class K7Main : public WApplication
{
public:
    K7Main(const WEnvironment& env);
    virtual ~K7Main();
    void Create();

private:
    /**
     * Translate UID trusts to string
     */
    typedef map<int, WString> UidValidityMap;
    UidValidityMap UidValidities;
    AppConfig * m_config;
    WGridLayout * m_grlMain;
    WContainerWidget * m_cwMain;
    TransientMessageWidget * m_tmwMessage;
    WLineEdit * m_leSearch;
    WPushButton * m_btnUpload;
    WPushButton * m_btnImport;
    WPushButton * m_btnDelete ;
    WTreeTable * m_ttbKeys;
    WTreeTable * m_ttbUids;
    WTreeTable * m_ttbSubKeys;
    Uploader * m_uploader;
    Deleter * m_deleter;
    /**
     * Finds public keys as per criteria,
     * and private keys if any is declared in config file for current client.
     * \nShows the keys in tree table.
     */
    void Search();
    /**
     * We want key identifier in config file to be real and complete.
     * @param k
     * @param configKeyId key identifier as entered in configuration file
     * @return 
     */
    bool ConfigKeyIdMatchesKey(const GpgME::Key& k, const WString& configKeyId) const;
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
     * Shows a non-blocking popup to upload a key,
     * with forward confirmation for upload.
     */
    void PopupUploader();
    /**
     * Shows a button to import an uploaded key
     * @param spool
     */
    void OnUploadCompleted(const WString& spool);
    /**
     * Actually import the uploaded file.
     */
    void DoImportKey();
    /**
     * Shows a non-blocking popup to delete a key
     */
    void PopupDeleter();
    /**
     * All public keys can be deleted.
     * Private keys can be deleted only if the user
     * manages that key, as declared in the configuration file.
     * @param fullKeyID
     * @return 
     */
    bool CanKeyBeDeleted(const WString& fullKeyID);
    /**
     * Actually delete the key, with forward confirmation in the popup.
     */
    void DoDeleteKey();
    /**
     * Translates unit time to readable date.
     */
    WString MakeDateTimeLabel(time_t ticks);
};

#endif /* K7MAIN_H */

