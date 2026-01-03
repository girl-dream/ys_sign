#include "common.h"
#include <fstream>
#include <winrt/Windows.Data.Json.h>
#pragma comment(lib, "windowsapp.lib")

using namespace winrt;

void HttpGetRequest(const std::string& url, std::string userAgent, const char* method,
	const std::string& body, std::string header, std::string cookie);
std::string getDS(std::string body);

int main() {
	std::locale::global(std::locale("zh_CN.UTF-8"));

	std::ifstream file("../data.json");

	if (!file.is_open()) {
		wprintf(L"无法打开json文件\n");
		return -1;
	}
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string content(size, ' ');
	file.seekg(0);
	file.read(&content[0], size);

	winrt::init_apartment();
	auto json = winrt::Windows::Data::Json::JsonObject::Parse(to_hstring(content));

	std::string miHoYo_cookie = to_string(json.GetNamedString(L"miHoYo_cookie"));
	std::string HoYoLAB_cookie = to_string(json.GetNamedString(L"HoYoLAB_cookie"));


	// 米哈游签到
	if (miHoYo_cookie != std::string(""))
	{
		wprintf(L"开始米游社签到\n");
		const char* url = "https://api-takumi.mihoyo.com/event/luna/hk4e/sign";
		std::string header = "Referer: https://act.mihoyo.com/";
		std::string ua = std::format("Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBS/{}", to_string(json.GetNamedString(L"miHoYo_ua_version")));
		std::string body = std::format(R"({{
			"act_id": "e202311201442471",
			"region": "cn_gf01",
			"uid": "{}",
			"lang": "zh-cn"
	}})", winrt::to_string(json.GetNamedString(L"miHoYo_uid")));
		HttpGetRequest(url, ua, "POST", body, header, miHoYo_cookie);
	}

	// HoYoLAB签到
	if (HoYoLAB_cookie != std::string(""))
	{
		wprintf(L"开始HoYoLAB签到\n");
		const char* url = "https://sg-hk4e-api.hoyolab.com/event/sol/sign?lang=zh-cn";
		std::string header = "Referer: https://act.hoyolab.com";
		std::string ua = std::format("Mozilla/5.0 (Linux; Android 12) Mobile miHoYoBBSOversea/{}", to_string(json.GetNamedString(L"HoYoLAB_ua_version")));
		std::string body = R"({"act_id":"e202102251931481"})";
		HttpGetRequest(url, ua, "POST", body, header, miHoYo_cookie);
	}
	wprintf(L"签到完成,3秒后退出程序");
	Sleep(3000);
}
