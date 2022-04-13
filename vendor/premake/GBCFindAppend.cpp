#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <filepath> <find_string> <append_string>";
		return 1;
	}

	std::ifstream inFile(argv[1]);
	if (inFile.is_open())
	{
		size_t findStringLength = std::strlen(argv[2]);

		std::string outContents;
		while (!inFile.eof())
		{
			std::string line;
			std::getline(inFile, line);

			outContents += line;
			size_t findIndex = line.find(argv[2]);
			if (findIndex != std::string::npos && line.find(argv[3], findIndex) != findIndex + findStringLength)
				outContents += argv[3];
			outContents += '\n';
		}
		inFile.close();

		std::ofstream outFile(argv[1]);
		outFile.write(outContents.c_str(), outContents.size());
		outFile.close();
	}
	else
	{
		std::cerr << "Invalid filepath=\"" << argv[1] << "\"\n";
		return 2;
	}

	return 0;
}
