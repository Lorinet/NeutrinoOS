#include "filesystem.h"
#if defined(__UNIX)
#include <dirent.h>
#elif defined(__WIN32)
#include "wdirent.h"
#endif
vector<byte> file::readAllBytes(string path)
{
	unsigned char* fcontent = NULL;
	size_t fsize = 0;
	FILE* f = fopen(path.c_str(), "rb");
	if (f == NULL)
	{
		return vector<byte>();
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	fcontent = (unsigned char*)malloc(fsize);
	fread(fcontent, 1, fsize, f);
	fclose(f);
	return vector<byte>(fcontent, fcontent + fsize);
}
void file::readAllBytes(string path, vector<byte>* v)
{
	unsigned char* fcontent = NULL;
	size_t fsize = 0;
	FILE* f = fopen(path.c_str(), "rb");
	if (f == NULL)
	{
		return;
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	fcontent = (unsigned char*)malloc(fsize);
	fread(fcontent, 1, fsize, f);
	fclose(f);
	v->assign(fcontent, fcontent + fsize);
}
string file::readAllText(string path)
{
	return bitconverter::tostring(file::readAllBytes(path));
}
void file::writeAllBytes(string path, vector<byte> data)
{
	ofstream ofile(path);
	ofile.seekp(0, ofile.beg);
	for (int i = 0; i < data.size(); i++)
	{
		ofile.write((char*)&data[i], 1);
	}
}
void file::writeAllText(string path, string text)
{
	FILE* f = fopen(path.c_str(), "w");
	if (f == NULL)
	{
		return;
	}
	fprintf(f, text.c_str());
	fclose(f);
}
void file::deleteFile(string path)
{
	remove(path.c_str());
}
void file::createFile(string path)
{
	FILE* f = fopen(path.c_str(), "w");
	fclose(f);
}
void file::deleteDirectory(string path)
{
	system(("rmdir /S /Q " + path).c_str());
}
void file::createDirectory(string path)
{
	system(("mkdir " + path).c_str());
}
bool file::fileExists(string path)
{
	FILE* f = fopen(path.c_str(), "r");
	if (f == NULL)
	{
		return false;
	}
	fclose(f);
	return true;
}
bool file::directoryExists(string path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	else return false;
}
vector<string> file::getFiles(string path)
{
	vector<string> fls;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			struct stat path_stat;
			stat(ent->d_name, &path_stat);
			if (S_ISREG(path_stat.st_mode))
			{
				fls.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	return fls;
}
vector<string> file::getDirectories(string path)
{
	vector<string> fls;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			struct stat path_stat;
			stat(ent->d_name, &path_stat);
			if (S_ISDIR(path_stat.st_mode))
			{
				fls.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	if (find(fls.begin(), fls.end(), ".") != fls.end()) fls.erase(find(fls.begin(), fls.end(), "."));
	if (find(fls.begin(), fls.end(), "..") != fls.end()) fls.erase(find(fls.begin(), fls.end(), ".."));
	return fls;
}