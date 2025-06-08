#include <system/Extensions.h>

#include <error/ClavisError.h>
#include <settings/Settings.h>

#include <language/Language.h>

#include <fstream>

#ifdef _WIN32
// Windows does not define the S_ISREG and S_ISDIR macros in stat.h, so we do.
// We have to define _CRT_INTERNAL_NONSTDC_NAMES 1 before #including sys/stat.h
// in order for Microsoft's stat.h to define names like S_IFMT, S_IFREG, and S_IFDIR,
// rather than just defining _S_IFMT, _S_IFREG, and _S_IFDIR as it normally does.
#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#include <direct.h>

#include <shlobj.h>
#include <Windows.h>

#elif defined __LINUX__
#include <sys/stat.h>
#include <gdkmm/clipboard.h>
#include <gdkmm/display.h>
#endif


namespace fs = std::filesystem;

namespace Clavis::System {
	bool CopyToClipboard(const std::string& text) {
#ifdef __WINDOWS__
		OpenClipboard(nullptr);
		EmptyClipboard();

		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
		if (!hg) {
			CloseClipboard();
			return false;
		}

		auto lock = GlobalLock(hg);
		if (lock == nullptr) {
			CloseClipboard();
			GlobalFree(hg);
			return false;
		}

		memcpy(lock, text.c_str(), text.size() + 1);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();

		GlobalUnlock(hg);
		GlobalFree(hg);

		return true;
#elif defined __LINUX__
		const auto display = Gdk::Display::get_default();
		if (!display) {
			return false;
		}

		const auto clipboard = display->get_clipboard();
		clipboard->set_text(text);

		return true;
#else
		RaiseClavisError(_(ERROR_UNKNOWN_PLATFORM));
#endif
	}

#ifdef __WINDOWS__
	std::string UnicodeToUTF8(const std::wstring& str) {
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
		std::string utf8_str(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &utf8_str[0], size_needed, nullptr, nullptr);
		return utf8_str;
	}
	std::wstring UTF8ToUnicode(const std::string& str) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
		std::wstring unicode_str(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &unicode_str[0], size_needed);
		return unicode_str;
	}
#endif

	std::filesystem::path GetHomeFolder() {
#ifdef __WINDOWS__
		// Get documents folder in Microsoft's wchar format.
		WCHAR my_documents[MAX_PATH];
		SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, my_documents);

		std::wstring ws(my_documents);
		auto s = UnicodeToUTF8(ws);
		return std::filesystem::path(s);
#elif defined __LINUX__
		return std::string(getenv("HOME"));
#endif
	}

#ifdef __WINDOWS__
	std::filesystem::path GetAppDataFolder() {
		// Get documents folder in Microsoft's wchar format.
		WCHAR my_documents[MAX_PATH];
		SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);

		std::wstring ws(my_documents);
		auto s = UnicodeToUTF8(ws);
		return std::filesystem::path(s);
		return std::string(getenv("HOME"));
	}
