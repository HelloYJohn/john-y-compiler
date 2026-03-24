#ifndef AST_H
#define AST_H
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <variant>
#include <map>

static int symbol_num = 0;
static int if_else_num = 0;
enum class UnaryExpType { primary, unary };
enum class PrimaryExpType { exp, number, lval };
enum class StmtType { if_, ifelse, simple };
enum class SimpleStmtType { lval, exp, block, ret };
enum class DeclType { const_decl, var_decl };
enum class BlockItemType { decl, stmt };
static std::vector<std::map<std::string, std::variant<int, std::string>>>
    symbol_tables;
static std::map<std::string, int> var_num;

static std::variant<int, std::string> look_up_symbol_tables(std::string l_val)
{
    for (auto it = symbol_tables.rbegin(); it != symbol_tables.rend(); it++)
        if (it->count(l_val))
            return (*it)[l_val];
    assert(false);
    return -1;
}

class BaseAST
{
public:
    virtual ~BaseAST() = default;
    virtual void dump() const = 0;
    virtual std::string dumpIR() const = 0;
    virtual int dumpExp() const { assert(false); return -1; }
};


class CompUnitAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST>> func_def_list;
    std::vector<std::unique_ptr<BaseAST>> decl_list;
    void dump() const override
    {
        std::cout << "CompUnitAST { ";
        for (auto && func_def : func_def_list)
            func_def->dump();
        std::cout << " }";
    }
    std::string dumpIR() const override
    {
        for (auto && func_def : func_def_list) {
            func_def->dumpIR();
            std::cout << std::endl;
        }
        return "";
    }
};


class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;
    void dump() const override
    {
        std::cout << "FuncDefAST { ";
        func_type->dump();
        std::cout << ", " << ident << ", ";
        block->dump();
        std::cout << " }";
    }
    std::string dumpIR() const override
    {
        std::cout << "fun @" << ident << "(): ";
        func_type->dumpIR();
        std::cout << " {" << std::endl << "\%entry:" << std::endl;
        block->dumpIR();
        std::cout << "}";
        return "";
    }
};


class FuncTypeAST : public BaseAST
{
public:
    std::string func_type_name;
    void dump() const override
    {
        std::cout << "FuncTypeAST{";
        std::cout << func_type_name;
        std::cout << "}";
    }
    std::string dumpIR() const override
    {
        if (func_type_name == "int") std::cout << "i32";
        else std::cout << "ERROR";
        return "";
    }
};


class BlockAST : public BaseAST
{
public:
    std::vector<std::unique_ptr<BaseAST>> block_item_list;
    void dump() const override
    {
        std::cout << "BlockAST { ";
        for (auto && block_item : block_item_list)
            block_item->dump();
        std::cout << " }";
    }
    std::string dumpIR() const override
    {
        std::string block_type = "";
        std::map<std::string, std::variant<int, std::string>> symbol_table;
        symbol_tables.push_back(symbol_table);
        for (auto && block_item : block_item_list) {
            block_type = block_item->dumpIR();
        }
        symbol_tables.pop_back();
        return block_type;
    }
};

class BlockItemAST : public BaseAST
{
public:
    BlockItemType type;
    std::unique_ptr<BaseAST> content;
    void dump() const override
    {
        content->dump();
    }
    std::string dumpIR() const override
    {
        return content->dumpIR();
    }
};

class StmtAST: public BaseAST
{
public:
    StmtType stmt_type;
    std::unique_ptr<BaseAST> exp_simple;
    std::unique_ptr<BaseAST> if_stmt;
    std::unique_ptr<BaseAST> if_else_stmt;
    std::string stmt;
    void dump() const override
    {
        std::cout << "StmtAST { ";
        exp_simple->dump();
        std::cout << " }";
    }
    std::string dumpIR() const override
    {
        if (stmt_type == StmtType::simple)
        {
            return exp_simple->dumpIR();
        } else {
            assert(false);
        } 
        std::cout << "\tret 0" << std::endl;
        return "";
    }
};

