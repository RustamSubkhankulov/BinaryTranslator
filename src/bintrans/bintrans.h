#pragma once 

#include "bintrans_conf.h"
#include "../logs/errors_and_logs.h"

//===============================================

struct Cmndline
{
    char input;
    const char* input_name;

    char opt;
};

//===============================================

#ifdef BINTRANS_LOGS

    #define bintrans_log_report() \
            log_report()

#else 

    #define bintrans_log_report() 

#endif

//===============================================

#ifdef TRANS_STRUCT_VALID_CHECK

    #define TRANS_STRUCT_VALID(trans_struct)            \
                                                        \
    do                                                  \
    {                                                   \
                                                        \
        if (trans_struct_validator(trans_struct) == -1) \
            return -1;                                  \
                                                        \
    } while(0);

#else

    #define TRANS_STRUCT_VALID(trans_struct)

#endif 

//===============================================

#ifdef ENTITY_ADD_NAME_STR

    #define FOR_DUMP(...) __VA_ARGS__ 

#else

    #define FOR_DUMP(...)

#endif 

//===============================================

#define LAST_ENTITY trans_struct->entities.data[trans_struct->entities.num - 1] 

//===============================================

#ifndef REMOVE_NOPS

    #define INSERT_NOPS(trans_struct) INIT_ENTITY(trans_struct, Two_nops)

#else

    #define INSERT_NOPS(trans_struct) 

#endif 

//===============================================

struct Binary_input 
{

    FILE* file_ptr;
    unsigned int size;

    char* buffer;
};

//===============================================

struct Trans_entity
{
    unsigned char* data;
    unsigned int   size;

    int type;

    #ifdef ENTITY_ADD_NAME_STR

        char* name_str;

    #endif
};


//===============================================

struct Input
{
    unsigned char* buffer;
    unsigned int   pos;
    unsigned int   size;
};

//===============================================

struct Result
{
    unsigned char* buffer;
    unsigned int   size;
    uint64_t       buffer_addr;

    unsigned int   cur_pos;
};

//===============================================

enum Patch_types
{
    RAM_START_ADDR = 228,
    STD_FUNC       = 230,
};

//-----------------------------------------------

struct Patch_instr
{
    int patch_type;

    unsigned int res_buf_pos;

    Trans_entity* entity;
    int std_func_code;
};

//-----------------------------------------------

struct Patch
{
    unsigned int num;
    unsigned int cap;

    Patch_instr* instructions;

    unsigned char ram_is_used;
    float*        ram_buffer;
};

//===============================================

struct Jumps
{
    Trans_entity** entities;
    unsigned int* jmp_pos;

    unsigned int num;
    unsigned int cap;

    unsigned int cur_index;
};

//===============================================

struct Pos
{
    unsigned int* inp_pos;
    unsigned int* res_pos;

    unsigned int num;
    unsigned int cap;
};

//===============================================

struct Entities
{
    Trans_entity** data;

    unsigned int num;
    unsigned int cap;
};

//===============================================

struct Trans_struct
{
    struct Entities entities;

    struct Input  input;
    struct Result result;

    struct Patch patch;
    struct Jumps jumps;

    struct Pos pos;

    float* ram_buffer;

    #ifdef BINTRANS_LISTING

        FILE* listing;

    #endif 
};

//===============================================

struct Register_info
{
    unsigned int register_num;
    int          register_val;
};

//===============================================

int _read_cmndline(Cmndline* cmndline, int argc, const char** argv 
                                                 FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _entities_struct_ctor(Entities* entities FOR_LOGS(, LOG_PARAMS));

int _entities_struct_dtor(Entities* entities FOR_LOGS(, LOG_PARAMS));

int _entities_struct_increase(Entities* entities FOR_LOGS(, LOG_PARAMS));

int _add_entity(Trans_entity* trans_entity, Entities* entities
                                            FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS));

