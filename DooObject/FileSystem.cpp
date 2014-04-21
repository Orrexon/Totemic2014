#include "FileSystem.h"
#include <Windows.h>
#include <iostream>
#include <direct.h>
#include <string>
#include <algorithm>

namespace FileSystem
{
	std::vector<std::string> getFilesFromDirectory(const std::string &directory, const std::string &refcstrExtension)
	{
		std::string              strPattern;
		std::string              extension;
		std::vector<std::string> files;
		HANDLE                   hFile;
		WIN32_FIND_DATAA         fileInformation;

		strPattern = directory + "\\*.*";
		hFile = FindFirstFileA(strPattern.c_str(), &fileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (fileInformation.cFileName[0] != '.')
				{
					// Check extension
					extension = fileInformation.cFileName;
					extension = extension.substr(extension.rfind(".") + 1);
					
					if (extension == "*" || extension == refcstrExtension)
					{
						files.push_back(fileInformation.cFileName);
					}
				}
			} while (FindNextFileA(hFile, &fileInformation) == TRUE);
			FindClose(hFile);
		}
		return files;
	}

	int countFiles(const std::string &refcstrRootDirectory, const std::string &refcstrExtension, bool bSubdirectories)
	{
		int              iCount = 0;
		std::string      strFilePath;          // Filepath
		std::string      strPattern;           // Pattern
		std::string      strExtension;         // Extension
		HANDLE           hFile;                // Handle to file
		WIN32_FIND_DATAA FileInformation;      // File information


		strPattern = refcstrRootDirectory + "\\*.*";
		hFile = FindFirstFileA(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FileInformation.cFileName[0] != '.')
				{
					strFilePath.erase();
					strFilePath = refcstrRootDirectory +
						"\\" +
						FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bSubdirectories)
						{
							// Search subdirectory
							int iRC = countFiles(strFilePath,
								refcstrExtension,
								bSubdirectories);
							if (iRC != -1)
								iCount += iRC;
							else
								return -1;
						}
					}
					else
					{
						// Check extension
						strExtension = FileInformation.cFileName;
						strExtension = strExtension.substr(strExtension.rfind(".") + 1);

						if ((refcstrExtension == "*") ||
							(strExtension == refcstrExtension))
						{
							// Increase counter
							++iCount;
						}
					}
				}
			} while (FindNextFileA(hFile, &FileInformation) == TRUE);

			// Close handle
			FindClose(hFile);
		}

		return iCount;
	}

	void SplitStr2Vec(const char* pszPath, std::vector<std::string>& vecString)
	{
		char * pch;
		pch = strtok(const_cast < char*> (pszPath), STR_TOKEN);
		while (pch != NULL)
		{
			vecString.push_back(pch);
			pch = strtok(NULL, STR_TOKEN);
			std::transform(vecString.back().begin(), vecString.back().end(), vecString.back().begin(), tolower);
		}
	}

	std::string getRelativePath(const char* pszPath1, const char* pszPath2)
	{
		std::vector<std::string> vecPath1, vecPath2;
		vecPath1.clear();
		vecPath2.clear();
		SplitStr2Vec(pszPath1, vecPath1);
		SplitStr2Vec(pszPath2, vecPath2);
		size_t iSize = (vecPath1.size() < vecPath2.size()) ? vecPath1.size() : vecPath2.size();
		unsigned int iSameSize(0);
		for (unsigned int i = 0; i<iSize; ++i)
		{
			if (vecPath1[i] != vecPath2[i])
			{
				iSameSize = i;
				break;
			}
		}

		std::string m_strRelativePath = "";
		for (unsigned int i = 0; i< (vecPath1.size() - iSameSize); ++i)
			m_strRelativePath += const_cast<char *> (LAST_FOLDER);

		for (unsigned int i = iSameSize; i<vecPath2.size(); ++i)
		{
			m_strRelativePath += vecPath2[i];
			if (i < (vecPath2.size() - 1))
				m_strRelativePath += const_cast<char *> (FOLDER_SEP);
		}

		return m_strRelativePath;
	}
	
	std::string getCurrentDirectory()
	{
		CHAR NPath[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, NPath);
		return NPath;
	}
}