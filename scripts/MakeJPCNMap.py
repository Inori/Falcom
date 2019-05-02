# -*-coding:utf-8-*-
import Common
from fuzzywuzzy import fuzz

# t_ittxt._dt和t_ittxt2._dt里
# 有类似
# 【HP5％回復 「戦闘不能」解除】{09}{09}{09}{09}\n　早くて安くて美味い。最も親しまれている東方料理。--【HP回复5％ 「战斗不能」解除】\n　简单便宜又美味，是最为普及的东方料理。
# 这样的文本，要特殊处理一下
def ProcessITTXTCNText(jp_text, cn_text):
    new_cn_text = cn_text
    if (('【' in jp_text and '】' in jp_text) or ('「' in jp_text and '」' in jp_text)) \
            and '{09}' in jp_text \
            and '\\n' in jp_text:
        pos = cn_text.find('\\n')
        new_cn_text = cn_text[:pos] + '{09}' + cn_text[pos:]
    return new_cn_text


def MakeStrDic(psp_jp_files, pc_cn_files, report):

    if len(psp_jp_files) != len(pc_cn_files):
        print('File num not match')
        input()
        return

    dst_dic = dict()
    match_dic = dict()

    for fn_jp, fn_cn in zip(psp_jp_files, pc_cn_files):

        if Common.BaseName(fn_jp) != Common.BaseName(fn_cn):
            print('Filename not match: {} {}'.format(fn_jp, fn_cn))
            input()
            exit(1)

        jp_group_list = []
        fname = Common.BaseName(fn_cn)
        print('process jp cn: {}'.format(fname))

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

            # if 't_ittxt' in fname:
            #     cn_text = ProcessITTXTCNText(jp_text, cn_text)

            jp_group = Common.SplitString(jp_text)
            cn_group = Common.SplitString(cn_text)

            if len(jp_group) != len(cn_group):
                log = 'Group not match {}:{}--{}'.format(fname, jp_text, cn_text)
                print(log)
                report.write(log + '\n')
                continue

            jp_group_list += jp_group

            for key, value in zip(jp_group, cn_group):
                dst_dic[key] = value

        match_dic[fname] = jp_group_list
        jp.close()
        cn.close()

    return dst_dic, match_dic


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
        fname = Common.BaseName(fn)
        print('process vita txt: {}'.format(fname))
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
                            ((not IsAlNum(string[0])) and IsAlNum(string[1]))) and \
                        (string[0] != ' ' and string[1] != ' '):
                    # this was originally prevent junk chars to be added
                    # since now the vita text is pretty better with almost no junk chars
                    # I just log it out and comment this line
                    # continue
                    print('alpha string:{}'.format(string))
                dst_list.append((fname, string))

        src.close()
    return dst_list


def FormatString(index, jp_line, cn_line):
    res = "○%08d○%s\n●%08d●%s\n\n"%(index, jp_line, index, cn_line)
    return res


def FuzzyMatch(src_line, fname, match_dic, report):

    lines = match_dic[fname]
    matched_line = ''
    for line in lines:
        score = fuzz.ratio(src_line, line)
        if score < 90:
            continue
        matched_line = line
        log = 'fuzzy match: {} > {} -- {}'.format(fname, src_line, matched_line)
        print(log)
        report.write(log + '\n')
        break
    return matched_line

# 有一些特殊的映射，编程处理不是很方便，直接手动放到 special_map.txt 文件里
def ParseSpecialMap():
    src = open('special_map.txt', 'r', encoding='utf16')
    _lines = src.readlines()
    src.close()
    lines = [line.rstrip('\n') for line in _lines if line]
    map_list = []
    for line in lines:
        pair = line.split('/=')
        key_str = pair[0]
        value_str = pair[1]
        map_list.append((key_str, value_str))
    return map_list

# 这里做一些修正
def FixKeyString(key_str):
    key_str = key_str.replace('――', '──')
    return key_str


def OutputMapFile(text_list, jpcn_dic, match_dic, dst_file, report_not_match, report_fuzzy_match):

    idx = 0
    for fname, key_str in text_list:
        if key_str not in jpcn_dic:
            matched_line = FuzzyMatch(key_str, fname, match_dic, report_fuzzy_match)
            if not matched_line:
                report_not_match.write('{}:{}\n'.format(fname, key_str))
                continue
            value_str = jpcn_dic[matched_line]
        else:
            value_str = jpcn_dic[key_str]

        key_str = FixKeyString(key_str)
        dst_file.write(FormatString(idx, key_str, value_str))
        idx += 1

    special_list = ParseSpecialMap()
    for key_str, value_str in special_list:
        dst_file.write(FormatString(idx, key_str, value_str))
        idx += 1

def main():

    report_not_match = open('ReportNotMatch.txt', 'w', encoding='utf16')
    report_group_error = open('ReportGroupError.txt', 'w', encoding='utf16')
    report_fuzzy_match = open('ReportFuzzyMatch.txt', 'w', encoding='utf16')

    vita_flist = Common.Walk('vita_jp_txt')
    jp_flist = Common.Walk('psp_jp_txt')
    cn_flist = Common.Walk('pc_cn_txt')

    # vita_flist = Common.Walk('vita_jp_dt')
    # jp_flist = Common.Walk('psp_jp_dt')
    # cn_flist = Common.Walk('pc_cn_dt')

    text_list = ReadVitaGroups(vita_flist)
    jpcn_dic, match_dic = MakeStrDic(jp_flist, cn_flist, report_group_error)

    dst = open('string_map.txt', 'w', encoding='utf16')
    # dst = open('sys_map.txt', 'w', encoding='utf16')

    OutputMapFile(text_list, jpcn_dic, match_dic, dst, report_not_match, report_fuzzy_match)

    dst.close()
    report_not_match.close()

    print('Process done.')


if __name__ == '__main__':
    main()