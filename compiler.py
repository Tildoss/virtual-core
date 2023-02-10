import sys
import struct

def compile_instruction(instruction, file):
    opcodes = {
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
        "RSH": 10
    }
    
    fields = instruction.split()
    BCC = 0 
    immediate_value_flag = 0
    opcode = opcodes[fields[0]]
    src1 = int(fields[1][1:].split(',')[0])
    src2 = int(fields[2][1:].split(',')[0])
    dst = int(fields[3][1:].split(',')[0])
    
    binary = (BCC << 28) + (0 << 25) + (immediate_value_flag << 24) + (opcode << 20) + (src1 << 16) + (src2 << 12) + (dst << 8)
    file.write(struct.pack("!I", binary))

if __name__ == "__main__":

    input_file_name = sys.argv[1]
    output_file_name = sys.argv[1].replace(".s", ".bin")

    with open(input_file_name, "r") as input_file:
        with open(output_file_name, "wb") as output_file:
            for line in input_file:
                instruction = line.strip()
                compile_instruction(instruction, output_file)