class SimpleStmtAST : public BaseAST
{
public:
    SimpleStmtType type;
    std::string lval;
    std::unique_ptr<BaseAST> block_exp;
    void dump() const override
    {
        if (type == SimpleStmtType::ret)
        {
            std::cout << "RETURN { ";
            block_exp->dump();
            std::cout << " } ";
        } else if (type == SimpleStmtType::lval) {
            std::cout << "LVAL { " << lval << " = ";
            block_exp->dump();
            std::cout << " } ";
        } else if (type == SimpleStmtType::exp) {
            if (block_exp != nullptr) {
                std::cout << "EXP { ";
                block_exp->dump();
                std::cout << " } ";
            }
        } else if (type == SimpleStmtType::block) {
            std::cout << "BLOCK { ";
            block_exp->dump();
            std::cout << " } ";
        }
        else assert(false);
    }
    std::string dumpIR() const override
    {
        if (type == SimpleStmtType::ret)
        {
            if (block_exp == nullptr)std::cout << '\t' << "ret" << std::endl;
            else
            {
                std::string result_var = block_exp->dumpIR();
                std::cout << '\t' << "ret " << result_var << std::endl;
            }
        } else if (type == SimpleStmtType::lval) {
            std::string result_var = block_exp->dumpIR();
            std::variant<int, std::string> value = look_up_symbol_tables(lval);
            assert(value.index() == 1);
            std::cout << "\tstore "  << result_var << ", " << std::get<std::string>(value) << std::endl;
        } else if (type == SimpleStmtType::exp) {
            if (block_exp != nullptr) {
                block_exp->dumpIR();
            }
        } else if (type == SimpleStmtType::block) {
            return block_exp->dumpIR();
        }
        else assert(false);
        return "";
    }
};

class DeclAST : public BaseAST
{
public:
    DeclType type;
    std::unique_ptr<BaseAST> decl;
    void dump() const override
    {
        decl->dump();
    }
    std::string dumpIR() const override
    {
        return decl->dumpIR();
    }
};

class ConstDeclAST : public BaseAST
{
public:
    std::string b_type;
    std::vector<std::unique_ptr<BaseAST>> const_def_list;
    void dump() const override
    {
        assert(b_type == "int");
        for (auto&& const_def : const_def_list)const_def->dump();
    }
    std::string dumpIR() const override
    {
        assert(b_type == "int");
        for (auto&& const_def : const_def_list)const_def->dumpIR();
        return "";
    }
};


class ConstDefAST : public BaseAST
{
public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    void dump() const override
    {
        std::cout << "ConstDefAST{" << ident << "=";
        const_init_val->dump();
        std::cout << "} ";
    }
    std::string dumpIR() const override
    {
        symbol_tables.back()[ident] = std::stoi(const_init_val->dumpIR());
        return "";
    }
};


class ConstInitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> const_exp;
    void dump() const override
    {
        std::cout << const_exp->dumpExp();
    }
    std::string dumpIR() const override
    {
        return std::to_string(const_exp->dumpExp());
    }
};

class ConstExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    void dump() const override
    {
        std::cout << exp->dumpExp();
    }
    std::string dumpIR() const override
    {
        return std::to_string(exp->dumpExp());
    }
    virtual int dumpExp() const override
    {
        return exp->dumpExp();
    }
};

class VarDeclAST : public BaseAST
{
public:
    std::string b_type;
    std::vector<std::unique_ptr<BaseAST>> var_def_list;
    void dump() const override
    {
        assert(b_type == "int");
        for (auto&& var_def : var_def_list)var_def->dump();
    }
    std::string dumpIR() const override
    {
        assert(b_type == "int");
        for (auto&& var_def : var_def_list)var_def->dumpIR();
        return "";
    }
};


