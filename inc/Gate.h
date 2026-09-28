#ifndef GATE_H
#define GATE_H

#include <string>
#include <stdexcept>
#include <vector>
#include "Wire.h"

using namespace std;
class Gate {
public:

    Wire* output; 
    vector<Wire*> inputs;

    Gate(Wire* out_name, vector<Wire*>& input_names) : output(out_name), inputs(input_names) {}

    virtual ~Gate() = default;

    virtual void evaluate() const {
        output->value = LogicValue::UNKNOWN;
    }

    const vector<Wire*>& getInputs() const { return inputs; }
    Wire* getOutput() const { return output; }
};

class AndGate : public Gate {
public:
    AndGate(Wire* out_name, vector<Wire*>& input_names) : Gate(out_name, input_names) {}
    ~AndGate() override = default;
    void evaluate() const override;
};

class OrGate : public Gate {
public:
    OrGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) {}
    ~OrGate() override = default;
    void evaluate() const override;
};
class NandGate : public Gate {
public:
    NandGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) {}
    ~NandGate() override = default;
    void evaluate() const override;
};
class NorGate : public Gate {
public:
    NorGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) {}
    ~NorGate() override = default;
    void evaluate() const override;
};

class XorGate : public Gate {
public:
    XorGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) { }
    ~XorGate() override = default;
    void evaluate() const override;
};

class XnorGate : public Gate { 
public:
    XnorGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) {}
    ~XnorGate() override = default;
    void evaluate() const override;
};

class NotGate : public Gate {
public:
    NotGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) {}
    ~NotGate() override = default;
    void evaluate() const override;
};

class BufGate : public Gate {
public:
    BufGate(Wire* out_wire, vector<Wire*>& input_wires) : Gate(out_wire, input_wires) { }
    ~BufGate() override = default;
    void evaluate() const override;
};





// class OrGate : public Gate {
// public:
//     OrGate(const string& out_name, const string& in1_name, const string& in2_name);
//     ~OrGate() override = default;
//     Value evaluate(Value val1, Value val2) const override;
// };

// class NandGate : public Gate {
// public:
//     NandGate(const string& out_name, const string& in1_name, const string& in2_name);
//     ~NandGate() override = default;
//     Value evaluate(Value val1, Value val2) const override;
// };

// class NorGate : public Gate {
// public:
//     NorGate(const string& out_name, const string& in1_name, const string& in2_name);
//     ~NorGate() override = default;
//     Value evaluate(Value val1, Value val2) const override;
// };

// class XorGate : public Gate {
// public:
//     XorGate(const string& out_name, const string& in1_name, const string& in2_name);
//     ~XorGate() override = default;
//     Value evaluate(Value val1, Value val2) const override;
// };

// class XNorGate : public Gate { // 保持一致的類名
// public:
//     XNorGate(const string& out_name, const string& in1_name, const string& in2_name);
//     ~XNorGate() override = default;
//     Value evaluate(Value val1, Value val2) const override;
// };

// // --- 1-Input Gates (NOT, BUF) ---
// class NotGate : public Gate {
// public:
//     // input1 將是 NOT 門的輸入信號名
//     NotGate(const string& out_name, const string& in_name);
//     ~NotGate() override = default;
//     Value evaluate(Value val1) const override; // 重寫單參數版本
// };

// class BufGate : public Gate {
// public:
//     // input1 將是 BUF 門的輸入信號名
//     BufGate(const string& out_name, const string& in_name);
//     ~BufGate() override = default;
//     Value evaluate(Value val1) const override; // 重寫單參數版本
// };
/*
// --- DFF Gate ---
class DffGate : public Gate {
public:
    // 基類的 input1 是 D 的名字, input2 是 CLK 的名字, output 是 Q 的名字
    DffGate(const string& q_out_name, const string& d_in_name, const string& clk_in_name);
    ~DffGate() override = default;
    Value evaluate() const override; // DFF 的 evaluate 返回其當前 Q 狀態 (由外部提供)
    void onDffClockEvent(Value d_val, Value clk_val, Value& q_state_to_update_ref) override;
    void setDffInitialState(Value initial_val, Value& q_state_to_update_ref) override;
};
*/
#endif // GATE_H