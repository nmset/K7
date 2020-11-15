/* 
 * File:   KeyringIO.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on November 13, 2020, 6:46 PM
 */

#ifndef KEYRINGIO_H
#define KEYRINGIO_H

#include "K7Main.h"
#include <Wt/WComboBox.h>
#include <Wt/WStreamResource.h>
#include "PopupUploader.h"
#include "PopupDeleter.h"
#include "PopupCreate.h"

using namespace Wt;

class K7Main;

/**
 * Manages the keyring : import, create and delete keys.
 * Is a pseudo-extension of K7Main. Both classes are friends to each other, and
 * everything is private here.
 * Does not edit keys.
 * @param owner
 */
class KeyringIO : public WObject
{
    friend class K7Main;
public:

private:
    KeyringIO(K7Main * owner);
    virtual ~KeyringIO();

    K7Main * m_owner;
    PopupUpload * m_popupUpload;
    PopupDelete * m_popupDelete;
    PopupCreate * m_popupCreate;

    // Variables from m_owner mapped here.
    AppConfig * m_config;
    TransientMessageWidget * m_tmwMessage;
    WPushButton * m_btnUpload;
    WPushButton * m_btnImport;
    WPushButton * m_btnDelete;
    WPushButton * m_btnCreate;
    WPushButton * m_btnExport;
    WLineEdit * m_leSearch;


    /**
     * Shows a non-blocking popup to upload a key,
     * with forward confirmation for upload.
     */
    void ShowPopupUpload();
    /**
     * Actually import the uploaded file.
     */
    void DoImportKey();
    /**
     * Shows a non-blocking popup to delete a key
     */
    void ShowPopupDelete();
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
     * Shows a non-blocking popup to create keys.
     */
    void ShowPopupCreate();
    /**
     * Validates input and creates the keys.
     */
    void DoCreateKey();
    /**
     * Shows a button to import an uploaded key
     * @param spool
     */
    void OnUploadCompleted(const WString& spool);

    void PrepareExport(const WString& fpr, bool isSecret);
};

/**
 * Export a public key for download.
 * \n Secret keys cannot be exported and are excluded.
 * @param fpr
 * @param isSecret
 * @param tmw
 */
class ExportKeyStreamResource : public WStreamResource
{
public:
    ExportKeyStreamResource(const WString& fpr, bool isSecret,
                            TransientMessageWidget * tmw);
    ExportKeyStreamResource(const WString& fpr, bool isSecret,
                            const string& mimeType,
                            TransientMessageWidget * tmw);
    virtual ~ExportKeyStreamResource();
    /**
     * Actually exports the public key and starts download on success.
     * @param request
     * @param response
     */
    void handleRequest(const Http::Request& request,
                       Http::Response& response) override;

private:
    WString m_fpr;
    bool m_isSecret;
    TransientMessageWidget * m_tmwMessage;
};

#endif /* KEYRINGIO_H */

