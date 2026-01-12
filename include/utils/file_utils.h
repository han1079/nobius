#pragma once
#include <pch.h>
#include <core/common.h>

namespace FileUtils {

    inline static std::string read_file_to_string(const std::string& file_path, bool default_path = true) {
        std::string full_path = file_path;
        if (default_path) {
            full_path = std::string(PROJECT_SOURCE_DIR) + "/" + file_path;
        }

        std::ifstream file_stream(full_path);
        if (!file_stream.is_open()) {
            Debug::log("Failed to open file: " + full_path, DebugLevel::ERROR);
            return "";
        }

        std::stringstream buffer;
        buffer << file_stream.rdbuf();
        file_stream.close();
        return buffer.str();
    }

    inline static nlohmann::json read_file_to_json(const std::string& file_path, bool default_path = true) {
        std::string file_content = read_file_to_string(file_path, default_path);
        if (file_content.empty()) {
            Debug::log("File content is empty for: " + file_path, DebugLevel::ERROR);
            return nlohmann::json();
        }

        try {
            nlohmann::json json_data = nlohmann::json::parse(file_content);
            return json_data;
        } catch (const nlohmann::json::parse_error& e) {
            Debug::log("JSON parse error in file: " + file_path + " - " + e.what(), DebugLevel::ERROR);
            return nlohmann::json();
        }
    }
} // namespace FileUtils