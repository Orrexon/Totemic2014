#pragma once

#include <string>
#include <array>
#include <map>
#include <Windows.h>

namespace sf
{
	class RenderWindow;
}


struct OPENFILEINFO
{
	OPENFILEINFO() {
		flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	}

	std::string caption;
	int flags;
	char* filters;
};

class WindowManager
{
public:
	WindowManager(std::string p_title);
	~WindowManager();

	void setWindowIcon(std::string p_filepath);
	sf::RenderWindow* getWindow();

	void setMenu(HMENU menu);
	std::string browseFile(OPENFILEINFO &ofi);
	std::string saveFile(OPENFILEINFO &ofi);
	void setFocus(bool p_value);
	bool isInFocus();
	bool postFocus();
	void setPostFocus(bool p_value);

	template<typename T>
	T CreateZeroed() {
		T t;
		memset(&t, 0, sizeof(T));
		return std::move(t);
	}

	template<typename T, std::size_t N>
	std::array<T, N> CreateZeroed() {
		std::array<T, N> a;
		memset(a.data(), 0, sizeof(T)* N);
		return std::move(a);
	}
private:
	sf::RenderWindow* m_window;
	bool m_inFocus;
	bool m_postFocus;
};

