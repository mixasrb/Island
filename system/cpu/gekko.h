#pragma once
#include <cstdint>
#include <sstream>

class flipper_t;

class gekko_t {
public:
	gekko_t();
	flipper_t* p_soc;

	void clock();

private:

	uint32_t CIA;
	uint32_t NIA;

	uint32_t CR;

	uint8_t clocks;


	uint32_t GPR[32];
	uint32_t spr[1023];
	uint64_t FPR[32];

	uint32_t MSR;

	void decodeExecute(const uint32_t& opcode);

	void mulli(const uint8_t& D, const uint8_t& A, const int16_t& SIMM);//debug text is missing
	void subfic(const uint8_t& D, const uint8_t& A, const int16_t& SIMM);//debug text is missing
	void cmpli(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint16_t& UIMM);
	void cmpi(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint16_t& SIMM);
	void addicp(const uint8_t& d, const uint8_t& a, const int16_t& simm);
	void addi(const uint8_t& d, const uint8_t& a, const int16_t& simm);
	void addis(const uint8_t& d, const uint8_t& a, const int16_t& simm);
	void bcx(const uint8_t& BO, const uint8_t& BI, const int16_t& BD, const uint8_t& AA, const uint8_t& LK);
	void bx(const int32_t& li, const uint8_t& aa, const uint8_t& lk);
	void bclrx(const uint8_t& bo, const uint8_t& bi, const uint8_t& lk);
	void crxor(const uint8_t& crbD, const uint8_t& crbA, const uint8_t& crbB);
	void bcctrx(const uint8_t& BO, const uint8_t& BI, const uint8_t& LK);//debug text is missing
	void rlwinmx(const uint8_t& s, const uint8_t& a, const uint8_t& sh, const uint8_t& mb, const uint8_t& me, const uint8_t& rc);
	void ori(const uint8_t& s, const uint8_t& a, const uint16_t& uimm);
	void oris(const uint8_t& s, const uint8_t& a, const uint16_t& uimm);
	void andip(const uint8_t& S, const uint8_t& A, const uint16_t& UIMM);//debug text is missing
	void cmp(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint8_t& B);//debug text is missing
	void lwzx(const uint8_t& D, const uint8_t& A, const uint8_t& B);
	void slwx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);//debug text is missing
	void cntlzwx(const uint8_t& S, const uint8_t& A, const uint8_t& Rc);
	void andx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);//debug text is missing
	void cmpl(const uint8_t& crfD, const uint8_t& L, const uint8_t& A, const uint8_t& B);//debug text is missing
	void subfx(const uint8_t& D, const uint8_t& A, const uint8_t& B, const uint8_t& OE, const uint8_t& Rc);
	void andcx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);//debug text is missing
	void mfmsr(const uint8_t& D);
	void norx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);
	void mtmsr(const uint8_t& S);
	void stwx(const uint8_t& S, const uint8_t& A, const uint8_t& B);//debug text is missing
	void addx(const uint8_t& D, const uint8_t& A, const uint8_t& B, const uint8_t& OE, const uint8_t& Rc);
	void mfspr(const uint8_t& d, const uint16_t& pspr);
	void orx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);
	void mtspr(const uint8_t& d, const uint16_t& pspr);
	void extshx(const uint8_t& S, const uint8_t& A, const uint8_t& Rc);//debug text is missing
	void lwz(const uint8_t& d, const uint8_t& a, const int16_t& dimm);
	void lwzu(const uint8_t& D, const uint8_t& A, const int16_t& d);//debug text is missing
	void lbz(const uint8_t& D, const uint8_t& A, const int16_t& d);//debug text is missing
	void lbzu(const uint8_t& D, const uint8_t& A, const int16_t& d);//debug text is missing
	void stw(const uint8_t& s, const uint8_t& a, const int16_t& dimm);
	void stwu(const uint8_t& s, const uint8_t& a, const int16_t& dimm);
	void stbu(const uint8_t& S, const uint8_t& A, const int16_t& d);//debug text is missing
	void sth(const uint8_t& S, const uint8_t& A, const int16_t& d);//debug text is missing
	void lmw(const uint8_t& D, const uint8_t& A, const int16_t& d);//debug text is missing
	void stmw(const uint8_t& S, const uint8_t& A, const int16_t& d);//debug text is missing
	void lfd(const uint8_t& D, const uint8_t& A, const int16_t& d);
	void fmrx(const uint8_t& D, const uint8_t& B, const uint8_t& Rc);

	void stb(const uint8_t& S, const uint8_t& A, const int16_t& d);//debug text is missing
	void xorx(const uint8_t& S, const uint8_t& A, const uint8_t& B, const uint8_t& Rc);//debug text is missing
	void negx(const uint8_t& D, const uint8_t& A, const uint8_t& OE, const uint8_t& Rc);//debug text is missing

	void dcbi(const uint8_t& A, const uint8_t& B);//debug text is missing
	void dcbf(const uint8_t& A, const uint8_t& B);//debug text is missing
	void icbi(const uint8_t& A, const uint8_t& B);//debug text is missing

	//debug
	std::string logOpcode(const uint32_t& opcode);
};