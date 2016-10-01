#include "Version.h"

// Methodes
void Version::downloadFile(std::string path){
	if(mName == std::string() || mFileWebPath == std::string())
		return;
	Downloader d(mFileWebPath, path + mName + ".download");
	d.run();
}
void Version::downloadVersion(std::string path){
	if(mName == std::string() || mVersionWebPath == std::string())
		return;
	Downloader d(mVersionWebPath, path + mName + "version.txt");
	d.run();
}
void Version::install(std::string executable, std::string path, std::string pathExec){
	if(mName == std::string() || mVersionWebPath == std::string())
		return;
	Zip z(mName,path);
	z.extract();

	std::ifstream ifs;
	ifs.open((mName+"_listOfFiles.txt").c_str());
	if(!ifs){
		std::cout << "Unable to find " << path+mName+"_listOfFiles.txt" << ". Make sure you gived the same path to the Version::install and the Version::download\n";
		exit(1);
	}
	std::string executablePath, origin;
	while(ifs && executablePath != executable){
		ifs >> executablePath;
		origin = executablePath;
/*		Example :
 *		executablePath = ../../Test/Example
 *		Then we reverse	: executablePath = elpmaxE/tseT/../..
 *		Then we cut	: executablePath = elpmaxE
 *		Then we reverse	: executablePath = Example
 */
		std::reverse(executablePath.begin(),executablePath.end());
		executablePath = getStringTo(executablePath, '/', false);
		std::reverse(executablePath.begin(),executablePath.end());
		//executablePath = getStringTo(executablePath, '.', false);

		std::cout << std::endl << "origin :\t" << origin << std::endl << "executablePath :\t" << executablePath << std::endl << "executable :\t" << executable << std::endl << std::endl;
	}
	ifs.close();

	if(executablePath == executable)
		createShortcut(origin,pathExec,executable);
}

// Static public methodes
std::vector<Version> Version::createVersions(std::string versionsFile){
	std::vector<Version> versions;
	std::ifstream ifs;
	ifs.open(versionsFile.c_str());

	if(!ifs){
		std::cerr << "Error while opening file " << versionsFile << std::endl;
		exit(-1);
	}

	while(ifs){
		char c;
		c = ifs.get();

		if(c == '#'){
			std::string buffer;
			getline(ifs, buffer);
			continue;
		}

		ifs.putback(c);

		Version v;
		ifs >> v;
		if(v.mName != std::string() || v.mVersionWebPath != std::string() || v.mFileWebPath != std::string())
			versions.push_back(v);
	}

	return versions;
}
Version Version::searchVersionByName(std::vector<Version> versions, std::string name){
	Version v;	
	for(int i = 0; i < versions.size(); i++)
		if(versions[i].mName == name)
			return versions[i];
	std::cout << "No " << name << " found in this vector\n";
	return Version();
}

// Operators
std::istream& operator>>(std::istream& is, Version& v){
	char c;	
	is >> v.mName >> c >> v.mVersionWebPath >> c >> v.mFileWebPath;

	return is;
}

// Static private methodes
void Version::createShortcut(std::string strSrcFile, std::string strDstPath, std::string strName){
#ifdef _WIN32
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;

		// Set the path to the shortcut target and add the description. 
		psl->SetPath(strDstPath.c_str());

		// Query IShellLink for the IPersistFile interface, used for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, strSrcFile.c_str(), -1, wsz, MAX_PATH);

			// Add code here to check return value from MultiByteWideChar 
			// for success.

			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE); 
			ppf->Release(); 
		} 
		psl->Release(); 
		}
#elif defined(__gnu_linux__)
	std::ofstream ofs;
	ofs.open((strDstPath + strName + ".desktop").c_str());
	ofs << "[Desktop Entry]" << std::endl << "Encoding=UTF-8" << std::endl << "Version=1.0" << std::endl << "Type=Link" << std::endl << "Name=" << strName << std::endl << "URL=" << strSrcFile << std::endl << std::endl;
	ofs.close();
