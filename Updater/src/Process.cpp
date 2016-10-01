#include "Process.h"

std::string Process::process(std::string command){
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
		if(args.size() != 2){
			answer = "Invalid Versions input, missing arguments. Use is Versions [path]";
			return answer;
		}
		Downloader downloader (args[0], "versions.txt");
		downloader.run();

		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			answer += versions[i].mName + ' ';
	}
	else if(prg == "Download"){
		if(args.size() < 1){			
			answer = "Invalid Download input, missing arguments. Use is Download [version(s)]";
			return answer;
		}
		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			for(int j = 0; j < args.size(); j++)
				if(versions[i].mName == args[j])
					versions[i].downloadFile("../../../Versions/");
		answer = "Done";
	}
	else if(prg == "Install"){
		if(args.size() != 2){			
			answer = "Invalid Install input, missing arguments. Use is Install [version]";
			return answer;
		}
		std::vector<Version> versions = Version::createVersions("versions.txt");
		for(int i = 0; i < versions.size(); i++)
			if(versions[i].mName == args[0])
				versions[i].install("LeDernierMorkid.exe", "../../../Versions/", "../../../");
		answer = "Done";
	}
	else if(prg == "Shutdown"){
		exit(0);
	}
	else {
		answer = "Invalid input " + command;
	}


	return answer;
}