#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

#ifdef __linux
#include <sys/time.h>
#include <unistd.h>
#endif

#include <curl/curl.h>

#ifndef CURLPIPE_MULTIPLEX
#define CURLPIPE_MULTIPLEX 0
#endif

// My includes
#include "File.h"

namespace VersionManager {

	class Downloader {
	public:
		static bool download(File dir, std::string url, int(*progressFunc)(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t) = NULL, void* userData = NULL);
	protected:
		static void dump(const char *text, int num, unsigned char *ptr, size_t size, char nohex);
		static int trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);
	private:
		Downloader();
		virtual ~Downloader() = 0;
	};

}

#endif // DOWNLOADER_H
