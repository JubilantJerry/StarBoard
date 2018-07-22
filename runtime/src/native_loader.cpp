#include <stdexcept>
#include <utility>
#include <dlfcn.h>

#include "native_loader.hpp"

LibraryHandle emptyLibraryHandle = nullptr;

SmartLibraryHandle dynamicLoad(
        std::string const &libraryPath,
        std::string const &libraryName) {

    std::string fullPath = libraryPath + "lib" + libraryName + ".so";
    LibraryHandle handle = dlopen(fullPath.c_str(), RTLD_NOW);
    if (handle == nullptr) {
        throw std::runtime_error(
            "Cannot find library " + libraryName +
            " in path " + libraryPath);
    }
    return SmartLibraryHandle{handle};
}

void dynamicUnload(LibraryHandle handle) {
    if (handle != emptyLibraryHandle) {
        dlclose(handle);
    }
}

ModuleFunction locateFunction(
        SmartLibraryHandle const &handle,
        std::string const &libraryName,
        std::string const &functionName) {

    dlerror();
    ModuleFunction function =
        (ModuleFunction)dlsym(handle.get(), functionName.c_str());
    char const *errorMsg = dlerror();
    if (dlerror() != nullptr) {
        throw std::runtime_error(
            "Cannot find function " + functionName +
            " in library " + libraryName + ": " + errorMsg);
    }
    return function;
}

ModuleFunction NativeLoader::load(
        std::string const &libraryPath,
        std::string const &libraryName,
        std::string const &functionName) {

    auto handleIterator = handles.find(libraryName);
    if (handleIterator == handles.end()) {
        handles.emplace(libraryName, dynamicLoad(libraryPath, libraryName));
    }

    SmartLibraryHandle const &handle = handles.at(libraryName);
    ModuleFunction function = locateFunction(
        handle, libraryName, functionName);
    return function;
}