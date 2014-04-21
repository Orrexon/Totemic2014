#pragma once

#include <atomic>
#include <string>
#include <vector>

bool CreateDirectoryIfRequired( const std::wstring& directory );

bool TryExecute( const std::wstring& executable, bool show = false );

std::wstring GetPathFromUser( const std::vector<std::wstring>& executables );

bool ConsoleExecute( const std::string& exec, const std::wstring& param, std::atomic_bool& completion_flag, std::atomic_bool& execution_flag );

std::wstring SelectDirectory( const std::wstring& caption );

void RemoveDirectoryA( const std::wstring& directory );

std::wstring GetWorkingDirectory();

void SetWorkingDirectory( const std::wstring& directory );

#include "System.inl"
