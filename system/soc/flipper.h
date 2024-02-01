#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>

#define RAM_SIZE 24 * 1024 * 1024 // 24mb

class flipper_t {
public:
	flipper_t();

	uint8_t* p_ipl;

	uint8_t* p_bootstrap1;

	uint8_t* p_exiCh0Buffer = nullptr;


	uint64_t read(uint32_t addr, const uint16_t& bytness);
	void write(uint32_t addr, const uint16_t& bytness, const uint64_t& data);

private:
	uint8_t ram[RAM_SIZE];

	uint32_t EXI0CSR;
	uint32_t EXI1CSR;
	uint32_t EXI2CSR;
};