/* 
 * File:   K7Main.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on 7 octobre 2019, 21:29
 */

#include "K7Main.h"
#include "global.h"
#include <iostream>
#include <Wt/WBreak.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WGridLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WLink.h>
#include "GpgMEWorker.h"
#include "GpgMECWorker.h"
#include "Tools.h"
#include "SensitiveTreeTableNodeText.h"

using namespace std;

K7Main::K7Main(const WEnvironment& env)
: WApplication(env)
{
    m_config = NULL;
    m_btnUpload = NULL;
    m_btnImport = NULL;
    m_btnDelete = NULL;
    m_btnCreate = NULL;
    WApplication::setTitle(_APPNAME_);
    const WString bundle = WApplication::appRoot() + _APPNAME_;
    WApplication::instance()->messageResourceBundle().use(bundle.toUTF8());
    if (!env.javaScript() || !env.ajax())
    {
        root()->addWidget(cpp14::make_unique<WText>(TR("NoJS")));
        WApplication::quit(WString::Empty);
    }
    // We want users to authenticate with an X509 certificate
    if (WApplication::instance()->environment().sslInfo() == NULL)
    {
        root()->addWidget(cpp14::make_unique<WText>(TR("NoAuth")));
        WApplication::quit(WString::Empty);
    }
    // Translate UID trusts to string
    UidValidities[UserID::Validity::Full] = TR("UidFull");
    UidValidities[UserID::Validity::Marginal] = TR("UidMarginal");
    UidValidities[UserID::Validity::Never] = TR("UidNever");
    UidValidities[UserID::Validity::Ultimate] = TR("UidUltimate");
    UidValidities[UserID::Validity::Undefined] = TR("UidUndefined");
    UidValidities[UserID::Validity::Unknown] = TR("UidUnknown");

    OwnerTrustLevel[GpgME::Key::OwnerTrust::Full] = TR("UidFull");
    OwnerTrustLevel[GpgME::Key::OwnerTrust::Marginal] = TR("UidMarginal");
    OwnerTrustLevel[GpgME::Key::OwnerTrust::Never] = TR("UidNever");
    OwnerTrustLevel[GpgME::Key::OwnerTrust::Ultimate] = TR("UidUltimate");
    OwnerTrustLevel[GpgME::Key::OwnerTrust::Undefined] = TR("UidUndefined");
    OwnerTrustLevel[GpgME::Key::OwnerTrust::Unknown] = TR("UidUnknown");
    m_keyEdit = new KeyEdit(this);

    WLink link;
    const WString cssFile = WApplication::appRoot() + _APPNAME_
            + WString(".css");
    link.setUrl(cssFile.toUTF8());
    WApplication::useStyleSheet(link);
}

K7Main::~K7Main()
{
    delete m_config;
    delete m_keyEdit;
    delete m_keyringIO;
}

