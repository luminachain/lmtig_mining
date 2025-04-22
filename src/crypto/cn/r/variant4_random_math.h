#ifndef VARIANT4_RANDOM_MATH_H
#define VARIANT4_RANDOM_MATH_H


#include <cstring>


#include "base/crypto/Algorithm.h"


extern "C"
{
    #include "crypto/cn/c_blake256.h"
}

enum V4_Settings
{
	
	TOTAL_LATENCY = 15 * 3,
	
	
	NUM_INSTRUCTIONS_MIN = 60,

	
	NUM_INSTRUCTIONS_MAX = 70,

	
	
	ALU_COUNT_MUL = 1,

	
	
	ALU_COUNT = 3,
};

enum V4_InstructionList
{
	MUL,	
	ADD,	
	SUB,	
	ROR,	
	ROL,	
	XOR,	
	RET,	
	V4_INSTRUCTION_COUNT = RET,
};








enum V4_InstructionDefinition
{
	V4_OPCODE_BITS = 3,
	V4_DST_INDEX_BITS = 2,
	V4_SRC_INDEX_BITS = 3,
};

struct V4_Instruction
{
	uint8_t opcode;
	uint8_t dst_index;
	uint8_t src_index;
	uint32_t C;
};

#ifndef FORCEINLINE
#ifdef __GNUC__
#define FORCEINLINE __attribute__((always_inline)) inline
#elif _MSC_VER
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
#endif

#ifndef UNREACHABLE_CODE
#ifdef __GNUC__
#define UNREACHABLE_CODE __builtin_unreachable()
#elif _MSC_VER
#define UNREACHABLE_CODE __assume(false)
#else
#define UNREACHABLE_CODE
#endif
#endif





template<typename v4_reg>
static void v4_random_math(const struct V4_Instruction* code, v4_reg* r)
{
	enum
	{
		REG_BITS = sizeof(v4_reg) * 8,
	};

#define V4_EXEC(i) \
	{ \
		const struct V4_Instruction* op = code + i; \
		const v4_reg src = r[op->src_index]; \
		v4_reg* dst = r + op->dst_index; \
		switch (op->opcode) \
		{ \
		case MUL: \
			*dst *= src; \
			break; \
		case ADD: \
			*dst += src + op->C; \
			break; \
		case SUB: \
			*dst -= src; \
			break; \
		case ROR: \
			{ \
				const uint32_t shift = src % REG_BITS; \
				*dst = (*dst >> shift) | (*dst << ((REG_BITS - shift) % REG_BITS)); \
			} \
			break; \
		case ROL: \
			{ \
				const uint32_t shift = src % REG_BITS; \
				*dst = (*dst << shift) | (*dst >> ((REG_BITS - shift) % REG_BITS)); \
			} \
			break; \
		case XOR: \
			*dst ^= src; \
			break; \
		case RET: \
			return; \
		default: \
			UNREACHABLE_CODE; \
			break; \
		} \
	}

#define V4_EXEC_10(j) \
	V4_EXEC(j + 0) \
	V4_EXEC(j + 1) \
	V4_EXEC(j + 2) \
	V4_EXEC(j + 3) \
	V4_EXEC(j + 4) \
	V4_EXEC(j + 5) \
	V4_EXEC(j + 6) \
	V4_EXEC(j + 7) \
	V4_EXEC(j + 8) \
	V4_EXEC(j + 9)

	
	
	
	
	
	
	
	
	
	
	
	
	

	
	V4_EXEC_10(0);		
	V4_EXEC_10(10);		
	V4_EXEC_10(20);		
	V4_EXEC_10(30);		
	V4_EXEC_10(40);		
	V4_EXEC_10(50);		
	V4_EXEC_10(60);		

#undef V4_EXEC_10
#undef V4_EXEC
}


static FORCEINLINE void check_data(size_t* data_index, const size_t bytes_needed, int8_t* data, const size_t data_size)
{
	if (*data_index + bytes_needed > data_size)
	{
		hash_extra_blake(data, data_size, (char*) data);
		*data_index = 0;
	}
}



