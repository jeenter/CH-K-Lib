
#include "shell.h"
#include "ili9320.h"

int CMD_LCD(int argc, char * const * argv)
{
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED\r\n");
    return 0;
#else
    uint32_t err_cnt;
    ili9320_Init();
    return 0;
#endif
}

const cmd_tbl_t CommandFun_LCD = 
{
    .name = "LCD",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LCD,
    .usage = "LCD",
    .complete = NULL,
    .help = "\r\n"
};
