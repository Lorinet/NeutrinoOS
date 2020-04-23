#include "nfsdimp.h"
map<int, nfs> nfsmgr::handles;
int nfsmgr::OpenImage(string filename, int process)
{
	/*for (pair<const int, nfs> const &p : handles)
	{
		if (p.second.fileName == filename) return -1;
	}
	int index = 0;
	while (handles.find(index) != handles.end()) index++;
	handles.emplace(index, nfs(filename));
	handles[index].ownerProcess = process;
	return index;*/
	return 0;
}
int nfsmgr::OpenImage(nfs n, int process)
{
	/*for (pair<const int, nfs> const& p : handles)
	{
		if (p.second.fileName == n.fileName) return -1;
	}
	int index = 0;
	while (handles.find(index) != handles.end()) index++;
	handles.emplace(index, n);
	handles[index].ownerProcess = process;
	return index;*/
	return 0;
}
void nfsmgr::CloseImage(int hndl, int process)
{
	if (handles.find(hndl) != handles.end())
	{
		if (handles[hndl].ownerProcess == process)
		{
			handles.erase(hndl);
		}
	}
}
nfs* nfsmgr::GetNFS(int hndl, int process)
{
	if (handles.find(hndl) != handles.end())
	{
		if (handles[hndl].ownerProcess == process)
		{
			return &handles[hndl];
		}
	}
	return NULL;
}
nfs::nfs()
{

}
nfs::nfs(string file)
{
	fileName = file;
	string filename = lvmgr::formatPath(file);
	fileio = fstream(filename, ios::binary | ios::ate | ios::in | ios::out);
	fileio.seekg(0, ios::end);
	if (fileio.tellg() < 3) return;
	fileio.seekg(0, ios::beg);
	byte* sig = new byte[3];
	fileio.read((char*)sig, 3);
	if (sig[0] == 'N' && sig[1] == 'F' && sig[2] == 'S')
	{
		sig = new byte[4];
		fileio.read((char*)sig, 4);
		int offset = bitconverter::toint32(sig, 0);
		files = map<string, nfs_file>();
		int pos = 0;
		while (pos < offset)
		{
			string fname = "";
			char fc;
			fileio.read(&fc, 1);
			while (fc != 0)
			{
				fname += fc;
				fileio.read(&fc, 1);
				pos += 1;
			}
			pos += 1;
			fileio.read((char*)sig, 4);
			int org = bitconverter::toint32(sig, pos);
			pos += 4;
			fileio.read((char*)sig, 4);
			int end = bitconverter::toint32(sig, pos);
			pos += 4;
			files.insert({ fname, nfs_file(fname, end - org, org - offset) });
		}
	}
}
nfs nfs::CreateFSImage(string file)
{
	Array<byte> b = Array<byte>();
	b.add('N'); b.add('F'); b.add('S'); b.add(0); b.add(0); b.add(0); b.add(0);
	file::writeAllBytes(lvmgr::formatPath(file), b);
	return nfs(file);
}
nfs nfs::CreateFSImage(string file, vector<string> files)
{
	Array<byte> b = Array<byte>();
	b.add('N'); b.add('F'); b.add('S'); b.add(0); b.add(0); b.add(0); b.add(0);
	file::writeAllBytes(lvmgr::formatPath(file), b);
	nfs n = nfs(file);
	for (string s : files) n.AddFile(s);
	return n;
}
void nfs::AddFile(string file)
{
	Array<byte> v = file::readAllBytes(file);
	files.insert({ file, nfs_file(file, v.size, 0) });
	files[file].contents = bitconverter::copy(v.holder, v.size);
	Commit();
}
void nfs::AddFiles(vector<string> filelist)
{
	for (string s : filelist)
	{
		Array<byte> v = file::readAllBytes(s);
		files.insert({ s, nfs_file(s, v.size, 0) });
		files[s].contents = bitconverter::copy(v.holder, v.size);
	}
	Commit();
}
void nfs::RemoveFile(string file)
{
	files.erase(file);
	Commit();
}
void nfs::RemoveFiles(vector<string> filelist)
{
	for (string s : filelist)
	{
		files.erase(s);
	}
	Commit();
}
void nfs::MoveFile(string file1, string file2)
{
	files.insert({ file2, files[file1] });
	files.erase(file1);
}
void nfs::CopyFile(string file1, string file2)
{
	files.insert({ file2, files[file1] });
}
bool nfs::FileExists(string file)
{
	return files.find(file) != files.end();
}
vector<string> nfs::GetFiles()
{
	vector<string> v;
	for (pair<string, nfs_file> p : files)
	{
		v.push_back(p.first);
	}
	return v;
}
int nfs::FileSize(string file)
{
	return files[file].size;
}
int nfs::ImageSize()
{
	int s = 0;
	for (pair<string, nfs_file> p : files)
	{
		s += p.second.size;
	}
	return s;
}
void nfs::ExtractFile(string filename, string destination)
{
	LoadFile(filename);
	string dest = destination;
	if (dest[dest.size() - 1] != '\\') dest += '\\';
	file::writeAllBytes(lvmgr::formatPath(dest + filename), Array<byte>(files[filename].contents, files[filename].size));
}
void nfs::ExtractAllFiles(string destination)
{
	for (pair<string, nfs_file> p : files) ExtractFile(p.first, destination);
}
Array<byte> nfs::ReadFile(string filename)
{
	LoadFile(filename);
	return Array<byte>(files[filename].contents, files[filename].size);
}
void nfs::WriteFile(string filename, vector<byte> data)
{
	files[filename].contents = reinterpret_cast<byte*>(data.data());
	files[filename].loaded = true;
	Commit();
}
void nfs::LoadFile(string file)
{
	fileio.seekg(files[file].offset, ios::beg);
	files[file].contents = new byte[files[file].size];
	fileio.read((char*)files[file].contents, files[file].size);
	files[file].loaded = true;
}
void nfs::LoadAllFiles()
{
	for (pair<string, nfs_file> p : files)
	{
		if(!p.second.loaded) LoadFile(p.first);
	}
}
void nfs::Commit()
{
	LoadAllFiles();
	int offset = 0;
	for (pair<string, nfs_file> p : files)
	{
		offset += p.first.size() + 9;
	}
	fileio.clear();
	fileio.seekp(0, ios::beg);
	fileio.write("NFS", 3);
	vector<byte> off = bitconverter::toarray(offset);
	byte* b = new byte[4];
	b[0] = off[0];
	b[1] = off[1];
	b[2] = off[2];
	b[3] = off[3];
	fileio.write((char*)b, 4);
	offset = 0;
	files.clear();
	int sta = 0;
	for (pair<string, nfs_file> p : files)
	{
		fileio.write(p.first.c_str(), p.first.size());
		fileio.write("\0", 1);
		off = bitconverter::toarray(offset);
		b[0] = off[0];
		b[1] = off[1];
		b[2] = off[2];
		b[3] = off[3];
		fileio.write((char*)b, 4);
		sta = offset;
		offset += p.second.size;
		off = bitconverter::toarray(offset);
		b[0] = off[0];
		b[1] = off[1];
		b[2] = off[2];
		b[3] = off[3];
		fileio.write((char*)b, 4);
		files[p.first].offset = sta;
	}
	for (pair<string, nfs_file> p : files)
	{
		fileio.write((char*)p.second.contents, p.second.size);
	}
}
void nfs::Erase()
{
	files.clear();
	Commit();
}
void nfs::Release()
{
	fileio.close();
	fileio.close();
}
nfs_file::nfs_file()
{

}
nfs_file::nfs_file(string n, int s, int off)
{
	name = n;
	size = s;
	offset = off;
}