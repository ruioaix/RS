import re
def k(fn): 
    with open(fn) as fo:
        lines = fo.readlines()
        R = RL = PL = HL = IL = NL = L1 = L2 = L3 = PL0 = PL1 = PL2 = PL3 = 0
        TL0 = TL1 = TL2 = TL3 = 0
        k = 0
        count = 0
        for line in lines:
            if "METRICS" in line:
                count += 1
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
        R /=  count
        RL /= count
        PL /= count
        HL /= count
        IL /= count
        NL /= count
        L1 /= count
        L2 /= count
        L3 /= count
        PL0/= count
        PL1/= count
        PL2/= count
        PL3/= count
        TL0/= count
        TL1/= count
        TL2/= count
        TL3/= count
        line = "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n".format(R, RL, PL, HL, IL, NL, L1, L2, L3, PL0, PL1, PL2, PL3, TL0, TL1, TL2, TL3)
        return line

with open("res", "w") as fo:
    fo.write("hp\tR\tRL\tPL\tHL\tIL\tNL\tL1\tL2\tL3\tPL0\tPL1\tPL2\tPL3\tTL0\tTL1\tTL2\tTL3\n")
    for id in range(0, 41):
        fn = str(id * 0.05)
        fn = re.sub(".[0]+$", '', fn);
        fo.write("{}\t".format(fn));
        fo.write(k(fn))
