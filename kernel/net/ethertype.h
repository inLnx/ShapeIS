/**
 * @file ethertype.h
 * @author Krisna Pranav aka (krishpranav)
 * @brief ether type
 * @version 1.0
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include <mods/types.h>

struct EtherType
{
    enum ETHER : u16
    {   
        ARP=0x0806,
        ipv4 = 0x0800,
        ipv6 = 0x86DD,
    };
}