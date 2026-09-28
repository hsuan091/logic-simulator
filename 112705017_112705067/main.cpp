#include "Gate.h"
#include "Circuit.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <queue>

using namespace std;

// struct Gate_Struct {
//     string gate_type; // and, or, xor
//     string output;    // 輸出參數
//     string input1;    // 輸入參數1
//     string input2;    // 輸入參數2
// };

// // ----- 輔助函式 -----
// vector<string> split_and_trim(const string& input, char delimiter) {
//     vector<string> result;
//     stringstream ss(input);
//     string item;
//     while (getline(ss, item, delimiter)) {
//         item = regex_replace(item, regex(R"(\s+)"), "");
//         if (!item.empty()) {
//             result.push_back(item);
//         }
//     }
//     return result;
// }

// bool parseVerilog(const string& filename, vector<string>& inputs, vector<string>& outputs, vector<string>& wires, vector<Gate_Struct>& gates) {
//     inputs.clear(); outputs.clear(); wires.clear(); gates.clear();

//     ifstream input_file(filename);
//     if (!input_file.is_open()) {
//         cerr << "[錯誤] 無法開啟檔案 '" << filename << "'" << endl;
//         return false;
//     }

//     stringstream buffer;
//     buffer << input_file.rdbuf();
//     string code = buffer.str();
//     input_file.close();

//     // 1. 預處理：移除註解
//     code = regex_replace(code, regex("//.*"), "");
//     code = regex_replace(code, regex("/\\*[\\s\\S]*?\\*/"), "");

//     set<string> input_set, output_set, wire_set;

//     // 2. 解析模組頭（ANSI 風格）
//     bool done = false;
//     regex module_regex(R"(module\s+\w+\s*\(\s*([^;]*?)\s*\)\s*;)");
//     smatch module_match;
//     if (regex_search(code, module_match, module_regex)) {
//         string header_content = module_match[1].str();
        
//         if (regex_search(header_content, regex(R"(\b(input|output)\b)"))) {
//             done = true;
//             regex port_regex(R"(\b(input|output)\b\s*([a-zA-Z_]\w*)\s*([,)]|$))");
//             for (auto i = sregex_iterator(header_content.begin(), header_content.end(), port_regex); i != sregex_iterator(); ++i) {
//                 smatch match = *i;
//                 string type = match[1].str();
//                 string name = match[2].str();
//                 if (type == "input") {
//                     input_set.insert(name);
//                 } else {
//                     output_set.insert(name);
//                 }
//             }
//         }
//     }

//     // 3. 傳統風格解析
//     auto parse_declarations = [&](const string& keyword, set<string>& target_set) {
//         regex decl_regex(keyword + R"(\s+([^;]+?);)");
//         for (auto i = sregex_iterator(code.begin(), code.end(), decl_regex); i != sregex_iterator(); ++i) {
//             for (const auto& var : split_and_trim((*i)[1].str(), ',')) {
//                 target_set.insert(var);
//             }
//         }
//     };

//     if (!done){
//         parse_declarations("input", input_set);
//         parse_declarations("output", output_set);
//     } 
//     parse_declarations("wire", wire_set);

//     // 4. 解析邏輯閘
//     regex gate_regex(R"((xor|and|or|nand|nor|not|buf)\s*\(\s*(\w+)\s*,\s*(\w+)\s*(?:,\s*(\w+))?\s*\);)");
//     map<string, int> driver_count;
//     for (auto i = sregex_iterator(code.begin(), code.end(), gate_regex); i != sregex_iterator(); ++i) {
//         smatch match = *i;
//         Gate_Struct new_gate;
//         new_gate.gate_type = match[1].str();
//         new_gate.output = match[2].str();
//         new_gate.input1 = match[3].str();
//         new_gate.input2 = match[4].matched ? match[4].str() : "";
//         gates.push_back(new_gate);

//     }

//     // 5. 複製到最終結果
//     inputs.assign(input_set.begin(), input_set.end());
//     outputs.assign(output_set.begin(), output_set.end());
//     wires.assign(wire_set.begin(), wire_set.end());

