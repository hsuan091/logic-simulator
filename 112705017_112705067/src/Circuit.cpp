// circuit.cpp

#include "Circuit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_set>

const string CONST_ONE_WIRE_NAME = "$$CONST_1$$";
const string CONST_ZERO_WIRE_NAME = "$$CONST_0$$";

// ----- 輔助函式 -----
vector<string> split_and_trim(const string& input, char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) {
        item = regex_replace(item, regex(R"(\s+)"), "");
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

void Circuit::topological_sort() {
    // 步驟 1: 建立高效的圖表示，並計算入度
    // `in_degree`: 儲存每個 Gate* 有多少個來自其他 Gate 的輸入。
    unordered_map<Gate*, int> in_degree;
    
    // Value: 一個 vector，包含所有以這個 Wire 為輸入的 Gate*
    unordered_map<Wire*, vector<Gate*>> consumers_of_wire;
    
    // 初始化所有閘的入度為 0 、 建立 consumers_of_wire 這個鄰接表。
    for (Gate* gate : m_gates_storage) {
        in_degree[gate] = 0;
        for (Wire* input_wire : gate->getInputs()) {
            consumers_of_wire[input_wire].push_back(gate);
        }
    }
    
    // 再次遍歷所有閘，這次是為了根據依賴關係，正確計算入度。
    for (Gate* gate : m_gates_storage) {
        Wire* output_wire = gate->getOutput();
        if (output_wire) {
            // 如果這個輸出 wire 被其他閘所消費（即作為它們的輸入）...
            if (consumers_of_wire.count(output_wire)) {
                // ...那麼所有消費它的下游閘，都依賴於目前這個閘。
                for (Gate* consumer_gate : consumers_of_wire.at(output_wire)) {
                    in_degree[consumer_gate]++;
                }
            }
        }
    }

    // 步驟 2: 找到所有入度為 0 的起始 Gate
    
    vector<Gate*> queue;
    for (Gate* gate : m_gates_storage) {
        if (in_degree[gate] == 0) {
            queue.push_back(gate);
        }
    }

    // 步驟 3: 高效的拓樸排序主迴圈
    
    m_simulation_order.clear();
    m_simulation_order.reserve(m_gates_storage.size()); // 預先分配記憶體

    size_t head = 0;
    while (head < queue.size()) {
        // 從佇列中取出一個 Gate (比 pop_back 效率稍高)
        Gate* u = queue[head++];
        
        m_simulation_order.push_back(u);
        
        Wire* output_wire_of_u = u->getOutput();
        if (!output_wire_of_u) {
            continue;
        }
        
        // **高效的關鍵**：直接從鄰接表中找到所有下游鄰居，不再需要遍歷所有閘！
        if (consumers_of_wire.count(output_wire_of_u)) {
            for (Gate* v : consumers_of_wire.at(output_wire_of_u)) {
                in_degree[v]--;
                if (in_degree[v] == 0) {
                    queue.push_back(v);
                }
            }
        }
    }
}


bool Circuit::build(const string filename) {

    bool multi_dirver_error = false;
    bool combinational_loop_error = false;

    Wire* const_one = get_or_create_wire(CONST_ONE_WIRE_NAME);
    const_one->value = LogicValue::ONE;

    Wire* const_zero = get_or_create_wire(CONST_ZERO_WIRE_NAME);
    const_zero->value = LogicValue::ZERO;


    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "[錯誤] 無法開啟檔案 '" << filename << "'" << endl;
        return false;
    }

    stringstream buffer;
    buffer << input_file.rdbuf();
    string code = buffer.str();
    input_file.close();

    // 1. 預處理：移除註解
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*[\\s\\S]*?\\*/"), "");

    // set<string> input_set, output_set, wire_set;

    // 2. 解析模組頭（ANSI 風格）
    bool done = false;
    regex module_regex(R"(module\s+\w+\s*\(\s*([^;]*?)\s*\)\s*;)");
    smatch module_match;
    if (regex_search(code, module_match, module_regex)) {
        string header_content = module_match[1].str();
        
        if (regex_search(header_content, regex(R"(\b(input|output)\b)"))) {
            done = true;
            regex port_regex(R"(\b(input|output)\b\s*([a-zA-Z_]\w*)\s*([,)]|$))");
            for (auto i = sregex_iterator(header_content.begin(), header_content.end(), port_regex); i != sregex_iterator(); ++i) {
                smatch match = *i;
                string type = match[1].str();
                string name = match[2].str();
                if (type == "input") {
                    m_primary_inputs.push_back(name);
                } 
                else {
                    m_primary_outputs.push_back(name);
                }
            }
        }
    }

    // 3. 傳統風格解析
    auto parse_declarations = [&](const string& keyword, vector<string>& target_vec) {
        regex decl_regex(keyword + R"(\s+([^;]+?);)");
        for (auto i = sregex_iterator(code.begin(), code.end(), decl_regex); i != sregex_iterator(); ++i) {
            for (const auto& var : split_and_trim((*i)[1].str(), ',')) {
                target_vec.push_back(var);
            }
        }
    };

    if (!done){
        parse_declarations("input", m_primary_inputs);
        parse_declarations("output", m_primary_outputs);
    } 


    vector<GateInfo> parsed_gates;


    // 4. 解析邏輯閘
    regex gate_regex(R"((xor|and|or|nand|nor|not|buf|xnor)\s*\(\s*([^,)\s]+)\s*,\s*([^,)\s]+)\s*(?:,\s*([^,)\s]+))?\s*\);)");
    map<string, int> driver_count;
    for (auto i = sregex_iterator(code.begin(), code.end(), gate_regex); i != sregex_iterator(); ++i) {
        smatch match = *i;

        GateInfo info;
        info.type = match[1].str();
        info.output_wire = match[2].str();

        string input1_str = match[3].str();

        if (input1_str == "1'b1") {
            info.input_wires.push_back(CONST_ONE_WIRE_NAME);
        } 
        else if (input1_str == "1'b0") {
            info.input_wires.push_back(CONST_ZERO_WIRE_NAME);
        } 
        else {
            info.input_wires.push_back(input1_str);
        }

        if (match.size() > 4 && match[4].matched) {
            string input2_str = match[4].str();
            if (input2_str == "1'b1") {
                info.input_wires.push_back(CONST_ONE_WIRE_NAME);
            } 
            else if (input2_str == "1'b0") {
                info.input_wires.push_back(CONST_ZERO_WIRE_NAME);
            } 
            else {
                info.input_wires.push_back(input2_str);
            }
        }
        parsed_gates.push_back(info);
    }

    // 5. 檢查多驅動 (Multi-driver) 錯誤
    unordered_set<string> driven_wire_names;

    // 標記所有 primary inputs 為驅動源
    for (const auto& name : m_primary_inputs) {
        driven_wire_names.insert(name);
    }

    for (const auto& gate_info : parsed_gates) {
        if (!driven_wire_names.insert(gate_info.output_wire).second){
            multi_dirver_error = true;
        }
    }

    // 創建所有輸入線路
    for(const auto& gate_info : parsed_gates) {
        // 確保輸出線路存在
        Wire* output_wire = this->get_or_create_wire(gate_info.output_wire);

        // 確保輸入線路存在
        vector<Wire*> input_wires;
        for (const string& input_name : gate_info.input_wires) {
            input_wires.push_back(this->get_or_create_wire(input_name));
        }

        // 根據類型建立對應的 Gate 物件
        Gate* new_gate = nullptr;
        if (gate_info.type == "and") {
            new_gate = new AndGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "or") {
            new_gate = new OrGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "nand") {
            new_gate = new NandGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "nor") {
            new_gate = new NorGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "xor") {
            new_gate = new XorGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "xnor") {
            new_gate = new XnorGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "not") {
            new_gate = new NotGate(output_wire, input_wires);
        } 
        else if (gate_info.type == "buf") {
            new_gate = new BufGate(output_wire, input_wires);
        } 
        else {
            cout << "[錯誤] 未知的閘類型 '" << gate_info.type << "'。" << endl;
            return false;
        }

        m_gates_storage.push_back(new_gate);
    }

    this->topological_sort();

    // conbination loop detecte
    if (m_simulation_order.size() != m_gates_storage.size()) {
        combinational_loop_error = true;
    }

    if (multi_dirver_error && combinational_loop_error) {
        cout << "Error! The netlist has both combinational loops and multi-driver nets" << endl;
        return false;
    }
    else if (multi_dirver_error) {
        cout << "Error! The netlist has multi-driver nets" << endl;
        return false;
    } 
    else if (combinational_loop_error) {
        cout << "Error! The netlist has combinational loops" << endl;
        return false;
    }

    return true;
}



