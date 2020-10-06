/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Jared Irwin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

inline std::vector<uint8_t> writeVarInt(uint32_t value) 
{
	std::vector<uint8_t> varInt;
    do {
        uint8_t temp = (uint8_t)(value & 0b01111111);
        // Note: >>> means that the sign bit is shifted with the rest of the number rather than being left alone
        value >>= 7;
        if (value != 0) {
            temp |= 0b10000000;
        }
        varInt.push_back(temp);
    } while (value != 0);
	return varInt;
}

inline uint32_t readVarInt(uint8_t* data, uint32_t& numRead) 
{
	numRead = 0;
    uint32_t result = 0;
    uint8_t read;
    do {
        read = data[numRead];
        int value = (read & 0b01111111);
        result |= (value << (7 * numRead));

        numRead++;
        if (numRead > 5) {
			return result;
        }
    } while ((read & 0b10000000) != 0);

    return result;
}