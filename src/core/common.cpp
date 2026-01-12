// Implementation file for common.h to help language server
#include <core/common.h>
#include <random>


UUID_t generate_uuid() {
    static std::random_device rd;
    static std::mt19937_64 rng(rd());
    static std::uniform_int_distribution<uint64_t> distr;

    return distr(rng);
}
// This file exists primarily to give the language server proper
// compilation context for common.h