#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

//===============================================

#include "bintrans.h"
#include "trans.h"
#include "patch.h"
#include "instr.h"
#include "../general/general.h"
#include "../../lang/proc/assembler/processor_general.h"
#include "standard.h"

//===============================================

int _read_cmndline(Cmndline* cmndline, int argc, const char** argv FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(cmndline);
    assert(argv);

    int arg_ct = 1;

    while(arg_ct < argc)
    {
        if (strcmp(argv[arg_ct], "-i") == 0)
        {
            cmndline->input = 1;
            cmndline->input_name = argv[arg_ct + 1];

            arg_ct += 2;
            continue;
        }

        if (strcmp(argv[arg_ct], "-opt") == 0)
        {
            cmndline->opt = 1;

            arg_ct += 1;
            continue;
        }
    } 

    return 0;
}

//===============================================

int _entities_struct_ctor(Entities* entities FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(entities);

    entities->cap = Entities_initial_cap;
    entities->num = 0;

    Trans_entity** data = (Trans_entity**) calloc(Entities_initial_cap, 
                                                  sizeof(Trans_entity*));
    if (!data) 
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    entities->data = data;

    return 0;
}

//-----------------------------------------------

int _entities_struct_dtor(Entities* entities FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(entities);

    free(entities->data);

    return 0;
}

//-----------------------------------------------

int _entities_struct_increase(Entities* entities FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(entities);

    unsigned int prev_cap = entities->cap;
    unsigned int incr_cap = entities->cap * 2;

    Trans_entity** prev_data = entities->data;
    Trans_entity** incr_data = (Trans_entity**) my_recalloc((void*)  prev_data,
                                                            (size_t) incr_cap,
                                                            (size_t) prev_cap,
                                                             sizeof( Trans_entity*));
    if (!incr_data) return -1;

    entities->data = incr_data;
    entities->cap  = incr_cap;

    return 0;
}

//-----------------------------------------------

int _add_entity(Trans_entity* trans_entity, Entities* entities
                                            FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_entity);
    assert(entities);

    unsigned int num = entities->num;

    if (num == entities->cap)
    {
        int ret_val = entities_struct_increase(entities);
        if (ret_val == -1) return -1;
    } 

    entities->data[num] = trans_entity;
    entities->num += 1;

    return 0;
}

//===============================================

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(binary_input);
    
    if (input_filename == NULL)
    {
        error_report(INPUT_INV_NAME);
        return -1;
    }

    FILE* input_file_ptr = open_file(input_filename, "rb");
    if (input_file_ptr == NULL)
        return -1;

    binary_input->file_ptr = input_file_ptr;

    int ret_val = input_load_to_buffer(binary_input);
    if (ret_val == -1) return -1;

    ret_val = close_file(input_file_ptr);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _free_binary_input(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    free(binary_input->buffer);

    return 0;
}

//-----------------------------------------------

int _input_load_to_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    long size = get_file_size(binary_input->file_ptr);
    if (size == -1) return -1;

    binary_input->size = (unsigned int)size;

    int ret_val = fill_input_buffer(binary_input);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _fill_input_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    char* buffer = (char*) calloc(binary_input->size, sizeof(char));
    if (!buffer)
    {
		error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    unsigned int ret_val = (unsigned int) fread(buffer, sizeof(char), binary_input->size, 
                                                                 binary_input->file_ptr);
    if (ret_val != binary_input->size)
    {
        error_report(FREAD_ERR);
        return -1;
    }

    binary_input->buffer = buffer;
    return 0;
}

//-----------------------------------------------