void
K7Main::Create()
{
    WContainerWidget * cwHeader = new WContainerWidget();
    WHBoxLayout * hblHeader = new WHBoxLayout();
    cwHeader->setLayout(unique_ptr<WHBoxLayout> (hblHeader));
    hblHeader->addWidget(cpp14::make_unique<WText>(_APPNAME_));
    // Error messages will go here
    m_tmwMessage = new TransientMessageWidget();
    m_tmwMessage->setTextAlignment(AlignmentFlag::Right);
    hblHeader->addWidget(unique_ptr<TransientMessageWidget>(m_tmwMessage));
    root()->addWidget(unique_ptr<WContainerWidget> (cwHeader));
    hblHeader->setStretchFactor(m_tmwMessage, 1);

    /*
     *  Load config JSON file.
     * On error, just abort, AppConfig will print an error message in m_tmwMessage
     */
    m_config = new AppConfig(m_tmwMessage);
    if (!m_config->LoadConfig())
        return;

    m_cwMain = new WContainerWidget();
    WGridLayout * grlMain = new WGridLayout();
    grlMain->setColumnStretch(0, 1);
    grlMain->setColumnStretch(1, 0);
    m_cwMain->setLayout(unique_ptr<WGridLayout> (grlMain));
    // Add a search zone : line edit and button
    m_leSearch = new WLineEdit();
    grlMain->addWidget(unique_ptr<WLineEdit> (m_leSearch), 0, 0);
    m_leSearch->enterPressed().connect(this, &K7Main::Search);
    WPushButton * btnSearch = new WPushButton(TR("Search"));
    btnSearch->setToolTip(TR("TTTSearch"));
    grlMain->addWidget(unique_ptr<WPushButton> (btnSearch), 0, 1);
    btnSearch->clicked().connect(this, &K7Main::Search);
    // Add a result zone as a tree table
    m_ttbKeys = new WTreeTable();
    grlMain->addWidget(unique_ptr<WTreeTable> (m_ttbKeys), 1, 0);

    WContainerWidget * cwButtons = new WContainerWidget();
    WVBoxLayout * vblButtons = new WVBoxLayout();
    cwButtons->setLayout(unique_ptr<WVBoxLayout> (vblButtons));
    // Add an import button if current user is allowed
    if (m_config->CanImport())
    {
        m_btnUpload = new WPushButton(TR("Upload"));
        m_btnUpload->setToolTip(TR("TTTUpload"));
        vblButtons->addWidget(unique_ptr<WPushButton> (m_btnUpload));
        m_btnImport = new WPushButton(TR("Import"));
        m_btnImport->setToolTip(TR("TTTImport"));
        vblButtons->addWidget(unique_ptr<WPushButton> (m_btnImport));
        m_btnImport->hide();
    }
    // Add a delete button if current user is allowed
    if (m_config->CanDelete())
    {
        m_btnDelete = new WPushButton(TR("Delete"));
        m_btnDelete->setToolTip(TR("TTTDelete"));
        vblButtons->addWidget(unique_ptr<WPushButton> (m_btnDelete));
        m_btnDelete->hide();
    }
    vblButtons->addSpacing(150);
    vblButtons->addStretch(1);
    if (m_config->CanCreateKeys())
    {
        m_btnCreate = new WPushButton(TR("Create"));
        m_btnCreate->setToolTip(TR("TTTCreate"));
        vblButtons->addWidget(unique_ptr<WPushButton> (m_btnCreate));
    }
    grlMain->addWidget(unique_ptr<WContainerWidget> (cwButtons), 1, 1);

    // Add and hide detail tables
    m_ttbUids = new WTreeTable();
    grlMain->addWidget(unique_ptr<WTreeTable> (m_ttbUids), 2, 0);
    m_ttbUids->hide();
    m_ttbSubKeys = new WTreeTable();
    grlMain->addWidget(unique_ptr<WTreeTable> (m_ttbSubKeys), 3, 0);
    m_ttbSubKeys->hide();

    root()->addWidget(cpp14::make_unique<WBreak>());
    root()->addWidget(unique_ptr<WContainerWidget> (m_cwMain));

    m_keyringIO = new KeyringIO(this);

#ifdef DEVTIME
    // Save my fingertips.
    m_leSearch->setText("s");
#endif
}

