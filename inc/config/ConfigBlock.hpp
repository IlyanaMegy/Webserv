#ifndef CONFIGBLOCK_HPP
# define CONFIGBLOCK_HPP

# include <iostream>
# include <vector>
# include <map>

class ConfigBlock
{
private:
	std::string _name; // nom du block
	std::map<std::string, std::string> _directives; // directives
	std::vector<ConfigBlock> _children; // liste sous-blocs enfants
	std::map<std::string, std::vector<std::string> > _multiDirectives;
public:
	ConfigBlock() : _name("") {};
	~ConfigBlock() {};
	void addDirective(const std::string &key, const std::string &value) {_directives[key] = value; }
	void addMultiDirectives(const std::string &key, const std::vector<std::string> &values) {_multiDirectives[key] = values; }
	void addChild(const ConfigBlock &child) {_children.push_back(child); }
	bool hasDirective(const std::string &directive) const {return _directives.find(directive) != _directives.end(); }
	std::string getDirective(const std::string &key) const
	{
		if (!hasDirective(key))
       		throw std::runtime_error("Directive " + key + " not found");
		return _directives.at(key);
	}
	const std::vector<ConfigBlock>& getChildren() const {return _children; }
};

#endif
