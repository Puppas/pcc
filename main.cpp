#include <string.h>
#include <iostream>
#include "tokenize.hpp"
#include "parse.hpp"
#include "codegen.hpp"
#include "utils/util.hpp"
#include "ir_core/IRContext.hpp"
#include "gen_ir.hpp"
#include "passes/mem2reg.hpp"
#include "passes/gvn.hpp"


#define GEN_IR


static char *opt_o;
static char *input_path;


static void usage(int status) {
    fprintf(stderr, "pcc [ -o <path> ] <file>\n");
    exit(status);
}

static void paese_args(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "--help")) {
            usage(0);
        }

        // parse -o filename 
        if (!strcmp(argv[i], "-o")) {
            if(!argv[++i])
                usage(1);
            opt_o = argv[i];
            continue;
        }

        // parse -ofilename 
        if(!strncmp(argv[i], "-o", 2)) {
            opt_o = argv[i] + 2;
            continue;
        }

        if (argv[i][0] == '-' && argv[i][1] != '\0') 
            error("unknown argument: %s", argv[i]);

        input_path = argv[i];
    }

    if (!input_path)
        error("no input files");
}


static FILE *open_file(char *path) {
    if(!path || strcmp(path, "-") == 0) 
        return stdout;

    FILE *out = fopen(path, "w");
    if(!out)
        error("cannot open output file: %s: %s", path, strerror(errno));
    return out;
}


int main(int argc, char **argv)
{
    paese_args(argc, argv);

    Token *tok = tokenize_file(input_path);
    Obj *prog = parse(tok);

#ifdef GEN_IR
    IRContext context;
    Module *module = gen_ir(prog, context);
    mem2reg(module);
    global_value_numbering(module);
    std::cout << *module << std::endl;

#else
    FILE *out = open_file(opt_o);
    // .file file_number file_name
    fprintf(out, ".file 1 \"%s\"\n", input_path);
    codegen(prog, out);

#endif
    return 0;
}