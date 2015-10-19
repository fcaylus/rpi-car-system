#ifndef DIRUTILITY_H
#define DIRUTILITY_H

/**
  * Contains some useful functions
  */

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <ctime>
#include <random>

namespace StringUtility
{
    static inline bool startsWith(const std::string &str, const std::string& start)
    {
        return str.find(start) == 0;
    }

    static inline bool endsWith(const std::string &str, const std::string &end)
    {
        const std::string::size_type len = end.length();
        const long long pos = str.length() - len;
        if(pos < 0)
            return false;
        auto pos_str = &str[pos];
        auto pos_end = &end[0];
        while(*pos_str)
            if(*pos_str++ != *pos_end++)
                return false;
        return true;
    }

    // Trim from start
    static inline std::string leftTrim(const std::string &str)
    {
        std::string str2 = str;
        str2.erase(str2.begin(), std::find_if(str2.begin(), str2.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return str2;
    }

    // Trim from end
    static inline std::string rightTrim(const std::string &str)
    {
        std::string str2 = str;
        str2.erase(std::find_if(str2.rbegin(), str2.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str2.end());
        return str2;
    }

    // Trim from both ends
    static inline std::string trim(const std::string &str)
    {
        return leftTrim(rightTrim(str));
    }

    static inline std::vector<std::string> split(const std::string &str, char delim = ' ', bool skipEmptyParts = true)
    {
        std::vector<std::string> elements;
        std::stringstream ss(str);
        std::string item;
        while(std::getline(ss, item, delim))
        {
            if((item == "" && !skipEmptyParts) || item != "")
                elements.push_back(item);
        }
        return elements;
    }

    static inline std::string uriDecode(const std::string &str)
    {
        std::string decodedStr;
        std::string::const_iterator it  = str.begin();
        std::string::const_iterator end = str.end();
        while (it != end)
        {
            char c = *it++;
            if (c == '%')
            {
                // Check the two characters after
                if (it == end)
                    continue;
                char hi = *it++;
                if (it == end)
                    continue;
                char lo = *it++;
                if (hi >= '0' && hi <= '9')
                    c = hi - '0';
                else if (hi >= 'A' && hi <= 'F')
                    c = hi - 'A' + 10;
                else if (hi >= 'a' && hi <= 'f')
                    c = hi - 'a' + 10;
                else
                    continue;
                c *= 16;
                if (lo >= '0' && lo <= '9')
                    c += lo - '0';
                else if (lo >= 'A' && lo <= 'F')
                    c += lo - 'A' + 10;
                else if (lo >= 'a' && lo <= 'f')
                    c += lo - 'a' + 10;
                else
                    continue;
            }
            decodedStr += c;
        }
        return decodedStr;
    }
}

#include <unistd.h>
#include <climits>

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
}

#endif // DIRUTILITY_H

