

#include "crypto/randomx/configuration.h"
#include "crypto/randomx/program.hpp"
#include "crypto/randomx/blake2/endian.h"
#include "crypto/randomx/superscalar.hpp"
#include "crypto/randomx/intrin_portable.h"
#include "crypto/randomx/reciprocal.h"

namespace randomx {

	static bool isMultiplication(SuperscalarInstructionType type) {
		return type == SuperscalarInstructionType::IMUL_R || type == SuperscalarInstructionType::IMULH_R || type == SuperscalarInstructionType::ISMULH_R || type == SuperscalarInstructionType::IMUL_RCP;
	}

	
	namespace ExecutionPort {
		using type = int;
		constexpr type Null = 0;
		constexpr type P0 = 1;
		constexpr type P1 = 2;
		constexpr type P5 = 4;
		constexpr type P01 = P0 | P1;
		constexpr type P05 = P0 | P5;
		constexpr type P015 = P0 | P1 | P5;
	}

	
	
	
	class MacroOp {
	public:
		MacroOp(const char* name, int size)
			: name_(name), size_(size), latency_(0), uop1_(ExecutionPort::Null), uop2_(ExecutionPort::Null) {}
		MacroOp(const char* name, int size, int latency, ExecutionPort::type uop)
			: name_(name), size_(size), latency_(latency), uop1_(uop), uop2_(ExecutionPort::Null) {}
		MacroOp(const char* name, int size, int latency, ExecutionPort::type uop1, ExecutionPort::type uop2)
			: name_(name), size_(size), latency_(latency), uop1_(uop1), uop2_(uop2) {}
		MacroOp(const MacroOp& parent, bool dependent)
			: name_(parent.name_), size_(parent.size_), latency_(parent.latency_), uop1_(parent.uop1_), uop2_(parent.uop2_), dependent_(dependent) {}
		const char* getName() const {
			return name_;
		}
		int getSize() const {
			return size_;
		}
		int getLatency() const {
			return latency_;
		}
		ExecutionPort::type getUop1() const {
			return uop1_;
		}
		ExecutionPort::type getUop2() const {
			return uop2_;
		}
		bool isSimple() const {
			return uop2_ == ExecutionPort::Null;
		}
		bool isEliminated() const {
			return uop1_ == ExecutionPort::Null;
		}
		bool isDependent() const {
			return dependent_;
		}
		static const MacroOp Add_rr;
		static const MacroOp Add_ri;
		static const MacroOp Lea_sib;
		static const MacroOp Sub_rr;
		static const MacroOp Imul_rr;
		static const MacroOp Imul_r;
		static const MacroOp Mul_r;
		static const MacroOp Mov_rr;
		static const MacroOp Mov_ri64;
		static const MacroOp Xor_rr;
		static const MacroOp Xor_ri;
		static const MacroOp Ror_rcl;
		static const MacroOp Ror_ri;
		static const MacroOp TestJz_fused;
		static const MacroOp Xor_self;
		static const MacroOp Cmp_ri;
		static const MacroOp Setcc_r;
	private:
		const char* name_;
		int size_;
		int latency_;
		ExecutionPort::type uop1_;
		ExecutionPort::type uop2_;
		bool dependent_ = false;
	};

	
	const MacroOp MacroOp::Add_rr = MacroOp("add r,r", 3, 1, ExecutionPort::P015);
	const MacroOp MacroOp::Sub_rr = MacroOp("sub r,r", 3, 1, ExecutionPort::P015);
	const MacroOp MacroOp::Xor_rr = MacroOp("xor r,r", 3, 1, ExecutionPort::P015);
	const MacroOp MacroOp::Imul_r = MacroOp("imul r", 3, 4, ExecutionPort::P1, ExecutionPort::P5);
	const MacroOp MacroOp::Mul_r = MacroOp("mul r", 3, 4, ExecutionPort::P1, ExecutionPort::P5);
	const MacroOp MacroOp::Mov_rr = MacroOp("mov r,r", 3);

	
	const MacroOp MacroOp::Lea_sib = MacroOp("lea r,r+r*s", 4, 1, ExecutionPort::P01);
	const MacroOp MacroOp::Imul_rr = MacroOp("imul r,r", 4, 3, ExecutionPort::P1);
	const MacroOp MacroOp::Ror_ri = MacroOp("ror r,i", 4, 1, ExecutionPort::P05);

	
	const MacroOp MacroOp::Add_ri = MacroOp("add r,i", 7, 1, ExecutionPort::P015);
	const MacroOp MacroOp::Xor_ri = MacroOp("xor r,i", 7, 1, ExecutionPort::P015);

	
	const MacroOp MacroOp::Mov_ri64 = MacroOp("mov rax,i64", 10, 1, ExecutionPort::P015);

	
	const MacroOp MacroOp::Ror_rcl = MacroOp("ror r,cl", 3, 1, ExecutionPort::P0, ExecutionPort::P5);
	const MacroOp MacroOp::Xor_self = MacroOp("xor rcx,rcx", 3);
	const MacroOp MacroOp::Cmp_ri = MacroOp("cmp r,i", 7, 1, ExecutionPort::P015);
	const MacroOp MacroOp::Setcc_r = MacroOp("setcc cl", 3, 1, ExecutionPort::P05);
	const MacroOp MacroOp::TestJz_fused = MacroOp("testjz r,i", 13, 0, ExecutionPort::P5);

