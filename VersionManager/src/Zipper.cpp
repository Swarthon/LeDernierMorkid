#include "Zipper.h"

namespace VersionManager {

	bool Zipper::unzip(File src, File dst, std::vector<File>& listOfFiles){
		struct zip *za;
		struct zip_file *zf;
		struct zip_stat sb;
		char buf[100];
		int err;
		int len;
		int fd;
		unsigned long long sum;

		if ((za = zip_open((src.getPath()+src.getName()).c_str(), 0, &err)) == NULL) {
			zip_error_to_str(buf, sizeof(buf), err, errno);
			fprintf(stderr, "Can't open zip archive `%s': %s\n", (src.getPath()+src.getName()).c_str(), buf);
			return false;
		}

		for (int i = 0; i < zip_get_num_entries(za, 0); i++) {
			if (zip_stat_index(za, i, 0, &sb) == 0) {
				std::string path = dst.getPath() + dst.getName() + std::string(sb.name);
				len = strlen(path.c_str());
				if (path.c_str()[len - 1] == '/')
					createDir(path);
				else {
					zf = zip_fopen_index(za, i, 0);
					if (!zf) {
						fprintf(stderr, "Could not open zip index\n");
						return false;
					}

	#ifdef __linux__
					fd = open(path.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);
	#elif defined(_WIN32)
					fd = _sopen_s(&fd, path.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0,0);
	#endif

					if (fd < 0) {
						fprintf(stderr, "Could not open file %s\n", path.c_str());
						return false;
					}

					sum = 0;
					while (sum != sb.size) {
						len = zip_fread(zf, buf, 100);
						if (len < 0) {
							fprintf(stderr, "The length of the archive isn't good\n");
							return false;
						}
	#ifdef _WIN32
						_write(fd, buf, len);
	#elif defined(__linux__)
						write(fd, buf, len);
	#endif
						sum += len;
					}

					std::string name;
					std::size_t found = path.rfind('/');
					if(found != std::string::npos){
						for(unsigned int i = found + 1; i < path.size(); i++)
							name += path[i];
						path = path.substr(0,path.size()-name.size());
					}
					listOfFiles.push_back(File(name,path));

	#ifdef __linux__
					close(fd);
	#elif defined(_WIN32)
					_close(fd);
	#endif
					zip_fclose(zf);
				}
			}
		}

		if (zip_close(za) == -1)
			fprintf(stderr, "Can't close zip archive `%s'\n", (src.getPath() + src.getName()).c_str());
		return true;
	}

	// Static private methodes
	void Zipper::createDir(std::string dir){
	#ifdef __linux__
		if (mkdir(dir.c_str(), 0755) < 0) {
	#elif defined(_WIN32)
		if (!CreateDirectory(dir.c_str(), NULL)){
	#endif
			if (errno != EEXIST) {
				perror(dir.c_str());
				exit(1);
			}
		}
	}

}