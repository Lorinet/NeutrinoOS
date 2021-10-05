#pragma once
#include "components.h"

#ifdef COMPONENT_NFS
#include <map>
#include "util.h"
#include "bitconverter.h"
#include "filesystem.h"
#include "lvmgr.h"
#include "iapi.h"

enum nfscmd
{
	OPEN = 0x00,
	CLOSE = 0x01,
	CREATE_NFS = 0x02,
	ERASE = 0x03,
	ADDFILE = 0x04,
	REMOVEFILE = 0x05,
	WRITEFILE = 0x06,
	READFILE = 0x07,
	EXTRACTIMAGE = 0x08,
	COPYFILE = 0x09,
	MOVEFILE = 0x0A
};

class nfs_api : public iapi
{
public:
	static nfs_api instance;
	static const int id = 1;
	static void initialize();
	Array<byte> message(Array<byte> indata, nvm* v);
};

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
	nfs();
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
	Array<byte> ReadFile(string filename);
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

#endif