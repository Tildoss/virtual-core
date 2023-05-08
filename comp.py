import sys

def assemble_instruction(instruction, opcode_dict):
    tokens = instruction.split()
    opcode = tokens[0]
    operands = [x.strip() for x in ''.join(tokens[1:]).split(',')]

    binary_instruction = 0

    if opcode in opcode_dict:
        binary_instruction |= (opcode_dict[opcode] << 20)

        if opcode in ["AND", "ORR", "EOR", "ADD", "ADC", "SUB", "SBC", "LSH", "RSH"]:
            if operands[0].startswith("R"):
                dest = int(operands[0][1:])
            else:
                dest = int(operands[0])
            binary_instruction |= (dest << 8)

            if operands[1].startswith("R"):
                ope1 = int(operands[1][1:])
            else:
                ope1 = int(operands[1])
            binary_instruction |= (ope1 << 16)

            if len(operands) == 3:
                if operands[2].startswith("R"):
                    ope2 = int(operands[2][1:])
                    binary_instruction &= ~(1 << 24)  # Set immediate flag to 0
                    binary_instruction |= (ope2 << 12)
                else:
                    ope2 = int(operands[2])
                    binary_instruction |= (1 << 24)  # Set immediate flag to 1
                if binary_instruction & (1 << 24):  # Only set immediate value when immediate flag is 1
                    binary_instruction |= (ope2 & 0xFF)  # Set immediate value
        elif opcode == "MOV":
            if operands[0].startswith("R"):
                dest = int(operands[0][1:])
            else:
                dest = int(operands[0])
            binary_instruction |= (dest << 8)

            if operands[1].startswith("R"):
                immediate_value = int(operands[1][1:])
                binary_instruction &= ~(1 << 24)  # Set immediate flag to 0
            else:
                immediate_value = int(operands[1])
                binary_instruction |= (1 << 24)  # Set immediate flag to 1
            binary_instruction |= (immediate_value << 12)
            if binary_instruction & (1 << 24):  # Only set immediate value when immediate flag is 1
                binary_instruction |= (immediate_value & 0xFF)  # Set immediate value
 
    # print("Bytes in binary_instruction:")
    # for i in range(4):
    #     byte = (binary_instruction >> (8 * (3 - i))) & 0xFF
    #     print(f"Byte {i + 1}: {byte:08b} ({byte:#04x})")

    return binary_instruction

def compile_assembly(assembly, opcode_dict):
    binary_instructions = []
    for instruction in assembly:
        binary_instructions.append(assemble_instruction(instruction, opcode_dict))
    return binary_instructions

def main():
    bcc_dict = {
        "B": 8,
        "BEQ": 9,
        "BNE": 10,
        "BLE": 11,
        "BGE": 12,
        "BL": 13,
        "BG": 14
    }
    
    opcode_dict = {
        "AND": 0,
        "ORR": 1,
        "EOR": 2,
        "ADD": 3,
        "ADC": 4,
        "CMP": 5,
        "SUB": 6,
        "SBC": 7,
        "MOV": 8,
        "LSH": 9,
        "RSH": 10,
    }
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    with open(input_file, 'r') as f:
        assembly = f.readlines()
    
    binary_instructions = compile_assembly(assembly, opcode_dict)
    
    with open(output_file, 'wb') as f:
        for instr in binary_instructions:
            f.write(instr.to_bytes(4, byteorder='big'))

if __name__ == "__main__":
    main()
