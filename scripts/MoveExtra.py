# -*- coding:utf-8 -*-

import os
import shutil
import Common

    
    

#这里把不需要导出文本的二进制文件移动到PSP_EXTRA_BIN
def main():
    pc_bin_list = Common.Walk('PC_CN_BIN')

    psp_bin_list = Common.Walk('PSP_JP_BIN')

    vita_bin_list = Common.Walk('VITA_JP_BIN')


    for file in psp_bin_list:
        found = False
        for fn in pc_bin_list:
            if os.path.basename(file) in fn:
                found = True

        if not found:
            print ('PspExtra: ' + file)
            shutil.move(file, 'PSP_EXTRA_BIN')
        found = False

    for file in vita_bin_list:
        found = False
        for fn in pc_bin_list:
            if os.path.basename(file) in fn:
                found = True

        if not found:
            print('VitaExtra: ' + file)
            shutil.move(file, 'VITA_EXTRA_BIN')
        found = False


if __name__ == '__main__':
    main()
