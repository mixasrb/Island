#include "gekko.h"
#include "..\soc\flipper.h"

extern bool g_pause;

gekko_t::gekko_t() {

	clocks = 0;

	//value at reset??
	MSR = 0x00000040;
	CIA = 0xfff00100;

}

void gekko_t::clock() {
	if (clocks == 0) {

		clocks = 1;

		//fetch, decode, execute
		NIA = CIA + 0x4;

		/*if (CIA == 0xfff035b0)
			NIA = CIA + 0x4;*/

			//if (CIA == 0xfff366ac) // hack needs fixing!!!
			//	CIA += 4;
			//if (CIA == 0xfff37ec4) // hack needs fixing!!!
			//	CIA += 4;
			//std::cout << "kur";
		decodeExecute(p_soc->read(CIA, 4));
		CIA = NIA;

	}
	clocks--;
}

void gekko_t::decodeExecute(const uint32_t& opcode) {
	std::cout << logOpcode(opcode);

	switch (opcode >> 26) {
	case 0b000111:
		mulli((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001000:
		subfic((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001010:
		cmpli((opcode >> 23) & 0x7, (opcode >> 21) & 0x1, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001011:
		cmpi((opcode >> 23) & 0x7, (opcode >> 21) & 0x1, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001100:
		addic((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001101:
		addicp((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001110:
		addi((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b001111:
		addis((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b010000:
		bcx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode & 0xffff) >> 2, (opcode >> 1) & 0x1, opcode & 0x1);
		break;
	case 0b010010:
		bx((opcode ^ 0x48000000) >> 2, opcode & 0x2, opcode & 0x1);
		break;
	case 0b010011:
		switch ((opcode >> 1) & 0x3ff) {
		case 0b0000010000:
			//if ((opcode >> 21) & 0x14)
			bclrx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0x1);
			/*else {
				g_pause = true;
				std::cout << "Fatal Error: Unhandled variant of bclrx opcode 0x" << std::hex << opcode << "\n";
			}*/
			break;
		case 0b0010010110:
			if (opcode != 0x4c00012c) { //isync
				g_pause = true;
				std::cout << "Fatal Error: Invalid form of isync opcode 0x" << std::hex << opcode << "\n";
			}
			break;
		case 0b0011000001:
			crxor((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b1000010000:
			bcctrx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0x1);
			break;
		default:
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled decode of opcode 0x" << std::hex << opcode << "\n";
			break;
		}
		break;
	case 0b010101:
		rlwinmx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, (opcode >> 6) & 0x1f, (opcode >> 1) & 0x1f, opcode & 0x1);
		break;
	case 0b011000:
		ori((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b011001:
		oris((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b011100:
		andip((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b011111:
		switch ((opcode >> 1) & 0x3ff) {
		case 0b0000000000:
			cmp((opcode >> 23) & 0x7, (opcode >> 21) & 0x1, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b0000010111:
			lwzx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b0000011000:
			slwx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0000011010:
			cntlzwx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0x1);
			break;
		case 0b0000011100:
			andx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0000100000:
			cmpl((opcode >> 23) & 0x7, (opcode >> 21) & 0x1, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b0000101000:
		case 0b1000101000:
			subfx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, (opcode >> 10) & 0x1, opcode & 0x1);
			break;
		case 0b0000110110:
			dcbst((opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b0000111100:
			andcx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0001010011:
			mfmsr((opcode >> 21) & 0x1f);
			break;
		case 0b0001010110:
			std::cout << "Warning: Unhandled dcbf opcode 0x" << std::hex << opcode << "\n";
			break;
		case 0b1001101000:
		case 0b0001101000:
			negx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0001111100:
			norx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0010010010:
			mtmsr((opcode >> 21) & 0x1f);
			break;
		case 0b0010010111:
			stwx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 0b0100001010:
		case 0b1100001010:
			addx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, (opcode >> 10) & 0x1, opcode & 0x1);
			break;
		case 0b0100111100:
			xorx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0101010011:
			mfspr((opcode >> 21) & 0x1f, (opcode >> 11) & 0x3ff);
			break;
		case 0b0110111100:
			orx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		case 0b0111010011:
			mtspr((opcode >> 21) & 0x1f, (opcode >> 11) & 0x3ff);
			break;
		case 0b0111010110:
			std::cout << "Warning: Unhandled dcbi opcode 0x" << std::hex << opcode << "\n";
			break;
		case 0b1001010110:
			if (opcode != 0x7c0004ac) { //sync
				g_pause = true;
				std::cout << "Fatal Error: Invalid form of sync opcode 0x" << std::hex << opcode << "\n";
			}
			break;
		case 0b1110011010:
			extshx((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0x1);
			break;
		case 0b1111010110:
			std::cout << "Warning: Unhandled icbi opcode 0x" << std::hex << opcode << "\n";
			break;
		default:
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled decode of opcode 0x" << std::hex << opcode << "\n";
			break;
		}
		break;
	case 0b100000:
		lwz((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100001:
		lwzu((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100010:
		lbz((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100011:
		lbzu((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100100:
		stw((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100101:
		stwu((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100110:
		stb((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b100111:
		stbu((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b101100:
		sth((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b101110:
		lmw((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b101111:
		stmw((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b110010:
		lfd((opcode >> 21) & 0x1f, (opcode >> 16) & 0x1f, opcode & 0xffff);
		break;
	case 0b111111:
		switch ((opcode >> 1) & 0x3ff) {
		case 0b0001001000:
			fmrx((opcode >> 21) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x1);
			break;
		default:
			//g_pause = true;
			std::cout << "Fatal Error: Unhandled decode of opcode 0x" << std::hex << opcode << "\n";
			break;
		}
		break;
	default:
		//g_pause = true;
		std::cout << "Fatal Error: Unhandled decode of opcode 0x" << std::hex << opcode << "\n";
		break;
	}
}

#define XER spr[1]
#define LR spr[8]
#define CTR spr[9]

void gekko_t::mulli(const uint8_t& D, const uint8_t& A, const int16_t& SIMM) { //
	GPR[D] = GPR[A] * SIMM;
}

void gekko_t::subfic(const uint8_t& D, const uint8_t& A, const int16_t& SIMM) { //
	GPR[D] = ~GPR[A] + int32_t(SIMM) + 1u;
}

void gekko_t::cmpli(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint16_t& UIMM) {
	if (L == 1) { //??
		g_pause = true;
		std::cout << "Fatal Error: invalid form of cmpli instruction \n";
		return;
	}

	const uint32_t a = GPR[A];
	uint32_t c = 0;
	if (a < uint32_t(UIMM))
		c = 0b100;
	else if (a > uint32_t(UIMM))
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u >> (4 * crfD);
	const uint32_t y = ~0u << (31 - (4 * crfD + 3));
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << ((7 - crfD) * 4));
}

void gekko_t::cmpi(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint16_t& SIMM) {
	if (L == 1) { //??
		g_pause = true;
		std::cout << "Fatal Error: invalid form of cmpi instruction \n";
		return;
	}

	const int32_t a = GPR[A];
	uint32_t c = 0;
	if (a < int32_t(SIMM))
		c = 0b100;
	else if (a > int32_t(SIMM))
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u >> (4 * crfD);
	const uint32_t y = ~0u << (31 - (4 * crfD + 3));
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << ((7 - crfD) * 4));
}

void gekko_t::addicp(const uint8_t& D, const uint8_t& A, const int16_t& SIMM) {
	GPR[D] = GPR[A] + int32_t(SIMM);

	//not handled XER

	const int32_t a = GPR[D];
	uint32_t c = 0;
	if (a < 0)
		c = 0b100;
	else if (a > 0)
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u;
	const uint32_t y = ~0u << 28;
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << 28);
}

void gekko_t::addi(const uint8_t& d, const uint8_t& a, const int16_t& simm) { //
	if (a)
		GPR[d] = GPR[a] + int32_t(simm);
	else
		GPR[d] = int32_t(simm);
}

void gekko_t::addis(const uint8_t& d, const uint8_t& a, const int16_t& simm) { //
	if (a)
		GPR[d] = GPR[a] + (uint32_t(simm) << 16);
	else
		GPR[d] = uint32_t(simm) << 16;
}

void gekko_t::bcx(const uint8_t& BO, const uint8_t& BI, const int16_t& BD, const uint8_t& AA, const uint8_t& LK) { //
	//??
	if ((BO & 0x4) == 0)
		CTR--;

	bool ctr_ok = (BO & 0x4) || ((CTR != 0) != (((BO >> 2) & 0x1) == 0x1));
	bool cond_ok = (BO & 0x10) || (((CR >> (31 - BI)) & 0x1) == ((BO >> 4) & 0x1));

	if (ctr_ok && cond_ok) {
		if (AA)
			NIA = int32_t(int16_t(BD << 2));
		else
			NIA = CIA + int32_t(int16_t(BD << 2));
		if (LK)
			LR = CIA + 0x4;
	}
}

void gekko_t::bx(const int32_t& li, const uint8_t& aa, const uint8_t& lk) { //
	if (aa)
		NIA = (li << 8) >> 6;
	else
		NIA = CIA + ((li << 8) >> 6);
	if (lk)
		LR = CIA + 0x4;
}

void gekko_t::bclrx(const uint8_t& BO, const uint8_t& BI, const uint8_t& lk) { //
	//??
	if ((BO & 0x4) == 0)
		CTR--;

	bool ctr_ok = (BO & 0x4) || ((CTR != 0) != (((BO >> 2) & 0x1) == 0x1));
	bool cond_ok = (BO & 0x10) || (((CR >> (31 - BI)) & 0x1) == ((BO >> 4) & 0x1));

	if (ctr_ok && cond_ok) {
		NIA = LR & ~0x3;
		if (lk)
			LR = CIA + 0x4;
	}
}

void gekko_t::crxor(const uint8_t& crbD, const uint8_t& crbA, const uint8_t& crbB) { //?
	/*CR &= 0xffffffff ^ (1 << (31 - crbD));
	CR |= (((CR >> (31 - crbA)) ^ (CR >> (31 - crbB))) & 0x1) << (31 - crbD);*/

	const uint32_t bitA = CR >> (31 - crbA);
	const uint32_t bitB = CR >> (31 - crbB);
	CR = (CR & (0xffffffff ^ (1 << (31 - crbD)))) | ((bitA ^ bitB) << (31 - crbD));
}

void gekko_t::bcctrx(const uint8_t& BO, const uint8_t& BI, const uint8_t& LK) {

}

void gekko_t::rlwinmx(const uint8_t& s, const uint8_t& A, const uint8_t& sh, const uint8_t& mb, const uint8_t& me, const uint8_t& rc) {
	const uint32_t rTemp = (GPR[s] << sh) | (GPR[s] >> (32 - sh));

	const uint32_t xTemp = ~0u >> mb;
	const uint32_t yTemp = ~0u << (31 - me);
	const uint32_t mTemp = (mb <= me) ? (xTemp & yTemp) : (xTemp | yTemp);

	GPR[A] = rTemp & mTemp;

	if (rc) {
		const int32_t a = GPR[A];
		uint32_t c = 0;
		if (a < 0)
			c = 0b100;
		else if (a > 0)
			c = 0b010;
		else
			c = 0b001;

		const uint32_t x = ~0u;
		const uint32_t y = ~0u << 28;
		const uint32_t m = ~(x & y);

		c = (c << 1) | (XER >> 31);

		CR = (CR & m) | (c << 28);
	}
}

void gekko_t::ori(const uint8_t& s, const uint8_t& a, const uint16_t& uimm) { //
	GPR[a] = GPR[s] | uint32_t(uimm);
}

void gekko_t::oris(const uint8_t& s, const uint8_t& a, const uint16_t& uimm) {
	GPR[a] = GPR[s] | (uint32_t(uimm) << 16);
}

void gekko_t::andip(const uint8_t& S, const uint8_t& A, const uint16_t& UIMM) { //
	GPR[A] = GPR[S] & uint32_t(UIMM);

	const int32_t a = GPR[A];
	uint32_t c = 0;
	if (a < 0)
		c = 0b100;
	else if (a > 0)
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u;
	const uint32_t y = ~0u << 28;
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << 28);
}

void gekko_t::cmp(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint8_t& B) { //
	if (L == 1) { //??
		g_pause = true;
		std::cout << "Fatal Error: invalid form of cmp instruction \n";
		return;
	}

	const int32_t a = GPR[A];
	const int32_t b = GPR[B];
	uint32_t c = 0;
	if (a < b)
		c = 0b100;
	else if (a > b)
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u >> (4 * crfD);
	const uint32_t y = ~0u << (31 - (4 * crfD + 3));
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << ((7 - crfD) * 4));
}

void gekko_t::lwzx(const uint8_t& D, const uint8_t& A, const uint8_t& B) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	uint32_t EA = b + GPR[B];
	GPR[D] = p_soc->read(EA, 4);
}

void gekko_t::slwx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of slwx instruction \n";
		return;
	}

	const uint8_t n = GPR[B] & 0x1f;

	const uint32_t r = (GPR[S] << n) | (GPR[S] >> (32 - n));
	uint32_t m = 0u;
	if ((GPR[B] & 0x20) == 0) {
		const uint32_t x = ~0u >> 0;
		const uint32_t y = ~0u << (31 - n);
		m = ~(x & y);
	}

	GPR[A] = r & m;
}

void gekko_t::cntlzwx(const uint8_t& S, const uint8_t& A, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of cntlzwx instruction \n";
		return;
	}

	uint8_t n = 0;
	while (n < 32) {
		if ((GPR[S] >> (31 - n)) & 0x1)
			break;
		n++;
	}
	GPR[A] = n;
}

void gekko_t::andx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of andx instruction \n";
		return;
	}

	GPR[A] = GPR[S] & GPR[B];
}

void gekko_t::cmpl(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint8_t& B) { //
	if (L == 1) { //??
		g_pause = true;
		std::cout << "Fatal Error: invalid form of cmpl instruction \n";
		return;
	}

	const uint32_t a = GPR[A];
	const uint32_t b = GPR[B];
	uint32_t c = 0;
	if (a < b)
		c = 0b100;
	else if (a > b)
		c = 0b010;
	else
		c = 0b001;

	const uint32_t x = ~0u >> (4 * crfD);
	const uint32_t y = ~0u << (31 - (4 * crfD + 3));
	const uint32_t m = ~(x & y);

	c = (c << 1) | (XER >> 31);

	CR = (CR & m) | (c << ((7 - crfD) * 4));
}

void gekko_t::subfx(const uint8_t& D, const uint8_t& A, const uint8_t& B, const uint8_t& OE, const uint8_t& Rc) { //
	if (OE || Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled OE or Rc bit of subfx instruction \n";
		return;
	}

	GPR[D] = ~GPR[A] + GPR[B] + 0x1u;
}

void gekko_t::andcx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of andcx instruction \n";
		return;
	}

	GPR[A] = GPR[S] & ~GPR[B];
}

void gekko_t::mfmsr(const uint8_t& D) { //
	GPR[D] = MSR;
}

void gekko_t::norx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of norx instruction \n";
		return;
	}

	GPR[A] = ~(GPR[S] | GPR[B]);
}

void gekko_t::mtmsr(const uint8_t& S) { //
	MSR = GPR[S];
}

void gekko_t::stwx(const uint8_t& S, const uint8_t& A, const uint8_t& B) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	const uint32_t EA = b + GPR[B];
	p_soc->write(EA, 4, GPR[S]);
}

void gekko_t::addx(const uint8_t& D, const uint8_t& A, const uint8_t& B, const uint8_t& OE, const uint8_t& Rc) { //
	if (OE || Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled OE or Rc bit of addx instruction \n";
		return;
	}

	GPR[D] = GPR[A] + GPR[B];
}

void gekko_t::mfspr(const uint8_t& d, const uint16_t& pspr) { //
	const uint8_t sprLo = pspr & 0x1f;
	const uint8_t sprHi = (pspr >> 5) & 0x1f;
	const uint16_t n = (sprLo << 5) | sprHi;
	GPR[d] = spr[n];
}

void gekko_t::orx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of orx instruction \n";
		return;
	}

	GPR[A] = GPR[S] | GPR[B];
}

void gekko_t::mtspr(const uint8_t& s, const uint16_t& pspr) { //
	const uint8_t sprLo = pspr & 0x1f;
	const uint8_t sprHi = (pspr >> 5) & 0x1f;
	const uint16_t n = (sprLo << 5) | sprHi;
	spr[n] = GPR[s];
}

void gekko_t::extshx(const uint8_t& S, const uint8_t& A, const uint8_t& Rc) { //??
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of extshx instruction \n";
		return;
	}

	GPR[A] = int32_t(int16_t(GPR[S]));
}

void gekko_t::lwz(const uint8_t& D, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	const uint32_t EA = b + (int32_t)d;
	GPR[D] = p_soc->read(EA, 4);
}

void gekko_t::lwzu(const uint8_t& D, const uint8_t& A, const int16_t& d) { //
	if ((A == 0) || (A == D)) {
		g_pause = true;
		std::cout << "Fatal Error: invalid form of lwzu instruction \n";
		return;
	}

	const uint32_t EA = GPR[A] + (int32_t)d;
	GPR[D] = p_soc->read(EA, 4);
	GPR[A] = EA;
}

void gekko_t::lbz(const uint8_t& D, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	const uint32_t EA = b + (int32_t)d;
	GPR[D] = p_soc->read(EA, 1);
}

void gekko_t::lbzu(const uint8_t& D, const uint8_t& A, const int16_t& d) { //
	if ((A == 0) || (A == D)) {
		g_pause = true;
		std::cout << "Fatal Error: invalid form of lbzu instruction \n";
		return;
	}

	const uint32_t EA = GPR[A] + (int32_t)d;
	GPR[D] = p_soc->read(EA, 1);
	GPR[A] = EA;
}

void gekko_t::stw(const uint8_t& S, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	uint32_t ea = b + (int32_t)d;
	p_soc->write(ea, 4, GPR[S]);
}

void gekko_t::stwu(const uint8_t& s, const uint8_t& a, const int16_t& dimm) { //
	if (a == 0) {
		g_pause = true;
		std::cout << "Fatal Error: invalid form of stwu instruction \n";
		return;
	}

	uint32_t ea = GPR[a] + (int32_t)dimm;
	p_soc->write(ea, 4, GPR[s]);
	GPR[a] = ea;
}

void gekko_t::stbu(const uint8_t& S, const uint8_t& A, const int16_t& d) { //
	if (A == 0) {
		g_pause = true;
		std::cout << "Fatal Error: invalid form of lbzu instruction \n";
		return;
	}

	const uint32_t EA = GPR[A] + int32_t(d);
	p_soc->write(EA, 1, (uint8_t)GPR[S]);
	GPR[A] = EA;
}

void gekko_t::sth(const uint8_t& S, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	uint32_t EA = b + (int32_t)d;
	p_soc->write(EA, 2, GPR[S]);
}

void gekko_t::lmw(const uint8_t& D, const uint8_t& A, const int16_t& d) { //
	if (A == 0) {
		g_pause = true;
		std::cout << "Fatal Error: invalid form of lmw instruction \n";
		return;
	}

	uint32_t b = 0;
	if (A)
		b = GPR[A];
	uint32_t EA = b + int32_t(d);

	uint8_t r = D;
	while (r < 32) {
		GPR[r] = p_soc->read(EA, 4);
		r++;
		EA += 4;
	}
}

void gekko_t::stmw(const uint8_t& S, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];
	uint32_t EA = b + int32_t(d);
	uint8_t r = S;

	while (r < 32) {
		p_soc->write(EA, 4, GPR[r]);
		r++;
		EA += 4;
	}
}

void gekko_t::lfd(const uint8_t& D, const uint8_t& A, const int16_t& d) {
	uint32_t b = 0;
	if (A)
		b = GPR[A];

	const uint32_t EA = b + (int32_t)d;

	FPR[D] = p_soc->read(EA, 8);
}

void gekko_t::fmrx(const uint8_t& D, const uint8_t& B, const uint8_t& Rc) {
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of fmrx instruction \n";
		return;
	}

	FPR[D] = FPR[B];
}

void gekko_t::stb(const uint8_t& S, const uint8_t& A, const int16_t& d) { //
	uint32_t b = 0;
	if (A)
		b = GPR[A];

	const uint32_t EA = b + (int32_t)d;
	p_soc->write(EA, 1, GPR[S]);
	//GPR[A] = EA;//!!!
}

void gekko_t::xorx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc) { //
	if (Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled RC bit of xorx instruction \n";
		return;
	}

	GPR[A] = GPR[S] ^ GPR[B];
}

void gekko_t::negx(const uint8_t& D, const uint8_t& A, const uint8_t& OE, const uint8_t& Rc) { //
	if (OE || Rc) {
		g_pause = true;
		std::cout << "Fatal Error: Unhandled OE or Rc bit of negx instruction \n";
		return;
	}

	GPR[D] = ~GPR[A] + 1;
}

void gekko_t::dcbst(const uint8_t& A, const uint8_t& B) {
	uint32_t b = 0;
	if (A)
		b = GPR[A];

	const uint32_t EA = b + GPR[B];
}

void gekko_t::addic(const uint8_t& D, const uint8_t& A, const int16_t& SIMM) {
	//XER and carry unhandled

	GPR[D] = GPR[A] + int32_t(SIMM);
}

std::string gekko_t::logOpcode(const uint32_t& opcode) {
	std::stringstream ss;

	ss << "0x" << std::hex << CIA << " : ";

	switch (opcode >> 26) {
	case 0b001010:
		ss << "cmpli  crf" << std::dec << ((opcode >> 23) & 0x7)
			<< "," << ((opcode >> 21) & 0x1)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b001011:
		ss << "cmpi  crf" << std::dec << ((opcode >> 23) & 0x7)
			<< "," << ((opcode >> 21) & 0x1)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b001101:
		ss << "addic.  r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b001110:
		ss << "addi  r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b001111:
		if ((opcode >> 16) & 0x1f)
			if (opcode & 0x8000)
				ss << "subis r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",r" << ((opcode >> 16) & 0x1f)
				<< ",-0x" << std::hex << uint16_t(0 - (opcode & 0xffff));
			else
				ss << "addis r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",r" << ((opcode >> 16) & 0x1f)
				<< ",0x" << std::hex << (opcode & 0xffff);
		else
			/*if (opcode & 0x8000)
				ss << "lis   r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",-0x" << std::hex << uint16_t(0 - (opcode & 0xffff));
			else*/
			ss << "lis   r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b010000:
		switch (opcode & 0x3) {
		case 0:
			ss << "bc    " << std::dec << ((opcode >> 21) & 0x1f) << ","
				<< ((opcode >> 16) & 0x1f) << ","
				<< std::hex << (CIA + (int32_t(((opcode & 0xffff) >> 2) << 18) >> 16));
			break;
		case 1:
			ss << "bcl   " << std::dec << ((opcode >> 21) & 0x1f) << ","
				<< ((opcode >> 16) & 0x1f) << ","
				<< std::hex << (CIA + (int32_t(((opcode & 0xffff) >> 2) << 18) >> 16));
			break;
		case 2:
			ss << "bca   " << std::dec << ((opcode >> 21) & 0x1f) << ","
				<< ((opcode >> 16) & 0x1f) << ","
				<< std::hex << (int32_t(((opcode & 0xffff) >> 2) << 18) >> 16);
			break;
		case 3:
			ss << "bcla  " << std::dec << ((opcode >> 21) & 0x1f) << ","
				<< ((opcode >> 16) & 0x1f) << ","
				<< std::hex << (int32_t(((opcode & 0xffff) >> 2) << 18) >> 16);
			break;
		}
		break;
	case 0b010010:
		switch (opcode & 0x3) {
		case 0:
			ss << "b     0x" << std::hex << (CIA + (int32_t((opcode ^ 0x48000000) >> 2 << 10) >> 8));
			break;
		case 1:
			ss << "bl    0x" << std::hex << (CIA + (int32_t((opcode ^ 0x48000000) >> 2 << 10) >> 8));
			break;
		case 2:
			ss << "ba    0x" << std::hex << (int32_t((opcode ^ 0x48000000) >> 2 << 10) >> 8);
			break;
		case 3:
			ss << "bla   0x" << std::hex << (int32_t((opcode ^ 0x48000000) >> 2 << 10) >> 8);
			break;
		}
		break;
	case 0b010011:
		switch ((opcode & 0x7ff) >> 1) {
		case 0b0000010000:
			if ((opcode >> 21) & 0x14)
				ss << "blr";
			break;
		case 0b0010010110:
			if (opcode == 0x4c00012c) {
				ss << "isync  ";
			}
			break;
		case 0b0011000001:
			ss << "crxor  crb" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",crb" << ((opcode >> 16) & 0x1f)
				<< ",crb" << ((opcode >> 11) & 0x1f);
			break;
		}
		break;
	case 0b010101:
		ss << "rlwinm  r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << ((opcode >> 11) & 0x1f)
			<< ",0x" << ((opcode >> 6) & 0x1f)
			<< ",0x" << ((opcode >> 1) & 0x1f);
		break;
	case 0b011000:
		if (opcode ^ 0x60000000)
			ss << "ori   r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		else
			ss << "nop";
		break;
	case 0b011001:
		ss << "oris  r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b011111:
		switch ((opcode >> 1) & 0x3ff) {
		case 0b0001111100:
			ss << "norx  r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",r" << ((opcode >> 16) & 0x1f)
				<< ",r" << ((opcode >> 11) & 0x1f)
				<< ",Rc" << (opcode & 0x1);
			break;
		case 0b0100001010:
		case 0b1100001010:
			ss << "addx  r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",r" << ((opcode >> 16) & 0x1f)
				<< ",r" << ((opcode >> 11) & 0x1f);
			break;
		case 0b0101010011: {
			const uint16_t pspr = (opcode >> 11) & 0x3ff;
			const uint8_t sprLo = pspr & 0x1f;
			const uint8_t sprHi = (pspr >> 5) & 0x1f;
			const uint16_t n = (sprLo << 5) | sprHi;
			ss << "mfspr r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",spr" << n;
			break;
		}
		case 0b0110111100:
			ss << "orx   r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",r" << ((opcode >> 16) & 0x1f)
				<< ",r" << ((opcode >> 11) & 0x1f);
			break;
		case 0b0111010011: {
			const uint16_t pspr = (opcode >> 11) & 0x3ff;
			const uint8_t sprLo = pspr & 0x1f;
			const uint8_t sprHi = (pspr >> 5) & 0x1f;
			const uint16_t n = (sprLo << 5) | sprHi;
			ss << "mtspr r" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",spr" << n;
			break;
		}
		case 0b1001010110:
			if (opcode == 0x7c0004ac) {
				ss << "sync  ";
			}
			break;
		}
		break;
	case 0b100100:
		ss << "stw   r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",d 0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b100000:
		ss << "lwz   r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",d 0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b100101:
		ss << "stwu  r" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",d 0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b110010:
		ss << "lfd   fr" << std::dec << ((opcode >> 21) & 0x1f)
			<< ",r" << ((opcode >> 16) & 0x1f)
			<< ",d 0x" << std::hex << (opcode & 0xffff);
		break;
	case 0b111111:
		switch ((opcode >> 1) & 0x3ff) {
		case 0b0001001000:
			ss << "fmrx   fr" << std::dec << ((opcode >> 21) & 0x1f)
				<< ",fr" << ((opcode >> 11) & 0x1f)
				<< ",Rc " << (opcode & 0x1);
			break;
		}
		break;
	}

	ss << "\n";
	return ss.str();
}
