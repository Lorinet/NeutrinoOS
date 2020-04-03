#pragma once
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include "util.h"
#include "bitconverter.h"
#include <algorithm>
#if defined(__UNIX)
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#elif defined(__WIN32)
#include "wdirent.h"
#endif
using namespace std;
class file
{
public:
	static vector<byte> readAllBytes(string path);
	static void readAllBytes(string path, vector<byte>* v);
	static string readAllText(string path);
	static void writeAllBytes(string path, vector<byte> data);
	static void writeAllText(string path, string text);
	static void deleteFile(string path);
	static void createFile(string path);
	static void deleteDirectory(string path);
	static void createDirectory(string path);
	static bool fileExists(string path);
	static bool directoryExists(string path);
	static vector<string> getFiles(string path);
	static vector<string> getDirectories(string path);
};