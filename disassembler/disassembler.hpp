#ifndef DISASSEMBLER_DISASSEMBLER_HPP_
#define DISASSEMBLER_DISASSEMBLER_HPP_

class Disassembler
{
public:
    static int Disassemble(char *codebuffer, int pc);

    int main(int argc, char **argv);
};

#endif // DISASSEMBLER_DISASSEMBLER_HPP_