Wire* Circuit::get_or_create_wire(const string& name) {
    auto it = m_wires_map.find(name);
    if (it != m_wires_map.end()) {
        return it->second;
    } 
    else {
        Wire* new_wire = new Wire(name);
        m_wires_map[name] = new_wire;
        return new_wire;
    }
}



bool Circuit::read_pattern(const string pattern_file) {
    // 1. 讀取 Pattern 檔案
    ifstream p_file(pattern_file);
    if (!p_file.is_open()) {
        cout << "錯誤: 無法開啟 pattern 檔案 '" << pattern_file << "'" << endl;
        return false;
    }

    string line, token;


    if (getline(p_file, line)) {
        stringstream ss(line);
        ss >> token; // 讀取並捨棄 ".i"
        while (std::getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t\n\r"));
            token.erase(token.find_last_not_of(" \t\n\r") + 1);
            input_pattern_names.push_back(token);
        }
    }

    map<string, LogicValue> convert_table = {
        {"0", LogicValue::ZERO},
        {"1", LogicValue::ONE},
        {"X", LogicValue::UNKNOWN},
        {"Z", LogicValue::BIG_Z}
    };

    while (getline(p_file, line) && line.find(".end") == string::npos) {
        if (line.empty()) continue;
        stringstream ss(line);
        vector<LogicValue> row;
        while (ss >> token) {
            row.push_back(convert_table[token]);
        }
        if (!row.empty()) {
            input_pattern_data.push_back(row);
        }
    }
    p_file.close();

    return true;
}

