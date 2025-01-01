//
// Created by charles on 27/09/23.
//

#ifndef LAMP_BG3_H
#define LAMP_BG3_H
#include "../gameControl.h"
#include "../../Lampray/Filesystem/lampFS.h"
#include "../../Lampray/Parse/lampParse.h"
namespace Lamp::Game {
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;

    class BG3 : public gameControl {
    public:

        lampReturn registerArchive(lampString Path, int ArchiveModType = -1) override;
        lampReturn ConfigMenu() override;
        lampReturn startDeployment() override;
        lampReturn preCleanUp() override;
        lampReturn preDeployment() override;
        lampReturn deployment() override;
        lampReturn postDeploymentTasks() override;
        void listArchives() override;

        std::vector<Core::Base::lampMod::Mod *> ModList {};

        std::map<std::string,std::string>& KeyInfo() override {
            return keyInfo;
        };

        Core::Base::lampTypes::lampIdent Ident() override {
            return {"Baldur's Gate 3","BG3"};
        };

        std::vector<Core::Base::lampMod::Mod *>& getModList() override;

        void launch() override {
            for (const auto& pair : keyInfo) {
                const std::string& key = pair.first;
                keyInfo[key] = (std::string) Lamp::Core::FS::lampIO::loadKeyData(key,Ident().ShortHand).returnReason;
                if(key == "ProfileList"){
                    if(pair.second == "" || pair.second == "Default") {
                        keyInfo[key] = "Default";
                        Lamp::Core::FS::lampIO::saveKeyData(key, keyInfo[key], Ident().ShortHand);
                    }
                }
                if(key == "CurrentProfile"){
                    if(pair.second == "" || pair.second == "Default") {
                        keyInfo[key] = "Default";
                        Lamp::Core::FS::lampIO::saveKeyData(key, keyInfo[key], Ident().ShortHand);
                    }
                }
            }
            ModList = Lamp::Core::FS::lampIO::loadModList(Ident().ShortHand, keyInfo["CurrentProfile"]);
        }

        int SeparatorModType() override {
            return MOD_SEPARATOR;
        }

        std::vector<std::pair<int, std::string> >& getModTypes() override {
            return ModTypeList;
        }

        std::map<int, std::string>& getModTypesMap() override{
            return ModTypeMap;
        }

        bool installPathSet() override{
            if(this->KeyInfo()["installDirPath"] == "" || this->KeyInfo()["appDataPath"] == ""){
                return false;
            }
            return true;
        }
        
        void unmount() override {
          if(modOverlay) {
            modOverlay->remove();
          }

          if(gameOverlay) {
            gameOverlay->remove();
          }
        }

	private:
        std::unique_ptr<Lamp::Core::Base::FileSystemOverlay> modOverlay;
        std::unique_ptr<Lamp::Core::Base::FileSystemOverlay> gameOverlay;

        enum ModType{
            BG3_ENGINE_INJECTION = 0,
            BG3_MOD,
            BG3_BIN_OVERRIDE,
            BG3_DATA_OVERRIDE,
            BG3_MOD_FIXER,
            NaN,
            MOD_SEPARATOR = 999,
        };

        // use a vector to keep things organized, this allows us to output mod types in the order we define
        std::vector<std::pair<int, std::string> > ModTypeList{
            { BG3_ENGINE_INJECTION, "Engine Injection" },
            { BG3_MOD, "Standard Mod" },
            { BG3_BIN_OVERRIDE, "Bin Overwrite" },
            { BG3_DATA_OVERRIDE, "Data Overwrite" },
            { BG3_MOD_FIXER, "No Json Mod" },
            { NaN, "Select Type" },
            { MOD_SEPARATOR, "Separator" },
        };
        // we will load the mod type vector above into this so we can get display values by the mod type value
        std::map<int, std::string> ModTypeMap = initModTypesMap();


        std::map<std::string,std::string> keyInfo{
                {"installDirPath",""},
                {"appDataPath",""},
                {"ProfileList","Default"},
                {"CurrentProfile", "Default"}
        };
    };
}

#endif //LAMP_BG3_H
