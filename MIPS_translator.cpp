#include<iostream>
#include<iomanip>
#include<string>
#include<algorithm>
#include<sstream>
#include<map>

using namespace std;
int32_t inst; //32位指令
string instName; //指令名称
string operandNames[3]; //操作数名称
map<string, int32_t> REGS; //寄存器名称与编号对应表
map<string, int> TAGS; //保存所有的tag对应的字地址
int instAddr = 0; //当前指令的字地址（即每多一条指令，instAddr自增1）

//将string转换为小写
inline string strtolower(std::string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

//string转换为int32_t（根据是否有'0x'自动检测16进制/10进制）
inline int32_t str2int(string t)
{
    t = strtolower(t);
    if(t.length() >= 2 && t[0] == '0' && t[1] == 'x')
    {
        int32_t tmp;
        istringstream ss(t); //输入string流
        ss >> hex >> tmp;
        return tmp;
    }
    return atoi(t.c_str());
}

//将读入的3个用逗号间隔的targetNum（<=3）操作数分割，存放到全局变量operandNames[]中
int divideAllOperandsWithCommas(string& allOperands, int targetNum)
{
    int cnt = 0;
    int comment_offset = -1;
    string tmp;

    //清除注释
    comment_offset = allOperands.find_first_of('#');
    if(comment_offset != string::npos) allOperands.erase(comment_offset);
    //将逗号替换为空格
    replace(allOperands.begin(), allOperands.end(), ',', ' '); 
    //输入string流
    istringstream ss(allOperands); 

    //分割allOperands字符串
    while(ss>>tmp)
    {
        if(cnt < targetNum)
        {
            operandNames[cnt] = tmp; //放入operandNames数组的0、1、2个位置
            cnt++;
        }
        else
        {
            #ifndef MIPSFILE
            cout<<"Wrong formats! Please enter again:"<<endl;
            #endif

            #ifdef MIPSFILE
            cout<<"# Cannot translate! Probably a wrong format"<<endl;
            #endif
            return 0;
        }
    }
    return 1;
}

//判断并保存tag
int saveTags(string& allOperands)
{
    int colon_offset = -1;
    //找第一个冒号
    colon_offset = allOperands.find_first_of(':');
    //如果没有冒号，则一定不是一个tag
    if(colon_offset == string::npos)
         return 0;
        
    //如果冒号不在最后，则一定不是一个tag
    if(colon_offset != allOperands.length() - 1)
         return 0;
    
    //上述都不满足的话，是一个合法的tag
    allOperands.erase(colon_offset); //只保留单词
    TAGS[allOperands] = instAddr; //保存至TAGS的map中，以后可能会用到
    #ifndef MIPSFILE
    cout<<"# Tag found! Tag "<<allOperands<<" corresponds to word address "<<instAddr<<endl;
    #endif
    return 1;
}

int readInst(string& instName);
void initializeREGS();

int main()
{
    initializeREGS();
    #ifndef MIPSFILE
    cout<<"Welcome to a MIPS translator written by VTU!"<<endl<<"Enter correct instructions or type 'quit' in a single line to quit."<<endl;
    #endif

    do
    {
        //读入指令名称
        if(!(cin>>instName))
        {
            #ifndef MIPSFILE
            cout<<"Goodbye"<<endl;
            #endif
            break;
        }
        instName = strtolower(instName); //转换为小写

        if(instName == "quit")
        {
            #ifndef MIPSFILE
            cout<<"Goodbye"<<endl;
            #endif
            break;
        }

        if(!readInst(instName)) continue; //翻译指令

        instAddr++; //对于合法指令（不包含tag），每次翻译完成字地址++

        #ifndef MIPSFILE
        cout<<hex<<"instruction: "<<setfill('0')<<setw(8)<<inst<<endl; //输出指令
        #endif
        
        #ifdef MIPSFILE
        cout<<hex<<setfill('0')<<setw(8)<<inst<<endl; //输出指令（直接将机器码输出到文件，不保留多余的说明）
        #endif
        
    } while (1);
    
    return 0;
}

int readInst(string& instName)
{
    if(instName == "add" || instName == "sub" || instName == "and" || instName == "or" \
    || instName == "nor" || instName == "xor" || instName == "slt")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容
        
        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;

        //获取寄存器编码
        int32_t rs, rt, rd;
        rs = REGS[operandNames[1]];
        rt = REGS[operandNames[2]];
        rd = REGS[operandNames[0]];

        //将寄存器编码放到inst对应位置
        inst = 0;
        inst |= (rs << 21);
        inst |= (rt << 16);
        inst |= (rd << 11);


        //接着处理func
        int32_t func;
        if(instName == "add") func = 32;
        else if(instName == "sub") func = 34;
        else if(instName == "and") func = 36;
        else if(instName == "or") func = 37;
        else if(instName == "nor") func = 39;
        else if(instName == "xor") func = 38;
        else if(instName == "slt") func = 42;
        inst |= func;
    }
    else if(instName == "srl")
    {
        string allOperands;
        getline(cin, allOperands); //读入该行剩余内容
        
        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //获取寄存器编码、shamt
        int32_t rt, rd, shamt;
        shamt = str2int(operandNames[2]);
        rt = REGS[operandNames[1]];
        rd = REGS[operandNames[0]];

        //将寄存器编码、shamt放到inst对应位置
        inst = 0;
        inst |= (shamt << 6);
        inst |= (rt << 16);
        inst |= (rd << 11);

        //接着处理func
        int32_t func = 2; //仅针对srl，后期扩展需用判定语句
        inst |= func;
    }
    else if(instName == "jr")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 1)) return 0;

        //获取寄存器编码
        int32_t rs;
        rs = REGS[operandNames[0]];

        //将寄存器编码放到inst对应位置
        inst = 0;
        inst |= (rs << 21);


        //接着处理func
        int32_t func = 8; //仅针对jr，后期扩展需用判定语句
        inst |= func;
    }
    else if(instName == "jalr")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 2)) return 0;
        
        //获取寄存器编码
        int32_t rs, rt, rd;
        rs = REGS[operandNames[0]];
        rd = REGS[operandNames[1]];

        //将寄存器编码放到inst对应位置
        inst = 0;
        inst |= (rs << 21);
        inst |= (rd << 11);


        //接着处理func
        int32_t func = 9; //仅针对jalr，后期扩展需用判定语句
        inst |= func;
    }
    else if(instName == "lw" || instName == "sw")
    {
        // int numOperand = 0; //操作数计数器
        // string allOperands, tmp;
        
        // getline(cin, allOperands); //读入该行剩余内容
        // replace(allOperands.begin(), allOperands.end(), ',', ' '); //将逗号替换为空格
        // replace(allOperands.begin(), allOperands.end(), '(', ' ');
        // replace(allOperands.begin(), allOperands.end(), ')', ' '); //将括号替换为空格

        // istringstream ss(allOperands); //输入string流
        // //分割allOperands字符串
        // while(ss>>tmp)
        // {
        //     if(numOperand < 3)
        //     {
        //         operandNames[numOperand] = tmp; //放入operandNames数组的0、1、2个位置
        //         numOperand++;
        //     }
        //     else
        //     {
        //         #ifndef MIPSFILE
        //         cout<<"Wrong formats! Please enter again:"<<endl;
        //         #endif

        //         #ifdef MIPSFILE
        //         cout<<"## Not recognized!"<<endl;
        //         #endif
        //         return 0;
        //     }
        // }

        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        replace(allOperands.begin(), allOperands.end(), '(', ' ');
        replace(allOperands.begin(), allOperands.end(), ')', ' '); //将括号替换为空格

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //获取寄存器编码、立即数
        int32_t rs, rt, imm;
        rs = REGS[operandNames[2]];
        rt = REGS[operandNames[0]];
        imm = str2int(operandNames[1]);

        //将寄存器编码、立即数放到inst对应位置
        inst = 0;
        inst |= imm;
        inst |= (rs << 21);
        inst |= (rt << 16);

        //接着处理opcode
        int32_t opcode;
        if(instName == "lw") opcode = 35;
        else if(instName == "sw") opcode = 43;
        inst |= (opcode << 26);
    }
    else if(instName == "beq" || instName == "bne")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //获取寄存器编码、立即数
        int32_t rs, rt, imm;
        imm = str2int(operandNames[2]);
        rs = REGS[operandNames[0]];
        rt = REGS[operandNames[1]];

        //将寄存器编码、立即数放到inst对应位置
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);
        inst |= (rs << 21);

        //接着处理opcode
        int32_t opcode;
        if(instName == "beq") opcode = 4;
        else if(instName == "bne") opcode = 5;
        inst |= (opcode << 26);
    }
    else if(instName == "addi" || instName == "andi" || instName == "ori"\
         || instName == "xori" || instName == "slti")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //获取寄存器编码、立即数
        int32_t rs, rt, imm;
        imm = str2int(operandNames[2]);
        rt = REGS[operandNames[0]];
        rs = REGS[operandNames[1]];

        //将寄存器编码、立即数放到inst对应位置
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);
        inst |= (rs << 21);

        //接着处理opcode
        int32_t opcode;
        if(instName == "addi") opcode = 8;
        else if(instName == "andi") opcode = 12;
        else if(instName == "ori") opcode = 13;
        else if(instName == "xori") opcode = 14;
        else if(instName == "slti") opcode = 10;
        inst |= (opcode << 26);
    }
    else if(instName == "lui")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 2)) return 0;
        
        //获取寄存器编码、立即数
        int32_t rt, imm;
        imm = str2int(operandNames[1]);
        rt = REGS[operandNames[0]];

        //将寄存器编码、立即数放到inst对应位置
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);

        //接着处理opcode
        int32_t opcode = 15; //仅针对lui，后期扩展需用判定语句
        inst |= (opcode << 26);
    }
    else if(instName == "j" || instName == "jal")
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容

        if(!divideAllOperandsWithCommas(allOperands, 1)) return 0;
        
        //以下判定实现识别用户使用了tag还是数字进行跳转！
        int32_t imm;
        if(operandNames[0][0] < '0' || operandNames[0][0] > '9')
        {
            //是一个tag，而不是数字
            imm = TAGS[strtolower(operandNames[0])];
            #ifndef MIPSFILE
            cout << "# Tag recognized! The tag " << strtolower(operandNames[0]) << " corresponds to word address " << imm << endl; //调试信息
            #endif
        }
        else
        {
            //否则是一个数字，而不是tag
            //获取寄存器编码
            imm = str2int(operandNames[0]) / 4;
        }


        //将寄存器编码放到inst对应位置
        inst = 0;
        inst |= imm;

        //接着处理opcode
        int32_t opcode;
        if(instName == "j") opcode = 2;
        else if(instName == "jal") opcode = 3;
        inst |= (opcode << 26);
    }
    else
    {
        string allOperands;
        
        getline(cin, allOperands); //读入该行剩余内容并忽略！

        //判定并保存一个tag
        if(saveTags(instName))
        {
            //是的话直接返回0（是tag但不是一条指令，因而什么都不输出，也不必提醒用户）
            return 0;
        }

        //否则不是一个tag，那么一定是一个不合法输入，要提醒用户
        #ifndef MIPSFILE
        cout<<"Instruction name not recognized! Please enter again:"<<endl;
        #endif

        #ifdef MIPSFILE
        cout<<"# Cannot translate! Instruction name not recognized!"<<endl;
        #endif
        return 0;
    }
    return 1;
}
    

