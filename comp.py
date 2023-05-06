import sys

def assemble_instruction(instruction, opcode_dict):
    tokens = instruction.split()
    opcode = tokens[0]
    operands = [x.strip() for x in ''.join(tokens[1:]).split(',')]
    
    binary_instruction = 0
    
    if opcode in opcode_dict:
        binary_instruction |= (opcode_dict[opcode] << 20)
        
        if len(operands) == 3:
            first_operand = int(operands[0][1:])
            second_operand = int(operands[1][1:])
            destination = int(operands[2][1:])
            
            binary_instruction |= (first_operand << 16)
            binary_instruction |= (second_operand << 12)
            binary_instruction |= (destination << 8)
        elif len(operands) == 2 and opcode == "MOV":
            first_operand = int(operands[0][1:])
            immediate_value = int(operands[1])
            
            binary_instruction |= (first_operand << 16)
            binary_instruction |= (1 << 24)
            binary_instruction |= (immediate_value & 0xFF)
            
    return binary_instruction

def compile_assembly(assembly, opcode_dict):
    binary_instructions = []
    for instruction in assembly:
        binary_instructions.append(assemble_instruction(instruction, opcode_dict))
    return binary_instructions

def main():
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
