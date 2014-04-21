#pragma once
#include <string>
#include <vector>

namespace FileSystem
{
	#ifdef _WIN32                                                                              
	#define STR_TOKEN "\\"                                                                 
	#define LAST_FOLDER "..\\"                                                             
	#define FOLDER_SEP "\\"                                                                
	#define LINE_BREAK "\r\n"                                                              
	#else                                                                                      
	#define STR_TOKEN "/"                                                                  
	#define LAST_FOLDER "../"                                                              
	#define FOLDER_SEP "/"                                                                 
	#define LINE_BREAK "/n"                                                               
	#endif // _WIN32                                                                           

	std::vector<std::string> getFilesFromDirectory(const std::string &directory, const std::string &refcstrExtension);
	int countFiles(const std::string &refcstrRootDirectory, const std::string &refcstrExtension, bool bSubdirectories = true);
	std::string getRelativePath(const char* pszPath1, const char* pszPath2);
	std::string getCurrentDirectory();
}