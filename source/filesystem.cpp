#include "filesystem.h"
Array<byte> file::readAllBytes(string path)
{
	size_t fsize = 0;
	FILE* f = fopen(path.c_str(), "rb");
	if (f == NULL)
	{
		return Array<byte>();
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	unsigned char* fcontent = new unsigned char[fsize];
	fread(fcontent, 1, fsize, f);
	fclose(f);
	Array<byte> ab = Array<byte>(fcontent, fsize);
	delete[] fcontent;
	return ab;
}
byte* file::readAllBytes(string path, int* size)
{
	size_t fsize = 0;
	FILE* f = fopen(path.c_str(), "rb");
	if (f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	unsigned char* fcontent = new unsigned char[fsize];
	fread(fcontent, 1, fsize, f);
	fclose(f);
	*size = fsize;
	return fcontent;
}
string file::readAllText(string path)
{
	return bitconverter::tostring(file::readAllBytes(path));
}
void file::writeAllBytes(string path, Array<byte> data)
{
	ofstream ofile(path);
	ofile.seekp(0, ofile.beg);
	for (int i = 0; i < data.size; i++)
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
	#ifndef __ESP32
		system(("rmdir /S /Q " + path).c_str());
	#else
		vector<string> fls;
		string dirs = util::replaceAll(file::readAllText(util::combinePath(util::pathGetParent(path), ".neutrino")), "\r", "");
		string c = "";
		for(int i = 0; i < dirs.size(); i++)
		{
			if(dirs[i] == '\n')
			{
				fls.push_back(c);
				c = "";
			}
			else
			{
				c += dirs[i];
			}
		}
		if(c != "") fls.push_back(c);
		if(find(fls.begin(), fls.end(), util::replaceAll(util::pathGetLast(path), "\\", "")) != fls.end())
		{
			fls.erase(find(fls.begin(), fls.end(), util::replaceAll(util::pathGetLast(path), "\\", "")));
		}
		dirs = "";
		for(int i = 0; i < fls.size(); i++)
		{
			dirs += fls[i] + "\n";
		}
		file::writeAllText(util::combinePath(util::pathGetParent(path), ".neutrino"), dirs);
	#endif
}
void file::createDirectory(string path)
{
	#ifndef __ESP32
	system(("mkdir " + path).c_str());
	#else
	string n = util::replaceAll(file::readAllText(util::combinePath(util::pathGetParent(path), ".neutrino")), "\r", "");
	if(n[n.size() - 1] != '\n') n += "\n";
	n += util::pathGetLast(path);
	file::writeAllText(util::combinePath(util::pathGetParent(path), ".neutrino"), n);
	file::writeAllText(util::combinePath(path, ".neutrino"), "");
	#endif
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
	#ifndef __ESP32
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	else return false;
	return false;
	#else
	return file::fileExists(util::combinePath(path, ".neutrino"));
	#endif
}
vector<string> file::getFiles(string path)
{
	string np = path;
	if(np[np.size() - 1] == '/') np = np.erase(np.size() - 1);
	vector<string> fls;
	DIR* dir;
	struct dirent* ent;
	string n = "";
	if ((dir = opendir(np.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			#ifndef __ESP32
			struct stat path_stat;
			stat(ent->d_name, &path_stat);
#ifdef __WIN32
			if(ent->d_type == 32768)
#else
			if (S_ISREG(path_stat.st_mode))
#endif
			{
				fls.push_back(ent->d_name);
			}
			#else
			n = ent->d_name;
			if(count(n.begin(), n.end(), '/') == 0)
			{
				if(n != ".neutrino")
				{
					fls.push_back(ent->d_name);
				}
			}
			#endif
		}
		closedir(dir);
	}
	return fls;
}
vector<string> file::getDirectories(string path)
{
	vector<string> fls;
	#ifndef __ESP32
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			struct stat path_stat;
			stat(ent->d_name, &path_stat);
#ifdef __WIN32
			if (ent->d_type == 16384)
#else
			if (S_ISDIR(path_stat.st_mode))
#endif
			{
				fls.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	if (find(fls.begin(), fls.end(), ".") != fls.end()) fls.erase(find(fls.begin(), fls.end(), "."));
	if (find(fls.begin(), fls.end(), "..") != fls.end()) fls.erase(find(fls.begin(), fls.end(), ".."));
	#else
	if(file::fileExists(util::combinePath(path, ".neutrino")))
	{
		string dirs = util::replaceAll(file::readAllText(util::combinePath(path, ".neutrino")), "\r", "");
		string c = "";
		for(int i = 0; i < dirs.size(); i++)
		{
			if(dirs[i] == '\n')
			{
				fls.push_back(c);
				c = "";
			}
			else
			{
				c += dirs[i];
			}
		}
		if(c != "") fls.push_back(c);
	}
	#endif
	return fls;
}