void K7Main::Search()
{
#ifndef DEVTIME
    // A reasonable minimal search criteria
    if (!m_leSearch->text().empty() && m_leSearch->text().toUTF8().length() < 3)
    {
        m_tmwMessage->SetText(TR("CriteriaTooShort"));
        return;
    }
#endif
    if (m_btnDelete)
        m_btnDelete->hide();
    m_ttbSubKeys->hide();
    m_ttbUids->hide();
    Error e;
    vector<WString> configPrivKeys = m_config->PrivateKeyIds();
    GpgMEWorker gpgw;
    vector<GpgME::Key> privkList, pubkList;

    // Find private keys if any is declared in config file for current user.
    for (uint i = 0; i < configPrivKeys.size(); i++)
    {
        /*
         * User may manage more than one private key.
         * configPrivKeys.at(i) : we want this to be a full key id(short, normal or fpr)
         * But it can be any string in the config file (name, email...).
         * lst can hence contain many keys.
         */
        vector<GpgME::Key> lst = gpgw.FindKeys(configPrivKeys.at(i).toUTF8().c_str(), true, e);
        if (e.code() != 0)
        {
            privkList.clear();
            m_tmwMessage->SetText(e.asString());
            return;
        }
        /*
         * If any key is not a full key id, abort. 
         */
        for (uint j = 0; j < lst.size(); j++)
        {
            const GpgME::Key k = lst.at(j);
            if (!Tools::ConfigKeyIdMatchesKey(k, configPrivKeys.at(i)))
            {
                m_tmwMessage->SetText(configPrivKeys.at(i) + TR("BadConfigKeyId"));
                privkList.clear();
                return;
            }
            privkList.push_back(k);
        }
    }
    // Find public keys as per criteria
    if (!m_leSearch->text().empty())
    {
        pubkList = gpgw.FindKeys(m_leSearch->text().toUTF8().c_str(), false, e);
        if (e.code() != 0)
        {
            pubkList.clear();
            m_tmwMessage->SetText(e.asString());
            return;
        }
    }

    if (m_ttbKeys->columnCount() == 1)
    {
        m_ttbKeys->addColumn(TR("ID"), 120);
        m_ttbKeys->addColumn(TR("OwnerTrust"), 210);
        m_ttbKeys->addColumn(TR("Fpr"), 300);
    }
    // The previous tree root is auto deleted by the use of smart pointers !!
    WTreeTableNode * rootNode = new WTreeTableNode(TR("Keys"));
    rootNode->setChildCountPolicy(ChildCountPolicy::Enabled);
    m_ttbKeys->setTreeRoot(unique_ptr<WTreeTableNode> (rootNode), TR("KeyRing"));
    rootNode->expand();
    // Show available keys
    if (pubkList.size())
        DisplayKeys(pubkList, TR("Publics"), true);
    if (privkList.size() && m_config->PrivateKeyIds().size() > 0)
        DisplayKeys(privkList, TR("Secrets"), false);
}

WString K7Main::MakeDateTimeLabel(time_t ticks)
{
    std::chrono::minutes offset = WApplication::instance()->environment().timeZoneOffset();
    const WDateTime dt = WDateTime::fromTime_t(ticks + (time_t) offset.count() * 60);
    return dt.toString(WString("yyyy-MM-dd - hh:mm:ss"));
}

void K7Main::DisplayKeys(const vector<GpgME::Key>& kList, const WString& grpLabel, bool expand)
{
    WTreeTableNode * grpNode = new WTreeTableNode(grpLabel);
    m_ttbKeys->treeRoot()->addChildNode(unique_ptr<WTreeTableNode> (grpNode));
    vector<WString> ourKeys = m_config->PrivateKeyIds();
    for (uint i = 0; i < kList.size(); i++)
    {
        const GpgME::Key k = kList.at(i);
        WTreeTableNode * keyNode = new WTreeTableNode(k.userID(0).name());
        keyNode->setToolTip(Tools::GetKeyStatus(k));
        WLink ln;
        ln.setUrl(WString(L"javascript:void(0)").toUTF8());
        WAnchor * anc = new WAnchor(ln, k.shortKeyID());
        anc->setId(k.shortKeyID());
        // We use widget attribute values to buffer properties.
        anc->setAttributeValue("hasSecret", k.hasSecret() ? "1" : "0");
        anc->clicked().connect(std::bind(&K7Main::OnKeyAnchorClicked, this, anc));
        keyNode->setColumnWidget(1, unique_ptr<WAnchor> (anc));
        WText * lblOwnerTrust = new WText(OwnerTrustLevel[k.ownerTrust()]);
        if (m_config->CanEditOwnerTrust())
        {
            /*
             * Here we allow the owner trust level of primary keys to be changed anytime.
             * Kleopatra doesn't do that for primary keys having ultimate trust level.
             */
            bool isOurKey = Tools::IsOurKey(k.primaryFingerprint(), ourKeys);
            if (!isOurKey || (isOurKey && k.hasSecret()))
            {
                lblOwnerTrust->doubleClicked().connect(std::bind(&KeyEdit::OnOwnerTrustDoubleClicked, m_keyEdit, keyNode, k.hasSecret()));
                lblOwnerTrust->setToolTip(TR("TTTDoubleCLick"));
            }
        }
        keyNode->setColumnWidget(2, unique_ptr<WText> (lblOwnerTrust));
        TreeTableNodeText * ttntFpr = new TreeTableNodeText(k.primaryFingerprint(), keyNode, 3);
        keyNode->setColumnWidget(3, unique_ptr<TreeTableNodeText> (ttntFpr));
        grpNode->addChildNode(unique_ptr<WTreeTableNode> (keyNode));
    }
    if (expand)
        grpNode->expand();
}

