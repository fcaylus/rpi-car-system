#ifndef DIRUTILITY_H
#define DIRUTILITY_H

#include <ctime>
#include <random>
#include <climits>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>

#include "stringutility.h"

namespace DirUtility
{
    // Return the file path for the specified URI
    static inline std::string filePathForURI(std::string uri)
    {
        if(StringUtility::startsWith(uri, "file://"))
            uri.erase(0, 7);

        return StringUtility::uriDecode(uri);
    }

    // Return the directory of the specified file
    static inline std::string dirFromFullPath(const std::string& path)
    {
        const std::size_t foundIdx = path.find_last_of("/\\");
        // Exclude path ending with a "/" because it's already a directory
        if(foundIdx != std::string::npos && foundIdx != path.length()-1)
            return path.substr(0, foundIdx);
        return path;
    }

    // Return the file name (the last component) of the specified path
    static inline std::string fileNameFromFullPath(const std::string& path)
    {
        const std::size_t foundIdx = path.find_last_of("/\\");
        if(foundIdx != std::string::npos)
            return path.substr(foundIdx+1);
        return path;
    }

    // Return file name without extension
    static inline std::string fileNameFromFullPathWithoutDot(const std::string& path)
    {
        const std::string fileName = fileNameFromFullPath(path);
        const std::size_t foundIdx = fileName.find_last_of(".");
        if(foundIdx != std::string::npos)
            return fileName.substr(0, foundIdx);
        return fileName;
    }

    // Return the executable path
    static inline std::string executablePath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
    }

    // Return the path of the directory that contains the executable (without "\" at the end)
    static inline std::string executableDir()
    {
        return dirFromFullPath(executablePath());
    }

    // Return the executable name
    static inline std::string executableName()
    {
        return fileNameFromFullPath(executablePath());
    }

    // Get a unique filename
    // Use the date-time and a random number
    // It's not guaranted to be unique, but there is a lot of chance
    // (if all temp files are generated with this method)
    static inline std::string uniqueFileName()
    {
        // Generate datetime
        time_t now = std::time(nullptr);
        struct tm tstruct = *std::localtime(&now);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tstruct);

        // Generate random number
        std::minstd_rand generator;
        std::uniform_int_distribution<> distrib(0, 10000000);
        const int randNb = distrib(generator);

        return std::string("tempfile-" APPLICATION_EXE_NAME "-") + std::string(buf) + std::to_string(randNb);
    }

    // Recursive method that will list all files in the specified dir (and directories)
    // When a file is found, func is called with the path as argument
    static inline void listAllFilesInDir(const std::string& path, std::function<void(const std::string& /*path*/)> func)
    {
        DIR *dp = NULL;
        struct dirent *dirp = NULL;
        if ((dp = opendir(path.c_str())) == NULL)
        {
            // File found ! call the specified function
            func(path);
            return;
        }

        while ((dirp = readdir(dp)) != NULL)
        {
            if(std::string(dirp->d_name) != "." && std::string(dirp->d_name) != "..")
                listAllFilesInDir(path + std::string("/") + std::string(dirp->d_name), func);
        }

        closedir(dp);
    }

    static inline long fileSize(const std::string& path)
    {
        struct stat stat_buf;
        const int result = stat(path.c_str(), &stat_buf);
        return (result == 0 ? stat_buf.st_size : -1);
    }

    static inline void copyFile(const std::string& from, const std::string& to)
    {
        int read_fd;
        int write_fd;
        struct stat stat_buf;
        off_t offset = 0;

        // Open the input file.
        read_fd = open(from.c_str(), O_RDONLY);
        // Stat the input file to obtain its size.
        fstat(read_fd, &stat_buf);
        // Open the output file for writing, with the same permissions as the source file.
        write_fd = open(to.c_str(), O_WRONLY | O_CREAT, stat_buf.st_mode);
        // Blast the bytes from one file to the other.
        sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
        // Close up.
        close(read_fd);
        close(write_fd);
    }
}

#endif // DIRUTILITY_H

