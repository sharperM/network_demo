#pragma once
#include <string>
#include <vector>
struct SendData
{
	std::string address;
	std::vector<char> data;
	std::string port;
}typedef SendData;