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
std::string Pipe::process(std::string command){
	std::string answer;

	std::string prg;
	std::vector<std::string> args;
	std::istringstream iss;
	iss.str(command);

	iss >> prg;
	while(iss){
		std::string s;
		iss >> s;
		args.push_back(s);
	}

	if(prg == "Versions"){
		Downloader downloader (args[0], "versions.txt");
		downloader.run();

		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			answer += versions[i].mName + ' ';
	}
	else if(prg == "Download"){
		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			if(versions[i].mName == args[0])
				versions[i].downloadFile("../Versions/");
		answer = "Done";
	}
	else if(prg == "Install"){
		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			if(versions[i].mName == args[0])
				versions[i].install("LeDernierMorkid.exe");
		answer = "Done";
	}
	else if(prg == "Shutdown"){
		exit(0);
	}
	else {
		answer = "Invalid pipe input " + command;
		std::cout << answer << std::endl;
	}


	return answer;
}