//     cout << "--- Inputs ---" << endl;
//     for (const auto& var : inputs) {
//         cout << "- " << var << endl;
//     }

//     cout << "\n--- Outputs ---" << endl;
//     for (const auto& var : outputs) {
//         cout << "- " << var << endl;
//     }

//     cout << "\n--- Wires ---" << endl;
//     for (const auto& var : wires) {
//         cout << "- " << var << endl;
//     }

//     cout << "\n--- Logic Gates ---" << endl;
//     for (const auto& gate : gates) {
//         cout << "Type: " << gate.gate_type
//              << ", Output: " << gate.output
//              << ", Input1: " << gate.input1
//              << ", Input2: " << gate.input2
//              << endl;
//     }

//     return true;
// }

// bool readPatternFile(const string& filename, vector<string>& variable_names, vector<vector<string>>& data_rows) {
//     ifstream file(filename);
//     if (!file.is_open()) {
//         cout << "錯誤: 無法開啟檔案 '" << filename << "'" << endl;
//         return false;
//     }

//     string line, token;

//     // 讀取第一行 (變數名稱)
//     if (getline(file, line)) {
//         stringstream ss(line);
//         ss >> token; // 讀取並捨棄第一個關鍵字
//         while (getline(ss, token, ',')) {
//             token.erase(0, token.find_first_not_of(" \t\n\r"));
//             token.erase(token.find_last_not_of(" \t\n\r") + 1);
//             variable_names.push_back(token);
//         }
//     }

//     // 讀取資料行，直到檔案結束或遇到 ".end"
//     while (getline(file, line) && line.find(".end") == string::npos) {
//         if (line.empty()) continue;

//         stringstream ss(line);
//         vector<string> current_row;
//         while (ss >> token) {
//             current_row.push_back(token);
//         }
        
//         if (!current_row.empty()) {
//             data_rows.push_back(current_row);
//         }
//     }

//     file.close();




//     cout << "\n--- 解析完成 ---" << endl;
    
//     cout << "偵測到的變數 (" << variable_names.size() << "個): ";
//     for (size_t i = 0; i < variable_names.size(); ++i) {
//         cout << variable_names[i] << (i == variable_names.size() - 1 ? "" : ", ");
//     }
//     cout << endl;

//     cout << "\n偵測到的資料 (" << data_rows.size() << "行):" << endl;
//     for (size_t i = 0; i < data_rows.size(); ++i) {
//         cout << "第 " << i + 1 << " 行: ";
//         for (size_t j = 0; j < data_rows[i].size(); ++j) {
//             if (j < variable_names.size()) {
//                 cout << variable_names[j] << "=" << data_rows[i][j];
//             } else {
//                 cout << "額外資料=" << data_rows[i][j];
//             }
//             cout << (j == data_rows[i].size() - 1 ? "" : ", ");
//         }
//         cout << endl;
//     }
//     return true;
// }


// bool topologySort(const vector<string>& primary_inputs, vector<Gate_Struct>& gates, vector<Gate*>& sorted_gates) {

//     set<string> primary_inputs_set(primary_inputs.begin(), primary_inputs.end());

//     // Step 1: 建立 `信號 -> Gate 索引` 的映射，並檢查多驅動
//     map<string, int> signal_to_gate_idx;
//     for (int i = 0; i < gates.size(); ++i) {
//         if (signal_to_gate_idx.count(gates[i].output)) {
//             cerr << "[錯誤] 多驅動！信號 '" << gates[i].output << "' 被多個 gate 驅動。" << endl;
//             return 0;
//         }
//         signal_to_gate_idx[gates[i].output] = i;
//     }

//     // Step 2: 計算每個 gate 的入度 (in-degree) 和建立鄰接表 (adjacency list)
//     vector<int> in_degree(gates.size(), 0);
//     map<int, vector<int>> adj_list; // key: 上游 gate 索引, value: 下游 gate 索引列表

