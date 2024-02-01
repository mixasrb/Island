#include "flipper.h"

extern bool g_pause;

#define IPL_SIZE 1048576 //1024*1024 = 1mb

flipper_t::flipper_t() {
	for (size_t i = 0; i < RAM_SIZE; i++) {
		ram[i] = 0xdc;
	}

	std::ifstream iplFile("D:/projects/emulation_gameCube/binaries/IPL/GC_Decrypted_IPL/ngc_usa_ipl.dol", std::ios::binary | std::ios::ate);
	//std::ifstream iplFile("D:/projects/emulation_gameCube/binaries/IPL/swiss/swiss_r1562.dol", std::ios::binary | std::ios::ate);

	std::ifstream bootstrap1File("D:/projects/emulation_gameCube/binaries/Bootstrap_1/PAL/bs.bin", std::ios::binary | std::ios::ate);

	if (!iplFile.is_open()) {
		g_pause = true;
		std::cout << "Fatal Error: Failed to open IPL file\n";
	}
	else {
		size_t iplFileSize = iplFile.tellg();
		iplFile.seekg(0);

		p_ipl = new uint8_t[iplFileSize];

		iplFile.read((char*)p_ipl, iplFileSize);

		iplFile.close();
	}


	if (!bootstrap1File.is_open()) {
		g_pause = true;
		std::cout << "Fatal Error: Failed to open IPL file\n";
	}
	else {
		size_t bootstrap1FileSize = bootstrap1File.tellg();
		bootstrap1File.seekg(0);

		p_bootstrap1 = new uint8_t[bootstrap1FileSize];

		bootstrap1File.read((char*)p_bootstrap1, bootstrap1FileSize);

		bootstrap1File.close();
	}

	//first mapping bootstrap 1
	p_exiCh0Buffer = p_bootstrap1;
}

uint64_t flipper_t::read(uint32_t addr, const uint16_t& bytness) {
	uint64_t data = 0;

	switch (bytness) {
	case 1:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM read out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return 0;
			}

			data = (uint64_t)ram[addr];

			std::cout << "read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		else {
			g_pause = true;
			std::cout << "Fatal Error: Unhandled read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
		}
		break;
	case 2:
		//g_pause = true;
		std::cout << "Fatal Error: Unhandled read addr 0x" << std::hex << addr
			<< " bytness " << std::dec << bytness << "\n";
		break;
	case 4:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM read out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return 0;
			}

			data = ((uint32_t)ram[addr] << 24) | ((uint32_t)ram[addr | 0x1] << 16) |
				((uint32_t)ram[addr | 0x2] << 8) | ((uint32_t)ram[addr | 0x3]);

			std::cout << "read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		else if (addr >= 0xfff00000) {
			addr ^= 0xfff00000;

			if ((addr | 0x3) > IPL_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: IPL read out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
			}

			data = ((uint32_t)p_exiCh0Buffer[addr] << 24) | ((uint32_t)p_exiCh0Buffer[addr | 0x1] << 16) |
				((uint32_t)p_exiCh0Buffer[addr | 0x2] << 8) | ((uint32_t)p_exiCh0Buffer[addr | 0x3]);

		}
		else if (addr == 0xcc00302c) { //Unknown register
			std::cout << "Warrning: Unhandled register read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
		}
		else if (addr == 0xcc006434) { //SICOMCSR - SI Communication Control Status Register (command)
			std::cout << "Warrning: SICOMCSR - SI Communication Control Status Register unhandled register read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
		}
		else if (addr == 0xcc006800) { //EXI0CSR - EXI Channel 0 Parameter Register (Status?)
			std::cout << "Warrning: EXI0CSR - EXI Channel 0 Parameter Register (Status?) unhandled register read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
			 data = EXI0CSR;
		}
		else if (addr == 0xcc006814) { //EXI1CSR - EXI Channel 1 Parameter Register
			std::cout << "Warrning: EXI1CSR - EXI Channel 1 Parameter Register unhandled register read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
			data = EXI1CSR;
		}
		else {
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
		}
		break;
	case 8:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM write out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return 0;
			}

			data = ((uint64_t)ram[addr] << 60) | ((uint64_t)ram[addr | 0x1] << 46) |
				((uint64_t)ram[addr | 0x2] << 42) | ((uint64_t)ram[addr | 0x3] << 28) |
				((uint64_t)ram[addr | 0x4] << 24) | ((uint64_t)ram[addr | 0x5] << 16) |
				((uint64_t)ram[addr | 0x6] << 8) | ((uint64_t)ram[addr | 0x7]);

			/*std::cout << "read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";*/
		}
		/*else if (addr >= 0xfff00000) {
			addr ^= 0xfff00000;

			if ((addr | 0x3) > IPL_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: IPL read out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
			}

			data = ((uint32_t)p_ipl[addr] << 24) | ((uint32_t)p_ipl[addr | 0x1] << 16) |
				((uint32_t)p_ipl[addr | 0x2] << 8) | ((uint32_t)p_ipl[addr | 0x3]);
		}*/
		else {
			g_pause = true;
			std::cout << "Fatal Error: Unhandled read addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness << "\n";
		}
		break;
	default:
		g_pause = true;
		std::cout << "Fatal Error: Incorrect SOC read bytness " << std::dec << bytness << "\n";
		break;
	}

	return data;
}

