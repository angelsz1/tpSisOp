#ifndef FILES_H_
#define FILES_H_

#include <iostream>
#include <fstream>
using namespace std;


class File {
public:
    File(string path);
    string readFile(string fileName);
    void writeFile(string fileName, string text);
    virtual ~File();
    void appendFile(string fileName, string text);
    void deleteFile(string fileName);
    void createFile(string fileName);
    void createFolder(string folderName);
    void deleteFolder(string folderName);
    bool containsFolder(string folderName);
    bool contains(string fileName);
    void copyFile(string fileName, string newFileName);
    void moveFile(string fileName, string newFileName);
    void copyFolder(string folderName, string newFolderName);
    void moveFolder(string folderName, string newFolderName);
    void deleteFolderRecursive(string folderName);
    void copyFolderRecursive(string folderName, string newFolderName);
    void moveFolderRecursive(string folderName, string newFolderName);
    string getPathActual();

private:
    string pathActual;
};


#endif /* FILES_H */