//     for (int i = 0; i < gates.size(); ++i) {
//         // 封裝成一個 lambda 函數來處理兩個輸入，避免重複程式碼
//         auto process_input = [&](const string& input_signal) {
//             // 如果這個輸入是由另一個 gate 產生的 (而不是主要輸入)
//             if (signal_to_gate_idx.count(input_signal)) {
//                 int producer_idx = signal_to_gate_idx[input_signal];
//                 // 增加一條從 producer 到當前 gate (i) 的邊
//                 adj_list[producer_idx].push_back(i);
//                 // 當前 gate (i) 的入度加 1
//                 in_degree[i]++;
//             } else if (!primary_inputs_set.count(input_signal)) {
//                 cerr << "[警告] Gate '" << gates[i].output << "' 的輸入 '" << input_signal << "' 既不是主要輸入，也不是由任何 gate 產生 (懸浮輸入)。" << endl;
//             }
//         };

//         process_input(gates[i].input1);
//         process_input(gates[i].input2);
//     }
    
//     // Step 3: 將所有入度為 0 的 gate 加入佇列
//     queue<int> q;
//     for (int i = 0; i < gates.size(); ++i) {
//         if (in_degree[i] == 0) {
//             q.push(i);
//         }
//     }

//     // Step 4: Kahn 演算法主體
//     vector<int> sorted_gate_indices;
//     while (!q.empty()) {
//         int u_idx = q.front();
//         q.pop();
//         sorted_gate_indices.push_back(u_idx);

//         // 對於 u 的每個下游鄰居 v
//         if (adj_list.count(u_idx)) {
//             for (int v_idx : adj_list[u_idx]) {
//                 in_degree[v_idx]--;
//                 // 如果 v 的所有依賴都已處理完畢
//                 if (in_degree[v_idx] == 0) {
//                     q.push(v_idx);
//                 }
//             }
//         }
//     }

//     // Step 5: 檢查是否有迴路並印出結果
//     if (sorted_gate_indices.size() != gates.size()) {
//         cout << "[錯誤] 檢測到組合邏輯迴路！無法完成拓撲排序。" << endl;
//         cout << "  只有 " << sorted_gate_indices.size() << " / " << gates.size() << " 個 gate 可以被排序。" << endl;
//         return 0;
//     }

//     cout << "\n--- 拓撲排序結果 ---" << endl;
//     for (size_t i = 0; i < sorted_gate_indices.size(); ++i) {
//         int gate_idx = sorted_gate_indices[i];

//         if (gates[gate_idx].gate_type == "and"){
//             AndGate and_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             AndGate* gate = new AndGate("out", "a", "b");
//             sorted_gates.push_back(&and_gate);
//         }
//         else if (gates[gate_idx].gate_type == "or"){
//             OrGate or_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             OrGate* gate = new OrGate("out", "a", "b");
//             sorted_gates.push_back(&or_gate);

//         }
//         else if (gates[gate_idx].gate_type == "nand"){
//             NandGate nand_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             NandGate* gate = new NandGate("out", "a", "b");
//             sorted_gates.push_back(&nand_gate);
//         }
//         else if (gates[gate_idx].gate_type == "nor"){
//             NorGate nor_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             NorGate* gate = new NorGate("out", "a", "b");
//             sorted_gates.push_back(&nor_gate);
//         }
//         else if (gates[gate_idx].gate_type == "xor"){
//             XorGate xor_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             XorGate* gate = new XorGate("out", "a", "b");
//             sorted_gates.push_back(&xor_gate);
//         }
//         else if (gates[gate_idx].gate_type == "xnor"){
//             XNorGate xnor_gate(gates[gate_idx].output, gates[gate_idx].input1, gates[gate_idx].input2);
//             XNorGate* gate = new XNorGate("out", "a", "b");
//             sorted_gates.push_back(&xnor_gate);
//         }
//         else if (gates[gate_idx].gate_type == "not"){
//             NotGate not_gate(gates[gate_idx].output, gates[gate_idx].input1);
//             NotGate* gate = new NotGate("out", "a");
//             sorted_gates.push_back(&not_gate);
//         }
//         else if (gates[gate_idx].gate_type == "buf"){
//             BufGate buf_gate(gates[gate_idx].output, gates[gate_idx].input1);
//             BufGate* gate = new BufGate("out", "a");
//             sorted_gates.push_back(&buf_gate);
//         }
//         // else if (gates[gate_idx].gate_type == "dff"){
//         //     Dff dff(gates[gate_idx].output, gates[gate_idx].input1); // 假設 DFF 只有一個輸入
//         //     Dff* gate = new Dff("out", "d");
//         //     sorted_gates.push_back(&dff);
//         // }
//         else {
//             cout << "[錯誤] 未知的閘類型 '" << gates[gate_idx].gate_type << "'。" << endl;
//             return 0;
//         }
//         cout << i + 1 << ". " << gates[gate_idx].gate_type << "(" << gates[gate_idx].output << ") = " << gates[gate_idx].input1 << ", " << gates[gate_idx].input2 << endl;
//     }