int _free_binary_input     (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

int _fill_input_buffer     (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

int _input_load_to_buffer  (Binary_input* binary_input FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _trans_struct_ctor     (Trans_struct* trans_struct, Binary_input* binary_input 
                                                           FOR_LOGS(, LOG_PARAMS));

int _binary_header_check   (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_translate      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _binary_execute        (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _trans_struct_dtor     (Trans_struct* trans_strcut FOR_LOGS(, LOG_PARAMS));

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _flush_entities_to_buf (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_translated_code  (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_prepare      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_allocate     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _translate_single_instruction(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _call_buf_change_acc_prot(Trans_struct* trans_struct, int prot FOR_LOGS(, LOG_PARAMS));

int _init_entity(Trans_struct* trans_struct, unsigned int size, const unsigned char* data, int type
                                      FOR_DUMP(, const char* name_str) FOR_LOGS(, LOG_PARAMS));
 
int _patch_entity(Trans_entity* trans_entity, unsigned int   patch_pos, unsigned int  patch_size,
                                              unsigned char* patch_data FOR_LOGS(, LOG_PARAMS));

int _ram_buffer_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _jumps_struct_ctor  (Jumps* jumps FOR_LOGS(, LOG_PARAMS));

int _jumps_struct_dtor  (Jumps* jumps FOR_LOGS(, LOG_PARAMS));

int _jumps_struct_resize(Jumps* jumps FOR_LOGS(, LOG_PARAMS));

int _add_jump_entity(Jumps* jumps, Trans_entity* trans_entity, 
                                   unsigned int jmp_pos, 
                                   unsigned int inp_dst FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _pos_struct_ctor   (Pos* pos FOR_LOGS(, LOG_PARAMS));

int _pos_struct_dtor   (Pos* pos FOR_LOGS(, LOG_PARAMS));

int _pos_struct_resize (Pos* pos FOR_LOGS(, LOG_PARAMS));

int _add_position_accordance(Pos* pos, unsigned int inp_pos,
                                       unsigned int res_pos  FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------


#ifdef BINTRANS_LISTING

    int _init_listing_file     (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

    int _end_listing_file      (Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

    int _write_listing(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

    int _listing_message(Trans_entity* trans_entity, unsigned int res_buf_pos, 
                                        FILE* listing FOR_LOGS(, LOG_PARAMS));

#endif 

//===============================================

#define read_cmndline(cmndline, argc, argv) \
       _read_cmndline(cmndline, argc, argv FOR_LOGS(, LOG_ARGS))

//===============================================

#define entities_struct_ctor(entities) \
       _entities_struct_ctor(entities FOR_LOGS(, LOG_ARGS))

#define entities_struct_dtor(entities) \
       _entities_struct_dtor(entities FOR_LOGS(, LOG_ARGS))

#define entities_struct_increase(entities) \
       _entities_struct_increase(entities FOR_LOGS(, LOG_ARGS))

#define add_entity(entity, entities) \
       _add_entity(entity, entities FOR_LOGS(, LOG_ARGS))

//===============================================

#define pos_struct_ctor(pos) \
       _pos_struct_ctor(pos FOR_LOGS(, LOG_ARGS))

#define pos_struct_dtor(pos) \
       _pos_struct_dtor(pos FOR_LOGS(, LOG_ARGS))

#define pos_struct_resize(pos) \
       _pos_struct_resize(pos FOR_LOGS(, LOG_ARGS))

#define add_position_accordance(pos, inp_pos, res_pos) \
       _add_position_accordance(pos, inp_pos, res_pos FOR_LOGS(, LOG_ARGS))

//===============================================

#define add_jump_entity(jumps, entity, res_pos, inp_dst) \
       _add_jump_entity(jumps, entity, res_pos, inp_dst FOR_LOGS(, LOG_ARGS))

#define jumps_struct_ctor(jumps) \
       _jumps_struct_ctor(jumps FOR_LOGS(, LOG_ARGS))

#define jumps_struct_dtor(jumps) \
       _jumps_struct_dtor(jumps FOR_LOGS(, LOG_ARGS))

#define jumps_struct_resize(jumps) \
       _jumps_struct_resize(jumps FOR_LOGS(, LOG_ARGS))

#define ram_buffer_allocate(trans_struct) \
       _ram_buffer_allocate(trans_struct FOR_LOGS(, LOG_ARGS))

#define increase_entities_array(trans_struct) \
       _increase_entities_array(trans_struct FOR_LOGS(, LOG_ARGS))

#define translate_single_instruction(trans_struct) \
       _translate_single_instruction(trans_struct FOR_LOGS(, LOG_ARGS))

#define translate_instructions(trans_struct) \
       _translate_instructions(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_translated_code(trans_struct) \
       _call_translated_code(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_buf_allocate(trans_struct) \
       _call_buf_allocate(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_buf_prepare(trans_struct) \
       _call_buf_prepare(trans_struct FOR_LOGS(, LOG_ARGS))

#define call_buf_change_acc_prot(trans_struct, prot) \
       _call_buf_change_acc_prot(trans_struct, prot FOR_LOGS(, LOG_ARGS))

#define init_listing_file(trans_struct) \
       _init_listing_file(trans_struct FOR_LOGS(, LOG_ARGS))

#define end_listing_file(trans_struct) \
       _end_listing_file(trans_struct FOR_LOGS(, LOG_ARGS))

#define write_listing(trans_struct) \
       _write_listing(trans_struct FOR_LOGS(, LOG_ARGS))

#define listing_message(trans_entity, res_buf_pos, listing) \
       _listing_message(trans_entity, res_buf_pos, listing FOR_LOGS(, LOG_ARGS))

#define binary_header_check(trans_struct) \
       _binary_header_check(trans_struct FOR_LOGS(, LOG_ARGS))

#define read_binary_input(binary_input, filename) \
       _read_binary_input(binary_input, filename FOR_LOGS(, LOG_ARGS)) 

#define free_binary_input(binary_input) \
       _free_binary_input(binary_input FOR_LOGS(, LOG_ARGS))

#define input_load_to_buffer(binary_input) \
       _input_load_to_buffer(binary_input FOR_LOGS(, LOG_ARGS))

#define fill_input_buffer(binary_input) \
       _fill_input_buffer(binary_input FOR_LOGS(, LOG_ARGS))

#define trans_struct_ctor(trans_struct, binary_input) \
       _trans_struct_ctor(trans_struct, binary_input FOR_LOGS(, LOG_ARGS))

#define trans_struct_dtor(trans_struct) \
       _trans_struct_dtor(trans_struct FOR_LOGS(, LOG_ARGS))

#define trans_struct_validator(trans_struct) \
       _trans_struct_validator(trans_struct FOR_LOGS(, LOG_ARGS))

#define binary_translate(trans_struct) \
       _binary_translate(trans_struct FOR_LOGS(, LOG_ARGS))

#define binary_execute(trans_struct) \
       _binary_execute(trans_struct FOR_LOGS(, LOG_ARGS))

#define flush_entities_to_buf(trans_struct) \
       _flush_entities_to_buf(trans_struct FOR_LOGS(, LOG_ARGS))

#define patch_entity(entity, pos, size, data) \
       _patch_entity(entity, pos, size, data FOR_LOGS(, LOG_ARGS))

#ifdef ENTITY_ADD_NAME_STR

    #define init_entity(trans_struct, size, data, type, name_str) \
           _init_entity(trans_struct, size, data, type, name_str FOR_LOGS(, LOG_ARGS))

#else

    #define init_entity(trans_struct, size, data, type) \
           _init_entity(trans_struct, size, data, type FOR_LOGS(, LOG_ARGS))

#endif 