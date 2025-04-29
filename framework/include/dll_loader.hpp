/*
    Developer: Alon Kalif
    File:      dll_loader.hpp
    Reviewer:  
    Date: 	   24.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef __ILRD_RD161_DLL_LOADER_HPP__
#define __ILRD_RD161_DLL_LOADER_HPP__

#include <string>
#include <functional>
#include <array>    

#include "dir_monitor.hpp"

namespace ilrd
{

class DllLoader
{
public:
    explicit DllLoader(DirMonitor* monitor, std::function<void(void)> onLoadFailure);
            ~DllLoader() = default;
             DllLoader(const DllLoader& other) = delete;

    DllLoader& operator=(const DllLoader& other) = delete;    
    
private:
    class HandleNewFile : public ICallback<std::string>
    {
    public:
         HandleNewFile(DllLoader*);
        ~HandleNewFile() = default;

        void Update(const std::string* fileName) override;

    private:
        DllLoader* m_dllLoader;
    };
    
    std::function<void(void)>       m_onLoadFailure;
    HandleNewFile                   m_newFileHandler;
    std::unordered_set<std::string> m_filesInFolder;    

    static const size_t MAGIC_SIZE;

    void Load   (const std::string& dllPath);
    void Unload (const std::string& fileName);

    void LoadExistingFiles (DirMonitor* monitor);
    bool IsFileLoaded      (const std::string& fileName);
    bool IsDllFile         (const std::string& fileName);

};

} // namespace ilrd

#endif // __ILRD_RD161_DLL_LOADER_HPP__
