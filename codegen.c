#include "pcc.h"


static FILE *output_file;
static int depth;
static char *argreg8[] = {"%dil", "%sil", "%dl", "cl", "%r8b", "%r9b"};
static char *argreg16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char *argreg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;

static void gen_expr(Node *node);
static void gen_stmt(Node *node);


static void  println(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(output_file, fmt, ap);
    va_end(ap);
    fprintf(output_file, "\n");
}


static int count()
{
    static int i = 1;
    return i++;
}

static void push()
{
    println("  push %%rax");
    ++depth;
}

static void pop(char *arg)
{
    println("  pop %s", arg);
    --depth;
}

// round up n to the nearest multiple of 'align'
int align_to(int n, int align)
{
    return (n + align - 1) / align * align;
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static void gen_addr(Node *node)
{
    switch (node->kind)
    {
    case ND_VAR:
        if (node->var->is_local)
        {
            println("  lea %d(%%rbp), %%rax", node->var->offset);
        }
        else
        {
            println("  lea %s(%%rip), %%rax", node->var->name);
        }
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        return;
    case ND_COMMA:
        gen_expr(node->lhs);
        gen_addr(node->rhs);
        return;
    case ND_MEMBER:
        gen_addr(node->lhs);
        println("  add $%d, %%rax", node->member->offset);
        return;
    }

    error_tok(node->tok, "not an lvalue");
}

// load a value from where %rax is pointing to
static void load(Type *ty)
{
    if (ty->kind == TY_ARRAY || ty->kind == TY_STRUCT || ty->kind == TY_UNION)
        return;

    // When we load a char or a short value to a register, we always
    // extend them to the size of int, so we can assume the lower half of
    // a register always contains a valid value. The upper half of a
    // register for char, short and int may contain garbage. When we load
    // a long value to a register, it simply occupies the entire register.
    if (ty->size == 1) {
        println("  movsbl (%%rax), %%eax");
    }
    else if(ty->size == 2) {
        println("  movswl (%%rax), %%eax");
    }
    else if(ty->size == 4) {
        println("  movsxd (%%rax), %%rax");
    }
    else {
        println("  mov (%%rax), %%rax");
    }
}

// store %rax to an address that the stack top is pointing to
static void store(Type *ty)
{
    pop("%rdi");

    if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
        for (int i = 0; i < ty->size; ++i) {
            println("  mov %d(%%rax), %%r8b", i);
            println("  mov %%r8b, %d(%%rdi)", i);
        }
        return;
    }

    if (ty->size == 1)
        println("  mov %%al, (%%rdi)");
    else if(ty->size == 2)
        println("  mov %%ax, (%%rdi)");
    else if(ty->size == 4)
        println("  mov %%eax, (%%rdi)");
    else
        println("  mov %%rax, (%%rdi)");
}


static void cmp_zero(Type *ty)
{
    if (is_integer(ty) && ty->size <= 4) {
        println("  cmp $0, %%eax");
    }
    else {
        println("  cmp $0, %%rax");
    }
}

enum {I8, I16, I32, I64};


static int get_type_id(Type *ty) 
{
    switch (ty->kind)
    {
    case TY_CHAR:
        return I8;
    case TY_SHORT:
        return I16;
    case TY_INT:
        return I32;    
    }

    return I64;
}


static char i8_to_i32[] = "movsbl %al, %eax";
static char i16_to_i32[] = "movswl %ax, %eax";
static char i32_to_i64[] = "movslq %eax, %rax";

static char *cast_table[][10] = {
  {NULL,      NULL,       NULL,      i32_to_i64},  // i8
  {i8_to_i32, NULL,       NULL,      i32_to_i64}, // i16
  {i8_to_i32, i16_to_i32, NULL,      i32_to_i64}, // i32
  {i8_to_i32, i16_to_i32, NULL,      NULL},       // i64
};


static void cast(Type *from, Type *to) 
{
    if (to->kind == TY_VOID)
        return;
    
    if (to->kind == TY_BOOL) {
        cmp_zero(from);
        println("  setne %%al");
        println("  movzx %%al, %%eax");
        return;
    }

    int t1 = get_type_id(from);
    int t2 = get_type_id(to);
    if(cast_table[t1][t2])
        println("  %s", cast_table[t1][t2]);
}


