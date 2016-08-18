#include "Pipe.h"

void Pipe::run(const char* pipeName){
	while(1){
		std::string command = read(pipeName);
		std::string answer = process(command);
		send(pipeName, answer);
	}
}

std::string Pipe::read(const char* pipeName){
	char buf[512];
	int fdr = open(pipeName, O_RDONLY);
	::read(fdr, buf, sizeof(buf));
	close(fdr);

	return std::string(buf);
}
void Pipe::send(const char* pipeName, std::string data){
	int fdw = open(pipeName, O_WRONLY);
	write(fdw, data.c_str(), 512);
	close(fdw);
}