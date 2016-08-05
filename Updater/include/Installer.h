#ifndef INSTALLER_H
#define INSTALLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <sys/time.h>
#include <unistd.h>

#include <curl/curl.h>

#ifndef CURLPIPE_MULTIPLEX
#define CURLPIPE_MULTIPLEX 0
#endif

class Installer {
public:
	Installer(std::string address);
	bool run();
private:
	static void dump(const char *text, int num, unsigned char *ptr, size_t size, char nohex);
	static int trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);
	static void setup(CURL *hnd, int num, std::string address);

	std::string mAddress;
};

#endif // INSTALLER_H