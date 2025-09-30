#include "SectionInfo.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

ConfigMgr::ConfigMgr()
{
    try {
		// ���ظ���Ŀ¼,��Ϊ�ҵ�cpp�ļ�����srcĿ¼��
        boost::filesystem::path current_path = boost::filesystem::current_path();

        // ����config.ini�ļ�������·��
        boost::filesystem::path config_path = current_path / "config.ini";
        //std::cout << "Config path: " << config_path << std::endl;

        // ��������ļ��Ƿ����
        if (!boost::filesystem::exists(config_path)) {
            std::cerr << "Error: Config file not found: " << config_path << std::endl;
            return;
        }

        // ʹ��Boost.PropertyTree����ȡINI�ļ�
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(config_path.string(), pt);

        // ����INI�ļ��е�����section
        for (const auto& section_pair : pt) {
            const std::string& section_name = section_pair.first;
            const boost::property_tree::ptree& section_tree = section_pair.second;

            // ����ÿ��section�����������е�key-value��
            SectionInfo section_config;
            for (const auto& key_value_pair : section_tree) {
                const std::string& key = key_value_pair.first;
                const std::string& value = key_value_pair.second.get_value<std::string>();
                section_config.addKeyValue(key,value);
            }

            // ����SectionInfo���󲢱��浽����ӳ�����
            SectionInfo sectionInfo(section_config);
            _config_map[section_name] = sectionInfo;
        }

        // ������е�section��key-value�ԣ�������Ϣ��
        std::cout << "=== Loaded Configuration ===" << std::endl;
        for (const auto& section_entry : _config_map) {
            const std::string& section_name = section_entry.first;
            const SectionInfo& section_config = section_entry.second;

            std::cout << "[" << section_name << "]" << std::endl;// ������������
            for (const auto& key_value_pair : section_config._section_map) {// ����˿�
                std::cout << "  " << key_value_pair.first << " = " << key_value_pair.second << std::endl;
            }
        }
        std::cout << "=== End of Configuration ===" << std::endl;

    }
    catch (const boost::property_tree::ini_parser_error& e) {
        // INI�ļ���������
        std::cerr << "INI Parser Error: " << e.what() << std::endl;
        std::cerr << "File: " << e.filename() << ", Line: " << e.line() << std::endl;
    }
    catch (const boost::filesystem::filesystem_error& e) {
        // �ļ�ϵͳ����
        std::cerr << "Filesystem Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // ������׼�쳣
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}
