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
