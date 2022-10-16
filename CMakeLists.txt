cmake_minimum_required(VERSION 3.0)

project(K7)

find_package(Wt COMPONENTS Wt HTTP CONFIG REQUIRED)
find_package(Gpgmepp CONFIG REQUIRED)

add_executable(k7 
    AppConfig.cpp     KeyringIO.cpp                   PopupCreate.cpp           SensitiveTreeTableNodeText.cpp
    GpgMECWorker.cpp  LoopbackPassphraseProvider.cpp  PopupDeleter.cpp          Tools.cpp
    GpgMEWorker.cpp   main.cpp                        PopupExpiryTime.cpp       TransientMessageWidget.cpp
    K7Main.cpp        PopupAddUid.cpp                 PopupExportSecretKey.cpp
    KeyEdit.cpp       PopupCertifyUserId.cpp          PopupUploader.cpp)

target_link_libraries(k7 Wt::Wt Wt::HTTP Gpgmepp)

install(TARGETS k7 RUNTIME DESTINATION bin)


