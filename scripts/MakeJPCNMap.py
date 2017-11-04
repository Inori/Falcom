import Common

def MakeStrDic(psp_jp_files, pc_cn_files):

    if len(psp_jp_files) != len(pc_cn_files):
        print('File num not match')
        input()
        return

    dst_dic = dict()

    for fn_jp, fn_cn in zip(psp_jp_files, pc_cn_files):

        if Common.BaseName(fn_jp) != Common.BaseName(fn_cn):
            print('Filename not match: {} {}'.format(fn_jp, fn_cn))
            input()
            exit(1)

        jp = open(fn_jp, 'r', encoding='utf16')
        cn = open(fn_cn, 'r', encoding='utf16')

        lines = jp.readlines()
        jp_lines = [line.rstrip('\n') for line in lines if line != '' and line != '\n']
        lines = cn.readlines()
        cn_lines = [line.rstrip('\n') for line in lines if line != '' and line != '\n']

        if len(jp_lines) != len(cn_lines):
            input('Line num not match: {}'.format(Common.BaseName(fn_jp)))
            exit(1)

        for jp_line, cn_line in zip(jp_lines, cn_lines):
            offset, size, jp_text = Common.ParseWQSGLine(jp_line)
            offset, size, cn_text = Common.ParseWQSGLine(cn_line)

            if (not jp_text) and (not cn_text):
                continue

            jp_group = Common.SplitString(jp_text)
            cn_group = Common.SplitString(cn_text)

            if len(jp_group) != len(cn_group):
                print('Group not match {}--{}'.format(jp_text, cn_text))
                continue

            for key, value in zip(jp_group, cn_group):
                dst_dic[key] = value

        jp.close()
        cn.close()

    return dst_dic


def IsAlNum(string):
    for ch in string:
        if ord(ch) >= 0x20 and ord(ch) <= 0x7F:
            continue
        else:
            return False
    return True


def ReadVitaGroups(vita_fn_list):

    dst_list = []
    for fn in vita_fn_list:
        src = open(fn, 'r', encoding='utf16')

        _lines = src.readlines()
        lines = [line.rstrip('\n') for line in _lines if line != '' and line != '\n']

        for line in lines:
            offset, size, text = Common.ParseWQSGLine(line)

            if not text:
                continue

            group = Common.SplitString(text)
            for string in group:

                if IsAlNum(string):
                    continue

                if len(string) >= 2 and \
                        ((IsAlNum(string[0]) and (not IsAlNum(string[1]))) or
                        ((not IsAlNum(string[0])) and IsAlNum(string[1]))):
                    continue

                dst_list.append(string)

        src.close()
    return dst_list


def FormatString(index, jp_line, cn_line):
    res = "○%08d○%s\n●%08d●%s\n\n"%(index, jp_line, index, cn_line)
    return res

def OutputMapFile(text_list, jpcn_dic, dst_file, report):

    idx = 0
    for key_str in text_list:
        if not key_str in jpcn_dic:
            report.write(key_str+'\n')
            continue

        value_str = jpcn_dic[key_str]
        dst_file.write(FormatString(idx, key_str, value_str))
        idx += 1


def main():

    vita_flist = Common.Walk('VITA_JP_OUTPUT')
    text_list = ReadVitaGroups(vita_flist)

    jp_flist = Common.Walk('PSP_JP_TXT')
    cn_flist = Common.Walk('PC_CN_TXT')
    jpcn_dic = MakeStrDic(jp_flist, cn_flist)

    dst = open('JpCnMap.txt', 'w', encoding='utf16')
    report = open('ImportNotMatch.txt', 'w', encoding='utf16')

    OutputMapFile(text_list, jpcn_dic, dst, report)

    dst.close()
    report.close()

    print('Process done.')


if __name__ == '__main__':
    main()