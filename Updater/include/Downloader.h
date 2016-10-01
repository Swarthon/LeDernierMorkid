#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifdef __linux
#include <sys/time.h>
#include <unistd.h>
#endif

#include <curl/curl.h>

#ifndef CURLPIPE_MULTIPLEX
#define CURLPIPE_MULTIPLEX 0
#endif

class Downloader {
public:
	Downloader(std::string address, std::string name);
	bool run();
private:
	static void dump(const char *text, int num, unsigned char *ptr, size_t size, char nohex);
	static int trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);
	void setup(CURL *hnd, std::string address, std::string name);

	std::string mAddress;
	std::string mName;
	FILE* mOut;
};

#endif // DOWNLOADER_H
