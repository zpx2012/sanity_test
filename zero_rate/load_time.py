#!/usr/bin/env python

import os
import sys
import subprocess
import time
import urllib2
import traceback,socket,datetime,logging
from selenium import webdriver

GOOD_INTL_WEBSITES = {
    'yandex': 'https://www.yandex.ru/',
    'ebay': 'https://www.ebay.com/',
    'mail': 'https://www.mail.ru/',
    'github': 'https://www.github.com/',
    'sciencedirect': 'https://www.sciencedirect.com/',
    'springer' : 'https://www.springer.com/',
    'nih.gov' : 'https://www.nih.gov',
    'naver.net' : 'https://ww.naver.net',
}

GOOD_INTL_FILES = {
    'yandex': 'https://an.yandex.ru/resource/context_static_r_8394.js',
    'ebay': 'https://developer.ebay.com/devzone/codebase/javasdk-jaxb/ebaysdkjava1085.zip',
    'mail': 'https://rfr.agent.mail.ru/magent.exe',
    'github': 'https://codeload.github.com/scipy/scipy/zip/master',
    'sciencedirect': 'https://holdings.sciencedirect.com/holdings/productReport.url?packageId=&productId=34&downloadId=1539921713612',
    'springer' : 'https://link.springer.com/content/pdf/10.1007%2Fs10853-018-2650-4.pdf',    
    'nih.gov' : 'https://obssr.od.nih.gov/wp-content/uploads/2018/03/OBSSR_Festival_Report_2017.pdf',
    'naver.net' : 'http://update.whale.naver.net/downloads/installers/naver-whale-stable_amd64.deb',
}


start_time = None


def start_tcpdump():
    print("Starting tcpdump...")
    p = subprocess.Popen(["sudo", "tcpdump", "-i", "any", "-w", os.path.expanduser("~/sanity_test/rs/pktdump.pcap.%s" % (start_time)), "tcp port 443"])
    return p

def stop_tcpdump(p):
    print("Stopping tcpdump...")
    os.system("kill %d" % p.pid)


def curl_timed(url,hn,st,sec,src_p=None):
    print 'curl timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = [os.path.expanduser('~')+'/sanity_test/zero_rate/curl_loop.sh',url,hn,sec,'https',st]
    p = subprocess.Popen(cmd) #stdout=subprocess.PIPE, stderr=subprocess.PIPE
    return p

def test_website_browser(website, url, sec):
    print("%s:Testing website %s..." % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),website)) 

    try:
        options = webdriver.firefox.options.Options()
        # options.add_argument("--headless")
        print("%s:options.add_argument(\"--headless\")" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))

        driver = webdriver.Firefox(firefox_options=options)
        driver.set_page_load_timeout(sec)
        print("%s:driver.set_page_load_timeout(sec)" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        # p = curl_timed(GOOD_INTL_FILES[website], website, start_time, sec)
        print("%s:p = curl_timed" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        start_stamp = time.time()
        flag = False
        print("%s:flag = False" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        print("%s:before driver.get(url)" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        driver.get(url)
        print("%s:after driver.get(url)" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))

    except (KeyboardInterrupt, SystemExit):   
        sys.exit(0)

    except: 
        print("%s:except" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        logging.debug(website + driver.title + ', timeout')
        logging.debug(traceback.format_exc())
        print '###\n%s' % traceback.format_exc() 

    else:
        print("%s:page loaded" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        logging.info(website + ', ' + driver.title)
        print driver.title
        if driver.title and driver.title != 'Problem loading page':
            flag = True
        time.sleep(time.time() - start_stamp)

    finally:
        print("%s:test website ends" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))         
        driver.quit()
        p.kill()
        os.killpg(os.getpgid(p.pid), signal.SIGKILL)
        return flag
    print 'skipped'


def test_group(target, ping_out, browser_out):
    for website, url in target.iteritems():
        try:
            flag = False
            print("%s:before test_website_browser" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
            ret_browser = test_website_browser(website, url, 90)
            print ret_browser
            print("%s:after test_website_browser" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
            # if ret_browser == False or ret_urllib2 == 'timeout':
            p = subprocess.Popen(['ping', '-c','50', '-i','0.2', website], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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
            print

        except (KeyboardInterrupt, SystemExit):   
            sys.exit(0)
        except:
            logging.debug(traceback.format_exc())

def test_websites():
    global start_time

    # p = start_tcpdump()
    # time.sleep(1)

    intl_ping_out = '%s/sanity_test/rs/ping_intl_%s_%s.txt' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    intl_out = '%s/sanity_test/rs/penalty_intl_%s_%s.csv' % (os.path.expanduser('~'),socket.gethostname(),start_time)
    
    try:
        while True:
            test_group(GOOD_INTL_WEBSITES,intl_ping_out,intl_out)

    except (KeyboardInterrupt, SystemExit):   
        # stop_tcpdump(p) 
        sys.exit(0)

if __name__ == "__main__":
    start_time = time.strftime("%Y%m%d%H%M%S")
    logging.basicConfig(filename=os.path.expanduser('~/sanity_test/rs/penalty_%s_%s.log' % (socket.gethostname(), start_time)), format='%(asctime)s,%(levelname)s, %(message)s', stream=sys.stdout)
    test_websites()