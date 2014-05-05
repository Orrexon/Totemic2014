#if !defined( _WIN32 )
#error This file should not be built on a non-Win32 system.
#endif

#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include "System.hpp"

bool CreateDirectoryIfRequired( const std::wstring& directory ) {
	if( directory.length() > 247 ) {
		std::cout << "CreateDirectoryIfRequired error: Path too long.\n";
	}

	auto pos = directory.find( '/' );

	while( pos != std::wstring::npos ) {
		std::wstring current_segment = directory.substr( 0, pos );

		if( !CreateDirectoryW( current_segment.c_str(), nullptr ) ) {
			auto error = GetLastError();

			if( error != ERROR_ALREADY_EXISTS ) {
				std::wcout << L"CreateDirectoryIfRequired couldn't create directory (" << current_segment << L"): " << error << L"\n";
				return false;
			}
		}

		pos = directory.find( '/', pos + 1 );
	}

	return true;
}

bool TryExecute( const std::wstring& executable, bool show ) {
	auto result = reinterpret_cast<int>( ShellExecuteW( nullptr, L"open", executable.c_str(), L"", nullptr, show ? SW_SHOW : SW_HIDE ) );

	std::wcout << "Trying to execute " << executable << ": " << result << "\n";

	if( result > 32 ) {
		return true;
	}

	return false;
}

std::wstring GetPathFromUser( const std::vector<std::wstring>& executables ) {
	auto strTitle = std::wstring{ L"Where is " };

	if( executables.empty() ) {
		std::cout << "No filter specified.\n";
		return L"";
	}
	else if( executables.size() == 1 ) {
		strTitle += executables.front();
	}
	else {
		auto end = std::end( executables );
		for( auto iter = std::begin( executables ); ; ) {
			strTitle += *iter;

			++iter;

			if( iter != end ) {
				strTitle += L" or ";
			}
			else {
				break;
			}
		}
	}

	strTitle += L"?";

	auto strFilter = std::wstring{ L"" };

	for( const auto& e : executables ) {
		strFilter += e + L'\0' + e + L'\0';
	}

	auto filename = CreateZeroed<TCHAR, 65536>();
	auto open_filename = CreateZeroed<OPENFILENAME>();

	open_filename.lStructSize = sizeof( OPENFILENAME );
	open_filename.lpstrFilter = strFilter.c_str();
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = strTitle.c_str();
	open_filename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	auto working_directory = GetWorkingDirectory();

	auto result = GetOpenFileName( &open_filename );

	SetWorkingDirectory( working_directory );

	if( !result ) {
		return L"";
	}

	return std::wstring( filename.data() );
}

bool ConsoleExecute( const std::wstring& exec, const std::wstring& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag ) {
	
	return true;
}

std::wstring SelectDirectory( const std::wstring& caption ) {
	std::wstring directory;

	CoInitialize( nullptr );

	auto dir_string = CreateZeroed<TCHAR, MAX_PATH>();
	auto browse_info = CreateZeroed<BROWSEINFO>();

	browse_info.pszDisplayName = dir_string.data();
	browse_info.lpszTitle = caption.c_str();
	browse_info.ulFlags = BIF_USENEWUI;
	browse_info.iImage = -1;

	auto item_id_list = SHBrowseForFolder( &browse_info );

	if( item_id_list ) {
		SHGetPathFromIDList( item_id_list, dir_string.data() );
		directory = dir_string.data();
	}

	CoUninitialize();

	std::replace( std::begin( directory ), std::end( directory ), '\\', '/' );

	return directory;
}

void RemoveDirectoryA( const std::wstring& directory ) {
	if( directory.empty() ) {
		return;
	}

	std::wcout << L"Removing " << directory << L"\n";

	auto file_operation = CreateZeroed<SHFILEOPSTRUCT>();
	auto from = directory;

	if( from.back() == '/' ) {
		from.pop_back();
	}

	from += L'\0';

	file_operation.wFunc = FO_DELETE;
	file_operation.pFrom = from.c_str();
	file_operation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	auto result = SHFileOperationW( &file_operation );

	if( result ) {
		std::wcout << "RemoveDirectory error: " << result << "\n";
	}

	std::wcout << "Removed " << directory << "\n";
}

std::wstring GetWorkingDirectory() {
	auto dir = CreateZeroed<TCHAR, MAX_PATH>();

	GetCurrentDirectoryW( dir.size(), dir.data() );

	std::replace( std::begin( dir ), std::end( dir ), '\\', '/' );

	return dir.data();
}

void SetWorkingDirectory( const std::wstring& directory ) {
	SetCurrentDirectoryW( directory.c_str() );
}