int _binary_translate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    TRANS_STRUCT_VALID(trans_struct);

    int is_ok = binary_header_check(trans_struct);
    if (is_ok == -1) return -1;

    trans_struct->input.pos += (unsigned int)sizeof(Header);

    INIT_ENTITY(trans_struct, Save_regs);
    INIT_ENTITY(trans_struct, Null_xmms);

    int ret_val = translate_instructions(trans_struct);
    if (ret_val == -1) return -1;

    #ifdef BINTRANS_LISTING

        ret_val = write_listing(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

int _translate_instructions(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int ret_val = 0;

    while (trans_struct->input.pos < trans_struct->input.size)
    {
        ret_val = add_position_accordance(&trans_struct->pos, 
                                           trans_struct->input.pos,
                                           trans_struct->result.cur_pos);
        if (ret_val == -1) return -1;

        ret_val = translate_single_instruction(trans_struct);
        if (ret_val == -1) return -1;

        TRANS_STRUCT_VALID(trans_struct);
    }

    ret_val = add_position_accordance(&trans_struct->pos, 
                                       trans_struct->input.pos,
                                       trans_struct->result.cur_pos);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

#define DEF_CMD_(arg_num, name, code, hash, instructions)   \
                                                            \
    case code:                                              \
    {                                                       \
        instructions                                        \
                                                            \
        break;                                              \
    }                                                           

#define DEF_JMP_(arg_num, name, code, hash, instructions)   \
                                                            \
    case code:                                              \
    {                                                       \
        instructions                                        \
                                                            \
        break;                                              \
    }  

//-----------------------------------------------

int _translate_single_instruction(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);  

    unsigned char oper_code = *(trans_struct->input.buffer 
                              + trans_struct->input.pos);

    switch(oper_code & OPER_CODE_MASK)
    {
        #include "../../text_files/commands.txt"
        #include "../../text_files/jumps.txt"

        default:
        {
            error_report(JIT_INV_OP_CODE);
            return -1;
        }
    }

    return 0;   
}

//-----------------------------------------------

#undef DEF_CMD_
#undef DEF_JMP_

//-----------------------------------------------

int _init_entity(Trans_struct* trans_struct, unsigned int size, const unsigned char* data, int type 
                                       FOR_DUMP(, const char* name_str) FOR_LOGS(, LOG_PARAMS))
 {
    bintrans_log_report(); 
    assert(trans_struct);

    Entities* entities = &trans_struct->entities;

    Trans_entity* trans_entity = (Trans_entity*) calloc(1, sizeof(Trans_entity));
    if (!trans_entity)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    trans_entity->type = type;

    trans_entity->data = (unsigned char*) calloc(size, sizeof(unsigned char));
    if (!trans_entity->data)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    int ret_val = fast_cpy((void*) trans_entity->data,
                           (void*) data,
                                   size);

    if (ret_val == -1) return -1;

    #ifdef ENTITY_ADD_NAME_STR

        assert(name_str);

        unsigned int name_len = (unsigned int) strlen(name_str) + 1;

        trans_entity->name_str = (char*) calloc(name_len, sizeof(char));
        if (!trans_entity->name_str)
        {
            error_report(CANNOT_ALLOCATE_MEM);
            return -1;
        }

        ret_val = fast_cpy((void*) trans_entity->name_str,
                           (void*) name_str,
                                   name_len);

        if (ret_val == -1) return -1;

    #endif 

    trans_entity->size            = size;
    trans_struct->result.cur_pos += size;

    ret_val = add_entity(trans_entity, entities);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _binary_header_check(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    Header header = { 0 };
    
    memcpy(&header, trans_struct->input.buffer, sizeof(Header));

    int err_num = 0;

    if (header.signature != SIGN)
    {    
        error_report(HDR_INV_SIGN);
        err_num++;
    }

    if (header.version   != VERSION)
    {
        error_report(HDR_INV_VERSION);
        err_num++;
    }

    if (header.file_size != trans_struct->input.size)
    { 
        error_report(HDR_INV_FILE_SIZE);
        err_num++;
    }

    if (err_num) return -1;

    return 0;
}


//-----------------------------------------------

int _binary_execute(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int ret_val = flush_entities_to_buf(trans_struct);
    if (ret_val == -1) return -1;

    ret_val = call_buf_change_acc_prot(trans_struct, PROT_EXEC | PROT_WRITE);
    if (ret_val == -1) return -1;

    #ifdef FILE_IO

        ret_val = init_file_io(Std_input_filename, 
                               Std_output_filename);
        if (ret_val == -1) return -1;

    #endif 

    ret_val = call_translated_code(trans_struct);
    if (ret_val == -1) return -1;

    #ifdef FILE_IO

        ret_val = close_file_io();
        if (ret_val == -1) return -1;

    #endif 

    ret_val = call_buf_change_acc_prot(trans_struct, PROT_READ | PROT_WRITE);
    if (ret_val == -1) return -1;
    
    free(trans_struct->result.buffer);

    return 0;
}

//-----------------------------------------------

int _call_translated_code(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int (*func) (void) = NULL;
    func = (int (*)(void)) trans_struct->result.buffer;

    #ifdef DEBUG_EXEC

        __asm__("int $3");

    #endif 

    int exit_code = func();
    printf("\n Exit code of translated code: %d \n", exit_code);

    return 0;
}

//-----------------------------------------------

int _call_buf_change_acc_prot(Trans_struct* trans_struct, int prot FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int size = trans_struct->result.size;

    int ret_val = mprotect((void*)trans_struct->result.buffer, size, prot);
    if (ret_val != 0)
    {
        error_report(MPROTECT_ERR);
        return -1;
    }

    return 0;
}

//-----------------------------------------------

int _call_buf_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int pagesize = (int) sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
    {
        error_report(SYSCONF_ERR);
        return -1;
    }

    unsigned int size = trans_struct->result.size;

    trans_struct->result.buffer = (unsigned char*) aligned_alloc ((size_t)pagesize,  
                                                     size * sizeof(unsigned char));
    if (!trans_struct->result.buffer) return -1;

    trans_struct->result.buffer_addr = (uint64_t)trans_struct->result.buffer;

    return 0;
}

//-----------------------------------------------

int _call_buf_prepare(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    trans_struct->result.size = 
    trans_struct->result.cur_pos;
                 
    int ret_val = call_buf_allocate(trans_struct);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _flush_entities_to_buf(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    unsigned int call_buf_pos = 0;

    Entities* entities        = &trans_struct->entities;
    unsigned int cur_index    = 0;
    unsigned int entities_num = entities->num;

    do 
    {
        unsigned int entity_size = entities->data[cur_index]->size;

        int ret_val = fast_cpy((void*)(trans_struct->result.buffer + call_buf_pos),
                               (void*) entities->data[cur_index]->data,
                                       entity_size);

        if (ret_val == -1) return -1;

        call_buf_pos += entity_size;
        cur_index    += 1;

    } while (cur_index != entities_num);

    return 0;
}

//-----------------------------------------------

int _trans_struct_ctor(Trans_struct* trans_struct, Binary_input* binary_input 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(trans_struct);
    assert(binary_input);

    trans_struct->input.buffer = (unsigned char*) binary_input->buffer;
    trans_struct->input.size   =                  binary_input->size; 
    
    trans_struct->input.pos   = 0;
    trans_struct->result.cur_pos = 0;

    int is_constr = entities_struct_ctor(&trans_struct->entities);
    if (is_constr == -1) return -1;

    int ret_val = init_patch_struct(&trans_struct->patch);
    if (ret_val == -1) return -1;

    ret_val = jumps_struct_ctor(&trans_struct->jumps);
    if (ret_val == -1) return -1;

    ret_val = pos_struct_ctor(&trans_struct->pos);
    if (ret_val == -1) return -1;

    #ifdef BINTRANS_LISTING

        ret_val = init_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

#ifdef BINTRANS_LISTING

//-----------------------------------------------

    int _init_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
    {
        bintrans_log_report(); 
        assert(trans_struct);

        FILE* listing = open_file(Listing_filename, "ab");
        if (!listing)
        {
            error_report(LISTING_CLOSE_ERR);
            return -1;
        }

        trans_struct->listing = listing;

        fprintf(listing, "\t Listing Start: ");
        fprintf(listing, "%s \n", get_time_string());

        return 0;
    }

//-----------------------------------------------

    int _end_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
    {
        bintrans_log_report(); 
        assert(trans_struct);

        fprintf(trans_struct->listing, "\t Listing End: ");
        fprintf(trans_struct->listing, "%s \n", get_time_string());

        int ret_val = close_file(trans_struct->listing);
        if (ret_val == -1)
        {
            error_report(LISTING_CLOSE_ERR);
            return -1;
        }

        return 0;
    }

//-----------------------------------------------

    int _write_listing(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
    {
        bintrans_log_report();
        assert(trans_struct);

        unsigned int res_buf_pos = 0;

        Entities* entities = &trans_struct->entities;
        FILE* listing      = trans_struct->listing;
        
        unsigned int cur_index    = 0;
        unsigned int entities_num = entities->num;

        int ret_val   = 0; 

        do
        {
            ret_val = listing_message(entities->data[cur_index], res_buf_pos, listing);
            if (ret_val == -1) return -1;

            res_buf_pos += entities->data[cur_index]->size;
            cur_index   += 1;

        } while (cur_index != entities_num);

        return 0;
    }

//-----------------------------------------------

    int _listing_message(Trans_entity* trans_entity, unsigned int res_buf_pos, FILE* listing FOR_LOGS(, LOG_PARAMS))
    {
        bintrans_log_report();

        assert(trans_entity);
        assert(listing);

        unsigned int size = trans_entity->size;

        fprintf(listing, "Pos: %08xh | Size: %02d | ", res_buf_pos, size);
        
        for (unsigned int counter = 1;
                        counter <= size;
                        counter++)
        {
            fprintf(listing, "%02x ", trans_entity->data[counter - 1]);

            if ((counter  % 8) == 0)
                fprintf(listing, "\n                            ");
        }

        fprintf(listing, "\n\n");

        #ifdef ENTITY_ADD_NAME_STR

            fprintf(listing, "\nEntity name: %s \n\n\n", trans_entity->name_str);

        #endif 

        fprintf(listing, "\\--------------------------------------------------\n");

        return 0;
    }

//-----------------------------------------------

#endif 

//-----------------------------------------------

int _trans_struct_dtor(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    Entities* entities = &trans_struct->entities;

    unsigned int cur_index    = 0;
    unsigned int entities_num = entities->num;

    do
    {
        // free unsigned char* data in Trans_entity
        free(entities->data[cur_index]->data);

        #ifdef ENTITY_ADD_NAME_STR

            // free char* name_str in Trans_entity
            free(entities->data[cur_index]->name_str);

        #endif 

        // free memory, allocated for Trans_entity structure
        free(entities->data[cur_index]);

        cur_index += 1;

    } while (cur_index != entities_num);
    
    if (trans_struct->patch.ram_is_used)
    {
        free(trans_struct->ram_buffer);
    }

    int is_destr = entities_struct_dtor(entities);
    if (is_destr == -1) return -1;

    #ifdef BINTRANS_LISTING

        int ret_val = end_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif   

    return 0;
}

//-----------------------------------------------

int _ram_buffer_allocate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    if (!trans_struct->patch.ram_is_used)
        return 0;

    int pagesize = (int) sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
    {
        error_report(SYSCONF_ERR);
        return -1;
    }

    size_t ram_buffer_size = sizeof(float) * Ram_size; 

    float* ram_buffer = (float*) aligned_alloc((size_t)pagesize,
                                                ram_buffer_size);
    if (!ram_buffer)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    int ret_val = mprotect((void*)ram_buffer, ram_buffer_size, PROT_READ | PROT_WRITE);
    if (ret_val == -1) 
    {
        error_report(MPROTECT_ERR);
        return -1;
    }

    trans_struct->ram_buffer       = ram_buffer;
    trans_struct->patch.ram_buffer = ram_buffer;

    return 0;
}

//-----------------------------------------------

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int err_num = 0;  

    if (!trans_struct->input.buffer)
    {
        error_report(TRANS_STRUCT_NULL_INPUT_BUFFER);
        err_num++;
    }

    if (trans_struct->input.pos > trans_struct->input.size)
    {
        error_report(TRANS_STRUCT_INV_BUFFER_POS);
        err_num++;
    }

    if (!trans_struct->jumps.jmp_pos)
    {
        error_report(NULL_JUMPS_RES_POS);
        err_num++;
    }

    if (!trans_struct->patch.instructions)
    {
        error_report(NULL_PATCH_ARRAY);
        err_num++;
    }

    #ifdef BINTRANS_LISTING

        if (trans_struct->listing == NULL)
        {
            error_report(LISTING_NULL_PTR);
            err_num++;
        }

    #endif 

    if (err_num != 0)
        return -1;

    return 0;
}

//-----------------------------------------------

int _jumps_struct_ctor(Jumps* jumps FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(jumps);

    jumps->jmp_pos = (unsigned int*) calloc(Initial_jumps_cap,
                                            sizeof(unsigned int));                             
    if (!jumps->jmp_pos) 
    { 
        error_report(CANNOT_ALLOCATE_MEM); 
        return -1;
    }

    jumps->entities = (Trans_entity**) calloc(Initial_jumps_cap,
                                              sizeof(Trans_entity*));
    if (!jumps->entities)
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    jumps->cap     = Initial_jumps_cap;
    jumps->num     = 0;

    return 0;
}

//-----------------------------------------------

int _jumps_struct_resize(Jumps* jumps FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(jumps);

    unsigned int old_jumps_cap = jumps->cap;
    unsigned int new_jumps_cap = old_jumps_cap * 2;

    jumps->jmp_pos = (unsigned int*) my_recalloc((void* ) jumps->jmp_pos,
                                                 (size_t) new_jumps_cap,
                                                 (size_t) old_jumps_cap,
                                                  sizeof (unsigned int));
    if (!jumps->jmp_pos) return -1;

    jumps->entities = (Trans_entity**) my_recalloc ((void* ) jumps->entities,
                                                    (size_t) new_jumps_cap,
                                                    (size_t) old_jumps_cap,
                                                     sizeof (Trans_entity*));
    if (!jumps->entities) return -1;

    jumps->cap = new_jumps_cap;

    return 0;
}

//-----------------------------------------------

int _jumps_struct_dtor(Jumps* jumps FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(jumps);

    free(jumps->jmp_pos);
    free(jumps->entities);

    return 0;
}

//-----------------------------------------------

int _pos_struct_ctor(Pos* pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(pos);

    pos->num = 0;
    pos->cap = Initial_pos_cap;

    pos->inp_pos = (unsigned int*) calloc(Initial_pos_cap, 
                                          sizeof(unsigned int));
    assert(pos->inp_pos);

    pos->res_pos = (unsigned int*) calloc(Initial_pos_cap, 
                                          sizeof(unsigned int));
    assert(pos->res_pos);

    return 0;
}


//-----------------------------------------------

int _pos_struct_dtor(Pos* pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(pos);

    free(pos->inp_pos);
    free(pos->res_pos);

    return 0;
}

//-----------------------------------------------

int _pos_struct_resize(Pos* pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(pos);

    unsigned int prev_cap = pos->cap;
    unsigned int incr_cap = prev_cap * 2;

    pos->cap = incr_cap;

    pos->inp_pos = (unsigned int*) my_recalloc((void* ) pos->inp_pos,
                                               (size_t) incr_cap,
                                               (size_t) prev_cap,
                                                sizeof (unsigned int));
    if (!pos->inp_pos) return -1;

    pos->res_pos = (unsigned int*) my_recalloc((void* ) pos->res_pos,
                                               (size_t) incr_cap,
                                               (size_t) prev_cap,
                                                sizeof (unsigned int));
    if (!pos->res_pos) return -1;

    return 0;
}

//-----------------------------------------------

int _add_position_accordance(Pos* pos, unsigned int inp_pos,
                                       unsigned int res_pos FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(pos);

    if (pos->num == pos->cap)
    {
        int ret_val = pos_struct_resize(pos);
        if (ret_val == -1) return -1;
    }

    pos->inp_pos[pos->num] = inp_pos;
    pos->res_pos[pos->num] = res_pos;

    pos->num += 1;

    return 0;
}

//-----------------------------------------------

int _add_jump_entity(Jumps* jumps, Trans_entity* trans_entity, 
                                   unsigned int jmp_pos, 
                                   unsigned int inp_dst FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(trans_entity);
    assert(jumps);

    if (jumps->num == jumps->cap)
    {
        int ret_val = jumps_struct_resize(jumps);
        if (ret_val == -1) return -1;
    }

    jumps->entities[jumps->num] = trans_entity;
    jumps->jmp_pos [jumps->num] = jmp_pos;

    jumps->num += 1;

    unsigned int   patch_size = 4;
    unsigned char* patch_data = (unsigned char*)&inp_dst;
    unsigned int   patch_pos  = 0;

    if (trans_entity->type == Near_rel_jmp_N_type
    ||  trans_entity->type == Rel_call_type)

        patch_pos = 1;
    else 
        patch_pos = 2;

    PATCH_ENTITY(trans_entity, patch_pos, patch_size, patch_data);

    return 0;
}

//-----------------------------------------------

int _patch_entity(Trans_entity* trans_entity, unsigned int   patch_pos, unsigned int  patch_size,
                                              unsigned char* patch_data FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    assert(trans_entity);
    assert(patch_data);

    if (patch_size == 0)
    {
        error_report(JIT_ZERO_PATCH_SIZE);
        return -1;
    }

    if (patch_pos > trans_entity->size)
    {
        error_report(JIT_INV_PATCH_POS);
        return -1;
    }

    unsigned char* destn_data = trans_entity->data + patch_pos;

    int ret_val = fast_cpy((void*)destn_data,
                           (void*)patch_data,
                           patch_size);
    
    if (ret_val == -1) return -1;

    return 0;
}


//===============================================
