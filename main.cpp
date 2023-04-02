/*
Beetle Crazy Cup/Beetle Buggin'/Kafer
https://forum.xentax.com/viewtopic.php?p=179690#p179690
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <windows.h>
#include <shlwapi.h>
#include "resource.h"

using namespace std;

std::string openfilename(char* filter = "All Files (*.*)\0*.*\0", HWND owner = NULL, uint32_t flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY) {
	std::string filename(MAX_PATH, '\0');
	OPENFILENAME ofn = { };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = &filename[0];
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File";
	ofn.Flags = flags;
	if (!GetOpenFileName(&ofn)) {return "";}
	return filename;
	}

std::string getFilenamePath (const std::string str) {
	size_t found;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found != std::string::npos) {
		strt = str.substr(0, found);
		return (strt + "\\");
		} else {return "";}
	}

std::string getFilenameFile(const std::string str) {
	size_t found;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find(".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
			}
		} else {strt = str;}
	size_t lastdot = strt.find_last_of(".");
	if (lastdot == std::string::npos) return strt;
	return strt.substr(0, lastdot);
	}

unsigned short readshort (char* &f, size_t &pos) {
	unsigned short result = {
		(unsigned int)((uint8_t)f[pos] * 0x00000001) + \
		(unsigned int)((uint8_t)f[pos+1] * 0x00000100)
		};
	pos += 2;
	return result;
	}

unsigned long readlong (char* &f, size_t &pos) {
	unsigned long result = {
		(unsigned int)((uint8_t)f[pos] * 0x00000001) + \
		(unsigned int)((uint8_t)f[pos+1] * 0x00000100) + \
		(unsigned int)((uint8_t)f[pos+2] * 0x00010000) + \
		(unsigned int)((uint8_t)f[pos+3] * 0x01000000)
		};
	pos += 4;
	return result;
	}

float readfloat (char* &f, size_t &pos) {
	float result = 0.0;
	unsigned long b = readlong(f, pos);
	memcpy(&result, &b, 4);
	return result;
	}

bool openFile (char* &f, size_t &fsize, std::string filename) {
	HANDLE hFile = CreateFile(
		filename.c_str(),		// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL					// no attr. template
		);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to Open File\n";
		return false;
		}
	fsize = GetFileSize(hFile, NULL);
	if (fsize == 0) {
		std::cout << "Failed to read file. File is Empty?\n";
		return false;
		}
	f = new char[fsize];
	unsigned long dwBytesRead = 0;
	if(ReadFile(hFile, f, fsize, &dwBytesRead, NULL) == FALSE || dwBytesRead != fsize) {
		std::cout << "Failed to copy file into memory\n";
		fsize = 0;
		delete[] f;
		f = NULL;
		CloseHandle(hFile);
		return false;
		}
	CloseHandle(hFile);
	}

struct fmtINF {
	uint32_t 												num_vertices;
	uint32_t 												num_triangles;
	float* 													vertices;
	float* 													tvertices;
	float* 													normals;
	uint16_t* 												nor_triangles;
	uint16_t* 												ver_triangles;
	uint32_t* 												unk_triangles;
	fmtINF () {
		vertices 											= NULL;
		tvertices 											= NULL;
		normals 											= NULL;
		nor_triangles 										= NULL;
		ver_triangles 										= NULL;
		unk_triangles 										= NULL;
		}
	~fmtINF () {
		if (vertices = NULL) 								{delete[] vertices;}
		if (tvertices = NULL) 								{delete[] tvertices;}
		if (normals = NULL) 								{delete[] normals;}
		if (nor_triangles = NULL) 							{delete[] nor_triangles;}
		if (ver_triangles = NULL) 							{delete[] ver_triangles;}
		if (unk_triangles = NULL) 							{delete[] unk_triangles;}
		}
	bool read (char* &f, size_t &pos, size_t &fsize, std::string &fpath, std::string &fname) {
		bool sucess 										= false;
		num_vertices 										= readlong(f, pos);
		num_triangles 										= readlong(f, pos);
		std::string verFile 								= fpath + fname + ".ver";
		std::string norFile 								= fpath + fname + ".nor";
		std::string triFile 								= fpath + fname + ".tri";
		if (num_vertices > 0) {
			if (f != NULL) {delete[] f; f = NULL; pos = 0;}
			if (openFile(f, fsize, verFile)) {
				vertices 									= new float [num_vertices * 3];
				tvertices 									= new float [num_vertices * 2];
				for (int i = 0; i < num_vertices; i++) {
					vertices[i * 3] 						= readfloat(f, pos);
					vertices[(i * 3) + 1] 					= readfloat(f, pos);
					vertices[(i * 3) + 2] 					= readfloat(f, pos);
					tvertices[i * 2] 						= readfloat(f, pos);
					tvertices[(i * 2) + 1] 					= readfloat(f, pos);
					}
				}
			if (f != NULL) {delete[] f; f = NULL; pos = 0;}
			if (openFile(f, fsize, norFile)) {
				normals 									= new float [num_vertices * 3];
				for (int i = 0; i < num_vertices; i++) {
					normals[i * 3] 							= readfloat(f, pos);
					normals[(i * 3) + 1] 					= readfloat(f, pos);
					normals[(i * 3) + 2] 					= readfloat(f, pos);
					}
				}
			sucess 											= (num_vertices > 0) || sucess;
			}
		if (num_triangles > 0) {
			if (f != NULL) {delete[] f; f = NULL; pos = 0;}
			if (openFile(f, fsize, triFile)) {
				nor_triangles 								= new uint16_t [num_triangles * 3];
				ver_triangles 								= new uint16_t [num_triangles * 3];
				unk_triangles 								= new uint32_t [num_triangles];
				for (int i = 0; i < num_triangles; i++) {
					ver_triangles[i * 3] 					= readshort(f, pos);
					nor_triangles[i * 3] 					= readshort(f, pos);
					ver_triangles[(i * 3) + 1] 				= readshort(f, pos);
					nor_triangles[(i * 3) + 1] 				= readshort(f, pos);
					ver_triangles[(i * 3) + 2] 				= readshort(f, pos);
					nor_triangles[(i * 3) + 2] 				= readshort(f, pos);
					unk_triangles[i] 						= readlong(f, pos);
					}
				}
			sucess 											= (num_triangles > 0) || sucess;
			}
		return sucess;
		}
	void write_obj (std::string file) {
		std::string line;
		std::ofstream obj(file.c_str());
		int fp = 6;
		if (obj.is_open()) {
			obj << "Beetle Crazy Cup/Beetle Buggin'/Kafer" << std::endl;
			obj << "https://forum.xentax.com/viewtopic.php?p=179690#p179690" << std::endl;
			obj << std::endl << "# Num Vertices " << num_vertices << std::endl;
			for (int i = 0; i < num_vertices; i++) {
				obj << "v " << std::fixed << std::setprecision(fp);
				obj << vertices[(i * 3)] << " ";
				obj << vertices[(i * 3) + 1] << " ";
				obj << vertices[(i * 3) + 2] << std::endl;
				}

			obj << std::endl << "# Num Texture Vertices " << num_vertices << std::endl;
			for (int i = 0; i < num_vertices; i++) {
				obj << "vt " << std::fixed << std::setprecision(fp);
				obj << tvertices[(i * 2)] << " ";
				obj << tvertices[(i * 2) + 1] << std::endl;
				}

			obj << std::endl << "# Num Normals " << num_vertices << std::endl;
			for (int i = 0; i < num_vertices; i++) {
				obj << "n " << std::fixed << std::setprecision(fp);
				obj << normals[(i * 3)] << " ";
				obj << normals[(i * 3) + 1] << " ";
				obj << normals[(i * 3) + 2] << std::endl;
				}

			obj << std::endl << "# Num Faces " << num_triangles << std::endl;
			for (int i = 0; i < num_triangles; i++) {
				obj << "f " << ver_triangles[(i * 3)] + 1 << "/";
				obj << nor_triangles[(i * 3)] + 1 << "/";
				obj << nor_triangles[(i * 3)] + 1 << " ";
				obj << ver_triangles[(i * 3) + 2] + 1 << "/";
				obj << nor_triangles[(i * 3) + 2] + 1 << "/";
				obj << nor_triangles[(i * 3) + 2] + 1 << " ";
				obj << ver_triangles[(i * 3) + 1] + 1 << "/";
				obj << nor_triangles[(i * 3) + 1] + 1 << "/";
				obj << nor_triangles[(i * 3) + 1] + 1 << std::endl;
				}
			obj.close();
			}
		else {cout << "Unable to open file";}
		}
	};

int main(int argc, char* argv[]) {
	std::string file = "";
	if (argc < 2) {
		file = openfilename("INF Files (*.inf)\0*.inf\0All Files (*.*)\0*.*\0");
		}
	else {file = argv[1];}
	if (file != "") {
		char* f 												= NULL;
		size_t fsize 											= 0;
		if (openFile(f, fsize, file)) {
			fmtINF inf;
			size_t ptr 											= 0;
			std::string fpath 									= getFilenamePath(file);
			std::string fname 									= getFilenameFile(file);
			inf.read(f, ptr, fsize, fpath, fname);
			inf.write_obj(fpath + fname + ".obj");
			}
		if (f != NULL) {delete[] f; f = NULL;}
		}
    return 0;
	}
