#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <asset/asset.h>

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() &&
           0 == str.compare(0, prefix.size(), prefix);
}

enum RuleType { START, END, FULL };

class Rule {
   public:
    std::string text;
    RuleType type;
    Rule(std::string text, RuleType type);
    bool match(std::string& to_match);
};

Rule::Rule(std::string text, RuleType type) : text(text), type(type) {}

bool Rule::match(std::string& to_match) {
    switch (this->type) {
        case RuleType::START:
            return startsWith(to_match, this->text);
        case RuleType::END:
            return endsWith(to_match, this->text);
        case RuleType::FULL:
            return this->text.size() == to_match.size() &&
                   startsWith(to_match, this->text);
    }
    return false;
}

std::vector<Rule> load_ignore(std::filesystem::path ignore_path) {
    std::ifstream in_file(ignore_path);
    if (!in_file.is_open()) {
        return std::vector<Rule>();
    }
    std::vector<Rule> rules;
    std::string line;
    while (std::getline(in_file, line)) {
        RuleType type = RuleType::FULL;
        if (line[0] == '*') {
            type = RuleType::END;
            line = line.substr(1);
        } else if (line[line.size() - 1] == '*') {
            type = RuleType::START;
            line = line.substr(0, line.size() - 1);
        }
        rules.push_back(Rule(line, type));
    }
    return rules;
}

int main(int argc, char const* argv[]) {
    if (argc == 2) {
        std::string resource_folder_name(argv[1]);
        std::filesystem::path resource_path =
            std::filesystem::absolute(resource_folder_name);
        logging::Logger logger;
        logger.debug_stream() << "bundling assets at path: " << resource_path
                              << logging::COLOR_RS << std::endl;
        std::filesystem::path ignore_path = resource_path;
        ignore_path.append(".assetignore");
        auto ignore_rules = load_ignore(ignore_path);

        asset::Assets assets(logger, resource_folder_name);
        for (const auto& path :
             std::filesystem::recursive_directory_iterator(resource_path)) {
            if (!path.is_directory()) {
                std::string filename = path.path().string();
                std::string resource_name =
                    filename.substr(resource_path.string().length());
                if (resource_name == ".assetignore") {
                    continue;
                }
                for (Rule& rule : ignore_rules) {
                    if (rule.match(resource_name)) {
                        logger.warn_stream()
                            << "skipping asset: " << resource_name
                            << logging::COLOR_RS << std::endl;
                        goto loop_end;
                    }
                }
                if (endsWith(resource_name, ".py")) {
                    assets.load_python(resource_name);
                } else if (endsWith(resource_name, ".png")) {
                    assets.load_image(resource_name);
                } else {
                    assets.load_generic(resource_name);
                }
            }
        loop_end:;
        }
        std::vector<unsigned char> data = assets.store_assets();
        std::ofstream out_file("assets.c");
        out_file << "const unsigned char assets[" << data.size() << "] = {";
        for (size_t i = 0; i < data.size(); i++) {
            out_file << "0x" << std::uppercase << std::hex << (int)data[i];
            if (i + 1 < data.size()) out_file << ", ";
        }
        out_file << "};" << std::endl;
        out_file << "const unsigned long long assets_len = " << std::dec
                 << data.size() << ";" << std::endl;
        return 0;
    }
    std::cerr << "usage: " << argv[0] << " <folder>" << std::endl;
    return 1;
}
