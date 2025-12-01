#pragma once

#ifndef PCH_H_INCLUDED
#include <variant>
#include <unordered_map>
#include <string>
#endif

typedef std::variant<std::nullptr_t, 
    std::reference_wrapper<int>, 
    std::reference_wrapper<uint32_t>,
    std::reference_wrapper<uint64_t>,
    std::reference_wrapper<float>, 
    std::reference_wrapper<double>, 
    std::reference_wrapper<bool>, 
    std::reference_wrapper<std::string>, 
    std::reference_wrapper<std::pair<int, int>>,
    std::monostate> P_WC_T;


class DebugConsole {
public:
    static DebugConsole& getInstance() {
        static DebugConsole instance;
        return instance;
    }

    // Hook a variable for monitoring (updates in place)
    void hookVariable(const std::string& name, const P_WC_T& variable) {
        m_variable_hooks[name] = variable;
        // Auto-enable visibility for newly registered variables
        if (m_variable_visibility.find(name) == m_variable_visibility.end()) {
            m_variable_visibility[name] = true;
        }
    }

    void addMessage(const std::string& ID, const std::string& msg) {
        m_messages[ID] = msg;
    }

    std::string getVariableValueAsString(const std::string& name) {
        auto data = m_variable_hooks.find(name);
        if (data == m_variable_hooks.end()) return "Not found";
        
        return std::visit([](auto&& var) -> std::string {
            using T = std::decay_t<decltype(var)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return "null";
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<int>>) {
                return std::to_string(var.get());
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<float>>) {
                return std::to_string(var.get());
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<double>>) {
                return std::to_string(var.get());
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<uint32_t>>) {
                return std::to_string(var.get());
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<uint64_t>>) {
                return std::to_string(var.get());
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<bool>>) {
                return var.get() ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<std::string>>) {
                return var.get();
            } else if constexpr (std::is_same_v<T, std::reference_wrapper<std::pair<int, int>>>) {
                auto p = var.get();
                return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                return "undefined";
            } else {
                return "unknown type";
            }
        }, data->second);
    }
    
    // Get all registered variable names
    std::vector<std::string> getRegisteredVariables() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : m_variable_hooks) {
            names.push_back(name);
        }
        return names;
    }
    
    // Remove a variable hook
    void unhookVariable(const std::string& name) {
        m_variable_hooks.erase(name);
        m_variable_visibility.erase(name);
    }
    void render(bool* p_open);
    
private:
    DebugConsole() = default;
    ~DebugConsole() = default;
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;

    std::unordered_map<std::string, P_WC_T> m_variable_hooks;
    std::unordered_map<std::string, bool> m_variable_visibility;  // Track which variables to show

    std::unordered_map<std::string, std::string> m_messages;
};
