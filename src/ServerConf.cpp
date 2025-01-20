#include "../inc/config/ServerConf.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

ServerConf::ServerConf(const std::string filePath) : _configFile(filePath) {}

ServerConf::~ServerConf() {}

void ServerConf::parseConfigFile(void) {
	std::ifstream file(_configFile.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: Unable to open config file: " +
								 _configFile);

	std::string line;
	ConfigBlock currBlock;
	bool insideBlock = false;

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string word;
		ss >> word;

		// Ignorer les lignes vides ou les commentaires
		if (word.empty() || word[0] == '#') continue;

		if (word == "{") {
			if (insideBlock)
				throw std::runtime_error(
					"Error: Nested blocks are not supported.");
			insideBlock = true;
			continue;
		}

		if (word == "}") {
			if (!insideBlock)
				throw std::runtime_error("Error: Unmatched closing brace.");

			//	valide le block
			validateConfigFile(currBlock);
			_configBlocks.push_back(currBlock);
			currBlock = ConfigBlock();
			insideBlock = false;
			continue;
		}

		// Ajoutez les directives au bloc actuel
		if (insideBlock) {
			std::string value;
			ss >> value;
			if (value.empty())
				throw std::runtime_error(
					"Error: Missing value for directive: " + word);
			currBlock.addDirective(word, value);
		} else
			throw std::runtime_error(
				"Error: Directive found outside of a block.");
	}
	if (insideBlock)
		throw std::runtime_error("Error: Missing closing brace for block.");
	file.close();
}

void ServerConf::validateConfigFile(const ConfigBlock &block) {
	if (!block.hasDirective("server_name")) {
		throw std::runtime_error("Error: Missing 'server_name' directive.");
	}

	if (!block.hasDirective("listen")) {
		throw std::runtime_error("Error: Missing 'listen' directive.");
	}

	//!\ Ajouter d'autres validations spécifiques ici...
}

const std::vector<ConfigBlock> &ServerConf::getConfigBlocks() const {
	return _configBlocks;
}

std::string ServerConf::getValue(const std::string &key) const {
	for (std::vector<ConfigBlock>::const_iterator it = _configBlocks.begin(); it != _configBlocks.end(); ++it)
	{
		const ConfigBlock &block = *it;
		// Vérification dans les directives du bloc courant
		if (block.hasDirective(key)) return block.getDirective(key);

		// Si la directive n'est pas trouvée dans ce bloc, on cherche dans ses
		// sous-blocs (enfants)
		for (std::vector<ConfigBlock>::const_iterator childIt = block.getChildren().begin(); childIt != block.getChildren().end(); ++childIt)
		{
        	const ConfigBlock &child = *childIt;
       		if (child.hasDirective(key)) return child.getDirective(key);
		}
    }
	throw std::runtime_error("Directive " + key +
							 " not found in configuration");
}
