#include "debug_register.h"
#include <iostream>
#include <stdexcept>

#ifdef DEBUG
    #define DEBUG_PRINT(x) std::cout << x << std::endl; 
#endif

debug_register::debug_register() :
    font_obj("Arial", 10),
    font_info_obj(font_obj) {}

void debug_register::confirmed_debug_data_is_consistent(){
    size_t num_of_msgs = lstf_preface.size();
    if (lstf_ptto_debug_value.size()    != num_of_msgs || 
        lstf_formatting_function.size() != num_of_msgs) {
        throw std::runtime_error("Inconsistent Data Fields.");
    }
}

bool debug_register::register_new_message(std::string preface, dbg_wc_ptr_t ptto_debug_value) {

        // Guard line to make sure that everything matches before adding to the list of debugs.
        confirmed_debug_data_is_consistent(); 

        lstf_preface.push_back(preface);
        lstf_ptto_debug_value.push_back(ptto_debug_value);

        auto formatter_function = [preface, ptto_debug_value]() -> std::string
        {
            auto run_me_to_dereference_and_format = [](auto&& wildcard) {
                // This function is called by std::visit - which means the wildcard
                // is one of the types IN the variant - not the variant itself.
                using wildcard_type = std::decay_t<decltype(wildcard)>;

                constexpr bool COMPILER_SEES_INT = std::is_same_v<wildcard_type, int*>;
                constexpr bool COMPILER_SEES_FLOAT = std::is_same_v<wildcard_type, float*>;
                constexpr bool COMPILER_SEES_DOUBLE = std::is_same_v<wildcard_type, double*>;
                constexpr bool COMPILER_SEES_STRING = std::is_same_v<wildcard_type, std::string*>;
                constexpr bool COMPILER_SEES_BOOL = std::is_same_v<wildcard_type, bool*>;
                constexpr bool COMPILER_SEES_NULL = std::is_same_v<wildcard_type, std::monostate>;

                if constexpr (COMPILER_SEES_INT || COMPILER_SEES_FLOAT || COMPILER_SEES_DOUBLE) {
                    return std::to_string(*wildcard);
                
                } else if constexpr (COMPILER_SEES_STRING) {
                    return std::string(*wildcard);
                } else if constexpr (COMPILER_SEES_BOOL) {
                    if (*wildcard) {
                        return std::string("True");
                    } else {
                        return std::string("False");
                    }
                } else if constexpr (COMPILER_SEES_NULL) {
                    return std::string("");
                } else {
                    throw(std::invalid_argument("Type not one of the wildcard somehow."));
                }
            };
            auto debug_value_str = std::visit(run_me_to_dereference_and_format, ptto_debug_value);
            return preface + debug_value_str;
        };
        
        lstf_formatting_function.push_back(formatter_function);
        return true;
}

}
