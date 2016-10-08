#include "Version.h"

namespace VersionManager {

	// Construction methodes
	Version::Version(){
		mDownloaded = false;
		mInstalled = false;
	}
	Version::Version(std::string name, std::string url, std::string downloadPath, std::string installPath, bool downloaded, bool installed){
		mName = name;
		mURL = url;
		mInstallPath = installPath;
		mDownloadPath = downloadPath;
		mDownloaded = downloaded;
		mInstalled = installed;
	}
	Version::~Version(){
	}

	// Various methodes
	bool Version::download(){
		if(!mDownloaded && !mInstalled){
			mDownloaded = Downloader::download(File(mName+".download",mDownloadPath),mURL);
			if(!mDownloaded)
				return false;
		}
		return true;
	}
	bool Version::install(std::string execName, std::string shortcutName, std::string execPath){
		if(mDownloaded && !mInstalled){
			if(!Zipper::unzip(File(mName+".download",mDownloadPath),File(std::string(),mInstallPath), mFiles))
				return false;
			for(unsigned int i = 0; i < mFiles.size(); i++){
				if(execName == mFiles[i].getName()){
					createShortcut(File(std::string(),execPath+mInstallPath).getRelativePath(mFiles[i])+execName, execPath, shortcutName);
					break;
				}
			}
			mInstalled = true;
		}
		return true;
	}

	// Return value methodes
	std::string Version::getName(){
		return mName;
	}
	std::string Version::getDownloadPath(){
		return mDownloadPath;
	}
	std::string Version::getInstallPath(){
		return mInstallPath;
	}
	std::string Version::getURL(){
		return mURL;
	}

	// Static protected methodes
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

}