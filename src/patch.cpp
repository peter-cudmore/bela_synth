
/*//
// Created by Pete on 9/09/2023.

//
#include "patch.h"

// midi cc change -> update patch
// ui change      -> update patch

// update patch   -> update ui
//                -> update kernel



#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define TO_JSON_ACCESSOR(x)  {TO_STRING(x), p.x}
#define TO_JSON_BOUNDS(x) {TO_STRING(x), p.x.bounds()}

#define FROM_JSON_ACCESSOR(x) j.at(TO_STRING(x)).get_to(p.x);

void to_json(json &j, const Patch &p) {
    j = json{
        #define ENTRY(CC, SECTION, NAME, KLASS, VALUE) TO_JSON_ACCESSOR(SECTION##_##NAME),
            CONTROL_TABLE
        #undef ENTRY
    };
}

void from_json(const json& j, Patch& p) {
    #define ENTRY(CC, SECTION, NAME, KLASS, VALUE) FROM_JSON_ACCESSOR(SECTION##_##NAME)
        CONTROL_TABLE
    #undef ENTRY
}




Patch load_from_file(std::string filename){
    std::ifstream f(filename);
    json data = json::parse(f);
    Patch p = data.template get<Patch>();
    return p;
}

void save_to_file(const Patch& patch, const std::string& filename){

    std::ofstream f(filename);
    json j = patch;
    f << std::setw(4) << j << std::endl;

}

*/