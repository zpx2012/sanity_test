import os,sys,time,datetime,socket,threading,csv
from urllib.parse import urlparse
from subprocess import Popen, PIPE
from time import sleep
from utils import run_cmd_wtimer,run_cmd_wtimer_slient
from random import randint

# FIX_WEBSITES = [
#     'www.baidu.com',
#     'www.qq.com',
#     'www.taobao.com',
#     'www.sina.com.cn',
#     'www.weibo.com',
#     'www.sohu.com',
#     'www.163.com',
#     'www.xinhuanet.com',
#     'www.ifeng.com',
#     'www.people.com.cn',
#     'www.alipay.com',
#     'www.youku.com',
#     'www.alibaba.com',
#     'www.gmw.cn',
#     'www.aliexpress.com',
#     'www.ku6.com',
#     'www.china.com',
#     'www.jd.com',
#     'www.iqiyi.com',
#     'www.toutiao.com',
#     'miui.com',
#     'bilibili.com',
#     '360.cn',
#     'douban.com',
#     'so.com',
#     'zhihu.com',
#     'bing.com',
#     'hao123.com',
#     '58.com',
#     'csdn.net',
#     'eastday.com',
#     'amemv.com',
#     'sogou.com',
#     'sm.cn',
#     'yidianzixun.com'
# ]

# RAMDON_WEBSITES = [
#     'https://detail.tmall.com/item.htm?id={}&spm=875.7931836/B.2017079.3.66144265odTsk0&scm=1007.12144.81309.73277_0&pvid=06b6105c-1857-43f8-9d0e-3bd1308029bc&utparam=%22x_src%22:%2273277%28',
#     'https://item.taobao.com/item.htm?spm=a21bo.2017.201867-rmds-0.1.5af911d9prbsPs&scm=1007.12807.84406.100200300000004&id={}&pvid=7b82484c-7bd7-4b37-8eaf-b854d374d62f'
# ]

# ID_LEN = 12

# def visit_cn_websites_sleep(visit_times,sec):
#     for i in range(visit_times):
#         run_cmd('set -x;curl -s https://%s > /dev/null' % FIX_WEBSITES[randint(0,len(FIX_WEBSITES)-1)])
#         sleep(sec)
#     for i in range(len(RAMDON_WEBSITES)):
#         rand = randint(10**(ID_LEN-1),(10**ID_LEN)-1)
#         run_cmd('set -x;curl -s -o /dev/null \'%s\'' % RAMDON_WEBSITES[i].format(str(rand)))
#         sleep(sec)

# def visit_cn_websites_continuous(visit_times):
#     for i in range(visit_times):
#         run_cmd('set -x;curl -s https://%s > /dev/null' % FIX_WEBSITES[randint(0,len(FIX_WEBSITES)-1)])

#     for i in range(len(RAMDON_WEBSITES)):
#         rand = randint(10**(ID_LEN-1),(10**ID_LEN)-1)
#         run_cmd('set -x;curl -s -o /dev/null \'%s\'' % RAMDON_WEBSITES[i].format(str(rand)))

def curl_poll_csv(infile_name):
    output_filename_list = []
    
    base_cmd = 'script -aqf -c \'curl -o /dev/null --limit-rate %s --speed-time 120 -LJv4k --resolve "%s:%d:%s" "%s"\' %s'

    out_dir = os.path.expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if not os.path.isfile(infile_name):
        print('File does not exist.')
    else:
        with open(infile_name,'r') as f:         
            domain_ip_list = list(csv.reader(f))
        for i,line in enumerate(domain_ip_list):
            output_file_name = out_dir + '_'.join(['curl',socket.gethostname(),line[2],line[0].split(':')[0],datetime.datetime.utcnow().strftime('%m%d%H%Mutc')]) +'.txt'
            output_filename_list.append(output_file_name)
            # os.system('traceroute -A {} > {}'.format(line[1],output_file_name.replace('curl','tr')),)
        num_tasks = 1 
        while True:
            for i,line in enumerate(domain_ip_list):
                cmd = base_cmd % (line[3],urlparse(line[0]).netloc, 443 if line[0].split(':')[0] == 'https' else 80,line[1],line[0],output_filename_list[i])
                with open(output_filename_list[i],'a') as f:
                    f.writelines('\n\n%s Task : %d\n %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), num_tasks, cmd))
                print('\n\n%s Task : %d\n %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), num_tasks, cmd))
                run_cmd_wtimer_slient(cmd,int(line[4]))
                print('curl poll: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')
                #visit_cn_websites(8)
                num_tasks += 1



if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:\n\tpython curl_downloader.py [URL] [IP] [Tool] [Site]\n\nOptions:\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)

    infile_name = os.path.expanduser(sys.argv[1])

    decorator = '\n********************************\n'
    print(decorator + 'Curl Downloader 1.1.4\nCtrl-C to terminate the program' + decorator + '\n')

    curl_poll_csv(infile_name)
    
