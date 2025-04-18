#include "std.h"

#include "debug.h"
#include "architecture.h"
#include "options.h"
#include "compiler.h"
#include "sym.h"
#include "reg.h"
#include "defaults.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

static bool driver (config conf);

static const char* plural (int n) {
    return n == 1 ? "" : "s";
}

static bool driver (config conf) {
    bool fail = false;

    compilerCtx comp;
    compilerInit(&comp, &conf.arch, &conf.includeSearchPaths);

    /*Compile each of the inputs to assembly*/
    for (int i = 0; i < conf.inputs.length; i++) {
        compiler(&comp,
                 vectorGet(&conf.inputs, i),
                 vectorGet(&conf.intermediates, i));
    }

    compilerEnd(&comp);

    if (comp.errors != 0 || comp.warnings != 0)
        printf("Compilation complete with %d error%s and %d warning%s\n",
               comp.errors, plural(comp.errors),
               comp.warnings, plural(comp.warnings));

    else if (internalErrors)
        printf("Compilation complete with %d internal error%s\n",
               internalErrors, plural(internalErrors));

    /*Assemble/link*/
    else if (conf.mode != modeNoAssemble) {
        /*Produce a string list of all the intermediates*/
        char* intermediates = strjoinwith((char**) conf.intermediates.buffer, conf.intermediates.length,
                                          " ", malloc);

        if (conf.mode == modeNoLink)
            fail |= systemf("cc %s -c %s", conf.arch.asflags, intermediates) != 0;

        else {
            fail |= systemf("cc %s %s -o %s", conf.arch.ldflags, intermediates, conf.output) != 0;

            if (conf.deleteAsm && !fail)
                systemf("rm %s", intermediates);
        }

        free(intermediates);
    }

    return fail || comp.errors != 0 || internalErrors != 0;
}

int main (int argc, char** argv) {
    debugInit(stdout);

    bool fail = false;

    /*Parse the command line options into a config*/
    config conf = configCreate();
    optionsParse(&conf, argc, argv);

    /*Initialize the arch with defaults from <fcc>/defaults.h, which is generated
      by <fcc>/makedefaults.sh and included (with -include) by the makefile.*/
    archSetup(&conf.arch, defaultOS, defaultWordsize);

    if (conf.fail)
        fail = true;

    else if (conf.mode == modeVersion) {
        puts("Fedjmike's C Compiler (fcc) v0.01b");
        puts("Copyright 2014 Sam Nipps.");
        puts("This is free software; see the source for copying conditions.  There is NO");
        puts("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");

    } else if (conf.mode == modeHelp) {
        puts("Usage: fcc [options...] <files...>");
        puts("Options:");
        puts("  -I <dir>   Add a directory to be searched for headers");
        puts("  -c         Compile and assemble only, do not link");
        puts("  -S         Compile only, do not assemble or link");
        puts("  -s         Keep temporary assembly output after compilation");
        puts("  -o <file>  Output into a specific file");
        puts("  --help     Display command line information");
        puts("  --version  Display version information");

    } else
        fail = driver(conf);

    configDestroy(conf);

    return fail ? 1 : 0;
}
