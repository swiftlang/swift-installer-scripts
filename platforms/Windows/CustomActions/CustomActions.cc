#include <Windows.h>
#include <msi.h>
#include <msiquery.h>

#include <filesystem>

std::wstring get_error_message(const std::error_code& error) {
    auto message = error.message();
    auto length = MultiByteToWideChar(CP_THREAD_ACP, 0, message.c_str(), message.size(), nullptr, 0);
    std::wstring result(length, L'\0');
    MultiByteToWideChar(CP_THREAD_ACP, 0, message.c_str(), message.size(), &result[0], length);
    return result;
}

namespace msi {
void log(MSIHANDLE hInstall, UINT eMessageType, std::wstring message) noexcept {
    PMSIHANDLE record = MsiCreateRecord(0);
    (void)MsiRecordSetStringW(record, 0, message.c_str());
    (void)MsiProcessMessage(hInstall, INSTALLMESSAGE(eMessageType), record);
}

void log_last_error(MSIHANDLE hInstall) noexcept {
    PMSIHANDLE record = MsiGetLastErrorRecord();
    (void)MsiProcessMessage(hInstall, INSTALLMESSAGE_ERROR, record);
}

std::wstring get_property(MSIHANDLE hInstall, std::wstring name, UINT &result) noexcept {
    DWORD size = 0;

    result = MsiGetPropertyW(hInstall, name.c_str(), L"", &size);
    switch (result) {
    case ERROR_MORE_DATA:
        break;
    case ERROR_SUCCESS:
        result = ERROR_INSTALL_FAILURE;
        return nullptr;
    default:
        log_last_error(hInstall);
        return nullptr;
    }

    std::vector<WCHAR> buffer;
    buffer.resize(size + 1);

    size = buffer.capacity();
    result = MsiGetPropertyW(hInstall, name.c_str(), buffer.data(), &size);
    switch (result) {
    case ERROR_SUCCESS:
        break;
    default:
        log_last_error(hInstall);
        return nullptr;
    }

    return {buffer.data(), buffer.capacity()};
}
}

extern "C" _declspec(dllexport) UINT __stdcall CopyClangResources(MSIHANDLE hInstall) {
    UINT result = ERROR_SUCCESS;

    // Get the runtime resource directory path
    auto resourceDirectory = msi::get_property(hInstall, L"CustomActionData", result);
    if (result != ERROR_SUCCESS) {
        return result;
    }
    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Swift runtime resources: " + resourceDirectory);

    // Get the source and destination paths
    std::filesystem::path resourcePath(resourceDirectory);
    if (resourcePath.has_filename()) {
        resourcePath = resourcePath.parent_path();
    }
    auto sourcePath = resourcePath.parent_path() / "clang";
    auto destinationPath = resourcePath / "clang";

    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Clang resources: " + sourcePath.wstring());
    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Clang resources for Swift: " + destinationPath.wstring());

    // Verify that Clang resources contain exactly one top-level directory
    {
        int directoryCount = 0, fileCount = 0;
        for (const auto& entry : std::filesystem::directory_iterator(sourcePath)) {
            if (entry.is_directory()) {
                directoryCount++;
                sourcePath = entry.path();
            } else {
                fileCount++;
            }
        }
        if (directoryCount != 1 || fileCount != 0) {
            msi::log(hInstall, INSTALLMESSAGE_ERROR | MB_OK, L"lib\\clang must contain exactly one directory.");
            return ERROR_FILE_CORRUPT;
        }
    }
    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Detected Clang version: " + sourcePath.filename().wstring());

    // Copy the contents of the single directory directly to the destination path
    std::error_code error;
    auto options = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;

    std::filesystem::copy(sourcePath, destinationPath, options, error);
    if (error) {
        std::wstring errorMessage = L"Error during directory copy: " + get_error_message(error);
        msi::log(hInstall, INSTALLMESSAGE_ERROR | MB_OK, errorMessage);
        return ERROR_DISK_OPERATION_FAILED;
    }

    return result;
}

extern "C" _declspec(dllexport) UINT __stdcall RemoveClangResources(MSIHANDLE hInstall) {
    UINT result = ERROR_SUCCESS;

    // Get the directory path for removal
    auto resourceDirectory = msi::get_property(hInstall, L"_usr_lib_swift", result);
    if (result != ERROR_SUCCESS) {
        return result;
    }
    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Swift runtime resources: " + resourceDirectory);

    std::filesystem::path resourcePath(resourceDirectory);
    if (resourcePath.has_filename()) {
        resourcePath = resourcePath.parent_path();
    }
    auto directoryPath = resourcePath / "clang";
    msi::log(hInstall, INSTALLMESSAGE_INFO, L"Clang resources for Swift: " + directoryPath.wstring());

    // Remove the directory and its contents
    std::error_code error;
    auto count = std::filesystem::remove_all(directoryPath, error);
    if (error) {
        std::wstring errorMessage = L"Error during directory removal: " + get_error_message(error);
        msi::log(hInstall, INSTALLMESSAGE_ERROR | MB_OK, errorMessage);
        return ERROR_DISK_OPERATION_FAILED;
    } else if (count == 0) {
        msi::log(hInstall, INSTALLMESSAGE_INFO, L"Clang resources for Swift doesn't exist!");
    }

    return result;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
