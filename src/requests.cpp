#include "requests.h"

ExecutionDetails decode_executed_order(const std::vector<unsigned char> & message)
{
    ExecutionDetails exec_details;
    const char * str = reinterpret_cast<const char *>(&message[9]);
    exec_details.cl_ord_id = "";
    int j = 9;
    while(message[j] != ' ' && j < 23){
        j++;
    }
    exec_details.cl_ord_id.assign(str, j - 9);
    exec_details.filled_volume = 0;
    for (int i = 0; i < 4; i++){
        exec_details.filled_volume *= 256;
        exec_details.filled_volume += static_cast<int>(message[23 + i]);
    }
    exec_details.price = 0;
    for (int i = 0; i < 4; i++){
        exec_details.price *= 256;
        exec_details.price += static_cast<int>(message[27 + i]);
    }
    exec_details.price /= 10000;
    exec_details.match_number = 0;
    for (int i = 0; i < 4; i++){
        exec_details.match_number *= 256;
        exec_details.match_number += static_cast<int>(message[32 + i]);
    }
    str += 27;
    exec_details.counterpart.assign(str, 4);
    int temp = static_cast<int>(message[43]);
    temp /= 8;
    if (temp % 4 == 1){
        exec_details.liquidity_indicator = LiquidityIndicator::Removed;
    } else if (temp % 4 == 0){
        exec_details.liquidity_indicator = LiquidityIndicator::Added;
    } else {
        exec_details.liquidity_indicator = LiquidityIndicator::None;
    }
    temp /= 4;
    if (temp % 2 == 0){
        exec_details.internalized = false;
    } else {
        exec_details.internalized = true;
    }
    temp /= 4;
    if (temp % 2 == 0){
        exec_details.self_trade = false;
    } else {
        exec_details.self_trade = true;
    }
    return exec_details;
}
