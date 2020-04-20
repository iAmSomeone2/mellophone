#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "Library.hpp"

using namespace mellophone;

Library::Library(std::shared_ptr<Database> db) {
    this->database = db;

    const char *homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        homeDir = getpwuid(getuid())->pw_dir;
    }

    if (homeDir != nullptr) {
        std::string pathStr = homeDir;
        pathStr.append("/Music");
        this->userMusicFolder = fs::path(pathStr);
    } else {
        this->userMusicFolder = fs::path("");
    }
}

/**
 * Returns a reference to the user's HOME music folder.
 * 
 * @returns reference to the user's HOME music folder path.
 */
fs::path& Library::getMusicFolderPath() {
    return this->userMusicFolder;
} 

/**
 * Scans through the user's HOME music folder to locate songs in supported 
 * formats and adds them to the database.
 */
void Library::scanLibrary() {
    
}