#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include "Wire.h"
#include "Gate.h"

using namespace std;

struct GateInfo {
    string type;
    string output_wire;
    vector<string> input_wires;
};

class Circuit {
    public:
        ~Circuit() {
            for (auto& gate : m_gates_storage) {
                delete gate;
            }
            for (auto& pair : m_wires_map) {
                delete pair.second;
            }
        }
        bool build(const string filename);
        bool run_all_patterns();
        bool read_pattern(const string pattern_file);
        bool print_result(const string output_file);

    private:

        vector<string> m_primary_inputs;
        vector<string> m_primary_outputs;

        unordered_map<string, Wire*> m_wires_map;

        vector<Gate*> m_gates_storage;
        vector<Gate*> m_simulation_order;
        
        Wire* get_or_create_wire(const string& name);
        void topological_sort();
        void simulate(const std::map<std::string, LogicValue>& inputs);

        vector<string> input_pattern_names;
        vector<vector<LogicValue>> input_pattern_data;
        
        vector<vector<LogicValue>> output_data;
};

#endif