import Common
import os

#比较一下TextOutPut_CnJpn.exe输出的文本(CN_OUTPUT)和最终导入文本(PC_CN_TXT 即完美文本)有哪些不同
def CompareOneFile(fn_out, fn_perfect, dst):
    with open(fn_out, encoding='utf16') as out, open(fn_perfect, encoding='utf16') as per:
        ln_out = 0
        for line in out.readlines():
            if line != '' and line != '\n':
                ln_out += 1

        ln_per = 0
        for line in per.readlines():
            if line != '' and line != '\n':
                ln_per += 1

        if ln_out != ln_per:
            print(fn_out)
            dst.write('{} -> OUT : {} lines -- PER : {} lines\n'.format(os.path.basename(fn_out),
                                                                      ln_out, ln_per))



def main():
    report = open('ReportDiff.txt', 'w')
    fl_out = Common.Walk('CN_OUTPUT')
    fl_per = Common.Walk('PC_CN_TXT')

    if len(fl_out) != len(fl_per):
        print('Error')
        input()

    for fn_out, fn_per in zip(fl_out, fl_per):
        CompareOneFile(fn_out, fn_per, report)

    report.close()




if __name__ == '__main__':
    main()