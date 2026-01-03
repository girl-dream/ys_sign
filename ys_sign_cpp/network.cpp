#pragma once
#include "common.h"
#include <wininet.h>
#pragma comment(lib, "wininet.lib")


void HttpGetRequest(const std::string& url, std::string userAgent, const char* method,
	const std::string& body, std::string header, std::string cookie)
{
	HINTERNET hInternet = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	BOOL sRequest = FALSE;

	char* response = NULL;
	DWORD responseSize = 0;

	// 初始化WinINet
	hInternet = InternetOpenA(userAgent.c_str(),
		INTERNET_OPEN_TYPE_DIRECT,
		NULL, NULL, 0);
	if (!hInternet) {
		MessageBox(NULL,
			(L"InternetOpen failed: " + std::to_wstring(GetLastError())).c_str(),
			L"错误",
			MB_ICONERROR);
		return;
	}

	DWORD flags = INTERNET_FLAG_RELOAD;
	int port = 80;
	std::string hostname;
	std::string path;

	// 处理协议并分离域名和路径
	if (url.find("https://") == 0) {
		port = 443;
		flags |= INTERNET_FLAG_SECURE;
		std::string without_protocol = url.substr(8);

		size_t slash_pos = without_protocol.find('/');
		if (slash_pos != std::string::npos) {
			hostname = without_protocol.substr(0, slash_pos);
			path = without_protocol.substr(slash_pos);
		}
		else {
			hostname = without_protocol;
			path = "/";
		}
	}
	else if (url.find("http://") == 0) {
		std::string without_protocol = url.substr(7);

		size_t slash_pos = without_protocol.find('/');
		if (slash_pos != std::string::npos) {
			hostname = without_protocol.substr(0, slash_pos);
			path = without_protocol.substr(slash_pos);
		}
		else {
			hostname = without_protocol;
			path = "/";
		}
	}
	else {
		size_t slash_pos = url.find('/');
		if (slash_pos != std::string::npos) {
			hostname = url.substr(0, slash_pos);
			path = url.substr(slash_pos);
		}
		else {
			hostname = url;
			path = "/";
		}
	}

	hConnect = InternetConnectA(hInternet, hostname.c_str(), port, NULL, NULL,
		INTERNET_SERVICE_HTTP, 0, 0);

	if (!hConnect)
	{
		MessageBox(NULL,
			(L"InternetConnect failed: " + std::to_wstring(GetLastError())).c_str(),
			L"错误",
			MB_ICONERROR);
		return;
	}

	if (cookie.size() > 0) {
		std::vector<std::string> result;

		size_t start = 0;
		size_t end = cookie.find(';', start);

		while (end != std::string::npos) {
			std::string part = cookie.substr(start, end - start + 1);
			result.push_back(part);

			start = end + 1;
			end = cookie.find(';', start);
		}

		for (const std::string& s : result) {
			BOOL SetCookie = InternetSetCookieA(url.c_str(), NULL, s.c_str());
			if (!SetCookie)
			{
				MessageBox(NULL,
					(L"SetCookie failed:" + std::to_wstring(GetLastError())).c_str(),
					L"错误",
					MB_ICONERROR);
				return;
			}
		}
	}

	hRequest = HttpOpenRequestA(hConnect, method, path.c_str(), NULL, NULL, NULL, flags, 0);
	if (!hRequest)
	{
		MessageBox(NULL,
			(L"HttpOpenRequest failed: " + std::to_wstring(GetLastError())).c_str(),
			L"错误",
			MB_ICONERROR);
		return;
	}


	std::string temp_headers = "Content-Type: application/json\r\n";
	temp_headers.append(header);
	const char* headers = temp_headers.c_str();
	if (strcmp(method, "POST") == 0 && !body.empty())
	{

		sRequest = HttpSendRequestA(hRequest, headers, strlen(headers),
			(LPVOID)body.c_str(), body.length());
	}
	else
	{
		sRequest = HttpSendRequestA(hRequest, headers, headers ? strlen(headers) : 0, NULL, 0);
	}

	if (!sRequest)
	{
		MessageBox(NULL,
			(L"HttpSendRequest failed. Error code: " + std::to_wstring(GetLastError())).c_str(),
			L"错误",
			MB_ICONERROR);
		return;
	}

	char buffer[4096];
	DWORD bytesRead;

	while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
		buffer[bytesRead] = '\0';

		char* temp = (char*)realloc(response, responseSize + bytesRead + 1);
		if (!temp) {
			free(response);
			response = NULL;
			break;
		}
		response = temp;

		memcpy(response + responseSize, buffer, bytesRead);
		responseSize += bytesRead;
		response[responseSize] = '\0';
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);

	if (response)
	{
		printf("%s\n", response);
		free(response);
	}
	else
	{
		MessageBoxA(NULL, "请求失败 ", "错误", MB_ICONERROR);
	}
	}
