import sys
sys.path.insert(0, 'syns/')
from run_cmd_over_ssh import parse_host_file

cn_lists = parse_host_file('/home/pzhu011/Dropbox/workspace/gfw_scripts/syns/0309_cn.csv')
ot_lists = parse_host_file('/home/pzhu011/Dropbox/workspace/gfw_scripts/syns/0309_other.csv')

ll = [ [] for x in cn_lists]
ns = [0,4,8,12,2,6]
for day in range(4):
    i=0
    ip_per_ot = [ [] for x in ot_lists]
    for l in range(len(cn_lists)):
    # for l in cn_lists:
        # print day,l
        s = ''
        n = 4 
        if day == l or (l > 3 and l-2 == day):
            n = 3
        for j in range(n):
            index = ns[l] % len(ot_lists)
            ll[l] += [index]
            ip_per_ot[index] += [l]
            s += ','.join(ot_lists[index][2:4]+[str(33456+2*j)]) + '\n'
            ns[l] += 1
        with open(cn_lists[l][3]+'_03'+str(11+day)+'.csv','w+') as f:
            f.writelines(s)
    for j in range(len(ot_lists)):
        with open(ot_lists[j][3]+'_03'+str(11+day)+'.csv','w') as f:
            f.writelines('\n'.join([ ','.join(cn_lists[x][2:4]+[str(33456+2*i)]) for i,x in enumerate(ip_per_ot[j])])+'\n')
#     print ip_per_ot
# for l in ll:
#     print l        