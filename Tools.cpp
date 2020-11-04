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

Tools::Tools() {
}

Tools::~Tools() {
}

bool Tools::ConfigKeyIdMatchesKey(const GpgME::Key& k, const WString& configKeyId)
{
    // We want key identifier in config file to be real and complete.
    return (configKeyId == WString(k.shortKeyID())
            || configKeyId == WString(k.keyID())
            || configKeyId == WString(k.primaryFingerprint()));
}

int Tools::ToInt(const string& s) {
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
    status += WString(WString::tr("KeyStatusIsNull")) + sep + TexttualBool(k.isBad()) + nl;
    status += WString(WString::tr("KeyStatusIsInvalid")) + sep + TexttualBool(k.isBad()) + nl;
    status += WString(WString::tr("KeyStatusIsDisabled")) + sep + TexttualBool(k.isBad()) + nl;
    status += WString(WString::tr("KeyStatusIsExpired")) + sep + TexttualBool(k.isBad()) + nl;
    status += WString(WString::tr("KeyStatusIsRevoked")) + sep + TexttualBool(k.isBad());
    
    return status;
}

WString Tools::GetUidStatus(const GpgME::UserID& uid)
{
    const WString nl("\n");
    const WString sep(" : ");
    WString status(WString::tr("UserStatus"));
    status += WString(" :") + nl + nl;
    status += WString(WString::tr("UserStatusIsBad")) + sep + TexttualBool(uid.isBad()) + nl;
    status += WString(WString::tr("UserStatusIsNull")) + sep + TexttualBool(uid.isBad()) + nl;
    status += WString(WString::tr("UserStatusIsInvalid")) + sep + TexttualBool(uid.isBad()) + nl;
    status += WString(WString::tr("UserStatusIsRevoked")) + sep + TexttualBool(uid.isBad());
    
    return status;
}

WString Tools::GetSigStatus(const GpgME::UserID::Signature& sig)
{
    const WString nl("\n");
    const WString sep(" : ");
    WString status(WString::tr("SigStatus"));
    status += WString(" :") + nl + nl;
    status += WString(WString::tr("SigStatusIsBad")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsNull")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsInvalid")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsExportable")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsExpired")) + sep + TexttualBool(sig.isBad()) + nl;
    status += WString(WString::tr("SigStatusIsRevokation")) + sep + TexttualBool(sig.isBad());
    
    return status;
}

