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