void flipper_t::write(uint32_t addr, const uint16_t& bytness, const uint64_t& data) {
	std::cout << "write addr 0x" << std::hex << addr
		<< " bytness " << std::dec << bytness
		<< " data 0x" << std::hex << data << "\n";

	switch (bytness) {
	case 1:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM write out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return;
			}

			ram[addr] = (uint8_t)data;
		}
		else {
			g_pause = true;
			std::cout << "Fatal Error: Unhandled write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		break;
	case 2:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM write out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return;
			}


			ram[addr] = data >> 8;
			ram[++addr] = data;
		}
		else if (addr == 0xcc00401c) { //MI interrupt mask
			std::cout << "Warning: MI interrupt mask unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		else {
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		break;
	case 4:
		if (addr < 0xc8000000) {
			addr &= 0x1ffffff;

			if ((addr | 0x3) >= RAM_SIZE) {
				g_pause = true;
				std::cout << "Fatal Error: RAM write out of bounds addr 0x" << std::hex << addr
					<< " bytness " << std::dec << bytness << "\n";
				return;
			}


			ram[addr] = data >> 24;
			ram[++addr] = data >> 16;
			ram[++addr] = data >> 8;
			ram[++addr] = data;
		}
		else if (addr == 0xcc003004) { //INTMR - interrupt mask
			std::cout << "Warning: INTMR - interrupt mask unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		else if (addr == 0xcc006430) { //SIPOLL - SI Poll Register (Joy-channel Control (?) (Calibration gun ?))
			std::cout << "Warning: SIPOLL - SI Poll Register unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		else if (addr == 0xcc006800) { //EXI0CSR - EXI Channel 0 Parameter Register (Status?)
			std::cout << "Warning: EXI0CSR - EXI Channel 0 Parameter Register (Status?) unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
			EXI0CSR = data;
		}
		else if (addr == 0xcc006814) { //EXI1CSR - EXI Channel 1 Parameter Register
			std::cout << "Warning: EXI1CSR - EXI Channel 1 Parameter Register unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
			EXI1CSR = data;
		}
		else if (addr == 0xcc006828) { //EXI1CSR - EXI Channel 2 Parameter Register
			std::cout << "Warning: EXI1CSR - EXI Channel 2 Parameter Register unhandled register write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
			EXI2CSR = data;
		}
		else {
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled write addr 0x" << std::hex << addr
				<< " bytness " << std::dec << bytness
				<< " data 0x" << std::hex << data << "\n";
		}
		break;
	default:
		g_pause = true;
		std::cout << "Fatal Error: Incorrect SOC write bytness " << std::dec << bytness << "\n";
		break;
	}
}