void K7Main::OnKeyAnchorClicked(WAnchor * source)
{
    // Show key details
    const WString hasSecret = source->attributeValue("hasSecret");
    bool secret = true;
    if (hasSecret == WString("0"))
        secret = false;
    const string id = source->id();
    // With secret = false, key signatures can be retrieved.
    DisplayUids(id);
    DisplaySubKeys(id, secret);
    if (m_config->CanDelete()) // m_btnDelete is NULL otherwise
        m_btnDelete->setHidden(!m_keyringIO->CanKeyBeDeleted(id));
}

void K7Main::DisplayUids(const WString& fullKeyID, bool secret)
{
    // Show UID details for a key
    Error e;
    GpgMEWorker gpgw;
    m_ttbUids->hide();
    const GpgME::Key k = gpgw.FindKey(fullKeyID.toUTF8().c_str(), e, secret);
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    if (m_ttbUids->columnCount() == 1)
    {
        m_ttbUids->addColumn(TR("Email"), 200);
        m_ttbUids->addColumn(TR("Trust"), 100);
        m_ttbUids->addColumn(TR("Comment"), 300);
    }
    WTreeTableNode * rootNode = new WTreeTableNode(fullKeyID);
    rootNode->setChildCountPolicy(ChildCountPolicy::Enabled);
    m_ttbUids->setTreeRoot(unique_ptr<WTreeTableNode> (rootNode), TR("UIDs"));
    rootNode->expand();
    vector<WString> privateKeys = m_config->PrivateKeyIds();
    for (uint i = 0; i < k.numUserIDs(); i++)
    {
        UserID uid = k.userID(i);
        WTreeTableNode * uidNode = new WTreeTableNode(uid.name());
        uidNode->setToolTip(Tools::GetUidStatus(uid));
        TreeTableNodeText * ttntUidEmail = new TreeTableNodeText(uid.email(), uidNode, 1);
        uidNode->setColumnWidget(1, unique_ptr<TreeTableNodeText> (ttntUidEmail));
        // Show key certify popup on double click
        WText * lblUidValidity = new WText(UidValidities[uid.validity()]);
        if (m_config->CanEditUidValidity())
        {
            lblUidValidity->setToolTip(TR("TTTDoubleCLick"));
            lblUidValidity->doubleClicked().connect(std::bind(&KeyEdit::OnUidValidityClicked, m_keyEdit, uidNode, privateKeys, WString(k.primaryFingerprint())));
        }
        uidNode->setColumnWidget(2, unique_ptr<WText> (lblUidValidity));
        TreeTableNodeText * ttntUidComment = new TreeTableNodeText(uid.comment(), uidNode, 3);
        uidNode->setColumnWidget(3, unique_ptr<TreeTableNodeText> (ttntUidComment));
        rootNode->addChildNode(unique_ptr<WTreeTableNode> (uidNode));
        // Set context KeyListMode to ::Signatures | ::Validate
        for (uint s = 0; s < uid.numSignatures(); s++)
        {
            UserID::Signature sig = uid.signature(s);
            const WString signer = WString(sig.signerName()) + _SPACE_
                    + WString(sig.signerKeyID());
            WTreeTableNode * sigNode = new WTreeTableNode(signer);
            sigNode->setToolTip(Tools::GetSigStatus(sig));
            TreeTableNodeText * ttntEmail = new TreeTableNodeText(sig.signerEmail(), sigNode, 1);
            sigNode->setColumnWidget(1, unique_ptr<TreeTableNodeText> (ttntEmail));
            WString exp = TR("Expiration") + _SPACE_ + _COLON_ + _SPACE_;
            exp += sig.neverExpires() ? TR("Never") : MakeDateTimeLabel(sig.expirationTime());
            sigNode->setColumnWidget(2, cpp14::make_unique<WText> (exp));
            TreeTableNodeText * ttntComment = new TreeTableNodeText(sig.signerComment(), sigNode, 3);
            sigNode->setColumnWidget(3, unique_ptr<TreeTableNodeText> (ttntComment));
            uidNode->addChildNode(unique_ptr<WTreeTableNode> (sigNode));
        }
    }
    m_ttbUids->show();
}

