//
// Created by charles on 27/09/23.
//
#include "lampConfig.h"
#include "../Filesystem/lampFS.h"

bool Lamp::Core::lampConfig::init() {
        Base::lampLog::getInstance().log("Initializing Lampray");

        if((std::string)bit7zLibraryLocation == "") {
            Base::lampLog::getInstance().log("Searching for 7z.so");
            std::filesystem::path f{"/usr/libexec/p7zip/7z.so"};
            if (std::filesystem::exists(f)) {
                bit7zLibraryLocation = "/usr/libexec/p7zip/7z.so";
            } else if (exists(std::filesystem::path{"/usr/lib/p7zip/7z.so"})) {
                bit7zLibraryLocation = "/usr/lib/p7zip/7z.so";
            } else if (exists(std::filesystem::path{"/usr/lib64/p7zip/7z.so"})) {
                bit7zLibraryLocation = "/usr/lib64/p7zip/7z.so";
            } else if (exists(std::filesystem::path{"/usr/libexec/7z.so"})) {
                bit7zLibraryLocation = "/usr/libexec/7z.so";
            } else if (exists(std::filesystem::path{"/usr/lib/7zip/7z.so"})) {
                bit7zLibraryLocation = "/usr/lib/7zip/7z.so";
            } else {
                Base::lampLog::getInstance().log("Fatal. Cannot locate 7z.so", Base::lampLog::ERROR, true,
                                                 Base::lampLog::LMP_NO7ZP);
                return false;
            }
        }

    return true;
}

ImGuiWindowFlags Lamp::Core::lampConfig::DefaultWindowFlags() {
    ImGuiWindowFlags windowFlags = 0;
    windowFlags += ImGuiWindowFlags_NoMove;
    windowFlags += ImGuiWindowFlags_NoResize;
    windowFlags += ImGuiWindowFlags_NoCollapse;
    windowFlags += ImGuiWindowFlags_NoTitleBar;
    windowFlags += ImGuiWindowFlags_MenuBar;
    return windowFlags;
}

