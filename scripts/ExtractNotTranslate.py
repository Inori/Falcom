import Common


def MakeJpCnDict(jp_lines, cn_lines):

    jp_lines = [line.rstrip('\n') for line in jp_lines if line != '' and line != '\n']
    cn_lines = [line.rstrip('\n') for line in cn_lines if line != '' and line != '\n']

    if len(jp_lines) != len(cn_lines):
        print('line num not match')
        exit(1)

    jp_cn_dic = {}
    for jp_line, cn_line in zip(jp_lines, cn_lines):

        o, l, jp_text = Common.ParseWQSGLine(jp_line)
        o, l, cn_text = Common.ParseWQSGLine(cn_line)

        jp_text = jp_text.rstrip('\n')
        cn_text = cn_text.rstrip('\n')

        if len(cn_text) > len(jp_text):
            print('OverLength: {}--{}'.format(jp_text, cn_text))
            continue

        jp_cn_dic[jp_text] = cn_text

    return jp_cn_dic


def main():

    jp_fname = 'psp_EBOOT_jp.BIN.txt'
    cn_fname = 'psp_EBOOT_cn.BIN.txt'
    src_fname = 'eboot.beauty.txt'
    dst_fname = 'vita_eboot_cn.txt'
    report_fname = 'ReportNotTranslate.txt'

    with open(jp_fname, 'r', encoding='utf16') as jp_file, open(cn_fname, 'r', encoding='utf16') as cn_file:
        jp_cn_dic = MakeJpCnDict(jp_file.readlines(), cn_file.readlines())

    with open(src_fname, 'r', encoding='utf16') as src, open(dst_fname, 'w', encoding='utf16') as dst, open(report_fname, 'w', encoding='utf16') as report:

        for line in src.readlines():
            if line == '' or line == '\n':
                continue

            if '全属性' in line:
                print(line)

            o, l, text = Common.ParseWQSGLine(line)
            text = text.rstrip('\n')



            if text in jp_cn_dic:
                cn_text = jp_cn_dic[text]
                dst.write(Common.FormatWQSGLine(o, l, cn_text) + '\n\n')
            else:
                dst.write(Common.FormatWQSGLine(o, l, text) + '\n\n')
                report.write(Common.FormatWQSGLine(o, l, text) + '\n\n')


if __name__ == '__main__':
    main()