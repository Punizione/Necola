#include "./utils/CustomParticleManager.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>



void CustomParticleManager::clear()
{
	// weaponTracersIndex.clear();
	weaponTracersList.clear();
	weaponTracersIndex.clear();
	qcTracers.clear();
	configTracers.clear();
	emptyCustomTracer.clear();

	weaponMuzzleList.clear();
	weaponMuzzleIndex.clear();
	qcMuzzle.clear();
	configMuzzle.clear();
	emptyCustomMuzzle.clear();

	// sceneList.clear();

}

void CustomParticleManager::clearSource()
{
	weaponSourceTracersList.clear();
	configSourceTracers.clear();
	qcSourceTracers.clear();

	weaponSourceMuzzleList.clear();
	configSourceMuzzle.clear();
	qcSourceMuzzle.clear();
	sceneList.clear();
	
	initDefaultWeaponTracers();
	// modelList.clear();
}


bool CustomParticleManager::hasTracersQc(int index)
{
	bool flag =  (qcTracers.find(index) != qcTracers.end());
	if(flag) {
		flag = (qcTracers[index].size() != 0);
	}
	return flag;
}

const std::string& CustomParticleManager::getTracersQc(int index)
{
	int size = qcTracers[index].size();
	if(size == 1) {
		return qcTracers[index].at(0);
	} 
		
	return qcTracers[index][rand_int(0, size - 1)];
}

bool CustomParticleManager::hasTracersConfig(int weaponID)
{
	bool flag = (configTracers.find(weaponID) != configTracers.end());
	if(flag) {
		flag = (configTracers[weaponID].size() != 0 );
	}
	return flag;
}

const std::string& CustomParticleManager::getTracersConfig(int weaponID)
{
	int size = configTracers[weaponID].size();
	if(size == 1) {
		return configTracers[weaponID].at(0);
	} 
		
	return configTracers[weaponID][rand_int(0, size - 1)];
}

bool CustomParticleManager::hasMuzzleQc(int index)
{
	bool flag =  (qcMuzzle.find(index) != qcMuzzle.end());
	if(flag) {
		flag = (qcMuzzle[index].size() != 0);
	}
	return flag;
}

const std::string& CustomParticleManager::getMuzzleQc(int index)
{
	int size = qcMuzzle[index].size();
	if(size == 1) {
		return qcMuzzle[index].at(0);
	} 
		
	return qcMuzzle[index][rand_int(0, size - 1)];
}

bool CustomParticleManager::hasMuzzleConfig(int weaponID)
{
	bool flag = (configMuzzle.find(weaponID) != configMuzzle.end());
	if(flag) {
		flag = (configMuzzle[weaponID].size() != 0 );
	}
	return flag;
}

const std::string& CustomParticleManager::getMuzzleConfig(int weaponID)
{
	int size = configMuzzle[weaponID].size();
	if(size == 1) {
		return configMuzzle[weaponID].at(0);
	} 
		
	return configMuzzle[weaponID][rand_int(0, size - 1)];
}


void CustomParticleManager::Settle() {
	// Cache Qc Tracer 
	for(auto it = qcSourceTracers.begin(); it != qcSourceTracers.end(); ++it) {
		int viweModelIndex = it->first;
		const std::vector<std::string>& tracerList = it->second;
		std::vector<std::string> filteredTracers;
		for (size_t i = 0; i < tracerList.size(); ++i) {
			const std::string& tracer = tracerList[i];
			if (hasTracersLoaded(tracer)) {
				filteredTracers.push_back(tracer);
			}
		}
		if (!filteredTracers.empty()) {
			qcTracers[viweModelIndex] = filteredTracers;
		} else {
			// Cache none Custom Tracer in QC
			addEmptyCustomQcTracerModelIndex(viweModelIndex);
		}
	}

	


	// Cache Config Tracer
	for(auto it = configSourceTracers.begin(); it != configSourceTracers.end(); ++it) {
		int necolaWeaponId = it->first;
		const std::vector<std::string>& tracerList = it->second;
		std::vector<std::string> filteredTracers;
		for (size_t i = 0; i < tracerList.size(); ++i) {
			const std::string& tracer = tracerList[i];
			if (hasTracersLoaded(tracer)) {
				filteredTracers.push_back(tracer);
			}
		}
		if (!filteredTracers.empty()) {
			configTracers[necolaWeaponId] = filteredTracers;
		} 
	}


	// Cache QC Muzzle
	for(auto it = qcSourceMuzzle.begin(); it != qcSourceMuzzle.end(); ++it) {
		int viweModelIndex = it->first;
		const std::vector<std::string>& muzzleList = it->second;
		std::vector<std::string> filteredMuzzle;
		for (size_t i = 0; i < muzzleList.size(); ++i) {
			const std::string& muzzle = muzzleList[i];
			if (hasMuzzleLoaded(muzzle)) {
				filteredMuzzle.push_back(muzzle);
			}
		}
		if (!filteredMuzzle.empty()) {
			qcMuzzle[viweModelIndex] = filteredMuzzle;
		} else {
			// Cache none Custom Tracer in QC
			addEmptyCustomQcMuzzleModelIndex(viweModelIndex);
		}
	}

	


	// Cache Config Muzzle
	for(auto it = configSourceMuzzle.begin(); it != configSourceMuzzle.end(); ++it) {
		int necolaWeaponId = it->first;
		const std::vector<std::string>& muzzleList = it->second;
		std::vector<std::string> filteredMuzzle;
		for (size_t i = 0; i < muzzleList.size(); ++i) {
			const std::string& muzzle = muzzleList[i];
			if (hasMuzzleLoaded(muzzle)) {
				filteredMuzzle.push_back(muzzle);
			}
		}
		if (!filteredMuzzle.empty()) {
			configMuzzle[necolaWeaponId] = filteredMuzzle;
		} 
	}

	timer = 0.0;
	clearSource();

}

