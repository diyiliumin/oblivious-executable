# obx

本项目由ayazumi基于oblivious开发 https://github.com/diyiliumin/oblivious 

全称oblivionis executable

在原有ob基础上会检测bash脚本块并返回，附带编号，用户选择编号来指定要运行的语句，obx会把语句预填充命令到终端输入行，用户编辑后回车执行

## 依赖与前置

obx依赖于oblivionis (ob) 的核心功能，使用前请确保已了解ob的基本用法：

### ob基础用法

使用前请熟悉你自己的文档结构，在ob命令的后面填写需要信息的标题回车查询，支持多级标题跳级匹配来缩小范围

```bash
ob ssh                  #打印标题为ssh的内容块
ob ssh 管理             #打印ssh主题下管理事项的内容块
ob <tab>                #列出所有一级标题
ob ssh <tab>            #列出所有ssh下一级标题
ob ave_mujica obli<tab> #自动补全成oblivionis
ob h<tab>               #自动补全成猴子也能懂
ob HOU<tab>             #小写首字母，大写全拼
ob 猴zYE<tab>           #支持混合
#使用建议：正常使用小写首字母即可，首字母相同时使用大写全拼
```

注意：输入拼音后需要使用tab补全成中文，enter送入查询的必须与原文档中的标题一致

如果输入的拼音组合对应两种可能性，终端补全特性将会吞掉拼音组合一样但字不一样的部分，此时请换用更精确的拼音组合或直接使用汉字

## obx使用方法

```bash
obx ssh 其他技巧 #补全与参数同ob
```
以上为例，obx将会回显：

```bash
## 其他技巧

### 查看nginx下资源访问量

\\`\\`\\`bash
zcat -f /var/log/nginx/access.log* | awk '{print $7}' | sort | uniq -c | sort -nr
\\`\\`\\`

***

#以上为文档原文

#接下来将会出现一个输入框，下面例举了在这个小分区里出现的所有bash语句
#输入数字来选择命令 可以是一个也可以是多个，多个将分别按次序执行，输入0代表什么也不做直接退出

> 1
1:zcat -f /var/log/nginx/access.log* | awk '{print $7}' | sort | uniq -c | sort -nr

#选择命令后脚本会把命令填充到输入行，可二次编辑后回车，如此处加了个head来筛选
#即使你无需改动，你也可以(且建议)对将要执行的命令进行二次审查，来保证安全性

$ zcat -f /var/log/nginx/access.log* | awk '{print $7}' | sort | uniq -c | sort -nr | head
gzip: /var/log/nginx/access.log*.gz: No such file or directory
#这里本地没装所以没效果（乐
#之后会继续填充剩下的指令，如果没有了就回到shell，使用结束
```

## 配置

与ob共享配置：

用户可以在/etc/obrc 或 ~/.obrc 中每行一条路径的写指定的md文档

oblivionis将会从这些路径中扫描

## 架构

程序由四部分组成，可执行文件，bash补全脚本，bash交互脚本，python拼音翻译器

可执行文件应该由obxsupport.c编译得来,放在/usr/local/bin/obxsupport 

交互脚本在/usr/local/bin/obx(仓库中我们叫它obx.sh以示区分，请改名并放入)

补全脚本在/etc/bash_completion.d/obx

python拼音翻译器在/usr/local/bin/ob_py.py 

部分与ob相同，共享，如果你安装了ob，无需重复安装

### ob的架构（供参考）

程序由三部分组成，可执行文件,bash脚本,python拼音翻译器

可执行文件由ob.c编译得来，放在/usr/local/bin/ob 

python拼音翻译器在/usr/local/bin/ob_py.py 

脚本在/etc/bash_completion.d/ob

如果一个用户生效,脚本应该放在~/.local/share/bash-completion/completions/ob
