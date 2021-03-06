#!/usr/bin/env python

import os
import sys
import subprocess
import time
import urllib2
import traceback,socket,datetime,logging,signal
from selenium import webdriver

GOOD_INTL_WEBSITES = {
    'yandex.ru': 'https://www.yandex.ru/',
    'ebay.com': 'https://www.ebay.com/',
    'mail.ru': 'https://www.mail.ru/',
    'github.com': 'https://www.github.com/',
    'sciencedirect.com': 'https://www.sciencedirect.com/',
    'springer.com' : 'https://www.springer.com/',
    'nih.gov' : 'https://www.nih.gov',
    'naver.net' : 'https://ww.naver.net',
}

GOOD_INTL_FILES = {
    'yandex.ru': 'https://an.yandex.ru/resource/context_static_r_8394.js',
    'ebay.com': 'https://developer.ebay.com/devzone/codebase/javasdk-jaxb/ebaysdkjava1085.zip',
    'mail.ru': 'https://rfr.agent.mail.ru/magent.exe',
    'github.com': 'https://codeload.github.com/scipy/scipy/zip/master',
    'sciencedirect.com': 'https://holdings.sciencedirect.com/holdings/productReport.url?packageId=&productId=34&downloadId=1539921713612',
    'springer.com' : 'https://link.springer.com/content/pdf/10.1007%2Fs10853-018-2650-4.pdf',    
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
    print datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),':curl timed'
    cmd = [os.path.expanduser('~')+'/sanity_test/zero_rate/curl_loop.sh',url,hn,str(sec),'https',st]
    p = subprocess.Popen(cmd,stdout=subprocess.PIPE, stderr=subprocess.PIPE) #
    # out, err = p.communicate()
    return p

def test_website_browser(website, url, sec):
    print("%s:Testing website %s..." % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),website)) 
    p = curl_timed(GOOD_INTL_FILES[website], website, start_time, sec)
    load_time = sec    

    try:
        options = webdriver.firefox.options.Options()
        options.add_argument("--headless")
        driver = webdriver.Firefox(firefox_options=options)
        driver.set_page_load_timeout(sec)
        print("%s:before get url" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        start_stamp = time.time()
        driver.get(url)

    except (KeyboardInterrupt, SystemExit):   
        sys.exit(0)

    except: 
        print("%s:except" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        if driver.title:
            print("title loaded")
        logging.debug(website + driver.title + ', timeout')
        logging.debug(traceback.format_exc())
        print '###\n%s' % traceback.format_exc() 

    else:
        print("%s:page loaded" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))
        logging.info(website + ', ' + driver.title)
        print driver.title
        if driver.title and driver.title != 'Problem loading page':
            load_time = time.time() - start_stamp
        time.sleep(90 - load_time)

    finally:
        print("%s:test website ends" % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')))         
        driver.quit()
        p.kill()
        # os.killpg(os.getpgid(p.pid), signal.SIGKILL)
        # os.system('ps -ef | grep curl')
        return load_time


def test_group(target):
    isEmpty=True
    while True:
        for website, url in target.iteritems():
            try:
                ping_out = '%s/sanity_test/rs/ping_intl_%s_%s.txt' % (os.path.expanduser('~'),socket.gethostname(),start_time)
                browser_out = '%s/sanity_test/rs/penalty_intl_%s_%s_%s.csv' % (os.path.expanduser('~'),socket.gethostname(),website,start_time)
                flag = False
                ret_browser = test_website_browser(website, url, 120)
                print ret_browser
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
                    if isEmpty:
                        outf.writelines('time, load_time, ping_result\n')
                    outf.writelines(','.join([datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'), website, str(ret_browser),str(flag)])+'\n')
                print 'write done'

            except (KeyboardInterrupt, SystemExit):   
                sys.exit(0)
            except:
                traceback.format_exc()
                logging.debug(traceback.format_exc())
        isEmpty = False

if __name__ == "__main__":
    start_time = time.strftime("%Y%m%d%H%M%S")
    logging.basicConfig(filename=os.path.expanduser('~/sanity_test/rs/penalty_%s_%s.log' % (socket.gethostname(), start_time)), format='%(asctime)s,%(levelname)s, %(message)s')
    test_group(GOOD_INTL_WEBSITES)