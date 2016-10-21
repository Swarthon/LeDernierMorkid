#include "File.h"

namespace VersionManager {

	// Construction methodes
	File::File(){
	}
	File::File(std::string name, std::string path){
		mName = name;
		mPath = path;
	}
	File::~File(){
	}

	// Return value methodes
	std::string File::getName(){
		return mName;
	}
	std::string File::getPath(){
		return mPath;
	}

	// Various methodes
	bool File::exist(){
		std::ifstream ifs;
		ifs.open((mPath + mName).c_str());
		bool b = ifs.good();
		ifs.close();
		return b;
	}
	void File::create(){
		if(!exist()){
			std::ofstream ofs;
			ofs.open((mPath + mName).c_str());
			ofs.close();
		}
	}
	std::string File::getRelativePath(File to){
/*	Algorithm
 *		1. Del shared path
 *		2. For each directory in src add ../
 *		3. Add the dest path
 */

		std::cout << mPath << std::endl << to.getPath() << std::endl;
		
		char* s = new char[400];
		realpath(mPath.c_str(),s);
		std::string src = s;
#ifdef __linux__
		DIR* directory = opendir(s);
		if(directory){
			src += '/';
			closedir(directory);
		}
#elif defined(_WIN32)
		DWORD dwAttrib = GetFileAttributes(s);

		if(dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
			src += '/';
#endif
		delete s;

		char* d = new char[400];
		realpath(to.getPath().c_str(),d);
		std::string dst = d;
#ifdef __linux__
		directory = opendir(d);
		if(directory){
			dst += '/';
			closedir(directory);
		}
#elif defined(_WIN32)
		DWORD dwAttrib = GetFileAttributes(d);

		if(dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
			dst += '/';
#endif
		delete d;

		if(src == dst){
			return "./";
		}

		// Delete the shared parts
		int i = 0, j = 0;
		int i2 = 0, j2 = 0;

		while((unsigned)i < (src.size() > dst.size() ? dst.size() : src.size())){
			std::string sr = src.substr(0, (i2 = src.find('/', i2) + 1));
			std::string ds = dst.substr(0, (j2 = dst.find('/', j2) + 1));

			if(i2 == 0 || j2 == 0 || sr != ds)
				break;
			i = i2;		j = j2;
		}

		std::string resultingPath;
		// Add ../
		while(1){
			int i2 = i;
			i = src.find('/', i2) + 1;
			if(i == 0)
				break;		
			resultingPath += "../";
		}

		resultingPath += dst.substr(j, dst.size());

		return resultingPath;
	}

}