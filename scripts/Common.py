# -*- coding:utf-8 -*-

import os
import re

#遍历文件夹，返回文件列表
def Walk(adr):
    mylist=[]
    for root,dirs,files in os.walk(adr):
        for name in files:
            adrlist=os.path.join(root, name)
            mylist.append(adrlist)
    return mylist


def BaseName(fullname):
    return os.path.basename(fullname)

def SplitString(string):

    if '\\n' in string:
        string = string.replace('\\n', '{0A}')

    #以 {01} {0B} 等这种字节转义作为分隔符,将字符串分组
    pattern = re.compile('\{[0-9A-Fa-f]{2}\}')

    str_list = []
    remain = string
    mc = pattern.search(remain)
    while mc:
        s = mc.start(0)
        e = mc.end(0)

        if s != 0:
            cur_item = remain[:s]
            str_list.append(cur_item)

        remain = remain[e:]
        mc = pattern.search(remain)

    if remain:
        str_list.append(remain)

    # 去掉带有#号的opcode
    for idx, cur_string in enumerate(str_list):
        if not '#' in cur_string:
            continue

        rip_string = ''
        for ch in string:
            if ord(ch) >= 0x20 and ord(ch) <= 0x7F:
                continue
            rip_string += ch
        str_list[idx] = rip_string

    return str_list


def ParseWQSGLine(line):
    group = line.split(',', 2)
    return (group[0], group[1], group[2])