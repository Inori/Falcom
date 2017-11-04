import Common


def CheckOnePair(fn_jp, fn_cn, report):
    jp = open(fn_jp, 'r', encoding='utf16')
    cn = open(fn_cn, 'r', encoding='utf16')

    lines = jp.readlines()
    jp_lines = [line for line in lines if line != '' and line != '\n']
    lines = cn.readlines()
    cn_lines = [line for line in lines if line != '' and line != '\n']

    if len(cn_lines) != len(jp_lines):
        log = 'Line num not match:{}\n'.format(Common.BaseName(fn_jp))
        report.write(log)
        return

    for jp_line, cn_line in zip(jp_lines, cn_lines):
        offset, size, jp_text = Common.ParseWQSGLine(jp_line.rstrip('\n'))
        offset, size, cn_text = Common.ParseWQSGLine(cn_line.rstrip('\n'))

        jp_group_count = len(Common.SplitString(jp_text))
        cn_group_count = len(Common.SplitString(cn_text))

        if jp_group_count != cn_group_count:
            log = 'Group num not match: {}:{}\t{}:{}\n'.format(fn_jp, jp_text, fn_cn, cn_text)
            report.write(log)


    jp.close()
    cn.close()



def main():
    cn_files = Common.Walk('PC_CN_TXT')
    jp_files = Common.Walk('PSP_JP_TXT')

    if len(cn_files) != len(jp_files):
        print('File num not match')
        return

    report = open('CNJPGroupNotMatch.txt', 'w', encoding='utf16')
    for fn_jp, fn_cn in zip(jp_files, cn_files):
        print('Compare {} - {}\n', fn_jp, fn_cn)
        CheckOnePair(fn_jp, fn_cn, report)

    report.close()

    print('Process done.')



if __name__ == '__main__':
    main()