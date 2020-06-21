#include <iostream>
#include "LEAN.h"
#include <fstream>
#include <cxxopts.hpp>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
	cxxopts::Options options("leanfs", "Creates and modifies a LEAN file system image");
	options.add_options()("s,size", "Size of the image in sectors", cxxopts::value<uint32_t>()->default_value("256"))
		("b,sbloc", "Location to put the superblock, blocks off all of the sectors before it", cxxopts::value<uint32_t>()->default_value("0"))
		("p,prealloc", "Amount to preallocate for blocks", cxxopts::value<uint8_t>()->default_value("2"))
		("d,dir", "Creates a directory (dir,dir,dir...)", cxxopts::value<std::string>())
		("i,insert", "Insert a file into the image (file,file,file...)", cxxopts::value<std::vector<std::string>>())
		("v,volume", "Set the volume name", cxxopts::value<std::string>()->default_value(argv[argc - 1]))
		("r,raw", "Add a raw file at a specific sector (file,sector,file,sector...)", cxxopts::value<std::vector<std::string>>())
		("h,help", "Displays this help message");

	if (argc < 2)
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	LeanFS* leanFs = new LeanFS(argv[argc - 1]);

	auto result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	leanFs->mkfs(result["prealloc"].as<uint8_t>(), 12, result["sbloc"].as<uint32_t>(), result["size"].as<uint32_t>(), result["volume"].as<std::string>());

	//const char* test = "This is some string for the text file\n";
	//leanFs->addFile("this/is/a/test/file.txt", (uint8_t*)test, strlen(test));

	if (result.count("raw"))
	{
		std::vector<std::string> rawFiles = result["raw"].as<std::vector<std::string>>();
		std::cout << "Adding raw files " << rawFiles.size() << std::endl;

		for (auto it = rawFiles.begin(); it < rawFiles.end(); )
		{
			std::string file = *it;
			it++;
			int sector = atoi((*it).c_str());
			std::cout << "Raw File " << file << ", " << sector << std::endl;
			it++;

			std::fstream rFile;
			rFile.open(file, std::fstream::binary | std::fstream::in);

			rFile.seekg(0, rFile.end);
			int length = rFile.tellg();
			rFile.seekg(0, rFile.beg);

			char* data = new char[length];

			rFile.read(data, length);
			rFile.close();

			leanFs->directWrite((uint8_t*)data, length, sector);

			delete data;
		}
	}

	if (result.count("insert"))
	{
		std::vector<std::string> rawFiles = result["insert"].as<std::vector<std::string>>();
		std::cout << "Adding files " << rawFiles.size() << std::endl;

		for (auto it = rawFiles.begin(); it < rawFiles.end(); )
		{
			std::string file = *it;
			it++;
			std::cout << "File " << file << std::endl;

			std::fstream rFile;
			rFile.open(file, std::fstream::binary | std::fstream::in);

			rFile.seekg(0, rFile.end);
			int length = rFile.tellg();
			rFile.seekg(0, rFile.beg);

			char* data = new char[length];

			rFile.read(data, length);
			rFile.close();

			leanFs->addFile(file, (uint8_t*)data, length, false);

			delete data;
		}
	}
}