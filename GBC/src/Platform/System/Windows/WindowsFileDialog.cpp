#include "gbcpch.h"
#include "GBC/Core/Application.h"
#include "GBC/IO/FileDialog.h"
#include "GBC/IO/FileIO.h"
#include <filesystem>
#include <commdlg.h>
#include <shellapi.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#define STRICT_TYPED_ITEMIDS
#include <ShlObj_core.h>

#define HWND_OWNER glfwGetWin32Window(static_cast<GLFWwindow*>(::gbc::Application::Get().GetWindow().GetNativeWindow()))

#define REQUIRE_INITIAL_DIRECTORY \
	std::wstring initialDirectory(__std_fs_max_path, L'\0'); \
	DWORD result = GetCurrentDirectoryW(__std_fs_max_path, initialDirectory.data()); \
	if (!result) return {}

std::filesystem::path OpenFolderDialogImpl(const std::filesystem::path& absoluteInitialDirectoryPath);

namespace gbc::FileDialog
{
	std::filesystem::path OpenFile(const wchar_t* filter, const std::filesystem::path& initialDirectoryPath)
	{
		OPENFILENAMEW ofn;
		WCHAR szFile[__std_fs_max_path]{ L'\0' };
		ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = HWND_OWNER;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = initialDirectoryPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
		ofn.lpstrDefExt = wcschr(filter, L'\0') + 1;

		if (GetOpenFileNameW(&ofn))
			return ofn.lpstrFile;
		return {};
	}

	std::filesystem::path OpenFile(const wchar_t* filter)
	{
		REQUIRE_INITIAL_DIRECTORY;
		return OpenFile(filter, initialDirectory);
	}

	std::filesystem::path SaveFile(const wchar_t* filter, const std::filesystem::path& initialDirectoryPath)
	{
		OPENFILENAMEW ofn;
		WCHAR szFile[__std_fs_max_path]{ L'\0' };
		ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = HWND_OWNER;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = initialDirectoryPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
		ofn.lpstrDefExt = wcschr(filter, L'\0') + 1;

		if (GetSaveFileNameW(&ofn))
			return ofn.lpstrFile;
		return {};
	}

	std::filesystem::path SaveFile(const wchar_t* filter)
	{
		REQUIRE_INITIAL_DIRECTORY;
		return SaveFile(filter, initialDirectory);
	}

	std::filesystem::path OpenFolder(const std::filesystem::path& initialDirectoryPath)
	{
		return OpenFolderDialogImpl(FileIO::Absolute(initialDirectoryPath));
	}

	std::filesystem::path OpenFolder()
	{
		REQUIRE_INITIAL_DIRECTORY;
		return OpenFolder(initialDirectory);
	}

	bool OpenFolderInExplorer(const std::filesystem::path& directoryPath)
	{
		if (!FileIO::DirectoryExists(directoryPath))
			return false;
		auto status = (INT_PTR)ShellExecuteW(nullptr, L"open", directoryPath.c_str(), nullptr, nullptr, SW_RESTORE);
		return status > 32;
	}

	bool OpenFileInExplorer(const std::filesystem::path& filepath)
	{
		if (!FileIO::FileExists(filepath)) return false;

		PCIDLIST_ABSOLUTE pidl = ILCreateFromPathW(filepath.c_str());
		if (!pidl) return false;

		// TODO: this blocks the current thread for a short bit.
		// Not the biggest issue in the world, but it's noticable.
		HRESULT hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
		ILFree((PIDLIST_RELATIVE)pidl);
		return hr == S_OK;
	}

	bool OpenFolderInTerminal(const std::filesystem::path& directoryPath)
	{
		std::string command;
		command.reserve(10 + directoryPath.native().length() + 1);
		command += "start /D \"";
		command += directoryPath.string();
		command += '"';
		return !system(command.c_str());
	}
}



// I fucking hate microsoft.

