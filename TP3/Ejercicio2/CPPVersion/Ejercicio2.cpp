#include <iostream>
#include "FileWatcher.h"
#include "files/Files.h"


int main() {
    // Create a FileWatcher instance that will check the current folder for changes every 5 seconds
    std::string s;
    std::cin >> s;
    FileWatcher fw{s, std::chrono::milliseconds(2500)};
    File f = File("logWatcher.txt");
    f.createFile(f.getPathActual());

    

    fw.start([] (std::string path_to_watch, FileStatus status) -> void {
        // Process only regular files, all other file types are ignored
        if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
            return;
        }

        switch(status) {
            case FileStatus::created:
                //std::cout << "File created: " << path_to_watch << '\n';
                f.appendFile(f.getPathActual(), "File created: " + path_to_watch + "\n");
                break;
            case FileStatus::modified:
                //std::cout << "File modified: " << path_to_watch << '\n';
                f.appendFile(f.getPathActual(), "File modified: " + path_to_watch + "\n");
                break;
            case FileStatus::erased:
                //std::cout << "File erased: " << path_to_watch << '\n';
                f.appendFile(f.getPathActual(), "File erased: " + path_to_watch + "\n");
                break;
            default:
                //std::cout << "Error! Unknown file status.\n";
                f.appendFile(f.getPathActual(), "Error! Unknown file status.\n");
        }
    });
}