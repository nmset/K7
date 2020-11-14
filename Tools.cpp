/* 
 * File:   Tools.cpp
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 * 
 * Created on November 1, 2020, 8:31 PM
 */

#include "Tools.h"
#include <sstream>
#include <iostream>

using namespace std;

Tools::Tools()
{
}

Tools::~Tools()
{
}

bool Tools::ConfigKeyIdMatchesKey(const GpgME::Key& k, const WString& configKeyId)
{
    // We want key identifier in config file to be real and complete.
    return (configKeyId == WString(k.shortKeyID())
            || configKeyId == WString(k.keyID())
            || configKeyId == WString(k.primaryFingerprint()));
}

int Tools::ToInt(const string& s)
{
    istringstream buffer(s.c_str());
    int num;
    buffer >> num;
    return num;
}

WString Tools::TexttualBool(bool value)
{
    const WString res = value
            ? WString::tr("Yes")
            : WString::tr("No");
    return res;
}

WString Tools::GetKeyStatus(const GpgME::Key& k)
{
    const WString nl("\n");
    const WString sep(" : ");
    WString status(WString::tr("KeyStatusFor"));
    status += WString(" ");
    status += WString(k.keyID()) + WString(" :") + nl + nl;
    status += WString(WString::tr("KeyStatusIsBad")) + sep + TexttualBool(k.isBad()) + nl;
    status += WString(WString::tr("KeyStatusIsNull")) + sep + TexttualBool(k.isNull()) + nl;
    status += WString(WString::tr("KeyStatusIsInvalid")) + sep + TexttualBool(k.isInvalid()) + nl;
    status += WString(WString::tr("KeyStatusIsDisabled")) + sep + TexttualBool(k.isDisabled()) + nl;
    status += WString(WString::tr("KeyStatusIsExpired")) + sep + TexttualBool(k.isExpired()) + nl;
    status += WString(WString::tr("KeyStatusIsRevoked")) + sep + TexttualBool(k.isRevoked()) + nl + nl;
    status += WString(WString::tr("KeyTypeIsSecret")) + sep + TexttualBool(KeyHasSecret(k.primaryFingerprint()));

    return status;
}

WString Tools::GetUidStatus(const GpgME::UserID& uid)
{
    const WString nl("\n");
    const WString sep(" : ");
    WString status(WString::tr("UserStatus"));
    status += WString(" :") + nl + nl;
    status += WString(WString::tr("UserStatusIsBad")) + sep + TexttualBool(uid.isBad()) + nl;
    status += WString(WString::tr("UserStatusIsNull")) + sep + TexttualBool(uid.isNull()) + nl;
    status += WString(WString::tr("UserStatusIsInvalid")) + sep + TexttualBool(uid.isInvalid()) + nl;
    status += WString(WString::tr("UserStatusIsRevoked")) + sep + TexttualBool(uid.isRevoked());

    return status;
}

WString Tools::GetSigStatus(const GpgME::UserID::Signature& sig)
{
    const WString nl("\n");
    const WString sep(" : ");
    WString status(WString::tr("SigStatus"));
    status += WString(" :") + nl + nl;
    status += WString(WString::tr("SigStatusIsBad")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsNull")) + sep + TexttualBool(sig.isNull()) + nl;
    status += WString(WString::tr("SigStatusIsInvalid")) + sep + TexttualBool(sig.isInvalid()) + nl;
    status += WString(WString::tr("SigStatusIsExportable")) + sep + TexttualBool(sig.isExportable()) + nl;
    status += WString(WString::tr("SigStatusIsExpired")) + sep + TexttualBool(sig.isExpired()) + nl;
    status += WString(WString::tr("SigStatusIsRevokation")) + sep + TexttualBool(sig.isRevokation()) + nl;
    status += WString(WString::tr("Algorithm")) + sep + WString(sig.algorithmAsString());

    return status;
}

bool Tools::KeyHasSecret(const WString& fpr)
{
    Error e;
    GpgMEWorker gpgw;
    GpgME::Key k = gpgw.FindKey(fpr.toUTF8().c_str(), e, true); // Look for a private key
    if (e.code() != 0 && e.code() != 16383)
    { // 16383 : end of file, when key is not private
        return false;
    }
    return (!k.isNull());
}

bool Tools::IsFound(const WString& item, vector<WString>& items)
{
    // std:find should be more aesthetic.
    vector<WString> ::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if ((*it) == item)
            return true;
    }
    return false;
}

bool Tools::IsOurKey(const WString& fpr, vector<WString>& ourPrivKeys)
{
    return IsFound(fpr, ourPrivKeys);
}
