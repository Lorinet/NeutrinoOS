#pragma once
#include <map>
#include "util.h"
#include "bitconverter.h"
#include "filesystem.h"
#include "lvmgr.h"
class nfs_file
{
public:
	string name;
	int size;
	int offset;
	byte* contents;
	bool loaded = false;
	nfs_file();
	nfs_file(string n, int s, int off);
};
class nfs
{
private:
	map<string, nfs_file> files;
	fstream fileio;
public:
	nfs(string file);
	int ownerProcess;
	string fileName;
	static nfs CreateFSImage(string file);
	static nfs CreateFSImage(string file, vector<string> files);
	void AddFile(string file);
	void AddFiles(vector<string> filelist);
	void RemoveFile(string file);
	void RemoveFiles(vector<string> filelist);
	void MoveFile(string file1, string file2);
	void CopyFile(string file1, string file2);
	bool FileExists(string file);
	vector<string> GetFiles();
	int FileSize(string file);
	int ImageSize();
	void ExtractFile(string filename, string destination);
	void ExtractAllFiles(string destination);
	vector<byte> ReadFile(string filename);
	void WriteFile(string filename, vector<byte> data);
	void LoadFile(string name);
	void LoadAllFiles();
	void Commit();
	void Erase();
	void Release();
};
class nfsmgr
{
public:
	static map<int, nfs> handles;
	static int OpenImage(string filename, int process);
	static void CloseImage(int hndl, int process);
	static nfs* GetNFS(int hndl, int process);
	static int OpenImage(nfs n, int process);
};