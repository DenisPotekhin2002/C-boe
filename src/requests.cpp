#include "requests.h"

#include <algorithm>
#include <map>

#define inc(key) str += len[#key];

#define parseHex(field) \
        exec_details.field = 0; \
        for (int i = 0; i < len[#field]; i++) { \
            exec_details.field *= 256; \
            exec_details.field += castInt(str); \
            str++; \
        }

#define parseBool(field) \
        mask /= 4; \
        mask % 2 == 0 ? exec_details.field = false : exec_details.field = true;

int castInt(const char * str)
{
    return (static_cast<int>(*str) + 256) % 256;
}

ExecutionDetails decode_executed_order(const std::vector<unsigned char> & message)
{
    ExecutionDetails exec_details;
    std::map<std::string, int> len;
    len["message"] = 9; //Message type and Timestamp
    len["cl_ord_id"] = 14; //Order Token
    len["filled_volume"] = 4; //Executed Quantity
    len["price"] = 4; //Execution Price
    len["flag"] = 1; //Liquidity Flag
    len["match_number"] = 4; //Match Number
    len["counterpart"] = 4; //Contra Firm
    len["mode"] = 1; //Trading Mode
    len["category"] = 1; //Transaction Category
    len["type"] = 1; //Transaction Type
    len["mask"] = 1; //Liquidity Attributes
    const char * str = reinterpret_cast<const char *>(&message[len["message"]]);
    const char * space = std::find(str, str + len["cl_ord_id"], ' ');
    exec_details.cl_ord_id.assign(str, space - str);
    inc(cl_ord_id)
    parseHex(filled_volume)
    parseHex(price)
    exec_details.price /= 10000;
    inc(flag)
    parseHex(match_number)
    exec_details.counterpart.assign(str, len["counterpart"]);
    inc(counterpart)
    inc(mode)
    inc(category)
    inc(type)
    int mask = castInt(str);
    mask /= 8;
    switch (mask % 4){
    case 1:
        exec_details.liquidity_indicator = LiquidityIndicator::Removed;
        break;
    case 0:
        exec_details.liquidity_indicator = LiquidityIndicator::Added;
        break;
    default:
        exec_details.liquidity_indicator = LiquidityIndicator::None;
    }
    parseBool(internalized)
    parseBool(self_trade)
    inc(mask)
    return exec_details;
}
