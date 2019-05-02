import Common


def main():
    src = open('eboot.bin.org.TXT', 'r', encoding='utf16')
    dst = open('eboot.beauty.txt', 'w', encoding='utf16')

    lines = src.readlines()

    for line in lines:
        if not line or line == '\n':
            continue

        offset, len, text = Common.ParseWQSGLine(line)

        text = text.rstrip('\n')
        if not text:
            continue

        if Common.IsAlNum(text):
            continue

        dst.write(Common.FormatWQSGLine(offset, len, text) + '\n')


    src.close()
    dst.close()


if __name__ == '__main__':
    main()