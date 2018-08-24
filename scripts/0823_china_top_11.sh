screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://upload.wikimedia.org/wikipedia/commons/a/a8/LOC_Main_Reading_Room_Highsmith.jpg' '208.80.153.240' https wiki_highsmith;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/scipy/scipy/archive/master.zip' '192.30.255.112' https github_scipy;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' '23.38.126.22' https amazon;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/C4E00AQEOf4NNZp_DSA/23033a33657347f2a6f2a1140146d656/feedshare-mp4_3300/1488578169071-zmy00q?e=1535169600&v=beta&t=1I0qpmHJXqzg66BYzLwxQQXHpAJER0XENgDR6ORQYxg' '23.38.127.219' https linkedin;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BYzgyYzM1YTUtNWU4YS00ODdjLThhYjgtYjgwZjQ3NTI1ZTQ3XkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1535168450&Signature=XZxs5Q~tpBeogKcx6jS5To0ezNIx0fYfHdTMxvUswXJHoJObFfru5msmpfgJ-o5kZCcVNcOWFQ~apmpSlu9fNEUCmOfFqK84nUz8eqon2k~9qrYPjVHudbA6EBH1~l2rr24Zlu83h~nASq0LQ6GJFeEqHxq7PogXOrzBDAORBXY_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' '54.192.232.51' https imdb;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://www.apple.com/105/media/us/home/2018/da585964_d062_4b1d_97d1_af34b440fe37/films/behind-the-mac/mac-behind-the-mac-tpl-cc-us-2018_1280x720h.mp4' '23.220.145.191' https apple ;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://holdings.sciencedirect.com/holdings/report.url?_acctId=228598&_prodId=3&_platform=SD&_site=science&_env=SD&_userId=12975512&md5=38bcda18353d6407ac5ef9343f7d82a5&format=CSV&downloadId=1535062952790' '52.3.105.19' https sciencedirect;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://obssr.od.nih.gov/wp-content/uploads/2018/03/OBSSR_Festival_Report_2017.pdf' '137.187.172.42' https nih.gov;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://noaanhc.files.wordpress.com/2017/07/outreach1.jpg' '192.0.72.24' https wordpress;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://graphics8.nytimes.com/packages/flash/nyregion/20110424_guastavino/panaa057Panorama.jpg' '151.101.1.164' https nytimes;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://link.springer.com/content/pdf/10.1007%2Fs10853-018-2650-4.pdf' '151.101.24.95' https springer;exec bash"
screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py upload.wikimedia.org wiki'' github; exec bash"