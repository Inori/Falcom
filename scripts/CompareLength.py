# -*- coding:utf-8 -*-

import Common

def CompareLength(psp_jp_files, pc_cn_files, report):

    if len(psp_jp_files) != len(pc_cn_files):
        print('File num not match')
        input()
        return

    for fn_jp, fn_cn in zip(psp_jp_files, pc_cn_files):

        if Common.BaseName(fn_jp) != Common.BaseName(fn_cn):
            print('Filename not match: {} {}'.format(fn_jp, fn_cn))
            input()
            exit(1)

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

            if len(jp_text) < len(cn_text):
                report.write('{} > {} -- {}\n'.format(fname, jp_text, cn_text))

            # jp_group = Common.SplitString(jp_text)
            # cn_group = Common.SplitString(cn_text)


        jp.close()
        cn.close()



def main():
    jp_flist = Common.Walk('psp_jp_dt')
    cn_flist = Common.Walk('pc_cn_dt')
    report = open('ReportLargeLength.txt', 'w', encoding='utf16')
    CompareLength(jp_flist, cn_flist, report)



if __name__ == '__main__':
    main()