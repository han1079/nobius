
#include <iostream>
#include <vector>
#include <string>



enum class Mode {LEFT, RIGHT};

bool interleave_and_append(std::vector<std::string> list_1, std::vector<std::string> list_2, std::string& string_container) {
    
    if (list_1.size() != list_2.size())
    {
        std::cerr << "Incorrect list sizes" << std::endl;
        return false;
    }
    
    for (int i = 0; i < list_1.size(); i++) 
    {
        string_container = string_container + list_1[i] + list_2[i];
    }
    
    return true;
}

std::string format_debug_message(std::vector<std::string> list_of_strings, std::vector<float> list_of_floats, Mode alignment) {
    
    std::string to_return;
    
    /*Checks
    1. Lengths must be within 1 of each other.
    2. If both are zero, error out and return.
    3. If one is zero - convert to string if needed, and return
    */
    
    int _sf = list_of_floats.size();
    int _ss = list_of_strings.size();
    
    if (abs(_sf - _ss) > 1) {
        std::cout << "Mismatched" << std::endl;
        return "";
    }
    
    else if (_sf == 0 && _ss == 0) {
        std::cout << "Empty lists" << std::endl;
        return "";
    }
    else if (_sf == 0 && _ss == 1) {
        return list_of_strings[0];
    }
    else if (_sf == 1 && _ss == 0) {
        return std::to_string(list_of_floats[0]);
    }

    /*Perform string-ification of non-string values.*/
    
    std::vector<std::string> stringy_floats;
    for (auto& i : list_of_floats)
    {
        stringy_floats.push_back(std::to_string(i));
    }
    
    /* Length operations
    1. If one is longer - that immediately gets pushed to be the start. Interleave the rest
    2. If they are the same - interleave and return
    */
    
    if (_sf > _ss) {
        
        to_return = stringy_floats[0];
        interleave_and_append(list_of_strings, 
                            std::vector<std::string>(stringy_floats.begin() + 1, stringy_floats.end()),
                            to_return);
        
    } else if (_ss > _sf) {
        
        to_return = list_of_strings[0];
        interleave_and_append(stringy_floats, 
                            std::vector<std::string>(list_of_strings.begin() + 1, list_of_strings.end()),
                            to_return);
    
    } else if (_sf == _ss && alignment == Mode::LEFT) {
        to_return = "";
        interleave_and_append(list_of_strings, stringy_floats, to_return);
    } else if (_sf == _ss && alignment == Mode::RIGHT) {
        to_return = "";
        interleave_and_append(stringy_floats, list_of_strings, to_return);
    } else {
        std::cout << "Shouldn't get here." << std::endl;
        return "";
    }
    
    return to_return;
}

int main()
{
    
    Mode alignment_mode = Mode::LEFT;
    
    std::vector<std::string> list_of_strings = {"One", "Two", "Three", "But No Four"};
    std::vector<float> list_of_floats = {1.0f, 2.0f, 3.0f};

    std::string debug_message = format_debug_message(list_of_strings, list_of_floats, Mode::LEFT);
    
    std::cout << debug_message << std::endl;
    return 0;
}