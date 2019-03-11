import re,os,subprocess as sp,shlex,traceback

def parse_hping3_outfile(dir,file):
    try:
        lines = []
        hcnt,cnt = 0,0
        tm,lr,rtt = None,'0','0'
        with open(dir+file,'r') as inf:
            lines = filter(None,inf.read().splitlines())            
        for line_num,line in enumerate(lines):   
            if re.match('^\d+ packets transmitted, \d+ packets received, \d+% packet loss$',line):
                lr = re.findall('[-]?\d+',line)[2]
                if int(lr) > 80:
                    hcnt += 1
                cnt += 1
        if hcnt/float(cnt) > 0.8:
            fn_cells = file.split('_')
            screen_name = '_'.join(['hping3_ptr',fn_cells[5],fn_cells[2]])
            print screen_name
            p = sp.Popen(shlex.split('screen -S %s -X quit'%screen_name)).communicate()

    except:
        print '###\n%s' % traceback.format_exc()


out_dir = os.path.expanduser('~/sanity_test/rs/')
for file in os.listdir(out_dir):
    if file.startswith('hping3') and 'stderr' in file:
        parse_hping3_outfile(out_dir,file)
