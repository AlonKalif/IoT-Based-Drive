/*
    Developer: Alon Kalif
    File:      dll_loader.cpp
    Reviewer:  
    Date: 	   28.12.2024
    Status:    In progress
    Version:   1.0
*/

#include <fstream>      // std::fstream

#include <dlfcn.h>      // For dlopen

#include "dll_loader.hpp"	// For Class definition
#include "logger.hpp"       // For documentation

namespace ilrd
{

extern Logger* g_logger;

/* ============================= Class DllLoader ============================ */

const size_t DllLoader::MAGIC_SIZE = 16;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ---------------------------------- ctor ---------------------------------- */

DllLoader::DllLoader(DirMonitor* monitor, std::function<void(void)> onLoadFailure) :
m_onLoadFailure(onLoadFailure), m_newFileHandler(this)
{
    if(nullptr == monitor)
    {
        g_logger->Log(DLL_LOADER_MSG::LOADER_INIT_ERROR, __FILE__, __LINE__);
    }

    LoadExistingFiles(monitor);

    monitor->Register(&m_newFileHandler, DirMonitor::DM_CREATE);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void DllLoader::Load(const std::string& dllPath)
{
    if (NULL == dlopen(dllPath.c_str(), RTLD_LAZY))
    {
        g_logger->Log(DLL_LOADER_MSG::DLOPEN_ERROR + dllPath, __FILE__, __LINE__);

        m_onLoadFailure();
    }

    m_filesInFolder.emplace(dllPath);

    #ifndef NDEBUG
        g_logger->Log(DLL_LOADER_MSG::LOADED + dllPath);
    #endif
}

void DllLoader::LoadExistingFiles(DirMonitor* monitor)
{
    m_filesInFolder = monitor->GetExistingFiles();

    for(auto file : m_filesInFolder)
    {
        if(IsDllFile(file))
        {
            Load(file);
        }
    }
}

bool DllLoader::IsDllFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if(!file.is_open())
    {
        g_logger->Log(DLL_LOADER_MSG::FAILED_TO_VERIFY_ERROR, __FILE__, __LINE__);

        return false;
    }

    std::array<char, MAGIC_SIZE + 1> buffer;

    file.read(buffer.data(), MAGIC_SIZE + 1);

    file.close();

    return (buffer[0] == 0x7F) && (buffer[1] == 0x45) && (buffer[2] == 0x4c) &&
           (buffer[3] == 0x46) && (buffer[MAGIC_SIZE] == 0x03);
}

bool DllLoader::IsFileLoaded(const std::string& fileName)
{
    for(auto file : m_filesInFolder)
    {
        if(file == fileName)
        {
            return true;
        }
    }

    return false;
}

/* ========================================================================== */



/* ======================= Nested Class InotifyWrapper ====================== */

/* ---------------------------------- ctor ---------------------------------- */

DllLoader::HandleNewFile::HandleNewFile(DllLoader* loader): m_dllLoader(loader)
{
}

/* ---------------------------- Member Functions ---------------------------- */

void DllLoader::HandleNewFile::Update(const std::string* fileName)
{
    if(m_dllLoader->IsDllFile(*fileName) && !m_dllLoader->IsFileLoaded(*fileName))
    { 
        m_dllLoader->Load(*fileName);
    }
}

/* ========================================================================== */

} // namespace ilrd





