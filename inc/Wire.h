#ifndef WIRE_H
#define WIRE_H

#include <string>
#include <iostream>

using namespace std;

// 使用 enum class 來定義三態邏輯值 (0, 1, X)。
// 它比傳統 enum 更具型別安全性，且可以指定底層型別 (uint8_t) 以節省空間。
enum class LogicValue {
    ZERO,
    ONE,
    UNKNOWN,
    BIG_Z
};

class Wire {
public:
    string name;   // 線路的名稱，例如 "a", "w1", "out"
    LogicValue value;   // 線路當前的邏輯值

public:

    Wire() : name(""), value(LogicValue::UNKNOWN) {}

    explicit Wire(const string& n) : name(n), value(LogicValue::UNKNOWN) {}

};

#endif // WIRE_H