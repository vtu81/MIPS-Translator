# MIPS_translator -- by VTU

该程序为MIPS格式下的（伪）汇编器。

+ 有两种工作模式

  + 用户输入一条（行）MIPS指令，汇编器就会将其翻译为一条机器码；输出带有交互。（**适合单条指令的翻译**）

    编译及运行命令:

    ``` bash
    g++ MIPS_translator -o MIPS_translator
    ./MIPS_translator
    ```

    该模式下会有交互输出，不适合批量处理汇编代码。

  + 用户可以直接将连续的汇编代码作为输入，不包含多余的交互显示/输出。（**适合批量的汇编代码翻译**）

    编译及运行命令:

    ``` bash
    g++ MIPS_translator -D MIPSFILE -o MIPS_translator
    ./MIPS_translator
    ```

    推荐编写脚本，将包含汇编代码的文本文件作为标准输入，输出至外部的另一个文本文件）。

    一个极其简单的Windows批处理命令`test.bat`文件已经在根目录下提供，可以用同目录下的`in.txt`作为标准输入，并输出至`out.txt`中。其他系统的脚本同样简单，请自行编写。

    需要注意的是：如果有不合法的指令、格式，错误信息会以#开头显示对应的行中以提醒用户；请注意检查输出。

+ 目前支持的指令有：`add` `sub` `and` `or` `nor` `xor` `slt` `srl` `jr` `jalr` `lw` `sw` `beq` `bne` `addi` `andi` `ori` `xori` `slti` `lui` `j` `jal`

+ 默认每行输入是一条指令或一个标签

+ 能够读入不带分号指令，允许在末尾添加以#开头的注释（如`add $t1, $t2, $t3 #Test assmbly code`）

+ 能够自动识别标签，不区分大小写（如`Start:`）

+ 立即数默认十进制，但在数字前添加`0x`会自动识别为16进制数

+ `j`、`jal`指令自动识别跳转到**字节地址**（如`j 400`，400是一个字节地址！）还是**标签**（如`j Start`）

+ `lw`、`sw`、`beq`、`bne`指令中提供的的立即数认为是**字地址**

#### 附：指令格式

#####   *R-type:* 

  *| opcode | rs  | rt  | rd  | shamt | funct |*

  *| 6-bit | 5-bit | 5-bit | 5-bit | 5-bit | 6-bit |*

#####   *I-type:*

  *| opcode | rs  | rt  | immediate       |*

  *| 6-bit | 5-bit | 5-bit | 16-bit        |*

#####   *J-type:*

  *| opcode | jump address             |*

  *| 6-bit | 26-bit                |*