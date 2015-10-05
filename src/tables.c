
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

const int SYMTBL_NON_UNIQUE = 0;
const int SYMTBL_UNIQUE_NAME = 1;

#define INITIAL_SIZE 5
#define SCALING_FACTOR 2

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
    write_to_log("Error: allocation failed\n");
    exit(1);
}

void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char* name) {
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_symbol(FILE* output, uint32_t addr, const char* name) {
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containing 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMTBL_NON_UNIQUE or SYMTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
    /* YOUR CODE HERE */
		SymbolTable* myTable = (SymbolTable*)malloc(sizeof(SymbolTable));
		if (!myTable) {
			allocation_failed();
		}
		myTable -> len = 0;
    myTable -> mode = mode;
		myTable -> cap = 5;
		myTable -> tbl = malloc(100 * sizeof(Symbol));
		if(!(myTable ->  tbl)) {
			allocation_failed();
		}
    return myTable;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
    /* YOUR CODE HERE */
		int table_size = table -> len;
		for (int i = 0; i < table_size; i++){
			free((table -> tbl)[i].name);
	  }
		free(table -> tbl);
		free(table);

}

/* A suggested helper function for copying the contents of a string. */
static char* create_copy_of_str(const char* str) {
    size_t len = strlen(str) + 1;
    char *buf = (char *) malloc(len);
    if (!buf) {
       allocation_failed();
    }
    strncpy(buf, str, len); 
    return buf;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   ADDR is given as the byte offset from the first instruction. The SymbolTable
   must be able to resize itself as more elements are added. 

   Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
   return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. If memory
   allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
    /* YOUR CODE HERE */
	if (addr%4 != 0) {
		addr_alignment_incorrect();
	return -1;
	}
	char new_name[strlen(name) + 1];
	strcpy(new_name, name);
	if((table -> mode) == SYMTBL_UNIQUE_NAME) {
	  int tbl_size = table -> len;
	  for (int i = 0; i < tbl_size; i++) {
			if (strcmp(new_name, ((table -> tbl)[i].name) == 0)) {
				name_already_exists(name);
				return -1;
		  }
    }
	}
  if (table->len == table-> cap) {
	  table->cap = (table->cap) * 5;
	  table->tbl = realloc(table->tbl, (table->cap) * sizeof(Symbol));
	  uint32_t size_need = strlen(name) +1;
    table->tbl[(table->len)].name = malloc(size_need * sizeof(char));
		strcpy(table->tbl[(table->len)].name, new_name);
		table -> tbl[(table->len)].addr = addr;
		table -> len = table->len + 1;
  } else {
	  uint32_t size_need = strlen(new_name) +1;
	  table->tbl[(table->len)].name = malloc(size_need * sizeof(char)); 
	  strcpy(table->tbl[(table->len)].name, new_name);
	  table->tbl[(table->len)].addr = addr;
	  table->len = table->len + 1;
  }
  return 0;
}
/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {
    /* YOUR CODE HERE */ 
		for (int i = 0; i < table -> len; i++) {
			if(strcmp(name, table->tbl[i].name) == 0)  {
				return table -> tbl[i].addr;
		  }
		}
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_symbol() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
    /* YOUR CODE HERE */
		for (int i = 0; i < table -> len; i++) {
			Symbol* pt_table = table -> tbl;
			Symbol current_symbol = pt_table[i];
			char* current_name = current_symbol.name;
			int64_t curr_addr = current_symbol.addr;
			write_symbol(output, curr_addr, current_name);
    }
}
