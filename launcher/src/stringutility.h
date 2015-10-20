#ifndef STRINGUTILITY_H
#define STRINGUTILITY_H

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>


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

#endif // STRINGUTILITY_H

