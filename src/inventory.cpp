#include "inventory.hpp"
#include <stdexcept>

InventoryNumber::InventoryNumber(int value) : value_(value){
    if(value <= 0){
        throw std::invalid_argument("Id must be greater than 0");
    }
}

int InventoryNumber::value() const{
    return value_;
}