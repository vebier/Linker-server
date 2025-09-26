#include "SectionInfo.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

ConfigMgr::ConfigMgr()
{
    try {
		// 返回父级目录,因为我的cpp文件都在src目录下
        boost::filesystem::path current_path = boost::filesystem::current_path();

        // 构建config.ini文件的完整路径
        boost::filesystem::path config_path = current_path / "config.ini";
        std::cout << "Config path: " << config_path << std::endl;

        // 检查配置文件是否存在
        if (!boost::filesystem::exists(config_path)) {
            std::cerr << "Error: Config file not found: " << config_path << std::endl;
            return;
        }

        // 使用Boost.PropertyTree来读取INI文件
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(config_path.string(), pt);

        // 遍历INI文件中的所有section
        for (const auto& section_pair : pt) {
            const std::string& section_name = section_pair.first;
            const boost::property_tree::ptree& section_tree = section_pair.second;

            // 对于每个section，遍历其所有的key-value对
            SectionInfo section_config;
            for (const auto& key_value_pair : section_tree) {
                const std::string& key = key_value_pair.first;
                const std::string& value = key_value_pair.second.get_value<std::string>();
                section_config.addKeyValue(key,value);
            }

            // 创建SectionInfo对象并保存到配置映射表中
            SectionInfo sectionInfo(section_config);
            _config_map[section_name] = sectionInfo;
        }

        // 输出所有的section和key-value对（调试信息）
        std::cout << "=== Loaded Configuration ===" << std::endl;
        for (const auto& section_entry : _config_map) {
            const std::string& section_name = section_entry.first;
            const SectionInfo& section_config = section_entry.second;

            std::cout << "[" << section_name << "]" << std::endl;// 输出服务的名字
            for (const auto& key_value_pair : section_config._section_map) {// 输出端口
                std::cout << "  " << key_value_pair.first << " = " << key_value_pair.second << std::endl;
            }
        }
        std::cout << "=== End of Configuration ===" << std::endl;

    }
    catch (const boost::property_tree::ini_parser_error& e) {
        // INI文件解析错误
        std::cerr << "INI Parser Error: " << e.what() << std::endl;
        std::cerr << "File: " << e.filename() << ", Line: " << e.line() << std::endl;
    }
    catch (const boost::filesystem::filesystem_error& e) {
        // 文件系统错误
        std::cerr << "Filesystem Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // 其他标准异常
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}
