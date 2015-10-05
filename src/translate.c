#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and blt pseudoinstructions. Your pseudoinstruction
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into
        a lui-ori pair.
  
   li $t1, -100   Set $t1 to 16-bit immediate (sign-extended)
   li $t1, 100    Set $t1 to 16-bit immediate (zero-extended)
   li $t1, 100000 Set $t1 to 32-bit immediate

   For move, blt, bgt, traddu, swpr:
    - your expansion should use the fewest number of instructions possible.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).

   int a = 2147483647;
   unsigned int b = 2147483648;
   printf("%d\n", a);
   printf("%ld\n", b);
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {
        if(num_args != 2) {
          return 0;
        }

        long int immediate;
        int result = translate_num(&immediate, args[1], -2147483648, 4294967295);
        if (result == -1)
          return 0;
        if ((immediate <= 65535 && immediate >= 0) ||
          (immediate >= -32768 && immediate <= 32767)) {
          char* instr = "addiu";
          char* zero_reg = "$zero";
          fprintf(output, "%s %s %s %ld\n", instr, args[0], zero_reg, immediate);
        } else {
          char* loadUpper = "lui";
          char* loadLower = "ori";
          long int topBits = immediate >> 16;
          fprintf(output, "%s %s %ld\n",loadUpper, args[0], topBits);
          //long int lowBits = (immediate << 16) >> 16;
          long int lowBits = immediate & 0xffff;

          fprintf(output, "%s %s %s %ld\n", loadLower, args[0], args[0], lowBits);
        }
        return 2;
    } else if (strcmp(name, "move") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 2) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(3 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen(args[0])+1);
        new_arg[1] = (char *)malloc(strlen(args[1])+1);
        new_arg[2] = (char *)malloc(strlen("$zero")+1);
        strcpy(new_arg[0], args[0]);
        strcpy(new_arg[1], args[1]);
        strcpy(new_arg[2], "$zero");
        write_inst_string(output, "addu", new_arg, 3);
        for (int i=0; i<3; i++) {
            free(new_arg[i]);
        }
        free(new_arg);
        return 1;
    } else if (strcmp(name, "blt") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(3 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen("$at")+1);
        new_arg[1] = (char *)malloc(strlen(args[0]+1));
        new_arg[2] = (char *)malloc(strlen(args[1]+2));
        strcpy(new_arg[0], "$at");
        strcpy(new_arg[1], args[0]);
        strcpy(new_arg[2], args[1]);
        write_inst_string(output, "slt", new_arg, 3);
        
        char **new_args;
        new_args = (char **)malloc(3 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen("$at")+1);
        new_args[1] = (char *)malloc(strlen("$zero")+1);
        new_args[2] = (char *)malloc(strlen(args[2])+1);
        strcpy(new_args[0], "$at");
        strcpy(new_args[1], "$zero");
        strcpy(new_args[2], args[2]);
        write_inst_string(output, "bne", new_args, 3);
        
        for (int i=0; i<3; i++) {
            free(new_arg[i]);
            free(new_args[i]);
        }
        free(new_arg);
        free(new_args);
        return 2;
    } else if (strcmp(name, "bgt") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(3 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen("$at")+1);
        new_arg[1] = (char *)malloc(strlen(args[0]+1));
        new_arg[2] = (char *)malloc(strlen(args[1]+2));
        strcpy(new_arg[0], "$at");
        strcpy(new_arg[1], args[0]);
        strcpy(new_arg[2], args[1]);
        write_inst_string(output, "slt", new_arg, 3);
        char **new_args;
        new_args = (char **)malloc(3 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen("$at")+1);
        new_args[1] = (char *)malloc(strlen("$zero")+1);
        new_args[2] = (char *)malloc(strlen(args[2])+1);
        strcpy(new_args[0], "$at");
        strcpy(new_args[1], "$zero");
        strcpy(new_args[2], args[2]);
        write_inst_string(output, "beq", new_args, 3);
        for (int i=0; i<3; i++) {
            free(new_arg[i]);
            free(new_args[i]);
        }
        free(new_arg);
        free(new_args);
        return 2;
    } else if (strcmp(name, "traddu") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(3 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen("$at")+1);
        new_arg[1] = (char *)malloc(strlen(args[1]+1));
        new_arg[2] = (char *)malloc(strlen(args[2]+1));
        strcpy(new_arg[0], "$at");
        strcpy(new_arg[1], args[1]);
        strcpy(new_arg[2], args[2]);
        write_inst_string(output, "addu", new_arg, 3);
        char **new_args;
        new_args = (char **)malloc(3 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen(args[0])+1);
        new_args[1] = (char *)malloc(strlen("$at")+1);
        new_args[2] = (char *)malloc(strlen("$zero")+1);
        strcpy(new_args[0], args[0]);
        strcpy(new_args[1], "$at");
        strcpy(new_args[2], "$zero");
        write_inst_string(output, "addu", new_args, 3);
        for (int i=0; i<3; i++) {
            free(new_arg[i]);
            free(new_args[i]);
        }
        free(new_arg);
        free(new_args);
        return 2;
    } else if (strcmp(name, "swpr") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 2) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(3 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen("$at")+1);
        new_arg[1] = (char *)malloc(strlen(args[1])+1);
        new_arg[2] = (char *)malloc(strlen("$zero")+1);
        strcpy(new_arg[0], "$at");
        strcpy(new_arg[1], args[1]);
        strcpy(new_arg[2], "$zero");
        write_inst_string(output, "addu", new_arg, 3);
        char **new_args;
        new_args = (char **)malloc(3 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen(args[1])+1);
        new_args[1] = (char *)malloc(strlen(args[0])+1);
        new_args[2] = (char *)malloc(strlen("$zero")+1);
        strcpy(new_args[0], args[1]);
        strcpy(new_args[1], args[0]);
        strcpy(new_args[2], "$zero");
        write_inst_string(output, "addu", new_args, 3);
        char **new_args2;
        new_args2 = (char **)malloc(3 * sizeof(char *));
        new_args2[0] = (char *)malloc(strlen(args[0])+1);
        new_args2[1] = (char *)malloc(strlen("$at")+1);
        new_args2[2] = (char *)malloc(strlen("$zero")+1);
        strcpy(new_args2[0], args[0]);
        strcpy(new_args2[1], "$at");
        strcpy(new_args2[2], "$zero");
        write_inst_string(output, "addu", new_args2, 3);
        for (int i=0; i<3; i++) {
            free(new_arg[i]);
            free(new_args[i]);
            free(new_args2[i]);
        }
        free(new_arg);
        free(new_args);
        free(new_args2);
        return 3;
    } else if (strcmp(name, "mul") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(2 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen(args[1])+1);
        new_arg[1] = (char *)malloc(strlen(args[2])+1);
        strcpy(new_arg[0], args[1]);
        strcpy(new_arg[1], args[2]);
        write_inst_string(output, "mult", new_arg, 2);
        for (int i=0; i<2; i++) {
            free(new_arg[i]);
        }
        free(new_arg);
        char **new_args;
        new_args = (char **)malloc(1 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen(args[0])+1);
        strcpy(new_args[0], args[0]);
        write_inst_string(output, "mflo", new_args, 1);
        for (int i=0; i<1; i++) {
            free(new_args[i]);
        }
        free(new_args);
        return 2;
    } else if (strcmp(name, "div") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(2 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen(args[1])+1);
        new_arg[1] = (char *)malloc(strlen(args[2])+1);
        strcpy(new_arg[0], args[1]);
        strcpy(new_arg[1], args[2]);
        write_inst_string(output, "div", new_arg, 2);
        for (int i=0; i<2; i++) {
            free(new_arg[i]);
        }
        free(new_arg);
        char **new_args;
        new_args = (char **)malloc(1 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen(args[0])+1);
        strcpy(new_args[0], args[0]);
        write_inst_string(output, "mflo", new_args, 1);
        for (int i=0; i<1; i++) {
            free(new_args[i]);
        }
        free(new_args);
        return 2;
    } else if (strcmp(name, "rem") == 0) {
        /* YOUR CODE HERE */
        if(num_args != 3) {
          return 0;
        }
        char **new_arg;
        new_arg = (char **)malloc(2 * sizeof(char *));
        new_arg[0] = (char *)malloc(strlen(args[1])+1);
        new_arg[1] = (char *)malloc(strlen(args[2])+1);
        strcpy(new_arg[0], args[1]);
        strcpy(new_arg[1], args[2]);
        write_inst_string(output, "div", new_arg, 2);
        for (int i=0; i<2; i++) {
            free(new_arg[i]);
        }
        free(new_arg);
        char **new_args;
        new_args = (char **)malloc(1 * sizeof(char *));
        new_args[0] = (char *)malloc(strlen(args[0])+1);
        strcpy(new_args[0], args[0]);
        write_inst_string(output, "mfhi", new_args, 1);
        for (int i=0; i<1; i++) {
            free(new_args[i]);
        }
        free(new_args);
        return 2;
    }
    write_inst_string(output, name, args, num_args);
    return 1;

}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error.
*/

