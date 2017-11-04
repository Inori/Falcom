import Common
import os



def MakeStrDic(psp_files):
    dst_dic = dict()
    for fn in psp_files:
        src = open(fn, 'r', encoding='utf16')

        for line in src.readlines():
            if line == '' or line == '\n':
                continue

            line = line.rstrip('\n')
            offset, size, text = Common.ParseWQSGLine(line)

            if not text:
                continue

            str_list = Common.SplitString(text)
            for s in str_list:
                dst_dic[s] = os.path.basename(fn)

        src.close()
    return dst_dic


def IsAlNum(string):
    for ch in string:
        if ord(ch) >= 0x20 and ord(ch) <= 0x7F:
            continue
        else:
            return False
    return True

def main():

    report = open('NotMatch.txt', 'w', encoding='utf16')

    vita_files = Common.Walk('VITA_JP_OUTPUT')
    psp_files = Common.Walk('PSP_JP_TXT')

    psp_txt_dic = MakeStrDic(psp_files)
    vita_txt_dic = MakeStrDic(vita_files)

    pre_list = []
    for string in vita_txt_dic.keys():
        if not string in psp_txt_dic:
            pre_list.append((vita_txt_dic[string], string))

    for fname, string in pre_list:
        if len(string) == 1:
            continue

        if IsAlNum(string):
            continue

        if (IsAlNum(string[0]) and (not IsAlNum(string[1]))) or ((not IsAlNum(string[0])) and IsAlNum(string[1])):
            continue

        if fname[1] == '_':
            continue

        log = '{}:{}\n'.format(fname, string)
        print(log)
        report.write(log)



    report.close()
    print('Parse finish.')


if __name__ == '__main__':
    main()