#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <iostream>

#include "ConfigBlock.hpp"

/**
 * @brief  Charge, check and valide the configuration file's data
 * @note   this class must read and extract data then store them
 */
class ServerConf {
   private:
	std::string _configFile;
	std::vector<ConfigBlock> _configBlocks;

	void parseConfigFile(void);
	void validateConfigFile(const ConfigBlock &block);

   public:
	ServerConf(const std::string filePath);
	~ServerConf(void);

	// Retourne la valeur d'une directive
	std::string getValue(const std::string &key) const;
	const std::vector<ConfigBlock> &getConfigBlocks() const;
};

#endif