int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args);
    else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args);
    else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args);
    else if (strcmp(name, "mult") == 0)  return write_rtype (0x18, output, args, num_args);
    else if (strcmp(name, "div") == 0)  return write_rtype (0x1a, output, args, num_args);
    else if (strcmp(name, "mfhi") == 0)  return write_rtype (0x10, output, args, num_args);
    else if (strcmp(name, "mflo") == 0)  return write_rtype (0x12, output, args, num_args);
    else if (strcmp(name, "jr") == 0)    return write_jr (0x08, output, args, num_args);
    else if (strcmp(name, "sll") == 0)   return write_shift (0x00, output, args, num_args);
    else if (strcmp(name, "addiu") == 0)  return write_addiu (0x09, output, args, num_args);
    else if (strcmp(name, "ori") == 0)  return write_ori (0x0d, output, args, num_args);
    else if (strcmp(name, "lui") == 0)  return write_lui (0x0f, output, args, num_args);
    else if (strcmp(name, "lb") == 0)  return write_mem (0x20, output, args, num_args);
    else if (strcmp(name, "lbu") == 0)  return write_mem (0x24, output, args, num_args);
    else if (strcmp(name, "lw") == 0)  return write_mem (0x23, output, args, num_args);
    else if (strcmp(name, "sb") == 0)  return write_mem (0x28, output, args, num_args);
    else if (strcmp(name, "sw") == 0)  return write_mem (0x2b, output, args, num_args);
    else if (strcmp(name, "beq") == 0)  return write_branch (0x04, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bne") == 0)  return write_branch (0x05, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "j") == 0)  return write_jump (0x02, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "jal") == 0)  return write_jump (0x03, output, args, num_args, addr, symtbl);
    else                                 return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
*/
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    /* TODO - Error checking */
    if (num_args > 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    int rd, rs, rt;
    uint32_t inst = 0;
    if (num_args == 3) {
        rd = translate_reg(args[0]);
        rs = translate_reg(args[1]);
        rt = translate_reg(args[2]);
        inst = (rs << 21) | (rt << 16) | (rd << 11) | (funct << 0);
    } else if (num_args == 2) {
        rs = translate_reg(args[0]);
        rt = translate_reg(args[1]);
        inst = (rs << 21) | (rt << 16) | (funct << 0);
    } else if (num_args == 1) {
        rd = translate_reg(args[0]);
        inst = (rd << 11) | (funct << 0);
    }

    write_inst_hex(output, instruction);
    return 0;
}

/* A helper function for writing shift instructions. You should use
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
*/
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
    if (num_args != 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    long int shamt;
    int rd = translate_reg(args[0]), rt = translate_reg(args[1]), er = translate_num(&shamt, args[2], 0, 31);
    if (er == -1) {
        return -1;
    }
    uint32_t inst = 0;
    instruction = (rt << 16) | (rd << 11) | (shamt << 6) | (funct << 0);
    write_inst_hex(output, inst);
    return 0;
}
int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
    if (num_args != 1) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    int rs = translate_reg(args[0]);
    uint32_t inst = 0;
    inst = (rs << 21) | (funct << 0);
    write_inst_hex(output, inst);
    return 0;
}

