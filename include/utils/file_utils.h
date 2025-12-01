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
} // namespace FileUtils