void initializeREGS()
{
    REGS["$zero"] = 0;
    REGS["$at"] = 1;
    REGS["$v0"] = 2;
    REGS["$v1"] = 3;
    REGS["$a0"] = 4;
    REGS["$a1"] = 5;
    REGS["$a2"] = 6;
    REGS["$a3"] = 7;
    REGS["$t0"] = 8;
    REGS["$t1"] = 9;
    REGS["$t2"] = 10;
    REGS["$t3"] = 11;
    REGS["$t4"] = 12;
    REGS["$t5"] = 13;
    REGS["$t6"] = 14;
    REGS["$t7"] = 15;
    REGS["$s0"] = 16;
    REGS["$s1"] = 17;
    REGS["$s2"] = 18;
    REGS["$s3"] = 19;
    REGS["$s4"] = 20;
    REGS["$s5"] = 21;
    REGS["$s6"] = 22;
    REGS["$s7"] = 23;
    REGS["$t8"] = 24;
    REGS["$t9"] = 25;
    REGS["$k0"] = 26;
    REGS["$k1"] = 27;
    REGS["$gp"] = 28;
    REGS["$sp"] = 29;
    REGS["$fp"] = 30;
    REGS["$ra"] = 31;
}
    

/****************************************************
 The instruction strctures
    R-type: 
    +------------------------------------------------+
    | opcode | rs    | rt    | rd    | shamt | funct |
    |--------+-------+-------+-------+-------+-------|
    | 6-bit  | 5-bit | 5-bit | 5-bit | 5-bit | 6-bit |
    +------------------------------------------------+
    I-type:
    +------------------------------------------------+
    | opcode | rs    | rt    | immediate             |
    |--------+-------+-------+-----------------------|
    | 6-bit  | 5-bit | 5-bit | 16-bit                |
    +------------------------------------------------+
    J-type:
    +------------------------------------------------+
    | opcode | jump address                          |
    |--------+---------------------------------------|
    | 6-bit  | 26-bit                                |
****************************************************/