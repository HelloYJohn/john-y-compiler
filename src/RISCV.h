#ifndef RISCV_H
#define RISCV_H

void visit(const koopa_raw_program_t &program);
void visit(const koopa_raw_slice_t &slice);
void visit(const koopa_raw_function_t &func);
void visit(const koopa_raw_basic_block_t &basic_block);
void visit(const koopa_raw_value_t &value);
void visit(const koopa_raw_return_t &ret);
void visit(const koopa_raw_integer_t &integer);

void visit(const koopa_raw_program_t &program)
{
    std::cout << ".text" << std::endl;
    visit(program.values);
    visit(program.funcs);
}

void visit(const koopa_raw_slice_t &slice)
{
    for (uint32_t i = 0; i < slice.len; i++)
    {
        auto ptr = slice.buffer[i];
        switch (slice.kind)
        {
            case KOOPA_RSIK_FUNCTION:
                visit(reinterpret_cast<const koopa_raw_function_t>(ptr));
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                visit(reinterpret_cast<const koopa_raw_basic_block_t>(ptr));
                break;
            case KOOPA_RSIK_VALUE:
                visit(reinterpret_cast<const koopa_raw_value_t>(ptr));
                break;
            default:
                assert(false);
        }
    }
}

void visit(const koopa_raw_function_t &func)
{
    std::cout << ".globl " << func->name + 1 << std::endl;
    std::cout << func->name + 1 << ":" << std::endl;
    visit(func->bbs);
}

void visit(const koopa_raw_basic_block_t &basic_block)
{
    visit(basic_block->insts);
}

void visit(const koopa_raw_value_t &value)
{
    switch (value->kind.tag)
    {
        case KOOPA_RVT_RETURN:
            visit(value->kind.data.ret);
            break;
        case KOOPA_RVT_INTEGER:
            visit(value->kind.data.integer);
            break;
        default:
            assert(false);
    }
}

void visit(const koopa_raw_return_t &ret)
{
    koopa_raw_value_t value = ret.value;
    visit(value);
    std::cout << "ret" << std::endl;
}

void visit(const koopa_raw_integer_t &integer)
{
    std::cout << "li a0, " << integer.value << std::endl;
}

#endif /* RISCV_H */