class VarDefAST : public BaseAST
{
public:
    std::string ident;
    bool has_init_val;
    std::unique_ptr<BaseAST> init_val;
    void dump() const override
    {
        std::cout << "VarDefAST{" << ident;
        if (has_init_val)
        {
            std::cout << "=";
            init_val->dump();
        }
        std::cout << "} ";
    }
    std::string dumpIR() const override
    {
        std::string var_name = "@" + ident;
        std::string name = var_name + "_" +
            std::to_string(var_num[var_name]++);
        std::cout << '\t' << name << " = alloc i32" << std::endl;
        symbol_tables.back()[ident] = name;
        if (has_init_val)
        {
            std::string val_var = init_val->dumpIR();
            std::cout << "\tstore " << val_var << ", " << name << std::endl;
        }
        return "";
    }
};


class InitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> exp;
    void dump() const override
    {
        exp->dump();
    }
    std::string dumpIR() const override
    {
        return exp->dumpIR();
    }
};

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> l_or_exp;
    void dump() const override
    {
        std::cout << "ExpAST { ";
        l_or_exp->dump();
        std::cout << " }";
    }
    std::string dumpIR() const override
    {
        return l_or_exp->dumpIR();
    }

    virtual int dumpExp() const override
    {
        return l_or_exp->dumpExp();
    }
};


class LOrExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> l_and_exp;
    std::unique_ptr<BaseAST> l_or_exp;
    void dump() const override
    {
        if (op == "")l_and_exp->dump();
        else
        {
            l_or_exp->dump();
            std::cout << op;
            l_and_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = l_and_exp->dumpIR();
        else if (op == "||")
        {
            std::string left_result = l_or_exp->dumpIR();
            std::string then_label = "\%then_" + std::to_string(if_else_num);
            std::string else_label = "\%else_" + std::to_string(if_else_num);
            std::string end_label = "\%end_" + std::to_string(if_else_num++);
            std::string result_var_ptr = "%" + std::to_string(symbol_num++);
            std::cout << '\t' << result_var_ptr << " = alloc i32" << std::endl;
            std::cout << "\tbr " << left_result << ", " << then_label << ", "
                << else_label << std::endl;
            std::cout << then_label << ":" << std::endl;
            std::cout << "\tstore 1, " << result_var_ptr << std::endl;
            std::cout << "\tjump " << end_label << std::endl;
            std::cout << else_label << ":" << std::endl;
            std::string tmp_result_var = "%" + std::to_string(symbol_num++);
            std::string right_result = l_and_exp->dumpIR();
            std::cout << '\t' << tmp_result_var << " = ne " << right_result
                << ", 0" << std::endl;
            std::cout << "\tstore " << tmp_result_var << ", " << result_var_ptr
                << std::endl;
            std::cout << "\tjump " << end_label << std::endl;
            std::cout << end_label << ":" << std::endl;
            result_var = "%" + std::to_string(symbol_num++);
            std::cout << '\t' << result_var << " = load " << result_var_ptr
                << std::endl;
        }
        else assert(false);
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 1;
        if (op == "")result = l_and_exp->dumpExp();
        else if (op == "||")
        {
            int left_result = l_or_exp->dumpExp();
            if (left_result)return 1;
            result = l_and_exp->dumpExp() != 0;
        }
        else assert(false);
        return result;
    }
};


class LAndExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> eq_exp;
    std::unique_ptr<BaseAST> l_and_exp;
    void dump() const override
    {
        if (op == "")eq_exp->dump();
        else
        {
            l_and_exp->dump();
            std::cout << op;
            eq_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = eq_exp->dumpIR();
        else if (op == "&&")
        {
            std::string left_result = l_and_exp->dumpIR();
            std::string then_label = "\%then_" + std::to_string(if_else_num);
            std::string else_label = "\%else_" + std::to_string(if_else_num);
            std::string end_label = "\%end_" + std::to_string(if_else_num++);
            std::string result_var_ptr = "%" + std::to_string(symbol_num++);
            std::cout << '\t' << result_var_ptr << " = alloc i32" << std::endl;
            std::cout << "\tbr " << left_result << ", " << then_label << ", "
                << else_label << std::endl;
            std::cout << then_label << ":" << std::endl;
            std::string tmp_result_var = "%" + std::to_string(symbol_num++);
            std::string right_result = eq_exp->dumpIR();
            std::cout << '\t' << tmp_result_var << " = ne " << right_result
                << ", 0" << std::endl;
            std::cout << "\tstore " << tmp_result_var << ", " << result_var_ptr
                << std::endl;
            std::cout << "\tjump " << end_label << std::endl;
            std::cout << else_label << ":" << std::endl;
            std::cout << "\tstore 0, " << result_var_ptr << std::endl;
            std::cout << "\tjump " << end_label << std::endl;
            std::cout << end_label << ":" << std::endl;
            result_var = "%" + std::to_string(symbol_num++);
            std::cout << '\t' << result_var << " = load " << result_var_ptr
                << std::endl;
        }
        else assert(false);
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (op == "")result = eq_exp->dumpExp();
        else if (op == "&&")
        {
            int left_result = l_and_exp->dumpExp();
            if (left_result == 0)return 0;
            result = eq_exp->dumpExp() != 0;
        }
        else assert(false);
        return result;
    }
};


class EqExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> rel_exp;
    std::unique_ptr<BaseAST> eq_exp;
    void dump() const override
    {
        if (op == "")rel_exp->dump();
        else
        {
            eq_exp->dump();
            std::cout << op;
            rel_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = rel_exp->dumpIR();
        else
        {
            std::string left_result = eq_exp->dumpIR();
            std::string right_result = rel_exp->dumpIR();
            result_var = "%" + std::to_string(symbol_num++);
            if (op == "==")
                std::cout << '\t' << result_var << " = eq " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == "!=")
                std::cout << '\t' << result_var << " = ne " << left_result <<
                    ", " << right_result << std::endl;
            else assert(false);
        }
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (op == "")result = rel_exp->dumpExp();
        else
        {
            int left_result = eq_exp->dumpExp();
            int right_result = rel_exp->dumpExp();
            if (op == "==")result = left_result == right_result;
            else if (op == "!=")result = left_result != right_result;
            else assert(false);
        }
        return result;
    }
};


class RelExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> add_exp;
    std::unique_ptr<BaseAST> rel_exp;
    void dump() const override
    {
        if (op == "")add_exp->dump();
        else
        {
            rel_exp->dump();
            std::cout << op;
            add_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = add_exp->dumpIR();
        else
        {
            std::string left_result = rel_exp->dumpIR();
            std::string right_result = add_exp->dumpIR();
            result_var = "%" + std::to_string(symbol_num++);
            if (op == "<")
                std::cout << '\t' << result_var << " = lt " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == ">")
                std::cout << '\t' << result_var << " = gt " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == "<=")
                std::cout << '\t' << result_var << " = le " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == ">=")
                std::cout << '\t' << result_var << " = ge " << left_result <<
                    ", " << right_result << std::endl;
            else assert(false);
        }
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (op == "")result = add_exp->dumpExp();
        else
        {
            int left_result = rel_exp->dumpExp();
            int right_result = add_exp->dumpExp();
            if (op == ">")result = left_result > right_result;
            else if (op == ">=")result = left_result >= right_result;
            else if (op == "<")result = left_result < right_result;
            else if (op == "<=")result = left_result <= right_result;
            else assert(false);
        }
        return result;
    }
};


class AddExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> mul_exp;
    std::unique_ptr<BaseAST> add_exp;
    void dump() const override
    {
        if (op == "")mul_exp->dump();
        else
        {
            add_exp->dump();
            std::cout << op;
            mul_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = mul_exp->dumpIR();
        else
        {
            std::string left_result = add_exp->dumpIR();
            std::string right_result = mul_exp->dumpIR();
            result_var = "%" + std::to_string(symbol_num++);
            if (op == "+")
                std::cout << '\t' << result_var << " = add " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == "-")
                std::cout << '\t' << result_var << " = sub " << left_result <<
                    ", " << right_result << std::endl;
            else assert(false);
        }
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (op == "")result = mul_exp->dumpExp();
        else
        {
            int left_result = add_exp->dumpExp();
            int right_result = mul_exp->dumpExp();
            if (op == "+")result = left_result + right_result;
            else if (op == "-")result = left_result - right_result;
            else assert(false);
        }
        return result;
    }
};


