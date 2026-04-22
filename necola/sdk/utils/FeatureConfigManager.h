#pragma once
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "../../libs/json.hpp"

namespace NecolaConfig {
	constexpr const char* CONFIG_PATH = "necola\\FeatureConfig.json";
	constexpr const char* CONFIG_DIR  = "necola";

	inline nlohmann::json LoadConfig() {
		try {
			std::ifstream file(CONFIG_PATH);
			if (!file.is_open()) return nlohmann::json{};
			nlohmann::json doc;
			file >> doc;
			return doc;
		} catch (...) {
			return nlohmann::json{};
		}
	}

	inline void SaveConfig(const nlohmann::json& doc) {
		try {
			std::filesystem::create_directories(CONFIG_DIR);
			std::ofstream outFile(CONFIG_PATH);
			if (outFile.is_open()) {
				outFile << std::setw(4) << doc;
			}
		} catch (...) {}
	}
}
