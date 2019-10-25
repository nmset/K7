/* 
 * File:   main.cpp
 * Author: SET - nmset@netcourrier.com
 * License : GPL v2
 * Copyright SET - © 2019
 *
 * Created on 7 octobre 2019, 21:06
 */

#include <cstdlib>
#include "K7Main.h"
#include <gpgme++/global.h>
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
using namespace std;
using namespace Wt;


unique_ptr<WApplication> createApplication(const WEnvironment& env)
{
    unique_ptr<K7Main> app = cpp14::make_unique<K7Main>(env);
    app->Create();
    return (app);
}

int main(int argc, char** argv)
{
    GpgME::initializeLibrary();
    return WRun(argc, argv, &createApplication);
}

