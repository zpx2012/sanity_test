#!/usr/bin/env python

import os
import sys
import subprocess
import time
import urllib2
import traceback,socket,datetime,logging
from selenium import webdriver



MAX_PROC_NUM = 10
JAIL_TIME = 92

GOODWORD = 'goodword'
BADWORD = 'ultrasurf'


BAD_INTL_WEBSITES = {
    'www.google.com' : 'https://www.google.com',
    'www.facebook.com' : 'https://www.facebook.com',
}

GOOD_INTL_WEBSITES = {
    'www.nba.com' : 'https://www.nba.com',
    'www.bing.com': 'https://www.bing.com/',
    'www.vk.com': 'https://www.vk.com/',
    'www.linkedin.com': 'https://www.linkedin.com/',
    'www.yandex.ru': 'https://www.yandex.ru/',
    'www.ebay.com': 'https://www.ebay.com/',
    'www.stackoverflow.com': 'https://www.stackoverflow.com/',
    'www.mail.ru': 'https://www.mail.ru/',
    'www.github.com': 'https://www.github.com/',
    'www.sciencedirect.com': 'https://www.sciencedirect.com/',
    'www.springer.com' : 'https://www.springer.com/',
}

CN_WEBSITES = {
    'www.baidu.com' : 'https://www.baidu.com',
    'www.qq.com' : 'https://www.qq.com',
    'www.taobao.com' : 'https://www.taobao.com',
}


start_time = None

result = {}
jail_time = {}
target_domains = {}
target_ips = {}


def start_tcpdump():
    print("Starting tcpdump...")
    p = subprocess.Popen(["tcpdump", "-i", "any", "-w", "~/sanity_test/results/pktdump.pcap.%s" % (start_time), "tcp port 443"])
    return p

def stop_tcpdump(p):
    print("Stopping tcpdump...")
    os.system("kill %d" % p.pid)


def test_website_browser(website, url, sec):
    print("Testing website %s..." % website) 
    
    options = webdriver.firefox.options.Options()
    options.add_argument("--headless")
    driver = webdriver.Firefox(firefox_options=options)
    driver.set_page_load_timeout(sec)
    try:
        driver.get(url)
        logging.info(website + ', ' + driver.title)

    except: 
        logging.debug(website + ', timeout')
        print '###\n%s' % traceback.format_exc() 

    finally:
        print 'final', driver.title
        title = driver.title
        driver.quit()
        if title and title != 'Problem loading page':
            return True
        return False


def test_website_urllib2(url):
    print("Testing website %s..." % url) 
    #pwget = subprocess.Popen("wget -4 -O /dev/null --tries=1 --timeout=5 --max-redirect 0 \"%s\"" % (url + KEYWORD), shell=True)
    #testing[website] = pwget
    # request = urllib2.Request("https://%s/%s%s" % (target_ips[website], TARGETS[website][7:].split('/', 1)[1], KEYWORD),
    #                           headers = {'Host': target_domains[website]})
    request = urllib2.Request(url, headers = {'Host': url[8:].split('/', 1)[0]})

    try:
        obj = urllib2.urlopen(request, timeout=10)
        retcode = obj.getcode()
        return 'success'
    except urllib2.HTTPError as herr:
        # 404
        if herr.code == 404:
            return '404'
        if herr.code == 301:
            return 'success'
        else:
            return str(herr.code)
    except urllib2.URLError as uerr:
        # timeout
        if isinstance(uerr.reason, socket.timeout):
            return 'timeout'
        else:
            return 'urlerror'
    except socket.error as serr:
        # reset
        return 'reset'

def test_group(target, ping_out, browser_out):
    for website, url in target.iteritems():
        flag = False
        ret_browser = test_website_browser(website, url,90)
        ret_urllib2 = test_website_urllib2(url)
        print ret_browser, ret_urllib2
        if ret_browser == False or ret_urllib2 == 'timeout':
            p = subprocess.Popen(['ping', '-c','10', website], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = p.communicate()
            print out
            print err
            if '100% packet loss' in out+err:
                flag = True
            with open(ping_out,'a') as outf:
                outf.writelines('ping %s\n' % website)
                outf.writelines(out+'\n'+err)
        with open(browser_out,'a') as outf:
            outf.writelines(','.join([datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'), website, str(ret_browser),ret_urllib2,str(flag)])+'\n')

def test_websites():
    global start_time

    p = start_tcpdump()
    time.sleep(1)
    testing = {}

    intl_ping_out = '%s/sanity_test/rs/ping_intl_%s_%s.txt' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    intl_out = '%s/sanity_test/rs/penalty_intl_%s_%s.csv' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    cn_ping_out = '%s/sanity_test/rs/ping_cn_%s_%s.txt' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    cn_out = '%s/sanity_test/rs/penalty_cn_%s_%s.csv' % (os.path.expanduser('~'),socket.gethostname(),start_time)

    # urllib2_out = '%s/sanity_test/rs/penalty_urllib2_%s_%s.csv' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    
    try:
        while True:
            for website, url in BAD_INTL_WEBSITES.iteritems():
                test_website_browser(website, url,10)
            test_group(GOOD_INTL_WEBSITES,intl_ping_out,intl_out)
            test_group(CN_WEBSITES,cn_ping_out,cn_out)

    except (KeyboardInterrupt, SystemExit):   
        stop_tcpdump(p) 
        sys.exit(0)

if __name__ == "__main__":
    start_time = time.strftime("%Y%m%d%H%M%S")
    logging.basicConfig(filename=os.path.expanduser('~/sanity_test/rs/penalty_%s_%s.log' % (socket.gethostname(), start_time)), level=logging.INFO, format='%(asctime)s,%(levelname)s, %(message)s')
    test_websites()