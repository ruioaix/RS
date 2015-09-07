import re
def k(fn): 
    print(fn)
    with open(fn) as fo:
        lines = fo.readlines()
        R = RL = PL = HL = IL = NL = L1 = L2 = L3 = PL0 = PL1 = PL2 = PL3 = 0
        TL0 = TL1 = TL2 = TL3 = 0
        k = 0
        for line in lines:
            if "METRICS" in line:
                parts = re.split("[ ,:]", line.strip());
                R += float(parts[18])
                RL += float(parts[22])
                PL += float(parts[26])
                HL += float(parts[30])
                IL += float(parts[34])
                NL += float(parts[38])
                L1 += float(parts[46])
                L2 += float(parts[50])
                L3 += float(parts[54])
                PL0 += float(parts[58])
                PL1 += float(parts[62])
                PL2 += float(parts[66])
                PL3 += float(parts[70])
                TL0 += float(parts[74])
                TL1 += float(parts[78])
                TL2 += float(parts[82])
                TL3 += float(parts[86])
        R /=  20
        RL /= 20
        PL /= 20
        HL /= 20
        IL /= 20
        NL /= 20
        L1 /= 20
        L2 /= 20
        L3 /= 20
        PL0/= 20
        PL1/= 20
        PL2/= 20
        PL3/= 20
        TL0/= 20
        TL1/= 20
        TL2/= 20
        TL3/= 20
        line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n".format(R, RL, PL, HL, IL, NL, L1, L2, L3, PL0, PL1, PL2, PL3, TL0, TL1, TL2, TL3)
        return line

with open("res", "w") as fo:
    fo.write("ur\tir\tR\tRL\tPL\tHL\tIL\tNL\tL1\tL2\tL3\tPL0\tPL1\tPL2\tPL3\tTL0\tTL1\tTL2\tTL3\n")
    for x in range(-8, 9):
        for y in range(-8, 9):
            iid = re.sub(".[0]+$", '', str(x * 0.25));
            idi = re.sub(".[0]+$", '', str(y * 0.25));
            fo.write("{}\t{}\t".format(iid, idi));
            fo.write(k('res_' + iid + '_' + idi))

#with open("res") as fo:
#    lines = fo.readlines()
#    with open("res1", 'w') as fow:
#        for line in lines:
#            parts = re.split('\t', line.strip())
#            x = parts[0]
#            y = parts[1]
#            PL0 = parts[9]
#            PL1 = parts[10]
#            PL2 = parts[11]
#            PL3 = parts[12]
#            nline = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n".format(R, RL, PL, HL, IL, NL, L1, L2, L3, PL0, PL1, PL2, PL3, TL0, TL1, TL2, TL3)
#            fow.write(nline)
