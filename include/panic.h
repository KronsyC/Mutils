#pragma once

#include<iostream>
#include "ansi.h"
#include "funcmod.h"


namespace mutils{
INLINE inline void panic(const char* msg){
    std::cerr << mutils::ansi::FormatBuilder() 
                    .fg(mutils::ansi::Color::Red) 
                    .bold() 
                    .write("panic: ") 
                    .reset() 
                    .write(msg) 
                    .str() << std::endl; 
    exit(1); 
}};