#define STRICT_TYPED_ITEMIDS
#include <ShlObj.h>
#include <Shlwapi.h>

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "Comctl32")
#pragma comment(lib, "Shlwapi")
#pragma comment(lib, "Propsys")

class DialogEventHandler : public IFileDialogEvents, public IFileDialogControlEvents
{
public:
	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(DialogEventHandler, IFileDialogEvents),
			QITABENT(DialogEventHandler, IFileDialogControlEvents),
			{ 0 }
		#pragma warning(suppress:4838)
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&_cRef); }
	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef) delete this;
		return cRef;
	}

	// IFileDialogEvents methods
	IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; }
	IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; }
	IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; }
	IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; }
	IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; }
	IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; }
	IFACEMETHODIMP OnTypeChange(IFileDialog* pfd) { return S_OK; }
	IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; }

	// IFileDialogControlEvents methods
	IFACEMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; }
	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD) { return S_OK; }
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL) { return S_OK; }
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD) { return S_OK; }
private:
	long _cRef = 1;
};

// Instance creation helper
HRESULT DialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
	*ppv = NULL;
	DialogEventHandler* dialogEventHandler = new DialogEventHandler();
	HRESULT hr = dialogEventHandler->QueryInterface(riid, ppv);
	dialogEventHandler->Release();
	return hr;
}

std::filesystem::path OpenFolderDialogImpl(const std::filesystem::path& absoluteInitialDirectoryPath)
{
	std::filesystem::path filepath;

	// The gotos require that I initialize these before them.
	// The only reason I'm using them is so I don't have an
	// if statement nested in another if statement nested in
	// another if statement nested in another if statement
	// nested in another if statement nested in another...
	IFileDialog* pfd = NULL;
	PIDLIST_ABSOLUTE pidl = NULL;
	IFileDialogEvents* pfde = NULL;
	PWSTR pszFilePath = NULL;

	// Create the Dialog.
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (!SUCCEEDED(hr)) goto fail1;

	// Parse the absolute initial directory path.
	hr = SHParseDisplayName(absoluteInitialDirectoryPath.c_str(), NULL, &pidl, SFGAO_FOLDER, NULL);
	if (!SUCCEEDED(hr)) goto fail2;

	// Get the initial folder handle.
	IShellItem* initialFolder;
	hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&initialFolder));
	if (!SUCCEEDED(hr)) goto fail3;

	// Set the initial folder.
	hr = pfd->SetFolder(initialFolder);
	if (!SUCCEEDED(hr)) goto fail3;

	// Create an event handler.
	hr = DialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
	if (!SUCCEEDED(hr)) goto fail3;

	// Hook up the event handler to the dialog.
	DWORD dwCookie;
	hr = pfd->Advise(pfde, &dwCookie);
	if (!SUCCEEDED(hr)) goto fail4;

	// Before setting, always get the options first in order not to override existing options.
	DWORD dwFlags;
	hr = pfd->GetOptions(&dwFlags);
	if (!SUCCEEDED(hr)) goto fail5;

	// Add relevant options.
	dwFlags |= FOS_NOCHANGEDIR | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_DONTADDTORECENT;

	// Update the options.
	hr = pfd->SetOptions(dwFlags);
	if (!SUCCEEDED(hr)) goto fail5;

	// Show the dialog.
	hr = pfd->Show(HWND_OWNER);
	if (!SUCCEEDED(hr)) goto fail5;

	// Get the dialog result.
	IShellItem* psiResult;
	hr = pfd->GetResult(&psiResult);
	if (!SUCCEEDED(hr)) goto fail5;

	// Get the filepath... finally.
	hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr)) goto fail6;

	filepath = pszFilePath;

	// Free everything that was successfully allocated.
	CoTaskMemFree(pszFilePath);
fail6:
	psiResult->Release();
fail5:
	pfd->Unadvise(dwCookie); // Unhook the event handler.
fail4:
	pfde->Release();
fail3:
	ILFree(pidl);
fail2:
	pfd->Release();
fail1:
	return filepath;
}
