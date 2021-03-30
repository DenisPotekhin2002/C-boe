#include "requests.h"

#include <algorithm>
#include <map>

#define inc(key) str += Order::key;

#define parseHex(field)                         \
    exec_details.field = 0;                     \
    for (size_t i = 0; i < Order::field; i++) { \
        exec_details.field *= 256;              \
        exec_details.field += castInt(str);     \
        str++;                                  \
    }

int castInt(const char * str)
{
    return (static_cast<int>(*str) + 256) % 256;
}

struct Order
{
    static constexpr size_t message = 9;       //Message type and Timestamp
    static constexpr size_t cl_ord_id = 14;    //Order Token
    static constexpr size_t filled_volume = 4; //Executed Quantity
    static constexpr size_t price = 4;         //Execution Price
    static constexpr size_t flag = 1;          //Liquidity Flag
    static constexpr size_t match_number = 4;  //Match Number
    static constexpr size_t counterpart = 4;   //Contra Firm
    static constexpr size_t mode = 1;          //Trading Mode
    static constexpr size_t category = 1;      //Transaction Category
    static constexpr size_t type = 1;          //Transaction Type
    static constexpr size_t mask = 1;          //Liquidity Attributes
};

ExecutionDetails decode_executed_order(const std::vector<unsigned char> & message)
{
    ExecutionDetails exec_details;
    std::map<std::string, int> len;
    const char * str = reinterpret_cast<const char *>(&message[Order::message]);
    const char * space = std::find(str, str + Order::cl_ord_id, ' ');
    exec_details.cl_ord_id.assign(str, space - str);
    inc(cl_ord_id);
    parseHex(filled_volume);
    parseHex(price);
    exec_details.price /= 10000;
    inc(flag);
    parseHex(match_number);
    exec_details.counterpart.assign(str, Order::counterpart);
    inc(counterpart);
    inc(mode);
    inc(category);
    inc(type);
    int mask = castInt(str);
    mask /= 8;
    switch (mask % 4) {
    case 1:
        exec_details.liquidity_indicator = LiquidityIndicator::Removed;
        break;
    case 0:
        exec_details.liquidity_indicator = LiquidityIndicator::Added;
        break;
    default:
        exec_details.liquidity_indicator = LiquidityIndicator::None;
    }
    mask /= 4;
    mask % 2 == 0 ? exec_details.internalized = false : exec_details.internalized = true;
    mask /= 4;
    mask % 2 == 0 ? exec_details.self_trade = false : exec_details.self_trade = true;
    inc(mask);
    return exec_details;
}