#endif

	std::filesystem::path GetConfigFolder() {
#ifdef __WINDOWS__
		return GetAppDataFolder();
#elif defined __LINUX__
		return GetHomeFolder() / ".config";
#endif
	}

    std::filesystem::path GetClavisConfigFolder() {
		return GetConfigFolder() / "Clavis";
	}

	std::filesystem::path GetPasswordStoreDefaultFolder() {
#ifdef __LINUX__
		return GetHomeFolder() / ".password-store";
#elif defined __WINDOWS__
		return GetHomeFolder() / "ClavisPasswords";
#endif
	}

	std::filesystem::path GetGPGIDPath() {
		return GetPasswordStoreFolder() / ".gpg-id";
	}


	std::filesystem::path GetPasswordStoreFolder() {
		return Settings::PASSWORD_STORE_PATH.GetValue();
	}


	json JSONParseFile(const std::filesystem::path& path) {
		std::ifstream file;
		file.open(path.string(), std::ios::in);

		if (!file.good())
			RaiseClavisError(_(ERROR_OPENING_FILE_FOR_READING, path.string()));

		return json::parse(file);
	}

	bool FileExists(const std::filesystem::path& path)
	{
		auto s = path.string();
		auto c = s.c_str();

		struct stat buffer{};
		return (stat(c, &buffer) == 0 && S_ISREG(buffer.st_mode));
	}

	bool DirectoryExists(const std::filesystem::path& path)
	{
		auto s = path.string();
		auto c = s.c_str();

		struct stat buffer{};
		return (stat(c, &buffer) == 0 && S_ISDIR(buffer.st_mode));
	}

	bool mkdir_impl(const std::filesystem::path& path) {
		auto s = path.string();

#ifdef _WIN32
		if (!CreateDirectory(s.c_str(), nullptr)) {
			if (GetLastError() != ERROR_ALREADY_EXISTS) {
#elif defined __LINUX__
		if (mkdir(s.c_str(), 0755) != 0) {
			if (errno != EEXIST) {
#endif
				return false;
			}
		}

		return true;

	}

	bool mkdir_p(const std::filesystem::path& directory) {
		auto rootpath = directory.root_path();
		fs::path currentPath;
		bool isRootPathFormed = false;

		for (const auto& component : directory) {
			currentPath /= component;

			if (currentPath == directory.root_path())
				isRootPathFormed = true;
			if (!isRootPathFormed)
				continue;


			if (DirectoryExists(currentPath))
				continue;

			if (!mkdir_impl(currentPath))
				return false;
		}

		return true;
	}

	std::vector<std::filesystem::path> ListContents(const std::filesystem::path& path, bool recursive, const std::vector<std::string>& ignorelist) {
		if (!DirectoryExists(path))
			RaiseClavisError(_(ERROR_NOT_A_DIRECTORY, path.string()));


		std::vector<std::filesystem::path> elements;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			const auto path = entry.path();

			bool ignore = false;
			for (auto& elem : ignorelist)
				if (path.string().find(elem) != std::string::npos)
					ignore = true;

			if (ignore)
				continue;

			elements.push_back(path);

			if (recursive && DirectoryExists(path)) {
				for (auto& rec : ListContents(path, true, ignorelist))
					elements.push_back(rec);
			}
		}

		return elements;
	}

	int GetNumberOfFiles(const std::filesystem::path &path, const std::string &extension, const std::vector<std::string>& ignorelist) {
		if (!DirectoryExists(path))
			RaiseClavisError(_(ERROR_NOT_A_DIRECTORY, path.string()));

		int ret = 0;
		auto contents = ListContents(path, true, ignorelist);

		for (auto& elem : contents) {
			if (FileExists(elem)) {
				if (extension.empty() || elem.extension() == extension)
					ret++;
			}
		}

		return ret;

	}

	int GetNumberOfSubdirectories(const std::filesystem::path &path, const std::vector<std::string>& ignorelist) {
		if (!DirectoryExists(path))
			RaiseClavisError(_(ERROR_NOT_A_DIRECTORY, path.string()));

		int ret = 0;
		auto contents = ListContents(path, true, ignorelist);

		for (auto& elem: contents)
			if (DirectoryExists(elem))
				ret++;

		return ret;
	}



	bool DirectoryIsEmpty(const std::filesystem::path &path) {
		return ListContents(path, false).empty();
	}


    bool TryReadFile(const std::filesystem::path& path, std::vector<uint8_t>& out) {
		std::ifstream file(path, std::ios::binary | std::ios::ate); // open at end to get size
		if (!file.is_open()) {
			return false;
		}

		const std::streamsize size = file.tellg(); // get file size
		if (size < 0) {
			return false;
		}

		out.resize(static_cast<size_t>(size));
		file.seekg(0, std::ios::beg);

		if (!file.read(reinterpret_cast<char*>(out.data()), size)) {
			out.clear(); // clear partially read data
			return false;
		}

		return true;
	}

	bool TryReadFile(const std::filesystem::path& path, std::string& out) {
		std::ifstream file(path, std::ios::binary | std::ios::ate); // open at end to get size
		if (!file.is_open()) {
			return false;
		}

		const std::streamsize size = file.tellg(); // get file size
		if (size < 0) {
			return false;
		}

		out.resize(static_cast<size_t>(size));
		file.seekg(0, std::ios::beg);

		if (!file.read(out.data(), size)) {
			out.clear(); // clear partially read data
			return false;
		}

		return true;
	}

	bool TryWriteFile(const std::filesystem::path& path, const std::vector<uint8_t>& data) {
		std::ofstream file(path, std::ios::binary);
		if (!file)
			return false;

		file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
		return file.good();
	}

	bool TryWriteFile(const std::filesystem::path& path, const std::string& data) {
		std::ofstream file(path, std::ios::binary);  // use binary to avoid CRLF issues on Windows
		if (!file)
			return false;

		file.write(data.data(), static_cast<std::streamsize>(data.size()));
		return file.good();
	}

	void ChDir(const std::filesystem::path& path) {
		auto s = path.string();

#ifdef __WINDOWS__
		auto w = UTF8ToUnicode(s);
		_wchdir(w.c_str());
#elif defined __LINUX__
		chdir(s.c_str());
#endif
	}

}