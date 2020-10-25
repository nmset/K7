/* 
 * File:   global.h
 * Author: SET - nmset@yandex.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 8 octobre 2019, 20:52
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Wt/WString.h>
using namespace Wt;
using namespace std;

#define TR(sk) WString::tr(sk)

const WString _APPNAME_("K7");
const WString _APPVERSION_("1");
const WString _SPACE_(" ");
const WString _COLON_(":");

typedef map<int, WString> UidValidityMap;
typedef map<int, WString> OwnerTrustMap;


#endif /* GLOBAL_H */

