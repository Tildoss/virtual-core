#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define MAX_INSTRUCTIONS 1024

uint32_t instructions[MAX_INSTRUCTIONS];
uint32_t PC = 0;
uint64_t registers[16];

typedef struct {
    uint32_t bcc;
    bool immediate_flag;
    uint32_t opcode;
    uint32_t first_operand;
    uint32_t second_operand;
    uint32_t destination;
    uint32_t immediate_value;
} Instruction;

Instruction decode(uint32_t binary_instruction) {
    Instruction instruction;

    instruction.bcc = (binary_instruction >> 28) & 0xF;
    instruction.immediate_flag = (binary_instruction >> 24) & 0x1;
    instruction.opcode = (binary_instruction >> 20) & 0xF;
    instruction.first_operand = (binary_instruction >> 16) & 0xF;
    instruction.second_operand = (binary_instruction >> 12) & 0xF;
    instruction.destination = (binary_instruction >> 8) & 0xF;
    instruction.immediate_value = binary_instruction & 0xFF;

    return instruction;
}

void execute(Instruction instruction) {
    uint64_t result = 0;
    uint64_t first_operand_value = registers[instruction.first_operand];
    uint64_t second_operand_value = instruction.immediate_flag ? instruction.immediate_value : registers[instruction.second_operand];

    switch (instruction.opcode) {
        case 0:  // AND
            result = first_operand_value & second_operand_value;
            break;
        case 1:  // ORR
            result = first_operand_value | second_operand_value;
            break;
        case 2:  // EOR
            result = first_operand_value ^ second_operand_value;
            break;
        case 3:  // ADD
            result = first_operand_value + second_operand_value;
            // Check for overflow
            if(result < first_operand_value || result < second_operand_value) {
                // Set carry flag
                registers[15] |= 1;
            } else {
                // Clear carry flag
                registers[15] &= ~1;
            }
            break;
        case 4:  // ADC
            {
                uint64_t carry = (registers[15] & 1);
                result = first_operand_value + second_operand_value + carry;
                // If there is an overflow, the result will be less than the operand(s).
                if(result < first_operand_value || (carry && result <= first_operand_value) || result < second_operand_value) {
                    // Set carry flag
                    registers[15] |= 1;
                } else {
                    // Clear carry flag
                    registers[15] &= ~1;
                }
            }
            break;
        case 5:  // CMP
            result = first_operand_value - second_operand_value;
            registers[15] = (result == 0) ? 1 : 0;
            return;
        case 6:  // SUB
            result = first_operand_value - second_operand_value;
            // Check for underflow
            if (first_operand_value < second_operand_value) {
                // Set carry flag
                registers[15] |= 1;
            } else {
                // Clear carry flag
                registers[15] &= ~1;
            }
            break;
        case 7:  // SBC
            {
                uint64_t borrow = (registers[15] & 1);
                result = first_operand_value - second_operand_value - borrow;
                // If there is an underflow, the result will be more than the operand(s).
                if (first_operand_value < result || (borrow && first_operand_value <= result)) {
                    // Set carry flag
                    registers[15] |= 1;
                } else {
                    // Clear carry flag
                    registers[15] &= ~1;
                }
            }
            break;
        case 8:  // MOV
            result = second_operand_value;
            break;
        case 9:  // LSH
            result = first_operand_value << second_operand_value;
            break;
        case 10:  // RSH
            result = first_operand_value >> second_operand_value;
            break;
        default:
            printf("Error: Unknown opcode 0x%01X\n", instruction.opcode);
            return;
    }

    registers[instruction.destination] = result;
}




uint32_t swap_endianness(uint32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

void fetch(FILE *binary_file) {
    uint32_t instruction;
    size_t read_count;

    // Read 4-byte (32-bit) instructions from the binary file
    read_count = fread(&instruction, sizeof(uint32_t), 1, binary_file);

    // Check if there's a valid instruction
    if (read_count == 1) {
        // Swap endianness from little-endian to big-endian
        instruction = swap_endianness(instruction);

        // Store the instruction in the instructions array
        instructions[PC] = instruction;

        // Increment the Program Counter (PC) to point to the next instruction
        PC++;
    }
}

int main(int argc, char *argv[]) {
    bool verbose = false;

    if (argc < 3) {
        printf("Usage: %s <binary_file> <register_state_file> [verbose]\n", argv[0]);
        return 1;
    }

    if (argc > 3 && strcmp(argv[3], "verbose") == 0) {
        verbose = true;
    }

    // Read the initial state of the registers from the file
    FILE *register_state_file = fopen(argv[2], "r");
    if (register_state_file == NULL) {
        printf("Error opening register state file: %s\n", argv[2]);
        return 1;
    }

    for (int i = 0; i < 16; i++) {
        char register_name[3];
        fscanf(register_state_file, "%2s=0x%llX", register_name, &registers[i]);
    }

    fclose(register_state_file);

    // Read the binary file and fetch the instructions
    FILE *binary_file = fopen(argv[1], "rb");
    if (binary_file == NULL) {
        printf("Error opening binary file: %s\n", argv[1]);
        return 1;
    }

    while (!feof(binary_file)) {
        fetch(binary_file);
    }

    fclose(binary_file);

    // Decode and execute the instructions
    for (uint32_t i = 0; i < PC; i++) {
        Instruction decoded_instruction = decode(instructions[i]);
        if (verbose) {
            for (int i = 0; i < 16; i++) {
                printf("R%u: %llu (0x%016llX)\n", i, registers[i], registers[i]);
            }
            printf("-----\n");
        }
        // printf("Instruction %u:\n", i);
        // printf("  BCC: 0x%01X\n", decoded_instruction.bcc);
        // printf("  Immediate Flag: %d\n", decoded_instruction.immediate_flag);
        // printf("  Opcode: 0x%01X\n", decoded_instruction.opcode);
        // printf("  First Operand: R%u\n", decoded_instruction.first_operand);
        // printf("  Second Operand: R%u\n", decoded_instruction.second_operand);
        // printf("  Destination: R%u\n", decoded_instruction.destination);
        // printf("  Immediate Value: 0x%02X\n", decoded_instruction.immediate_value);
        execute(decoded_instruction);

    }

    // Print the final state of the registers if verbose mode is enabled

    printf("Final state of the registers :\n");
    for (int i = 0; i < 16; i++) {
        printf("R%u: %llu (0x%016llX)\n", i, registers[i], registers[i]);
    }
    return 0;
}