	const MacroOp IMULH_R_ops_array[] = { MacroOp::Mov_rr, MacroOp::Mul_r, MacroOp::Mov_rr };
	const MacroOp ISMULH_R_ops_array[] = { MacroOp::Mov_rr, MacroOp::Imul_r, MacroOp::Mov_rr };
	const MacroOp IMUL_RCP_ops_array[] = { MacroOp::Mov_ri64, MacroOp(MacroOp::Imul_rr, true) };

	class SuperscalarInstructionInfo {
	public:
		const char* getName() const {
			return name_;
		}
		int getSize() const {
			return ops_.size();
		}
		bool isSimple() const {
			return getSize() == 1;
		}
		int getLatency() const {
			return latency_;
		}
		const MacroOp& getOp(int index) const {
			return ops_[index];
		}
		SuperscalarInstructionType getType() const {
			return type_;
		}
		int getResultOp() const {
			return resultOp_;
		}
		int getDstOp() const {
			return dstOp_;
		}
		int getSrcOp() const {
			return srcOp_;
		}
		static const SuperscalarInstructionInfo ISUB_R;
		static const SuperscalarInstructionInfo IXOR_R;
		static const SuperscalarInstructionInfo IADD_RS;
		static const SuperscalarInstructionInfo IMUL_R;
		static const SuperscalarInstructionInfo IROR_C;
		static const SuperscalarInstructionInfo IADD_C7;
		static const SuperscalarInstructionInfo IXOR_C7;
		static const SuperscalarInstructionInfo IADD_C8;
		static const SuperscalarInstructionInfo IXOR_C8;
		static const SuperscalarInstructionInfo IADD_C9;
		static const SuperscalarInstructionInfo IXOR_C9;
		static const SuperscalarInstructionInfo IMULH_R;
		static const SuperscalarInstructionInfo ISMULH_R;
		static const SuperscalarInstructionInfo IMUL_RCP;
		static const SuperscalarInstructionInfo NOP;
	private:
		const char* name_;
		SuperscalarInstructionType type_;
		std::vector<MacroOp> ops_;
		int latency_;
		int resultOp_ = 0;
		int dstOp_ = 0;
		int srcOp_ = 0;

		SuperscalarInstructionInfo(const char* name)
			: name_(name), type_(SuperscalarInstructionType::INVALID), latency_(0) {}
		SuperscalarInstructionInfo(const char* name, SuperscalarInstructionType type, const MacroOp& op, int srcOp)
			: name_(name), type_(type), latency_(op.getLatency()), srcOp_(srcOp) {
			ops_.push_back(MacroOp(op));
		}
		template <size_t N>
		SuperscalarInstructionInfo(const char* name, SuperscalarInstructionType type, const MacroOp(&arr)[N], int resultOp, int dstOp, int srcOp)
			: name_(name), type_(type), latency_(0), resultOp_(resultOp), dstOp_(dstOp), srcOp_(srcOp) {
			for (unsigned i = 0; i < N; ++i) {
				ops_.push_back(MacroOp(arr[i]));
				latency_ += ops_.back().getLatency();
			}
			static_assert(N > 1, "Invalid array size");
		}
	};

