//
// Created by Pete on 9/09/2023.
//
#include "patch.h"

NLOHMANN_JSON_SERIALIZE_ENUM(OscillatorType, {
                                                     {Sine, "sin"},
                                                     {Saw, "saw"},
                                                     {Square, "sqr"},
                                             })

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OscillatorParams, type, coarse, fine, gain)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnvelopeParams, attack, decay, sustain, release)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FilterParams, cutoff, resonance, adsr, cutoff_envelope, resonance_envelope)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MixerParams, gain, adsr)

void to_json(json& j, const Patch& p){
    j = json{
            {"osc_1", p.osc_1},
            {"osc_2", p.osc_2},
            {"osc_3", p.osc_3},
            {"filter", p.filter},
            {"mixer", p.mixer}
    };
}

void from_json(const json& j, Patch &p) {
    j.at("osc_1").get_to(p.osc_1);
    j.at("osc_2").get_to(p.osc_2);
    j.at("osc_3").get_to(p.osc_3);
    j.at("filter").get_to(p.filter);
    j.at("mixer").get_to(p.mixer);
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