template<xmrig::Algorithm::Id ALGO>
static int v4_random_math_init(struct V4_Instruction* code, const uint64_t height)
{
	
	
	
	
	
	
	
	const int op_latency[V4_INSTRUCTION_COUNT] = { 3, 2, 1, 2, 2, 1 };

	
	const int asic_op_latency[V4_INSTRUCTION_COUNT] = { 3, 1, 1, 1, 1, 1 };

	
	const int op_ALUs[V4_INSTRUCTION_COUNT] = { ALU_COUNT_MUL, ALU_COUNT, ALU_COUNT, ALU_COUNT, ALU_COUNT, ALU_COUNT };

	int8_t data[32];
	memset(data, 0, sizeof(data));
	uint64_t tmp = SWAP64LE(height);
	memcpy(data, &tmp, sizeof(uint64_t));
	if (ALGO == xmrig::Algorithm::CN_R)	{
		data[20] = -38;
	}

	
	
	
	size_t data_index = sizeof(data);

	int code_size;

	
	
	bool r8_used;
	do {
		int latency[9];
		int asic_latency[9];

		
		
		
		
		
		
		
		uint32_t inst_data[9] = { 0, 1, 2, 3, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF };

		bool alu_busy[TOTAL_LATENCY + 1][ALU_COUNT];
		bool is_rotation[V4_INSTRUCTION_COUNT];
		bool rotated[4];
		int rotate_count = 0;

		memset(latency, 0, sizeof(latency));
		memset(asic_latency, 0, sizeof(asic_latency));
		memset(alu_busy, 0, sizeof(alu_busy));
		memset(is_rotation, 0, sizeof(is_rotation));
		memset(rotated, 0, sizeof(rotated));
		is_rotation[ROR] = true;
		is_rotation[ROL] = true;

		int num_retries = 0;
		code_size = 0;

		int total_iterations = 0;
		r8_used = false;

		
		
		while (((latency[0] < TOTAL_LATENCY) || (latency[1] < TOTAL_LATENCY) || (latency[2] < TOTAL_LATENCY) || (latency[3] < TOTAL_LATENCY)) && (num_retries < 64))
		{
			
			++total_iterations;
			if (total_iterations > 256)
				break;

			check_data(&data_index, 1, data, sizeof(data));

			const uint8_t c = ((uint8_t*)data)[data_index++];

			
			
			
			
			
			uint8_t opcode = c & ((1 << V4_OPCODE_BITS) - 1);
			if (opcode == 5)
			{
				check_data(&data_index, 1, data, sizeof(data));
				opcode = (data[data_index++] >= 0) ? ROR : ROL;
			}
			else if (opcode >= 6)
			{
				opcode = XOR;
			}
			else
			{
				opcode = (opcode <= 2) ? MUL : (opcode - 2);
			}

			uint8_t dst_index = (c >> V4_OPCODE_BITS) & ((1 << V4_DST_INDEX_BITS) - 1);
			uint8_t src_index = (c >> (V4_OPCODE_BITS + V4_DST_INDEX_BITS)) & ((1 << V4_SRC_INDEX_BITS) - 1);

			const int a = dst_index;
			int b = src_index;

			
			if (((opcode == ADD) || (opcode == SUB) || (opcode == XOR)) && (a == b)) {
				
				b = 8;
				src_index = b;
			}

			
			if (is_rotation[opcode] && rotated[a])
			{
				continue;
			}

			
			
			
			if ((opcode != MUL) && ((inst_data[a] & 0xFFFF00) == (opcode << 8) + ((inst_data[b] & 255) << 16)))
			{
				continue;
			}

			
			int next_latency = (latency[a] > latency[b]) ? latency[a] : latency[b];
			int alu_index = -1;
			while (next_latency < TOTAL_LATENCY)
			{
				for (int i = op_ALUs[opcode] - 1; i >= 0; --i)
				{
					if (!alu_busy[next_latency][i])
					{
						
						if ((opcode == ADD) && alu_busy[next_latency + 1][i])
						{
							continue;
						}

						
						if (is_rotation[opcode] && (next_latency < rotate_count * op_latency[opcode]))
						{
							continue;
						}

						alu_index = i;
						break;
					}
				}
				if (alu_index >= 0)
				{
					break;
				}
				++next_latency;
			}

			
			if (next_latency > latency[a] + 7)
			{
				continue;
			}

			next_latency += op_latency[opcode];

			if (next_latency <= TOTAL_LATENCY)
			{
				if (is_rotation[opcode])
				{
					++rotate_count;
				}

				
				alu_busy[next_latency - op_latency[opcode]][alu_index] = true;
				latency[a] = next_latency;

				
				asic_latency[a] = ((asic_latency[a] > asic_latency[b]) ? asic_latency[a] : asic_latency[b]) + asic_op_latency[opcode];

				rotated[a] = is_rotation[opcode];

				inst_data[a] = code_size + (opcode << 8) + ((inst_data[b] & 255) << 16);

				code[code_size].opcode = opcode;
				code[code_size].dst_index = dst_index;
				code[code_size].src_index = src_index;
				code[code_size].C = 0;

				if (src_index == 8)
				{
					r8_used = true;
				}

				if (opcode == ADD)
				{
					
					alu_busy[next_latency - op_latency[opcode] + 1][alu_index] = true;

					
					check_data(&data_index, sizeof(uint32_t), data, sizeof(data));
					uint32_t t;
					memcpy(&t, data + data_index, sizeof(uint32_t));
					code[code_size].C = SWAP32LE(t);
					data_index += sizeof(uint32_t);
				}

				++code_size;
				if (code_size >= NUM_INSTRUCTIONS_MIN)
				{
					break;
				}
			}
			else
			{
				++num_retries;
			}
		}

		
		
		
		const int prev_code_size = code_size;
		while ((code_size < NUM_INSTRUCTIONS_MAX) && (asic_latency[0] < TOTAL_LATENCY) && (asic_latency[1] < TOTAL_LATENCY) && (asic_latency[2] < TOTAL_LATENCY) && (asic_latency[3] < TOTAL_LATENCY))
		{
			int min_idx = 0;
			int max_idx = 0;
			for (int i = 1; i < 4; ++i)
			{
				if (asic_latency[i] < asic_latency[min_idx]) min_idx = i;
				if (asic_latency[i] > asic_latency[max_idx]) max_idx = i;
			}

			const uint8_t pattern[3] = { ROR, MUL, MUL };
			const uint8_t opcode = pattern[(code_size - prev_code_size) % 3];
			latency[min_idx] = latency[max_idx] + op_latency[opcode];
			asic_latency[min_idx] = asic_latency[max_idx] + asic_op_latency[opcode];

			code[code_size].opcode = opcode;
			code[code_size].dst_index = min_idx;
			code[code_size].src_index = max_idx;
			code[code_size].C = 0;
			++code_size;
		}

	
	
	}  while (!r8_used || (code_size < NUM_INSTRUCTIONS_MIN) || (code_size > NUM_INSTRUCTIONS_MAX));

	
	
	code[code_size].opcode = RET;
	code[code_size].dst_index = 0;
	code[code_size].src_index = 0;
	code[code_size].C = 0;

	return code_size;
}

#endif
