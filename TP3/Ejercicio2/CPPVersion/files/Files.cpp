#include "Files.h"

string File::readFile(string path) {
    ifstream file(path);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

void File::writeFile(string path, string content) {
    ofstream file(path);
    file << content;
}

void File::appendFile(string path, string content) {
    ofstream file(path, ios::app);
    file << content;
}

void File::deleteFile(string path) {
    remove(path.c_str());
}

bool File::contains(string path) {
    ifstream file(path);
    return file.good();
}

void File::createFile(string path) {
    ofstream file(path);
}   

void File::createFolder(string path) {
    mkdir(path.c_str(), 0777);
}

void File::deleteFolder(string path) {
    rmdir(path.c_str());
}

bool File::containsFolder(string path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        return false;
    } else {
        return false;
    }
}

void File::copyFile(string path, string newPath) {
    ifstream source(path, ios::binary);
    ofstream dest(newPath, ios::binary);
    dest << source.rdbuf();
}

void File::moveFile(string path, string newPath) {
    rename(path.c_str(), newPath.c_str());
}

void File::copyFolder(string path, string newPath) {
    mkdir(newPath.c_str(), 0777);
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            copyFolder(entry.path(), newPath + "/" + entry.path().filename().string());
        } else {
            copyFile(entry.path(), newPath + "/" + entry.path().filename().string());
        }
    }
}

void File::moveFolder(string path, string newPath) {
    rename(path.c_str(), newPath.c_str());
}

void File::deleteFolderRecursive(string path) {
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            deleteFolderRecursive(entry.path());
        } else {
            deleteFile(entry.path());
        }
    }
    deleteFolder(path);
}

void File::copyFolderRecursive(string path, string newPath) {
    mkdir(newPath.c_str(), 0777);
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            copyFolderRecursive(entry.path(), newPath + "/" + entry.path().filename().string());
        } else {
            copyFile(entry.path(), newPath + "/" + entry.path().filename().string());
        }
    }
}

void File::moveFolderRecursive(string path, string newPath) {
    mkdir(newPath.c_str(), 0777);
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            moveFolderRecursive(entry.path(), newPath + "/" + entry.path().filename().string());
        } else {
            moveFile(entry.path(), newPath + "/" + entry.path().filename().string());
        }
    }
    deleteFolder(path);
}


string File::getPathActual() {
    return pathActual;
}

File::File(string path) {
    pathActual = path;
}