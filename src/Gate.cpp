#include "Gate.h"

static constexpr LogicValue and_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ZERO, LogicValue::ZERO,    LogicValue::ZERO,    LogicValue::ZERO},
    /* ONE  */ {LogicValue::ZERO, LogicValue::ONE,     LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* UNKN */ {LogicValue::ZERO, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::ZERO, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue or_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ZERO, LogicValue::ONE,    LogicValue::UNKNOWN,    LogicValue::UNKNOWN},
    /* ONE  */ {LogicValue::ONE, LogicValue::ONE,     LogicValue::ONE,LogicValue::ONE},
    /* UNKN */ {LogicValue::UNKNOWN, LogicValue::ONE, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::UNKNOWN, LogicValue::ONE, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue nand_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ONE, LogicValue::ONE,    LogicValue::ONE,    LogicValue::ONE},
    /* ONE  */ {LogicValue::ONE, LogicValue::ZERO,     LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* UNKN */ {LogicValue::ONE, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::ONE, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue nor_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ONE, LogicValue::ZERO,    LogicValue::UNKNOWN,    LogicValue::UNKNOWN},
    /* ONE  */ {LogicValue::ZERO, LogicValue::ZERO,     LogicValue::ZERO,LogicValue::ZERO},
    /* UNKN */ {LogicValue::UNKNOWN, LogicValue::ZERO, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::UNKNOWN, LogicValue::ZERO, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue xor_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ZERO, LogicValue::ONE,    LogicValue::UNKNOWN,    LogicValue::UNKNOWN},
    /* ONE  */ {LogicValue::ONE, LogicValue::ZERO,     LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* UNKN */ {LogicValue::UNKNOWN, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::UNKNOWN, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue xnor_lut[4][4] = {
    /*          ZERO               ONE                UNKNOWN      Z */
    /* ZERO */ {LogicValue::ONE, LogicValue::ZERO,    LogicValue::UNKNOWN,    LogicValue::UNKNOWN},
    /* ONE  */ {LogicValue::ZERO, LogicValue::ONE,     LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* UNKN */ {LogicValue::UNKNOWN, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
    /* Z */    {LogicValue::UNKNOWN, LogicValue::UNKNOWN, LogicValue::UNKNOWN,LogicValue::UNKNOWN},
                
};
static constexpr LogicValue buf_lut[4] = {
    /* ZERO */ LogicValue::ZERO,
    /* ONE  */ LogicValue::ONE,
    /* UNKN */ LogicValue::UNKNOWN,
    /*Z*/ LogicValue::UNKNOWN 
};

static constexpr LogicValue not_lut[4] = {
    /* ZERO */ LogicValue::ONE,
    /* ONE  */ LogicValue::ZERO,
    /* UNKN */ LogicValue::UNKNOWN,
    /*Z*/ LogicValue::UNKNOWN 
};



// 輔助函式，將 LogicValue 轉換為索引
inline size_t to_idx(LogicValue v) {
    return static_cast<size_t>(v);
}


void AndGate::evaluate() const {
    if (inputs.size() != 2) {
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = and_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}
void OrGate::evaluate() const {
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) {
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = or_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}

void NandGate::evaluate() const {
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) {
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = nand_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}

void NorGate::evaluate() const {
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) {
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = nor_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}

void XorGate::evaluate() const {
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) {
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = xor_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}


void XnorGate::evaluate() const { 
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) { 
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = xnor_lut[to_idx(inputs[0]->value)][to_idx(inputs[1]->value)];
}

void NotGate::evaluate() const {
    if (inputs.size() != 1 || !inputs[0]) { 
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = not_lut[to_idx(inputs[0]->value)];
}

void BufGate::evaluate() const {
    if (!output) return;
    if (inputs.size() != 1 || !inputs[0]) { 
        output->value = LogicValue::UNKNOWN;
        return;
    }
    output->value = buf_lut[to_idx(inputs[0]->value)];
}