class MulExpAST : public BaseAST
{
public:
    std::string op;
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> mul_exp;
    void dump() const override
    {
        if (op == "")unary_exp->dump();
        else
        {
            mul_exp->dump();
            std::cout << op;
            unary_exp->dump();
        }
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (op == "")result_var = unary_exp->dumpIR();
        else
        {
            std::string left_result = mul_exp->dumpIR();
            std::string right_result = unary_exp->dumpIR();
            result_var = "%" + std::to_string(symbol_num++);
            if (op == "*")
                std::cout << '\t' << result_var << " = mul " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == "/")
                std::cout << '\t' << result_var << " = div " << left_result <<
                    ", " << right_result << std::endl;
            else if (op == "%")
                std::cout << '\t' << result_var << " = mod " << left_result <<
                    ", " << right_result << std::endl;
            else assert(false);
        }
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (op == "")result = unary_exp->dumpExp();
        else
        {
            int left_result = mul_exp->dumpExp();
            int right_result = unary_exp->dumpExp();
            if (op == "*")result = left_result * right_result;
            else if (op == "/")result = left_result / right_result;
            else if (op == "%")result = left_result % right_result;
            else assert(false);
        }
        return result;
    }
};


class UnaryExpAST : public BaseAST
{
public:
    UnaryExpType type;
    std::string op;
    std::unique_ptr<BaseAST> exp;
    void dump() const override
    {
        if (type == UnaryExpType::unary)std::cout << op;
        exp->dump();
    }
    std::string dumpIR() const override
    {
        if (type == UnaryExpType::primary)return exp->dumpIR();
        else
        {
            std::string result_var = exp->dumpIR();
            std::string next_var = "%" + std::to_string(symbol_num);
            if (op == "+")return result_var;
            else if (op == "-")std::cout << '\t' << next_var << " = sub 0, " <<
                result_var << std::endl;
            else if (op == "!")std::cout << '\t' << next_var << " = eq " <<
                result_var << ", 0" << std::endl;
            else assert(false);
            symbol_num++;
            return next_var;
        }
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (type == UnaryExpType::primary)result = exp->dumpExp();
        else if (type == UnaryExpType::unary)
        {
            int tmp = exp->dumpExp();
            if (op == "+")result = tmp;
            else if (op == "-")result = -tmp;
            else if (op == "!")result = !tmp;
            else assert(false);
        }
        else assert(false);
        return result;
    }
};


class PrimaryExpAST : public BaseAST
{
public:
    PrimaryExpType type;
    std::unique_ptr<BaseAST> exp;
    std::string lval;
    int number;
    void dump() const override
    {
        if (type == PrimaryExpType::exp)exp->dump();
        else if (type == PrimaryExpType::number)std::cout << number;
        else if (type == PrimaryExpType::lval) std::cout << lval;
        else assert(false);
    }
    std::string dumpIR() const override
    {
        std::string result_var = "";
        if (type == PrimaryExpType::exp)result_var = exp->dumpIR();
        else if (type == PrimaryExpType::number)
            result_var = std::to_string(number);
        else if (type == PrimaryExpType::lval)
        {
            std::variant<int, std::string> value = look_up_symbol_tables(lval);
            if (value.index() == 0)
                result_var = std::to_string(std::get<int>(value));
            else if (value.index() == 1) {
                result_var = "%" + std::to_string(symbol_num++);
                std::cout << '\t' << result_var << " = load " << std::get<std::string>(value) << std::endl;
            }
        }
        else assert(false);
        return result_var;
    }

    virtual int dumpExp() const override
    {
        int result = 0;
        if (type == PrimaryExpType::exp)result = exp->dumpExp();
        else if (type == PrimaryExpType::number)result = number;
        else if (type == PrimaryExpType::lval)
        {
            std::variant<int, std::string> value = look_up_symbol_tables(lval);
            assert(value.index() == 0);
            result = std::get<int>(value);
        }
        else assert(false);
        return result;
    }
};

#endif /* AST_H */