bool Circuit::print_result(const string output_file) {
    ofstream out_file(output_file);
    if (!out_file.is_open()) {
        cout << "錯誤: 無法開啟輸出檔案 '" << output_file << "'" << endl;
        return false;
    }

    out_file << "output";
    for (const auto& output : m_primary_outputs) {
        out_file << " " << output;
        if (output != m_primary_outputs.back()) {
            out_file << ",";
        } 
        else {
            out_file << endl; // 最後一個輸出後換行
        }
    }

    for (const auto& data_vec : output_data) {
        out_file << "   ";
        for(const auto& data : data_vec) {
            switch (data) {
                case LogicValue::ZERO:
                    out_file << " 0";
                    break;
                case LogicValue::ONE:
                    out_file << " 1";
                    break;
                case LogicValue::UNKNOWN:
                    out_file << " X";
                    break;
                case LogicValue::BIG_Z:
                    out_file << " Z";
                    break;
            }
        }
        out_file << endl;
    }
    out_file << ".end";

    out_file.close();
    return true;
}



void Circuit::simulate(const map<string, LogicValue>& inputs) {
    // 1. 重設所有 wire 的狀態
    for (const auto& pair : m_wires_map) {
        const string& name = pair.first;
        Wire* wire_ptr = pair.second;
        
        if (name != CONST_ONE_WIRE_NAME && name != CONST_ZERO_WIRE_NAME) {
            wire_ptr->value = LogicValue::UNKNOWN;
        }
    }

    // 2. 設定主輸入的值
    for (const auto& pair : inputs) {
        const string& input_name = pair.first;
        LogicValue value = pair.second;
        auto it = m_wires_map.find(input_name);
        
        if (it != m_wires_map.end()) {
            it->second->value = value;
        }
    }

    // 3. 按照拓樸順序執行模擬
    for (Gate* gate : m_simulation_order) {
        gate->evaluate();
    }
}

bool Circuit::run_all_patterns() {
    output_data.clear();
    output_data.reserve(input_pattern_data.size());

    // 1. 遍歷每一個輸入 pattern (每一行測試向量)
    for (const auto& current_pattern : input_pattern_data) {
        // cout << endl;
        
        // 2. 準備本次模擬的輸入 map
        //    將 vector<LogicValue> 轉換為 simulate() 函式需要的 map<string, LogicValue>
        map<string, LogicValue> simulation_inputs;
        for (size_t i = 0; i < input_pattern_names.size(); ++i) {
            simulation_inputs[input_pattern_names[i]] = current_pattern[i];
            // cout.flush();
            // cout << "Setting " << input_pattern_names[i] << " to " << static_cast<int>(current_pattern[i]) << endl;
            // cout.flush();
        }

        // 3. 執行單次模擬
        this->simulate(simulation_inputs);

        // 4. 收集這次模擬的輸出結果
        vector<LogicValue> current_output_row;
        current_output_row.reserve(m_primary_outputs.size()); // 預分配空間

        // 嚴格按照 m_primary_outputs 中定義的順序來提取輸出值
        for (const string& output_name : m_primary_outputs) {
            // 獲取 Wire 指標
            Wire* out_wire = get_or_create_wire(output_name);
            current_output_row.push_back(out_wire->value);
        }

        output_data.push_back(current_output_row);
    }
    return true;
}