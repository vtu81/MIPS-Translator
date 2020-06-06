#include<iostream>
#include<iomanip>
#include<string>
#include<algorithm>
#include<sstream>
#include<map>

using namespace std;
int32_t inst; //32λָ��
string instName; //ָ������
string operandNames[3]; //����������
map<string, int32_t> REGS; //�Ĵ����������Ŷ�Ӧ��
map<string, int> TAGS; //�������е�tag��Ӧ���ֵ�ַ
int instAddr = 0; //��ǰָ����ֵ�ַ����ÿ��һ��ָ�instAddr����1��

//��stringת��ΪСд
inline string strtolower(std::string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

//stringת��Ϊint32_t�������Ƿ���'0x'�Զ����16����/10���ƣ�
inline int32_t str2int(string t)
{
    t = strtolower(t);
    if(t.length() >= 2 && t[0] == '0' && t[1] == 'x')
    {
        int32_t tmp;
        istringstream ss(t); //����string��
        ss >> hex >> tmp;
        return tmp;
    }
    return atoi(t.c_str());
}

//�������3���ö��ż����targetNum��<=3���������ָ��ŵ�ȫ�ֱ���operandNames[]��
int divideAllOperandsWithCommas(string& allOperands, int targetNum)
{
    int cnt = 0;
    int comment_offset = -1;
    string tmp;

    //���ע��
    comment_offset = allOperands.find_first_of('#');
    if(comment_offset != string::npos) allOperands.erase(comment_offset);
    //�������滻Ϊ�ո�
    replace(allOperands.begin(), allOperands.end(), ',', ' '); 
    //����string��
    istringstream ss(allOperands); 

    //�ָ�allOperands�ַ���
    while(ss>>tmp)
    {
        if(cnt < targetNum)
        {
            operandNames[cnt] = tmp; //����operandNames�����0��1��2��λ��
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

//�жϲ�����tag
int saveTags(string& allOperands)
{
    int colon_offset = -1;
    //�ҵ�һ��ð��
    colon_offset = allOperands.find_first_of(':');
    //���û��ð�ţ���һ������һ��tag
    if(colon_offset == string::npos)
         return 0;
        
    //���ð�Ų��������һ������һ��tag
    if(colon_offset != allOperands.length() - 1)
         return 0;
    
    //������������Ļ�����һ���Ϸ���tag
    allOperands.erase(colon_offset); //ֻ��������
    TAGS[allOperands] = instAddr; //������TAGS��map�У��Ժ���ܻ��õ�
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
        //����ָ������
        if(!(cin>>instName))
        {
            #ifndef MIPSFILE
            cout<<"Goodbye"<<endl;
            #endif
            break;
        }
        instName = strtolower(instName); //ת��ΪСд

        if(instName == "quit")
        {
            #ifndef MIPSFILE
            cout<<"Goodbye"<<endl;
            #endif
            break;
        }

        if(!readInst(instName)) continue; //����ָ��

        instAddr++; //���ںϷ�ָ�������tag����ÿ�η�������ֵ�ַ++

        #ifndef MIPSFILE
        cout<<hex<<"instruction: "<<setfill('0')<<setw(8)<<inst<<endl; //���ָ��
        #endif
        
        #ifdef MIPSFILE
        cout<<hex<<setfill('0')<<setw(8)<<inst<<endl; //���ָ�ֱ�ӽ�������������ļ��������������˵����
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
        
        getline(cin, allOperands); //�������ʣ������
        
        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;

        //��ȡ�Ĵ�������
        int32_t rs, rt, rd;
        rs = REGS[operandNames[1]];
        rt = REGS[operandNames[2]];
        rd = REGS[operandNames[0]];

        //���Ĵ�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= (rs << 21);
        inst |= (rt << 16);
        inst |= (rd << 11);


        //���Ŵ���func
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
        getline(cin, allOperands); //�������ʣ������
        
        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //��ȡ�Ĵ������롢shamt
        int32_t rt, rd, shamt;
        shamt = str2int(operandNames[2]);
        rt = REGS[operandNames[1]];
        rd = REGS[operandNames[0]];

        //���Ĵ������롢shamt�ŵ�inst��Ӧλ��
        inst = 0;
        inst |= (shamt << 6);
        inst |= (rt << 16);
        inst |= (rd << 11);

        //���Ŵ���func
        int32_t func = 2; //�����srl��������չ�����ж����
        inst |= func;
    }
    else if(instName == "jr")
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 1)) return 0;

        //��ȡ�Ĵ�������
        int32_t rs;
        rs = REGS[operandNames[0]];

        //���Ĵ�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= (rs << 21);


        //���Ŵ���func
        int32_t func = 8; //�����jr��������չ�����ж����
        inst |= func;
    }
    else if(instName == "jalr")
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 2)) return 0;
        
        //��ȡ�Ĵ�������
        int32_t rs, rt, rd;
        rs = REGS[operandNames[0]];
        rd = REGS[operandNames[1]];

        //���Ĵ�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= (rs << 21);
        inst |= (rd << 11);


        //���Ŵ���func
        int32_t func = 9; //�����jalr��������չ�����ж����
        inst |= func;
    }
    else if(instName == "lw" || instName == "sw")
    {
        // int numOperand = 0; //������������
        // string allOperands, tmp;
        
        // getline(cin, allOperands); //�������ʣ������
        // replace(allOperands.begin(), allOperands.end(), ',', ' '); //�������滻Ϊ�ո�
        // replace(allOperands.begin(), allOperands.end(), '(', ' ');
        // replace(allOperands.begin(), allOperands.end(), ')', ' '); //�������滻Ϊ�ո�

        // istringstream ss(allOperands); //����string��
        // //�ָ�allOperands�ַ���
        // while(ss>>tmp)
        // {
        //     if(numOperand < 3)
        //     {
        //         operandNames[numOperand] = tmp; //����operandNames�����0��1��2��λ��
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
        
        getline(cin, allOperands); //�������ʣ������

        replace(allOperands.begin(), allOperands.end(), '(', ' ');
        replace(allOperands.begin(), allOperands.end(), ')', ' '); //�������滻Ϊ�ո�

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //��ȡ�Ĵ������롢������
        int32_t rs, rt, imm;
        rs = REGS[operandNames[2]];
        rt = REGS[operandNames[0]];
        imm = str2int(operandNames[1]);

        //���Ĵ������롢�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= imm;
        inst |= (rs << 21);
        inst |= (rt << 16);

        //���Ŵ���opcode
        int32_t opcode;
        if(instName == "lw") opcode = 35;
        else if(instName == "sw") opcode = 43;
        inst |= (opcode << 26);
    }
    else if(instName == "beq" || instName == "bne")
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //��ȡ�Ĵ������롢������
        int32_t rs, rt, imm;
        imm = str2int(operandNames[2]);
        rs = REGS[operandNames[0]];
        rt = REGS[operandNames[1]];

        //���Ĵ������롢�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);
        inst |= (rs << 21);

        //���Ŵ���opcode
        int32_t opcode;
        if(instName == "beq") opcode = 4;
        else if(instName == "bne") opcode = 5;
        inst |= (opcode << 26);
    }
    else if(instName == "addi" || instName == "andi" || instName == "ori"\
         || instName == "xori" || instName == "slti")
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 3)) return 0;
        
        //��ȡ�Ĵ������롢������
        int32_t rs, rt, imm;
        imm = str2int(operandNames[2]);
        rt = REGS[operandNames[0]];
        rs = REGS[operandNames[1]];

        //���Ĵ������롢�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);
        inst |= (rs << 21);

        //���Ŵ���opcode
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
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 2)) return 0;
        
        //��ȡ�Ĵ������롢������
        int32_t rt, imm;
        imm = str2int(operandNames[1]);
        rt = REGS[operandNames[0]];

        //���Ĵ������롢�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= imm;
        inst |= (rt << 16);

        //���Ŵ���opcode
        int32_t opcode = 15; //�����lui��������չ�����ж����
        inst |= (opcode << 26);
    }
    else if(instName == "j" || instName == "jal")
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ������

        if(!divideAllOperandsWithCommas(allOperands, 1)) return 0;
        
        //�����ж�ʵ��ʶ���û�ʹ����tag�������ֽ�����ת��
        int32_t imm;
        if(operandNames[0][0] < '0' || operandNames[0][0] > '9')
        {
            //��һ��tag������������
            imm = TAGS[strtolower(operandNames[0])];
            #ifndef MIPSFILE
            cout << "# Tag recognized! The tag " << strtolower(operandNames[0]) << " corresponds to word address " << imm << endl; //������Ϣ
            #endif
        }
        else
        {
            //������һ�����֣�������tag
            //��ȡ�Ĵ�������
            imm = str2int(operandNames[0]) / 4;
        }


        //���Ĵ�������ŵ�inst��Ӧλ��
        inst = 0;
        inst |= imm;

        //���Ŵ���opcode
        int32_t opcode;
        if(instName == "j") opcode = 2;
        else if(instName == "jal") opcode = 3;
        inst |= (opcode << 26);
    }
    else
    {
        string allOperands;
        
        getline(cin, allOperands); //�������ʣ�����ݲ����ԣ�

        //�ж�������һ��tag
        if(saveTags(instName))
        {
            //�ǵĻ�ֱ�ӷ���0����tag������һ��ָ����ʲô���������Ҳ���������û���
            return 0;
        }

        //������һ��tag����ôһ����һ�����Ϸ����룬Ҫ�����û�
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