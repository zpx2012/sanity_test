screen -dmS curl_cn python ~/sanity_test/curl_downloader.py 'https://mirror.tuna.tsinghua.edu.cn/ubuntu-releases/ubuntu-core/16/ubuntu-core-16-pi2.img.xz' '101.6.8.193' tsinghua 1 500k 0
screen -dmS curl_cn python ~/sanity_test/curl_downloader.py 'http://ftp.sjtu.edu.cn/ubuntu-cd/ubuntu-core/16/ubuntu-core-16-pi2.img.xz' '202.38.97.230' sjtu 1 500k 0
screen -dmS curl_cn python ~/sanity_test/curl_downloader.py 'http://mirrors.huaweicloud.com/repository/ubuntu-cdimage/ubuntu-core/16/current/ubuntu-core-16-armhf%2Braspi3.img.xz' '117.78.24.36' huawei 1 500k 0
