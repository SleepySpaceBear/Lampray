#include <cstdlib>
#include <filesystem>
#include <vector>

#include "lampBase.h"
static int counter = 0;

void moveDirectory(const std::filesystem::path& oldDir, const std::filesystem::path& newDir) {
  std::filesystem::remove(newDir);
  std::filesystem::rename(oldDir,newDir);
}

const char* Lamp::Core::Base::FileSystemOverlay::managed_prefix = "lampray_managed_";
bool Lamp::Core::Base::FileSystemOverlay::overlayExists() {
  std::string command = "mountpoint \"" + baseDir.string() + "\"";

  int status = std::system(command.c_str());

  return WEXITSTATUS(status) == 0;
}

Lamp::Core::Base::FileSystemOverlay::FileSystemOverlay(const std::filesystem::path& base, 
                                                       const std::filesystem::path& work,
                                                       const bool buildOverlay = true) :
  baseDir{std::filesystem::absolute(base)}, workDir{std::filesystem::absolute(work)} {
  
  managedDir = baseDir.parent_path() / (managed_prefix + baseDir.filename().stem().string());
  
  tmpDir = workDir.parent_path() / ("work_" + workDir.filename().string());
  std::filesystem::create_directory(tmpDir);
  
  mountCommand = "pkexec mount -t overlay overlay ";
  mountCommand += "-o lowerdir=\"" + managedDir.string() + "\"";
  mountCommand += ",upperdir=\"" + workDir.string() + "\"";
  mountCommand += ",workdir=\"" + tmpDir.string() + "\"";
  mountCommand += " \"" + baseDir.string() + "\"";
  ++counter;
  
  unmountCommand = "pkexec umount \"" + baseDir.string() + "\" "
    "&& pkexec rm -r \"" + tmpDir.string() + "\"";

  if(overlayExists()) {
    isActive = true;
  } else if (buildOverlay /*&& !overlayExists() */) {
    build();
  }
}

Lamp::Core::Base::FileSystemOverlay::~FileSystemOverlay() {
  remove();
}

bool Lamp::Core::Base::FileSystemOverlay::build() {
  if (isActive) {
    return true;
  }
  
  // folder has not been moved to the managed directory
  if(!std::filesystem::is_empty(baseDir) || !std::filesystem::exists(managedDir)) {
    moveDirectory(baseDir, managedDir);
    std::filesystem::create_directory(baseDir);
  }

  std::filesystem::create_directory(tmpDir);

  int status = std::system(mountCommand.c_str());
  
  if (WEXITSTATUS(status) != 0) {
    return false;
  }

  isActive = true;
  return true;
}

bool Lamp::Core::Base::FileSystemOverlay::remove() {
  if (!isActive) {
    return true;
  }

  if(!overlayExists()) {
    return false;
  }

  int status = std::system(unmountCommand.c_str());
  isActive = false;

  if(WEXITSTATUS(status) != 0) {
    return false;
  }

  moveDirectory(managedDir, baseDir);
  std::system(cleanDirsCommand.c_str());

  return true;
}
