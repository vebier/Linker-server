#pragma once
#include "const.h"

struct SectionInfo {

public:
	SectionInfo() {}

	SectionInfo(const std::string& Key,const std::string& Value) {
		_section_map.insert(std::make_pair(Key,Value));
	}

	void addKeyValue(const std::string& Key, const std::string& Value) {
		_section_map.insert(std::make_pair(Key, Value));
	}

	~SectionInfo() {}

	SectionInfo(const SectionInfo& other) {
		_section_map = other._section_map;
	}

	SectionInfo operator=(const SectionInfo& other) {
		if (this != &other) {
			_section_map = other._section_map;
		}
		return *this;
	}

	std::string operator[](const std::string& Key) {
		auto it = _section_map.find(Key);  
		if (it == _section_map.end()) {
			return "";
		}
		return it->second;
	}
	std::unordered_map<std::string, std::string> _section_map;
};

class ConfigMgr {
public:
	ConfigMgr();

	~ConfigMgr() {}

	ConfigMgr(const ConfigMgr& other) {
		_config_map = other._config_map;
	}

	ConfigMgr operator=(const ConfigMgr& other) {
		if (this != &other) {
			_config_map = other._config_map;
		}
		return *this;
	}

	SectionInfo operator[](const std::string& Section) {
		auto it = _config_map.find(Section);  // C++20 Ö§³Ö
		if (it == _config_map.end()) {
			return SectionInfo();
		}
		return it->second;
	}
private:
	std::unordered_map<std::string, SectionInfo> _config_map;
};