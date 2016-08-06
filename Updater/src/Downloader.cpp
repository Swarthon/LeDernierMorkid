#include "Downloader.h"

Downloader::Downloader(std::string address, std::string name){
	mAddress = address;
	mName = name;
	mOut = NULL;
}
bool Downloader::run(){
	CURL* easy;
	CURLM *multi_handle;
	int i;
	int still_running;

	multi_handle = curl_multi_init();

	easy = curl_easy_init();
	setup(easy, mAddress, mName);

	curl_multi_add_handle(multi_handle, easy);
	curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
	curl_multi_perform(multi_handle, &still_running);

	do {
		struct timeval timeout;
		int rc; 
		CURLMcode mc;

		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		int maxfd = -1;

		long curl_timeo = -1;

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		curl_multi_timeout(multi_handle, &curl_timeo);
		if(curl_timeo >= 0) {
			timeout.tv_sec = curl_timeo / 1000;
			if(timeout.tv_sec > 1)
				timeout.tv_sec = 1;
			else
				timeout.tv_usec = (curl_timeo % 1000) * 1000;
		}

		mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

		if(mc != CURLM_OK) {
			fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
			break;
		}

		if(maxfd == -1) {
#ifdef _WIN32
			Sleep(100);
			rc = 0;
#else
			struct timeval wait = { 0, 100 * 1000 };
			rc = select(0, NULL, NULL, NULL, &wait);
#endif
		}
		else {
			rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
		}

		switch(rc) {
		case -1:
			break;
		case 0:
		default:
			curl_multi_perform(multi_handle, &still_running);
	     		break;
		}
	} while(still_running);

	curl_multi_cleanup(multi_handle);
	curl_easy_cleanup(easy);

	if(mOut)
		fclose(mOut);

	return 0;
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
void Downloader::setup(CURL *hnd, std::string address, std::string name){
	char filename[128];

	snprintf(filename, 128, name.c_str());

	mOut = fopen(filename, "wb");

	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, mOut);

	curl_easy_setopt(hnd, CURLOPT_URL, address.c_str());

	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, trace);

	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1);

#if (CURLPIPE_MULTIPLEX > 0)
	curl_easy_setopt(hnd, CURLOPT_PIPEWAIT, 1L);
#endif
}