void K7Main::DisplaySubKeys(const WString& fullKeyID, bool secret)
{
    // Show subkey details for a key
    Error e;
    GpgMEWorker gpgw;
    m_ttbSubKeys->hide();
    const GpgME::Key k = gpgw.FindKey(fullKeyID.toUTF8().c_str(), e, secret);
    if (e.code() != 0)
    {
        m_tmwMessage->SetText(e.asString());
        return;
    }
    if (m_ttbSubKeys->columnCount() == 1)
    {
        m_ttbSubKeys->addColumn(TR("Fpr"), 300);
        m_ttbSubKeys->addColumn(TR("Expiration"), 150);
        m_ttbSubKeys->addColumn(TR("Usage"), 70);
        m_ttbSubKeys->addColumn(TR("Secret"), 50);
        m_ttbSubKeys->addColumn(TR("Algorithms"), 50);
    }
    WTreeTableNode * rootNode = new WTreeTableNode(fullKeyID);
    rootNode->setChildCountPolicy(ChildCountPolicy::Enabled);
    m_ttbSubKeys->setTreeRoot(unique_ptr<WTreeTableNode> (rootNode), TR("SubKeys"));
    rootNode->expand();
    vector<WString> ourKeys = m_config->PrivateKeyIds();
    bool canEditExpiry = m_config->CanEditExpiryTime()
            && Tools::KeyHasSecret(k.primaryFingerprint())
            && Tools::IsOurKey(k.primaryFingerprint(), ourKeys);
    for (uint i = 0; i < k.numSubkeys(); i++)
    {
        Subkey sk = k.subkey(i);
        WTreeTableNode * skNode = new WTreeTableNode(sk.keyID());
        skNode->setToolTip(Tools::GetKeyStatus(k));
        TreeTableNodeText * ttntFpr = new TreeTableNodeText(sk.fingerprint(), skNode, 1);
        skNode->setColumnWidget(1, unique_ptr<TreeTableNodeText> (ttntFpr));
        WString exp = sk.neverExpires() ? TR("Never") : MakeDateTimeLabel(sk.expirationTime());
        WText * lblExpiry = new WText(exp);
        if (canEditExpiry)
        {
            lblExpiry->setToolTip(TR("TTTDoubleCLick"));
            lblExpiry->doubleClicked().connect(std::bind(&KeyEdit::OnExpiryClicked, m_keyEdit, skNode, WString(k.primaryFingerprint())));
        }
        skNode->setColumnWidget(2, unique_ptr<WText> (lblExpiry));
        WString usage = sk.canAuthenticate() ? WString("A") : WString::Empty;
        usage += sk.canCertify() ? WString("C") : WString::Empty;
        usage += sk.canEncrypt() ? WString("E") : WString::Empty;
        usage += sk.canSign() ? WString("S") : WString::Empty;
        skNode->setColumnWidget(3, cpp14::make_unique<WText> (usage));
        const WString isSecret = sk.isSecret() ? TR("Yes") : TR("No");
        skNode->setColumnWidget(4, cpp14::make_unique<WText> (isSecret));
        const WString algos = sk.publicKeyAlgorithmAsString() + _SPACE_ + _DASH_
                + _SPACE_ + sk.algoName();
        skNode->setColumnWidget(5, cpp14::make_unique<WText> (algos));
        rootNode->addChildNode(unique_ptr<WTreeTableNode> (skNode));
    }
    m_ttbSubKeys->show();
}