#elif defined(__APPLE__) && defined(__MACH__)
	NSWorkspace *pWS = [NSWorkspace sharedWorkspace];
	NSArray *pApps = [pWS launchedApplications];
	bool fFFound = false;
	ProcessSerialNumber psn;
	for (NSDictionary *pDict in pApps){
		if ([[pDict valueForKey:@"NSApplicationBundleIdentifier"]isEqualToString:@"com.apple.finder"]){
			psn.highLongOfPSN = [[pDictvalueForKey:@"NSApplicationProcessSerialNumberHigh"] intValue];
			psn.lowLongOfPSN  = [[pDictvalueForKey:@"NSApplicationProcessSerialNumberLow"] intValue];
			fFFound = true;
			break;
		}
	}
	if (!fFFound)
		return false;
	/* Now the event fun begins. */
	OSErr err = noErr;
	AliasHandle hSrcAlias = 0;
	AliasHandle hDstAlias = 0;
	do {
		/* Create a descriptor which contains the target psn. */
		NSAppleEventDescriptor *finderPSNDesc = [NSAppleEventDescriptor descriptorWithDescriptorType:typeProcessSerialNumber bytes:&psnlength:sizeof(psn)];
		if (!finderPSNDesc)
			break;
		/* Create the Apple event descriptor which points to the Finder target already. */
		NSAppleEventDescriptor *finderEventDesc = [NSAppleEventDescriptor appleEventWithEventClass:kAECoreSuite eventID:kAECreateElement argetDescriptor:finderPSNDesc returnID:kAutoGenerateReturnID transactionID:kAnyTransactionID];
		if (!finderEventDesc)
			break;
		/* Create and add an event type descriptor: Alias */
		NSAppleEventDescriptor *osTypeDesc = [NSAppleEventDescriptor descriptorWithTypeCode:typeAlias];
		if (!osTypeDesc)
			break;
		[finderEventDesc setParamDescriptor:osTypeDesc forKeyword:keyAEObjectClass];
		/* Now create the source Alias, which will be attached to the event. */
		err = FSNewAliasFromPath(nil, [strSrcFile fileSystemRepresentation], 0, &hSrcAlias, 0);
		if (err != noErr)
			break;
		char handleState;
		handleState = HGetState((Handle)hSrcAlias);
		HLock((Handle)hSrcAlias);
		NSAppleEventDescriptor *srcAliasDesc = [NSAppleEventDescriptor descriptorWithDescriptorType:typeAlias bytes:*hSrcAlias length:GetAliasSize(hSrcAlias)];
		if (!srcAliasDesc)
			break;
		[finderEventDesc setParamDescriptor:srcAliasDesc forKeyword:keyASPrepositionTo];
		HSetState((Handle)hSrcAlias, handleState);
		/* Next create the target Alias and attach it to the event. */
		err = FSNewAliasFromPath(nil, [strDstPath fileSystemRepresentation], 0, &hDstAlias, 0);
		if (err != noErr)
			break;
		handleState = HGetState((Handle)hDstAlias);
		HLock((Handle)hDstAlias);
		NSAppleEventDescriptor *dstAliasDesc = [NSAppleEventDescriptor descriptorWithDescriptorType:typeAlias bytes:*hDstAlias length:GetAliasSize(hDstAlias)];
		if (!dstAliasDesc)
			break;
		[finderEventDesc setParamDescriptor:dstAliasDesc forKeyword:keyAEInsertHere];
		HSetState((Handle)hDstAlias, handleState);
		/* Finally a property descriptor containing the target
		* Alias name. */
		NSAppleEventDescriptor *finderPropDesc = [NSAppleEventDescriptor recordDescriptor];
		if (!finderPropDesc)
			break;
		[finderPropDesc setDescriptor:[NSAppleEventDescriptor descriptorWithString:strName] forKeyword:keyAEName];
		[finderEventDesc setParamDescriptor:finderPropDesc forKeyword:keyAEPropData];
		/* Now send the event to the Finder. */
		err = AESend([finderEventDesc aeDesc], NULL, kAENoReply, kAENormalPriority, kNoTimeOut, 0, nil);
	} while(0);
	/* Cleanup */
	if (hSrcAlias)
		DisposeHandle((Handle)hSrcAlias);
	if (hDstAlias)
		DisposeHandle((Handle)hDstAlias);

#endif
}
std::string Version::getStringTo(std::string base, char to, bool includeTo){
	std::string s;
	for(int i = 0; i < base.size() && base[i] != to; i++)
		s += base[i];
	if(includeTo)
		s += to;

	return s;
}
std::string Version::readTo(std::istream& is, char to, bool includeTo){
	std::string s;
	char c = (to == ' ' ? 'a' : ' ');
	while(is && c != to)
		s += (c = is.get());
	if(!includeTo){
		is.putback(s[s.size()-1]);
		s.erase(s[s.size()-1]);
	}
}
