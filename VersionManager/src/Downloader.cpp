#include "Downloader.h"

namespace VersionManager {
	bool Downloader::download(File dir, std::string url, int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t), void* userData){
		CURL* easy;
		FILE* file;

		char filename[128];

		snprintf(filename, 128, (dir.getPath() + dir.getName()).c_str());
		file = fopen(filename, "wb");
		if(!file){
			std::cout << "Impossible d'ouvrir le fichier " << dir.getPath() + dir.getName() << std::endl;
			return false;
		}


		easy = curl_easy_init();

		curl_easy_setopt(easy, CURLOPT_WRITEDATA, file);

		curl_easy_setopt(easy, CURLOPT_URL, url.c_str());

		curl_easy_setopt(easy, CURLOPT_VERBOSE, 1L);
	//	curl_easy_setopt(easy, CURLOPT_DEBUGFUNCTION, trace);

		curl_easy_setopt(easy, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

		curl_easy_setopt(easy, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(easy, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, 1);

		if(progressFunc)
			curl_easy_setopt(easy, CURLOPT_XFERINFOFUNCTION, progressFunc);
		if(userData)
			curl_easy_setopt(easy, CURLOPT_XFERINFODATA, userData);
		curl_easy_setopt(easy, CURLOPT_NOPROGRESS, 0L);
		CURLcode res = curl_easy_perform(easy);
		if(res != CURLE_OK){
		      fprintf(stderr, "%s\n", curl_easy_strerror(res));
		      return false;
	      	}

		curl_easy_cleanup(easy);

		if(file)
			fclose(file);

		return true;
	}

	// Static methodes
	void Downloader::dump(const char *text, int num, unsigned char *ptr, size_t size, char nohex) {
		size_t i;
		size_t c;

		unsigned int width=0x10;

		if(nohex)
			width = 0x40;

		for(i=0; i<size; i+= width) {
			for(c = 0; (c < width) && (i+c < size); c++) {
				if(nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
					i+=(c+2-width);
					break;
				}
				if(nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
					i+=(c+3-width);
					break;
				}
			}
		}
	}
	int Downloader::trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp){
		const char *text;
		int num = 0;
		(void)handle;
		(void)userp;
		switch (type) {
		case CURLINFO_TEXT:
			fprintf(stderr, "Info: %s", data);
		default:
			return 0;
		}

		dump(text, num, (unsigned char *)data, size, 1);
		return 0;
	}
}