void CustomParticleManager::optionResetMuzzle(int weaponId, const std::string& muzzleName) {
	std::vector<std::string> filteredMuzzle;
	filteredMuzzle.push_back(muzzleName);
	configMuzzle[weaponId] = filteredMuzzle;
	hasChange = true;
}


void CustomParticleManager::optionResetTracer(int weaponId, const std::string& tracerName) {
	std::vector<std::string> filteredTracers;
	filteredTracers.push_back(tracerName);
	configTracers[weaponId] = filteredTracers;
	// std::cout << "weaponId:[{" << weaponId << "}] tracerName:[{" << tracerName << "}]" << std::endl;
	hasChange = true;
	
}



bool CustomParticleManager::initializeFromJson(const json& jsonData) {
	if(jsonData.is_array()) {
		if(!jsonData.empty()){
			for(const auto& element : jsonData) {
				int weaponId = element.value("id", 0);
				std::string particle = element.value("particle", "");
				std::string muzzle = element.value("muzzle", "");
				if( weaponId != 0  && !particle.empty()) {
					addConfigWeaponTracers(weaponId, particle);
				}
				if( weaponId != 0  && !muzzle.empty()) {
					// muzzleConfig[weaponId] = muzzle;
					addConfigWeaponMuzzle(weaponId, muzzle);
				}
			}
			return true;
		}
		
	}
	return false;
}

bool CustomParticleManager::initializeFromJsonFile(const std::string& filename) {
	try {
		std::ifstream file(filename);
		if (!file.is_open()) {
			return false;
		}
		json jsonData;
		file >> jsonData;
		file.close();
		return initializeFromJson(jsonData);
	} catch (const std::exception& e) {
		// std::cout << "error: " << e.what() << std::endl;
		return false;
	}
}

bool CustomParticleManager::exportToFile(const std::string& filename) {
	
	try{
		std::filesystem::path filePath(filename);
		if (filePath.has_parent_path()) {
			std::filesystem::create_directories(filePath.parent_path());
		}
		json jsonData = toJsonData();
		std::ofstream file1(filename);
		if(!file1.is_open()) {
			return false;
		}
		file1 << std::setw(4) << jsonData;
		file1.close();

	} catch (const std::exception& e) {
		// std::cout << "error: " << e.what() << std::endl;
		return false;
	}
	return true;
}


json CustomParticleManager::toJsonData() {
	json jsonData = json::array();
	std::unordered_set<int> all_ids;
	all_ids.reserve(configTracers.size() + configMuzzle.size());

	for (const auto& [id, _] : configTracers) {
		all_ids.insert(id);
	}
	for (const auto& [id, _] : configMuzzle) {
		all_ids.insert(id);
	}
	for (const auto& id : all_ids) {
		std::vector<std::string> configTracersList = configTracers[id];
		std::string tracerStr = vectorToString(configTracersList, ";");

		std::vector<std::string> configMuzzleList = configMuzzle[id];
		std::string muzzleStr = vectorToString(configMuzzleList, ";");
		if(!tracerStr.empty() || !muzzleStr.empty()) {
			jsonData.push_back({
				{"id", id},
				{"name", getNCLName(id)},
				{"particle", tracerStr },
				{"muzzle", muzzleStr }
			});
		}
		
	}
	return jsonData;
}


const char* CustomParticleManager::getNCLName(int necolaId) {
	if(necolaId == 2) {
		return "NECOLA_WEAPON_UZI";
	} else if(necolaId == 3) {
		return "NECOLA_WEAPON_PUMP_SHOTGUN";
	} else if(necolaId == 4) {
		return "NECOLA_WEAPON_AUTO_SHOTGUN";
	} else if(necolaId == 5) {
		return "NECOLA_WEAPON_M16A1";
	} else if(necolaId == 6) {
		return "NECOLA_WEAPON_HUNTING_RIFLE";
	} else if(necolaId == 7) {
		return "NECOLA_WEAPON_MAC10";
	} else if(necolaId == 8) {
		return "NECOLA_WEAPON_CHROME_SHOTGUN";
	} else if(necolaId == 9) {
		return "NECOLA_WEAPON_SCAR";
	} else if(necolaId == 10) {
		return "NECOLA_WEAPON_MILITARY_SNIPER";
	} else if(necolaId == 11) {
		return "NECOLA_WEAPON_SPAS";
	} else if(necolaId == 26) {
		return "NECOLA_WEAPON_AK47";
	} else if(necolaId == 33) {
		return "NECOLA_WEAPON_MP5";
	} else if(necolaId == 34) {
		return "NECOLA_WEAPON_SSG552";
	} else if(necolaId == 35) {
		return "NECOLA_WEAPON_AWP";
	} else if(necolaId == 36) {
		return "NECOLA_WEAPON_SCOUT";
	} else if(necolaId == 37) {
		return "NECOLA_WEAPON_M60";
	} else if(necolaId == 1) {
		return "NECOLA_WEAPON_PISTOL";
	} else if(necolaId == 32) {
		return "NECOLA_WEAPON_DEAGLE";
	} else if(necolaId == 82) {
		return "NECOLA_WEAPON_PISTOL_DUAL";
	} 
	return "";
}

std::string CustomParticleManager::vectorToString(const std::vector<std::string>& vec, const std::string& delimiter) {
	if (vec.empty()) {
		return "";
	}
	std::ostringstream oss;
	oss << vec[0];
	for (size_t i = 1; i < vec.size(); ++i) {
		oss << delimiter << vec[i];
	}
	
	return oss.str();
}