	const SuperscalarInstructionInfo SuperscalarInstructionInfo::ISUB_R = SuperscalarInstructionInfo("ISUB_R", SuperscalarInstructionType::ISUB_R, MacroOp::Sub_rr, 0);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IXOR_R = SuperscalarInstructionInfo("IXOR_R", SuperscalarInstructionType::IXOR_R, MacroOp::Xor_rr, 0);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IADD_RS = SuperscalarInstructionInfo("IADD_RS", SuperscalarInstructionType::IADD_RS, MacroOp::Lea_sib, 0);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IMUL_R = SuperscalarInstructionInfo("IMUL_R", SuperscalarInstructionType::IMUL_R, MacroOp::Imul_rr, 0);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IROR_C = SuperscalarInstructionInfo("IROR_C", SuperscalarInstructionType::IROR_C, MacroOp::Ror_ri, -1);

	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IADD_C7 = SuperscalarInstructionInfo("IADD_C7", SuperscalarInstructionType::IADD_C7, MacroOp::Add_ri, -1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IXOR_C7 = SuperscalarInstructionInfo("IXOR_C7", SuperscalarInstructionType::IXOR_C7, MacroOp::Xor_ri, -1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IADD_C8 = SuperscalarInstructionInfo("IADD_C8", SuperscalarInstructionType::IADD_C8, MacroOp::Add_ri, -1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IXOR_C8 = SuperscalarInstructionInfo("IXOR_C8", SuperscalarInstructionType::IXOR_C8, MacroOp::Xor_ri, -1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IADD_C9 = SuperscalarInstructionInfo("IADD_C9", SuperscalarInstructionType::IADD_C9, MacroOp::Add_ri, -1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IXOR_C9 = SuperscalarInstructionInfo("IXOR_C9", SuperscalarInstructionType::IXOR_C9, MacroOp::Xor_ri, -1);

	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IMULH_R = SuperscalarInstructionInfo("IMULH_R", SuperscalarInstructionType::IMULH_R, IMULH_R_ops_array, 1, 0, 1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::ISMULH_R = SuperscalarInstructionInfo("ISMULH_R", SuperscalarInstructionType::ISMULH_R, ISMULH_R_ops_array, 1, 0, 1);
	const SuperscalarInstructionInfo SuperscalarInstructionInfo::IMUL_RCP = SuperscalarInstructionInfo("IMUL_RCP", SuperscalarInstructionType::IMUL_RCP, IMUL_RCP_ops_array, 1, 1, -1);

	const SuperscalarInstructionInfo SuperscalarInstructionInfo::NOP = SuperscalarInstructionInfo("NOP");

	
	
	
	const int buffer0[] = { 4, 8, 4 };
	const int buffer1[] = { 7, 3, 3, 3 };
	const int buffer2[] = { 3, 7, 3, 3 };
	const int buffer3[] = { 4, 9, 3 };
	const int buffer4[] = { 4, 4, 4, 4 };
	const int buffer5[] = { 3, 3, 10 };

	class DecoderBuffer {
	public:
		static const DecoderBuffer Default;
		template <size_t N>
		DecoderBuffer(const char* name, int index, const int(&arr)[N])
			: name_(name), index_(index), counts_(arr), opsCount_(N) {}
		const int* getCounts() const {
			return counts_;
		}
		int getSize() const {
			return opsCount_;
		}
		int getIndex() const {
			return index_;
		}
		const char* getName() const {
			return name_;
		}
		const DecoderBuffer* fetchNext(SuperscalarInstructionType instrType, int cycle, int mulCount, Blake2Generator& gen) const {
			
			
			
			if (instrType == SuperscalarInstructionType::IMULH_R || instrType == SuperscalarInstructionType::ISMULH_R)
				return &decodeBuffer3310;

			
			
			if (mulCount < cycle + 1)
				return &decodeBuffer4444;

			
			if(instrType == SuperscalarInstructionType::IMUL_RCP)
				return (gen.getByte() & 1) ? &decodeBuffer484 : &decodeBuffer493;

			
			return fetchNextDefault(gen);
		}
	private:
		const char* name_ = nullptr;
		int index_ = -1;
		const int* counts_ = nullptr;
		int opsCount_ = 0;
		DecoderBuffer() = default;
		static const DecoderBuffer decodeBuffer484;
		static const DecoderBuffer decodeBuffer7333;
		static const DecoderBuffer decodeBuffer3733;
		static const DecoderBuffer decodeBuffer493;
		static const DecoderBuffer decodeBuffer4444;
		static const DecoderBuffer decodeBuffer3310;
		static const DecoderBuffer* decodeBuffers[4];
		const DecoderBuffer* fetchNextDefault(Blake2Generator& gen) const {
			return decodeBuffers[gen.getByte() & 3];
		}
	};

	const DecoderBuffer DecoderBuffer::decodeBuffer484 = DecoderBuffer("4,8,4", 0, buffer0);
	const DecoderBuffer DecoderBuffer::decodeBuffer7333 = DecoderBuffer("7,3,3,3", 1, buffer1);
	const DecoderBuffer DecoderBuffer::decodeBuffer3733 = DecoderBuffer("3,7,3,3", 2, buffer2);
	const DecoderBuffer DecoderBuffer::decodeBuffer493 = DecoderBuffer("4,9,3", 3, buffer3);
	const DecoderBuffer DecoderBuffer::decodeBuffer4444 = DecoderBuffer("4,4,4,4", 4, buffer4);
	const DecoderBuffer DecoderBuffer::decodeBuffer3310 = DecoderBuffer("3,3,10", 5, buffer5);

	const DecoderBuffer* DecoderBuffer::decodeBuffers[4] = {
			&DecoderBuffer::decodeBuffer484,
			&DecoderBuffer::decodeBuffer7333,
			&DecoderBuffer::decodeBuffer3733,
			&DecoderBuffer::decodeBuffer493,
	};

	const DecoderBuffer DecoderBuffer::Default = DecoderBuffer();

	const SuperscalarInstructionInfo* slot_3[]  = { &SuperscalarInstructionInfo::ISUB_R, &SuperscalarInstructionInfo::IXOR_R };
	const SuperscalarInstructionInfo* slot_3L[] = { &SuperscalarInstructionInfo::ISUB_R, &SuperscalarInstructionInfo::IXOR_R, &SuperscalarInstructionInfo::IMULH_R, &SuperscalarInstructionInfo::ISMULH_R };
	const SuperscalarInstructionInfo* slot_4[]  = { &SuperscalarInstructionInfo::IROR_C, &SuperscalarInstructionInfo::IADD_RS };
	const SuperscalarInstructionInfo* slot_7[]  = { &SuperscalarInstructionInfo::IXOR_C7, &SuperscalarInstructionInfo::IADD_C7 };
	const SuperscalarInstructionInfo* slot_8[] = { &SuperscalarInstructionInfo::IXOR_C8, &SuperscalarInstructionInfo::IADD_C8 };
	const SuperscalarInstructionInfo* slot_9[] = { &SuperscalarInstructionInfo::IXOR_C9, &SuperscalarInstructionInfo::IADD_C9 };
	const SuperscalarInstructionInfo* slot_10   = &SuperscalarInstructionInfo::IMUL_RCP;

	static bool selectRegister(std::vector<int>& availableRegisters, Blake2Generator& gen, int& reg) {
		int index;
		if (availableRegisters.size() == 0)
			return false;

		if (availableRegisters.size() > 1) {
			index = gen.getUInt32() % availableRegisters.size();
		}
		else {
			index = 0;
		}
		reg = availableRegisters[index];
		return true;
	}

	class RegisterInfo {
	public:
		RegisterInfo() : latency(0), lastOpGroup(SuperscalarInstructionType::INVALID), lastOpPar(-1), value(0) {}
		int latency;
		SuperscalarInstructionType lastOpGroup;
		int lastOpPar;
		int value;
	};

	
	class SuperscalarInstruction {
	public:
		void toInstr(Instruction& instr) { 
			instr.opcode = (int)getType();
			instr.dst = dst_;
			instr.src = src_ >= 0 ? src_ : dst_;
			instr.setMod(mod_);
			instr.setImm32(imm32_);
		}

		void createForSlot(Blake2Generator& gen, int slotSize, int fetchType, bool isLast, bool isFirst) {
			switch (slotSize)
			{
			case 3:
				
				if (isLast) {
					create(slot_3L[gen.getByte() & 3], gen);
				}
				else {
					create(slot_3[gen.getByte() & 1], gen);
				}
				break;
			case 4:
				
				if (fetchType == 4 && !isLast) {
					create(&SuperscalarInstructionInfo::IMUL_R, gen);
				}
				else {
					create(slot_4[gen.getByte() & 1], gen);
				}
				break;
			case 7:
				create(slot_7[gen.getByte() & 1], gen);
				break;
			case 8:
				create(slot_8[gen.getByte() & 1], gen);
				break;
			case 9:
				create(slot_9[gen.getByte() & 1], gen);
				break;
			case 10:
				create(slot_10, gen);
				break;
			default:
				UNREACHABLE;
			}
		}

		void create(const SuperscalarInstructionInfo* info, Blake2Generator& gen) {
			info_ = info;
			reset();
			switch (info->getType())
			{
			case SuperscalarInstructionType::ISUB_R: {
				mod_ = 0;
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::IADD_RS;
				groupParIsSource_ = true;
			} break;

			case SuperscalarInstructionType::IXOR_R: {
				mod_ = 0;
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::IXOR_R;
				groupParIsSource_ = true;
			} break;

			case SuperscalarInstructionType::IADD_RS: {
				mod_ = gen.getByte();
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::IADD_RS;
				groupParIsSource_ = true;
			} break;

			case SuperscalarInstructionType::IMUL_R: {
				mod_ = 0;
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::IMUL_R;
				groupParIsSource_ = true;
			} break;

			case SuperscalarInstructionType::IROR_C: {
				mod_ = 0;
				do {
					imm32_ = gen.getByte() & 63;
				} while (imm32_ == 0);
				opGroup_ = SuperscalarInstructionType::IROR_C;
				opGroupPar_ = -1;
			} break;

			case SuperscalarInstructionType::IADD_C7:
			case SuperscalarInstructionType::IADD_C8:
			case SuperscalarInstructionType::IADD_C9: {
				mod_ = 0;
				imm32_ = gen.getUInt32();
				opGroup_ = SuperscalarInstructionType::IADD_C7;
				opGroupPar_ = -1;
			} break;

			case SuperscalarInstructionType::IXOR_C7:
			case SuperscalarInstructionType::IXOR_C8:
			case SuperscalarInstructionType::IXOR_C9: {
				mod_ = 0;
				imm32_ = gen.getUInt32();
				opGroup_ = SuperscalarInstructionType::IXOR_C7;
				opGroupPar_ = -1;
			} break;

			case SuperscalarInstructionType::IMULH_R: {
				canReuse_ = true;
				mod_ = 0;
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::IMULH_R;
				opGroupPar_ = gen.getUInt32();
			} break;

			case SuperscalarInstructionType::ISMULH_R: {
				canReuse_ = true;
				mod_ = 0;
				imm32_ = 0;
				opGroup_ = SuperscalarInstructionType::ISMULH_R;
				opGroupPar_ = gen.getUInt32();
			} break;

			case SuperscalarInstructionType::IMUL_RCP: {
				mod_ = 0;
				do {
					imm32_ = gen.getUInt32();
				} while (isZeroOrPowerOf2(imm32_));
				opGroup_ = SuperscalarInstructionType::IMUL_RCP;
				opGroupPar_ = -1;
			} break;

			default:
				break;
			}
		}

		bool selectDestination(int cycle, bool allowChainedMul, RegisterInfo (&registers)[8], Blake2Generator& gen) {
			
			std::vector<int> availableRegisters;
			
			
			
			
			
			
			
			
			
			
			for (int i = 0; i < 8; ++i) {
				if (registers[i].latency <= cycle && (canReuse_ || i != src_) && (allowChainedMul || opGroup_ != SuperscalarInstructionType::IMUL_R || registers[i].lastOpGroup != SuperscalarInstructionType::IMUL_R) && (registers[i].lastOpGroup != opGroup_ || registers[i].lastOpPar != opGroupPar_) && (info_->getType() != SuperscalarInstructionType::IADD_RS || i != RegisterNeedsDisplacement))
					availableRegisters.push_back(i);
			}
			return selectRegister(availableRegisters, gen, dst_);
		}

		bool selectSource(int cycle, RegisterInfo(&registers)[8], Blake2Generator& gen) {
			std::vector<int> availableRegisters;
			
			for (unsigned i = 0; i < 8; ++i) {
				if (registers[i].latency <= cycle)
					availableRegisters.push_back(i);
			}
			
			if (availableRegisters.size() == 2 && info_->getType() == SuperscalarInstructionType::IADD_RS) {
				if (availableRegisters[0] == RegisterNeedsDisplacement || availableRegisters[1] == RegisterNeedsDisplacement) {
					opGroupPar_ = src_ = RegisterNeedsDisplacement;
					return true;
				}
			}
			if (selectRegister(availableRegisters, gen, src_)) {
				if (groupParIsSource_)
					opGroupPar_ = src_;
				return true;
			}
			return false;
		}

		SuperscalarInstructionType getType() {
			return info_->getType();
		}
		int getSource() {
			return src_;
		}
		int getDestination() {
			return dst_;
		}
		SuperscalarInstructionType getGroup() {
			return opGroup_;
		}
		int getGroupPar() {
			return opGroupPar_;
		}

		const SuperscalarInstructionInfo& getInfo() const {
			return *info_;
		}

		static const SuperscalarInstruction Null;

	private:
		const SuperscalarInstructionInfo* info_;
		int src_ = -1;
		int dst_ = -1;
		int mod_ = 0;
		uint32_t imm32_ = 0;
		SuperscalarInstructionType opGroup_ = SuperscalarInstructionType::INVALID;
		int opGroupPar_ = 0;
		bool canReuse_ = false;
		bool groupParIsSource_ = false;

		void reset() {
			src_ = dst_ = -1;
			canReuse_ = groupParIsSource_ = false;
		}

		SuperscalarInstruction(const SuperscalarInstructionInfo* info) : info_(info) {
		}
	};

	const SuperscalarInstruction SuperscalarInstruction::Null = SuperscalarInstruction(&SuperscalarInstructionInfo::NOP);

	constexpr int CYCLE_MAP_SIZE = RANDOMX_SUPERSCALAR_MAX_LATENCY + 4;
	constexpr int LOOK_FORWARD_CYCLES = 4;
	constexpr int MAX_THROWAWAY_COUNT = 256;

	template<bool commit>
	static int scheduleUop(ExecutionPort::type uop, ExecutionPort::type(&portBusy)[CYCLE_MAP_SIZE][3], int cycle) {
		
		
		for (; cycle < static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency) + 4; ++cycle) {
			if ((uop & ExecutionPort::P5) != 0 && !portBusy[cycle][2]) {
				if (commit) {
					if (trace) std::cout << "; P5 at cycle " << cycle << std::endl;
					portBusy[cycle][2] = uop;
				}
				return cycle;
			}
			if ((uop & ExecutionPort::P0) != 0 && !portBusy[cycle][0]) {
				if (commit) {
					if (trace) std::cout << "; P0 at cycle " << cycle << std::endl;
					portBusy[cycle][0] = uop;
				}
				return cycle;
			}
			if ((uop & ExecutionPort::P1) != 0 && !portBusy[cycle][1]) {
				if (commit) {
					if (trace) std::cout << "; P1 at cycle " << cycle << std::endl;
					portBusy[cycle][1] = uop;
				}
				return cycle;
			}
		}
		return -1;
	}

	template<bool commit>
	static int scheduleMop(const MacroOp& mop, ExecutionPort::type(&portBusy)[CYCLE_MAP_SIZE][3], int cycle, int depCycle) {
		
		
		if (mop.isDependent()) {
			cycle = (cycle > depCycle) ? cycle : depCycle;
		}
		
		if (mop.isEliminated()) {
			if (commit)
				if (trace) std::cout << "; (eliminated)" << std::endl;
			return cycle;
		}
		else if (mop.isSimple()) {
			
			return scheduleUop<commit>(mop.getUop1(), portBusy, cycle);
		}
		else {
			
			for (; cycle < static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency) + 4; ++cycle) {

				int cycle1 = scheduleUop<false>(mop.getUop1(), portBusy, cycle);
				int cycle2 = scheduleUop<false>(mop.getUop2(), portBusy, cycle);

				if (cycle1 >= 0 && cycle1 == cycle2) {
					if (commit) {
						scheduleUop<true>(mop.getUop1(), portBusy, cycle1);
						scheduleUop<true>(mop.getUop2(), portBusy, cycle2);
					}
					return cycle1;
				}
			}
		}

		return -1;
	}

	void generateSuperscalar(SuperscalarProgram& prog, Blake2Generator& gen) {

		ExecutionPort::type portBusy[CYCLE_MAP_SIZE][3];
		memset(portBusy, 0, sizeof(portBusy));
		RegisterInfo registers[8];

		const DecoderBuffer* decodeBuffer = &DecoderBuffer::Default;
		SuperscalarInstruction currentInstruction = SuperscalarInstruction::Null;
		int macroOpIndex = 0;
		int codeSize = 0;
		int macroOpCount = 0;
		int cycle = 0;
		int depCycle = 0;
		int retireCycle = 0;
		bool portsSaturated = false;
		int programSize = 0;
		int mulCount = 0;
		int decodeCycle;
		int throwAwayCount = 0;

		
		
		
		
		
		for (decodeCycle = 0; decodeCycle < static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency) && !portsSaturated && programSize < 3 * static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency) + 2; ++decodeCycle) {

			
			decodeBuffer = decodeBuffer->fetchNext(currentInstruction.getType(), decodeCycle, mulCount, gen);
			if (trace) std::cout << "; ------------- fetch cycle " << cycle << " (" << decodeBuffer->getName() << ")" << std::endl;

			int bufferIndex = 0;

			
			while (bufferIndex < decodeBuffer->getSize()) {
				int topCycle = cycle;

				
				if (macroOpIndex >= currentInstruction.getInfo().getSize()) {
					if (portsSaturated || programSize >= 3 * static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency) + 2)
						break;
					
					currentInstruction.createForSlot(gen, decodeBuffer->getCounts()[bufferIndex], decodeBuffer->getIndex(), decodeBuffer->getSize() == bufferIndex + 1, bufferIndex == 0);
					macroOpIndex = 0;
					if (trace) std::cout << "; " << currentInstruction.getInfo().getName() << std::endl;
				}
				const MacroOp& mop = currentInstruction.getInfo().getOp(macroOpIndex);
				if (trace) std::cout << mop.getName() << " ";

				
				int scheduleCycle = scheduleMop<false>(mop, portBusy, cycle, depCycle);
				if (scheduleCycle < 0) {
					if (trace) std::cout << "Unable to map operation '" << mop.getName() << "' to execution port (cycle " << cycle << ")" << std::endl;
					
					portsSaturated = true;
					break;
				}

				
				if (macroOpIndex == currentInstruction.getInfo().getSrcOp()) {
					int forward;
					
					for (forward = 0; forward < LOOK_FORWARD_CYCLES && !currentInstruction.selectSource(scheduleCycle, registers, gen); ++forward) {
						if (trace) std::cout << "; src STALL at cycle " << cycle << std::endl;
						++scheduleCycle;
						++cycle;
					}
					
					if (forward == LOOK_FORWARD_CYCLES) {
						if (throwAwayCount < MAX_THROWAWAY_COUNT) {
							throwAwayCount++;
							macroOpIndex = currentInstruction.getInfo().getSize();
							if (trace) std::cout << "; THROW away " << currentInstruction.getInfo().getName() << std::endl;
							
							continue;
						}
						
						if (trace) std::cout << "Aborting at cycle " << cycle << " with decode buffer " << decodeBuffer->getName() << " - source registers not available for operation " << currentInstruction.getInfo().getName() << std::endl;
						currentInstruction = SuperscalarInstruction::Null;
						break;
					}
					if (trace) std::cout << "; src = r" << currentInstruction.getSource() << std::endl;
				}
				
				if (macroOpIndex == currentInstruction.getInfo().getDstOp()) {
					int forward;
					for (forward = 0; forward < LOOK_FORWARD_CYCLES && !currentInstruction.selectDestination(scheduleCycle, throwAwayCount > 0, registers, gen); ++forward) {
						if (trace) std::cout << "; dst STALL at cycle " << cycle << std::endl;
						++scheduleCycle;
						++cycle;
					}
					if (forward == LOOK_FORWARD_CYCLES) { 
						if (throwAwayCount < MAX_THROWAWAY_COUNT) {
							throwAwayCount++;
							macroOpIndex = currentInstruction.getInfo().getSize();
							if (trace) std::cout << "; THROW away " << currentInstruction.getInfo().getName() << std::endl;
							
							continue;
						}
						
						if (trace) std::cout << "Aborting at cycle " << cycle << " with decode buffer " << decodeBuffer->getName() << " - destination registers not available" << std::endl;
						currentInstruction = SuperscalarInstruction::Null;
						break;
					}
					if (trace) std::cout << "; dst = r" << currentInstruction.getDestination() << std::endl;
				}
				throwAwayCount = 0;

				
				scheduleCycle = scheduleMop<true>(mop, portBusy, scheduleCycle, scheduleCycle);

				if (scheduleCycle < 0) {
					if (trace) std::cout << "Unable to map operation '" << mop.getName() << "' to execution port (cycle " << scheduleCycle << ")" << std::endl;
					portsSaturated = true;
					break;
				}

				
				depCycle = scheduleCycle + mop.getLatency();

				
				
				
				
				if (macroOpIndex == currentInstruction.getInfo().getResultOp()) {
					int dst = currentInstruction.getDestination();
					RegisterInfo& ri = registers[dst];
					retireCycle = depCycle;
					ri.latency = retireCycle;
					ri.lastOpGroup = currentInstruction.getGroup();
					ri.lastOpPar = currentInstruction.getGroupPar();
					if (trace) std::cout << "; RETIRED at cycle " << retireCycle << std::endl;
				}
				codeSize += mop.getSize();
				bufferIndex++;
				macroOpIndex++;
				macroOpCount++;

				
				if (scheduleCycle >= static_cast<int>(RandomX_CurrentConfig.SuperscalarLatency)) {
					portsSaturated = true;
				}
				cycle = topCycle;

				
				if (macroOpIndex >= currentInstruction.getInfo().getSize()) {
					currentInstruction.toInstr(prog(programSize++));
					mulCount += isMultiplication(currentInstruction.getType());
				}
			}
			++cycle;
		}

		double ipc = (macroOpCount / (double)retireCycle);

		memset(prog.asicLatencies, 0, sizeof(prog.asicLatencies));

		
		
		for (int i = 0; i < programSize; ++i) {
			Instruction& instr = prog(i);
			int latDst = prog.asicLatencies[instr.dst] + 1;
			int latSrc = instr.dst != instr.src ? prog.asicLatencies[instr.src] + 1 : 0;
			prog.asicLatencies[instr.dst] = (latDst > latSrc) ? latDst : latSrc;
		}

		
		int asicLatencyMax = 0;
		int addressReg = 0;
		for (int i = 0; i < 8; ++i) {
			if (prog.asicLatencies[i] > asicLatencyMax) {
				asicLatencyMax = prog.asicLatencies[i];
				addressReg = i;
			}
			prog.cpuLatencies[i] = registers[i].latency;
		}

		prog.setSize(programSize);
		prog.setAddressRegister(addressReg);

		prog.cpuLatency = retireCycle;
		prog.asicLatency = asicLatencyMax;
		prog.codeSize = codeSize;
		prog.macroOps = macroOpCount;
		prog.decodeCycles = decodeCycle;
		prog.ipc = ipc;
		prog.mulCount = mulCount;


		
	}

	void executeSuperscalar(int_reg_t(&r)[8], SuperscalarProgram& prog) {
		for (unsigned j = 0; j < prog.getSize(); ++j) {
			Instruction& instr = prog(j);
			switch ((SuperscalarInstructionType)instr.opcode)
			{
			case SuperscalarInstructionType::ISUB_R:
				r[instr.dst] -= r[instr.src];
				break;
			case SuperscalarInstructionType::IXOR_R:
				r[instr.dst] ^= r[instr.src];
				break;
			case SuperscalarInstructionType::IADD_RS:
				r[instr.dst] += r[instr.src] << instr.getModShift();
				break;
			case SuperscalarInstructionType::IMUL_R:
				r[instr.dst] *= r[instr.src];
				break;
			case SuperscalarInstructionType::IROR_C:
				r[instr.dst] = rotr64(r[instr.dst], instr.getImm32());
				break;
			case SuperscalarInstructionType::IADD_C7:
			case SuperscalarInstructionType::IADD_C8:
			case SuperscalarInstructionType::IADD_C9:
				r[instr.dst] += signExtend2sCompl(instr.getImm32());
				break;
			case SuperscalarInstructionType::IXOR_C7:
			case SuperscalarInstructionType::IXOR_C8:
			case SuperscalarInstructionType::IXOR_C9:
				r[instr.dst] ^= signExtend2sCompl(instr.getImm32());
				break;
			case SuperscalarInstructionType::IMULH_R:
				r[instr.dst] = mulh(r[instr.dst], r[instr.src]);
				break;
			case SuperscalarInstructionType::ISMULH_R:
				r[instr.dst] = smulh(r[instr.dst], r[instr.src]);
				break;
			case SuperscalarInstructionType::IMUL_RCP:
				r[instr.dst] *= randomx_reciprocal(instr.getImm32());
				break;
			default:
				UNREACHABLE;
			}
		}
	}
}
