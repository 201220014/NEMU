#include "cpu/instr.h"
/*
Put the implementations of `sub' instructions here.
*/

static void instr_execute_2op() 
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
	opr_dest.val = alu_sub(sign_ext(opr_src.val, opr_dest.data_size), opr_dest.val, opr_dest.data_size);
	operand_write(&opr_dest);
}

make_instr_impl_2op(sub, i, rm, bv)
make_instr_impl_2op(sub, rm, r, v)