int write_addiu11(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (!args || num_args != 3)  return -1;
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if (rt == -1) return -1;
    if (rs == -1) return -1;
    if (err == -1) return -1;
    uint32_t inst = 0;
    inst += ((int)opcode << 26);
    inst += (rs << 21);
    inst += (rt << 16);
    imm = imm & 0x0000ffff;
    inst += imm;
    write_inst_hex(output, inst);
    return 0;
}
int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    if (num_args != 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if (err == -1) {
        int err = translate_num(&imm, args[2], 0, INT32_MAX);
        if (err == -1) {
            fprintf(output, "%s\n", "Error in strtol");
            return -1;
        }
    }
    uint32_t instruction = 0;
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (imm << 0);
    write_inst_hex(output, instruction);
    return 0;
}
int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    if (num_args != 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, UINT16_MAX);
    if (err == -1) {
        int err = translate_num(&imm, args[2], 0, INT32_MAX);
        if (err == -1) {
            fprintf(output, "%s\n", "Error in strtol");
            return -1;
        }
    }
    uint32_t instruction = 0;
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (imm << 0);
    write_inst_hex(output, instruction);
    return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    if (num_args != 2) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);
    if (err == -1) {
        fprintf(output, "%s\n", "Error in strtol");
        return -1;
    }
    uint32_t inst = 0;
    inst = (opcode << 26) | (rt << 16) | (imm << 0);
    write_inst_hex(output, inst);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    if (num_args != 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int er = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    if (er == -1) {
    }
    uint32_t inst = 0;
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (imm << 0);
    write_inst_hex(output, inst);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
*/
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}

int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    if (num_args != 3) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int label_addr = get_addr_for_symbol(symtbl, args[2]);
    uint32_t inst = 0;
    if (can_branch_to(addr, label_addr)) {
        inst = (opcode << 26) | (rs << 21) | (rt << 16) | (label_addr << 0);
        write_inst_hex(output, inst);
        return 0;
    } else {
        return -1;
    }
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    if (num_args != 1) {
        fprintf(output, "%s\n", "Invalid number of arguments");
        return -1;
    }
    uint32_t label_addr = get_addr_for_symbol(symtbl, args[0]);
    if (can_branch_to(addr, label_addr)) {
        uint32_t inst = 0;
        inst = (opcode << 26) | (label_addr << 0);
        write_inst_hex(output, inst);
        return 0;
    } else {
        return -1;
    }
}
  