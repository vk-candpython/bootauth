#=================================#
# [ OWNER ]
#     CREATOR  : Vladislav Khudash
#     AGE      : 17
#     LOCATION : Ukraine
#
# [ PINFO ]
#     DATE     : 09.03.2026
#     PROJECT  : GENPASS-AUTH
#     PLATFORM : ANY
#=================================#




from sys import argv 
from os.path import basename


MSG = f'{basename(__file__)} (BIOS | UEFI) (YOUR PASSWORD)'
XOR_KEY = 0x3F




def xor(s):
    return f'BIOS PASSWORD: (  {", ".join(hex(b).upper() for b in (ord(c) ^ XOR_KEY for c in s))}  )'


def twister_hash(s):
    state    = [0] * 16
    state[0] = 0x6c69

    for i in range(1, 16):
        state[i] = ((state[i - 1] * 0x9E37) + i) & 0xFFFF

    for char in s:
        c = ord(char)

        for j in range(16):
            x = (state[j] ^ c) & 0xFFFF
            x ^= (((x >> 4) ^ (x << 3)) ^ (x << 7)) ^ (x >> 8)
            x &= 0xFFFF
            
            state[j]     = (((x << j) | (x >> (16 - j))) & 0xFFFF) ^ state[(j + 7) & 15]

        for j in range(0, 16, 4):
            t            = (state[j] ^ state[j + 1]) + (state[j + 2] * state[j + 3])
            state[j]     = t & 0xFFFF
            state[j + 3] = ((state[j + 3] << 5) | (state[j + 3] >> 11)) & 0xFFFF

    out = []

    for i in range(8):
        h = ((state[i] << 16) | state[i + 8]) & 0xFFFFFFFF
        h = ((h << 5) | (h >> 27)) & 0xFFFFFFFF
        out.extend([ (h >> 24) & 0xFF, (h >> 16) & 0xFF, (h >> 8) & 0xFF, h & 0xFF ])
    
    return f'UEFI PASSWORD: (  {", ".join(hex(b).upper() for b in out)}  )'


def main():
    if len(argv) != 3:
        print(MSG)
        exit(1)

    t, p = argv[1:]

    if t not in {'BIOS', 'UEFI'}:
        print(MSG)
        exit(1)
    
    if t == 'BIOS':
        print(xor(p))
    else:
        print(twister_hash(p))
    
    exit(0)


if __name__ == '__main__': main()
