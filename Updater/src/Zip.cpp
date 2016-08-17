#include "Zip.h"

Zip::Zip(std::string archiveName, std::string path){
	mName = archiveName;
	mPath = path;
}

void Zip::extract(){
	struct zip *za;
	struct zip_file *zf;
	struct zip_stat sb;
	char buf[100];
	int err;
	int len;
	int fd;
	long long sum;

	if ((za = zip_open((mName + ".download").c_str(), 0, &err)) == NULL) {
		zip_error_to_str(buf, sizeof(buf), err, errno);
		fprintf(stderr, "Can't open zip archive `%s': %s/n", (mName + ".download").c_str(), buf);
		return;
	}

	std::vector<std::string> listOfFiles;

	for (int i = 0; i < zip_get_num_entries(za, 0); i++) {
		if (zip_stat_index(za, i, 0, &sb) == 0) {
			std::string path = mPath + std::string(sb.name);
			len = strlen(path.c_str());
			if (path.c_str()[len - 1] == '/')
				safe_create_dir(path.c_str());
			else {
				zf = zip_fopen_index(za, i, 0);
				if (!zf) {
				    fprintf(stderr, "Could not open zip index\n");
				    exit(100);
				}

				fd = open(path.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);
				if (fd < 0) {
					fprintf(stderr, "Could not open file %s\n", path.c_str());
					exit(101);
				}

				sum = 0;
				while (sum != sb.size) {
					len = zip_fread(zf, buf, 100);
					if (len < 0) {
						fprintf(stderr, "The length of the archive isn't good\n");
						exit(102);
					}
					write(fd, buf, len);
					sum += len;
				}

				listOfFiles.push_back(path);

				close(fd);
				zip_fclose(zf);
			}
		}
	}

	std::ofstream ofs;
	ofs.open((mName + "_listOfFiles.txt").c_str());
	for(int i = 0; i < listOfFiles.size(); i++)
		ofs << listOfFiles[i] << std::endl;
	ofs.close();

	if (zip_close(za) == -1)
		fprintf(stderr, "Can't close zip archive `%s'/n", (mName + ".download").c_str());
}

// Static private methodes
void Zip::safe_create_dir(const char *dir){
	if (mkdir(dir, 0755) < 0) {
		if (errno != EEXIST) {
			perror(dir);
			exit(1);
		}
	}
}