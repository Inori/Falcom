import struct, os, sys, re

'''
//脚本封包文件头
#pragma pack (1)

typedef struct _acr_header
{
	uint32_t index_count; //包含索引数，即字符串数
	uint32_t compress_flag; //是否压缩。 0没有压缩
	uint32_t compsize; //如果有压缩，为压缩后大小，否则等于orgsize
	uint32_t orgsize; //如果有压缩，为压缩前【压缩部分】大小，否则为实际大小
}acr_header;

typedef struct _acr_index
{
	uint32_t hash; //oldstr的hash值，用于map查找
	uint32_t new_str_off; //newstr 地址
	uint32_t new_str_len; //newstr 长度
}acr_index;

#pragma pack ()
'''

COMPRESS = 0
HEADERLEN = 0x10
INDEXLEN = 0xC

CODE_TABLE_FILE = 'ImportCodeTable.TBL'



# 将4字节byte转换成整数
def byte2int(byte):
    long_tuple = struct.unpack('=I', byte)
    long = long_tuple[0]
    return long


# 将整数转换为4字节二进制byte
def int2byte(num):
    return struct.pack('=I', num)



# 将txt转换成文本列表,重写此函数可以适应于不同格式的文本
def makestr(lines):
    oldstr_list = []
    newstr_list = []
    old_pat = re.compile('○[0-9A-Fa-f]+○')
    new_pat = re.compile('●[0-9A-Fa-f]+●')
    for index, line in enumerate(lines):
        if old_pat.match(line):
            oldstr_list.append(line[10:-1])

        if new_pat.match(line):
            newstr_list.append(line[10:-1])

    return oldstr_list, newstr_list


def ParseCodeTable(fname):
    ct_dic = {}
    ct = open(fname, 'r', encoding='utf16')

    lines = ct.readlines()
    for line in lines:
        if line == '' or line == '\n':
            continue

        line = line.rstrip('\n')
        code, char = line.split('=', 1)
        ct_dic[char] = bytearray.fromhex(code)

    ct.close()
    return ct_dic


def EncodeString(ct, string):
    bstring = b''
    for ch in string:
        if ch in ct:
            bstring += ct[ch]
        else:
            print('Encode error: {}'.format(ch))
            bstring += ct['?']

    return bstring


def BKDRHash(bytes):
    hash = 0
    seed = 131
    for c in bytes:
        if c != 0:
            hash = hash * seed + c
    return (hash & 0x7FFFFFFF)


def WriteString(src, oldstr_list, newstr_list, ct, report):
    index = []
    for oldstr, newstr in zip(oldstr_list, newstr_list):

        # bstring = oldstr.encode('sjis', errors='ignore')
        try:
            bstring = oldstr.encode('cp932')
        except Exception as e:
            bstring = oldstr.encode('cp932', errors='ignore')
            sjis_string = bstring.decode('cp932')
            log = '{} -- {}'.format(oldstr, sjis_string)
            report.write(log + '\n')
            print(e)
            print(log)

        hash = BKDRHash(bstring)

        newoff = src.tell()
        bstring = EncodeString(ct, newstr)
        newlen = len(bstring)
        src.write(bstring)

        index.append((hash, newoff, newlen))

    filesize = src.tell()
    return index, filesize


def WriteIndex(dst, index):
    dst.seek(HEADERLEN)
    for [hash, newoff, newlen] in index:
        dst.write(int2byte(hash))
        dst.write(int2byte(newoff))
        dst.write(int2byte(newlen))


def Main():
    text_filename = sys.argv[1]
    pac_filename = text_filename[:-4] + '.acr'

    txt = open(text_filename, 'r', encoding='utf16')
    dst = open(pac_filename, 'wb')
    report = open('ReportEncodeError.txt', 'w', encoding='utf16')

    lines = txt.readlines()

    oldstr_list, newstr_list = makestr(lines)

    if len(oldstr_list) != len(newstr_list):
        print('number not match')
        exit(1)


    ct = ParseCodeTable(CODE_TABLE_FILE)

    index_count = len(newstr_list)
    dst.seek(HEADERLEN + index_count * INDEXLEN)
    index_list, filesize = WriteString(dst, oldstr_list, newstr_list, ct, report)

    dst.seek(HEADERLEN)
    WriteIndex(dst, index_list)

    dst.seek(0)
    dst.write(int2byte(index_count))
    dst.write(int2byte(COMPRESS))
    dst.write(int2byte(filesize - HEADERLEN))
    dst.write(int2byte(filesize - HEADERLEN))

    txt.close()
    dst.close()

    print('pack map file done.')


Main()