static void gen_expr(Node *node)
{
    // .loc file_number line_number
    println(" .loc 1 %d", node->tok->line_no);

    switch (node->kind)
    {
    case ND_NUM:
        println("  mov $%ld, %%rax", node->val);
        return;

    case ND_NEG:
        gen_expr(node->lhs);
        println("  neg %%rax");
        return;
    case ND_VAR:
    case ND_MEMBER:
        gen_addr(node);
        load(node->ty);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        load(node->ty);
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        store(node->ty);
        return;
    case ND_STMT_EXPR:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_COMMA:
        gen_expr(node->lhs);
        gen_expr(node->rhs);
        return;
    case ND_CAST:
        gen_expr(node->lhs);
        cast(node->lhs->ty, node->ty);
        return;
    case ND_NOT:
        gen_expr(node->lhs);
        println("  cmp $0, %%rax");
        println("  sete %%al");
        println("  movzx %%al, %%rax");
        return;
    case ND_FUNCALL:
    {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next)
        {
            gen_expr(arg);
            push();
            ++nargs;
        }

        for (int i = nargs - 1; i >= 0; --i)
        {
            pop(argreg64[i]);
        }

        println("  mov $0, %%rax");
        println("  call %s", node->funcname);
        return;
    }
    }

    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("%rdi");

    char *ax, *di;
    if(node->lhs->ty->kind == TY_LONG || node->lhs->ty->base) {
        ax = "%rax";
        di = "%rdi";
    }
    else {
        ax = "%eax";
        di = "%edi";
    }

    switch (node->kind)
    {
    case ND_ADD:
        println("  add %s, %s", di, ax);
        return;
    case ND_SUB:
        println("  sub %s, %s", di, ax);
        return;
    case ND_MUL:
        println("  imul %s, %s", di, ax);
        return;
    case ND_DIV:
        if (node->lhs->ty->size == 8)
            println("  cqo");
        else 
            println("  cdq");
        println("  idiv %s", di);
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
        println("  cmp %s, %s", di, ax);

        if (node->kind == ND_EQ)
            println("  sete %%al");
        else if (node->kind == ND_NE)
            println("  setne %%al");
        else if (node->kind == ND_LT)
            println("  setl %%al");
        else if (node->kind == ND_LE)
            println("  setle %%al");

        println("  movzb %%al, %%rax");
        return;
    }

    error_tok(node->tok, "invalid expression");
}

static void gen_stmt(Node *node)
{
    // .loc file_number line_number
    println(" .loc 1 %d", node->tok->line_no);

    switch (node->kind)
    {
    case ND_IF:
    {
        int c = count();
        gen_expr(node->cond);
        println("  cmp $0, %%rax");
        println("  je .L.else.%d", c);
        gen_stmt(node->then);
        println("  jmp .L.end.%d", c);
        println(".L.else.%d:", c);
        if (node->els)
        {
            gen_stmt(node->els);
        }
        println(".L.end.%d:", c);
        return;
    }

    case ND_FOR:
    {
        int c = count();
        if (node->init)
            gen_stmt(node->init);
        println(".L.begin.%d:", c);
        if (node->cond)
        {
            gen_expr(node->cond);
            println("  cmp $0, %%rax");
            println("  je .L.end.%d", c);
        }
        gen_stmt(node->then);
        if (node->inc)
        {
            gen_expr(node->inc);
        }
        println("  jmp .L.begin.%d", c);
        println(".L.end.%d:", c);
        return;
    }

    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
        {
            gen_stmt(n);
        }
        return;

    case ND_RETURN:
        gen_expr(node->lhs);
        println("  jmp .L.return.%s", current_fn->name);
        return;

    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    }

    error_tok(node->tok, "invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog)
{
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
        {
            continue;
        }

        int offset = 0;
        for (Obj *var = fn->locals; var; var = var->next)
        {
            offset += var->ty->size;
            offset = align_to(offset, var->ty->align);
            var->offset = -offset;
        }
        fn->stack_size = align_to(offset, 16);
    }
}

static void emit_data(Obj *prog)
{
    for (Obj *var = prog; var; var = var->next)
    {
        if (var->is_function)
            continue;

        println("  .data");
        println("  .globl %s", var->name);
        println("%s:", var->name);
        
        if (var->init_data) {
            for(int i = 0; i < var->ty->size; ++i)
                println("  .byte %d", var->init_data[i]);
        }
        else {
            println("  .zero %d", var->ty->size);
        }
    }
}

static void store_gp(int r, int offset, int sz) {
    switch (sz)
    {
    case 1:
        println("  mov %s, %d(%%rbp)", argreg8[r], offset);
        return;
    case 2:
        println("  mov %s, %d(%%rbp)", argreg16[r], offset);
        return;
    case 4:
        println("  mov %s, %d(%%rbp)", argreg32[r], offset); 
        return;
    case 8:
        println("  mov %s, %d(%%rbp)", argreg64[r], offset);
        return;
    }

    unreachable();
}


static void emit_text(Obj *prog)
{
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function || !fn->is_definition)
        {
            continue;
        }

        if (fn->is_static)
            println("  .local %s", fn->name);
        else
            println("  .globl %s", fn->name);

        println("  .text");
        println("%s:", fn->name);
        current_fn = fn;

        println("  push %%rbp");
        println("  mov %%rsp, %%rbp");
        println("  sub $%d, %%rsp", fn->stack_size);

        // save arguments to stack
        int i = 0;
        for (Obj *var = fn->params; var; var = var->next)
        {
            store_gp(i++, var->offset, var->ty->size);
        }

        gen_stmt(fn->body);
        assert(depth == 0);

        println(".L.return.%s:", fn->name);
        println("  mov %%rbp, %%rsp");
        println("  pop %%rbp");
        println("  ret");
    }
}

void codegen(Obj *prog, FILE *out)
{
    output_file = out;
    assign_lvar_offsets(prog);
    emit_data(prog);
    emit_text(prog);
}