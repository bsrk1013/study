#pragma once
#include <map>
#include <string>
#include <any>
#include <yaml-cpp/yaml.h>

namespace DBBD
{
	class Config {
	public:
		void set(std::string key, std::any value) {
			tables[key] = value;
		}

		template <typename T>
		T get(const std::string& key) {
			auto it = tables.find(key);
			if (it == tables.end()) {
				throw std::exception("illegal config key value...");
			}

			if constexpr (std::is_same<std::string, T>::value) {
				return *tables[key]._Cast<std::string>();
			}
			else if constexpr (std::is_same<int, T>::value) {
				return *tables[key]._Cast<int>();
			}
			else if constexpr (std::is_same<float, T>::value) {
				return *tables[key]._Cast<float>();
			}
			else if constexpr (std::is_same<std::vector<std::string>, T>::value) {
				return *tables[key]._Cast<std::vector<std::string>>();
			}
			else if constexpr (std::is_same<std::vector<int>, T>::value) {
				return *tables[key]._Cast<std::vector<int>>();
			}
			else if constexpr (std::is_same<std::vector<float>, T>::value) {
				return *tables[key]._Cast<std::vector<float>>();
			}
			else if constexpr (std::is_same<Config, T>::value) {
				return *tables[key]._Cast<Config>();
			}
			else {
				throw std::exception("illegal config template parameter value...");
			}
		}

	private:
		std::map<std::string, std::any> tables;
	};

	class YamlParser
	{
	public:
		static Config loadConfig(std::string path) {
			auto doc = YAML::LoadFile(path);
			return parse(doc);
		}

	private:
		static Config parse(YAML::Node doc) {
			Config config;
			for (auto node : doc) {
				rawParse(&config, node);
			}
			return config;
		}

		static void rawParse(Config* config, YAML::detail::iterator_value root) {
			std::string key = root.first.as<std::string>();
			if (root.second.IsScalar()) {
				try {
					config->set(key, root.second.as<int>());
					return;
				}
				catch (const std::exception&) {
					try {
						config->set(key, root.second.as<float>());
						return;
					}
					catch (const std::exception&) {
						config->set(key, root.second.as<std::string>());
					}
				}
			}
			else if (root.second.IsSequence()) {
				try {
					config->set(key, root.second.as<std::vector<int>>());
				}
				catch (const std::exception&) {
					try {
						config->set(key, root.second.as<std::vector<float>>());
					}
					catch (const std::exception&) {
						config->set(key, root.second.as<std::vector<std::string>>());
					}
				}
			}
			else if (root.second.IsMap()) {
				auto childConfig = parse(root.second);
				config->set(key, childConfig);
			}
		}
	};
}