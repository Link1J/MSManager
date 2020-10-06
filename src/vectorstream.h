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

#include <vector>

class VectorStream
{
	std::vector<uint8_t>& vector;
	int index = 0;

public:
	VectorStream(std::vector<uint8_t>& vector)
		: vector(vector)
	{
	}

	std::string GetValue(std::string key)
	{
		MovePast(key);
		std::string value((char*)&vector[index]);
		index += (int)value.size();
		return value;
	}

	void MovePast(std::string key)
	{
		int i;

		for (; index < vector.size(); index++)
		{
			for (i = 0; i < key.size(); i++)
			{
				if (vector[index + i] != key[i])
					break;
			}

			if (i == key.size())
				break;
		}

		index += (int)key.size() + 1;
	}

	std::string GetValueDelim(uint8_t delim)
	{
		std::vector<char> value;

		for (; index < vector.size(); index++)
		{
			if (vector[index] == delim)
				break;

			value.push_back(vector[index]);
		}

		index++;
		value.push_back('\0');
		return value.data();
	}

	std::string GetValueDelim(std::vector<uint8_t> delims)
	{
		std::vector<char> value;
		bool done = false;

		for (; index < vector.size() && !done; index++)
		{
			for (uint8_t delim : delims)
				if (vector[index] == delim)
					done = true;
			
			if (!done)
				value.push_back(vector[index]);
		}

		value.push_back('\0');
		return value.data();
	}

	void Ignore(int count)
	{
		index += count;
	}

	uint8_t Peek()
	{
		return vector[index];
	}
};