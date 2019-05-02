import struct

def GetSJISChar(code):
    if code <= 0x7F:
        char = chr(code)
    else:
        if code >= 0xA1 and code <= 0xDF:
            bs = bytes([code&0xFF])
        else:
            bs = bytes([(code >> 8)&0xFF, code&0xFF])

        try:
            char = bs.decode('shift_jis_2004')
        except:
            print(bs)
            char = '?'

    if len(char) != 1:
        return ''
    return char

def FormatLine(code, char):

    code_str = ''
    for c in code:
        code_str += '{:2X}'.format(c)

    line = "{0:}={1:}\n".format(code_str, char)
    return line

def WriteLine(dst, code):
    char = GetSJISChar(code)
    if not char:
        return

    if code <= 0xFF:
        bin_code = struct.pack('B', code)
    else:
        bin_code = struct.pack('>H', code)

    try:
        utf8_code = bin_code.decode('shift_jis_2004').encode('utf8')
    except:
        print(bin_code)
        return

    line = FormatLine(utf8_code, char)
    dst.write(line)


def main():

    dst = open('UTF8Table.txt', 'w', encoding='utf16', newline='\r\n')
    for code in range(0x20, 0x7F):
        WriteLine(dst, code)

    for code in range(0xA1, 0xDF+1):
        WriteLine(dst, code)

    for first_byte in range(0x81, 0x9F+1):
        for second_byte in range(0x40, 0x7E+1):
            code = first_byte << 8 | second_byte
            WriteLine(dst, code)
        for second_byte in range(0x80, 0xFC+1):
            code = first_byte << 8 | second_byte
            WriteLine(dst, code)

    for first_byte in range(0xE0, 0xEF+1):
        for second_byte in range(0x40, 0x7E+1):
            code = first_byte << 8 | second_byte
            WriteLine(dst, code)

        for second_byte in range(0x80, 0xFC+1):
            code = first_byte << 8 | second_byte
            WriteLine(dst, code)

    dst.close()


if __name__ == '__main__':
    main()