//     return 1;
// }

// int main(int argc, char* argv[]) {
//     string netlist, pattern, output;
//     for (int i = 1; i < argc; i += 2) {
//         string arg = argv[i];
//         if (arg == "-i") netlist = argv[i + 1];
//         else if (arg == "-p") pattern = argv[i + 1];
//         else if (arg == "-o") output = argv[i + 1];
//     }

//     if (netlist.empty() || pattern.empty() || output.empty()) {
//         cout << "用法: ./ls -i <netlist.v> -p <pattern.pat> -o <output.out>" << endl;
//         return 1;
//     }

//     vector<string> inputs;
//     vector<string> outputs;
//     vector<string> wires;
//     vector<Gate_Struct> gates;

//     if (!parseVerilog(netlist, inputs, outputs, wires, gates)){
//         cout << "解析 Verilog 檔案失敗。" << endl;
//         return 1;
//     }

//     vector<string> pattern_variable_names;
//     vector<vector<string>> pattern_rows;

//     if (!readPatternFile(pattern, pattern_variable_names, pattern_rows)) {
//         cout << "解析 Pattern 檔案失敗。" << endl;
//         return 1;
//     }

//     vector<Gate*> sorted_gates;

//     if(!topologySort(inputs, gates, sorted_gates)) {
//         cout << "拓撲排序失敗。" << endl;
//         return 1;
//     }

//     ofstream output_file(output);

//     if (!output_file.is_open()) {
//         cout << "錯誤: 無法開啟輸出檔案 '" << output << "'" << endl;
//         return 1;
//     }

//     output_file << "output";
//     for(const auto& o : output){
//         output_file << " " << o;
//         if (o != output.back()) {
//             output_file << ",";
//         }
//         else {
//             output_file << endl;
//         }
//     }
    
//     map<string, string> temp_set;
//     for(const auto& pattern : pattern_rows) {

//         int i = 0;
//         temp_set.clear();

//         for(const auto& variable : pattern_variable_names) {
//             temp_set[variable] = pattern[i++];
//         }

//         for(const auto& addr : sorted_gates){

//         }

//         for(const auto& output_var : outputs) {

//             output_file << "   ";

//             if (temp_set.find(output_var) == temp_set.end()) {
//                 output_file << " X"; // 如果沒有找到，輸出 X
//             } 
//             else {
//                 output_file << " " << temp_set[output_var];
//             }

//             if (output_var == outputs.back()) {
//                 output_file << endl;
//             }
//         }
//     }

//     output_file << ".end" << endl;
//     output_file.close();

//     return 0;
// }


int main(int argc, char* argv[]) {
    string netlist, pattern, output;
    for (int i = 1; i < argc; i += 2) {
        string arg = argv[i];
        if (arg == "-i") netlist = argv[i + 1];
        else if (arg == "-p") pattern = argv[i + 1];
        else if (arg == "-o") output = argv[i + 1];
    }

    if (netlist.empty() || pattern.empty() || output.empty()) {
        cout << "用法: ./ls -i <netlist.v> -p <pattern.pat> -o <output.out>" << endl;
        return 1;
    }


    Circuit circuit;
    if (!circuit.build(netlist)) {
        return 1;
    }

    if (!circuit.read_pattern(pattern)) {
        return 1;
    }

    if (!circuit.run_all_patterns()) {
        return 1;
    }

    if (!circuit.print_result(output)) {
        return 1;
    }

    return 0;
}