#include "Nictophobia_DirDevice.h"
#include "Nictophobia_PhysicalFileReader.h"

#include <fstream>
#include <filesystem>

using namespace std;

DirDevice::DirDevice(string basepath) : basepath(basepath) {
}

DirDevice::~DirDevice() {
}

FileReader* DirDevice::read(std::string path) {
	std::ifstream* file = new std::ifstream();
	file->open(basepath+"/"+path, ios::in | ios::binary);
	if (!file->is_open()) {
		return nullptr;
	}
	file->exceptions(std::ifstream::badbit);
	return new PhysicalFileReader(file);
}

bool DirDevice::exists(std::string path){
	return std::filesystem::exists(basepath+"/"+path);
}

bool DirDevice::isFile(std::string path){
	return std::filesystem::is_regular_file(basepath+"/"+path);
}

bool DirDevice::isDir(std::string path){
	return std::filesystem::is_directory(basepath+"/"+path);
}

void DirDevice::writeBytes(std::string path, unsigned char* bytes, size_t size) {
	std::ofstream file(basepath+"/"+path, ios::out | ios::binary);
	if (!file.is_open()) {
		std::cerr << "Could not to open file " << basepath << "/" << path << std::endl;
		return;
	}
	file.exceptions(std::ifstream::badbit);
	file.write((char*)bytes, size);
}
