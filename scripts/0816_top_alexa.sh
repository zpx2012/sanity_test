kill -9 $(pgrep screen)
screen -wipe
cd ~/sanity_test/
git pull
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://upload.wikimedia.org/wikipedia/commons/a/a8/LOC_Main_Reading_Room_Highsmith.jpg' 0 clean wiki_highsmith;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/C4E00AQEOf4NNZp_DSA/23033a33657347f2a6f2a1140146d656/feedshare-mp4_3300/1488578169071-zmy00q?e=1534525200&v=beta&t=qeNh1UhF9AFHL9e9mx_9YJq_0U0CQdIiDf5jKNOzYr4' 0 clean linkedin;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' 0 clean amazon;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BZTNhZjk0ODktMTIwMy00Yjk2LTk3NTQtMGI3M2VhZDAwYzlkXkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1534522017&Signature=Xe5bNXg5xDSIr98TlQIWA-kX-Kwrud9UOj3nV-Q2YBVrmtSRWG~1STOc-CgrbzZalSjFi1FPFXzD1EUmi0-9qUC8CG-qdzof-aBxWgD32XUg~88rsDeWeA6C12eTSN7k8eM6xzS36-3a-YDOvo4PitvQeykP7Bhorufp5AgRYYc_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' 0 clean imdb;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/multipath-tcp/mptcp/archive/mptcp_v0.94.zip' 0 clean github;exec bash"
traceroute -A dms.licdn.com > /dev/null
traceroute -A dms.licdn.com > ~/sanity_test/rs/tr_licdn.log
traceroute -A upload.wikimedia.org > ~/sanity_test/rs/tr_wiki.log
traceroute -A m.media-amazon.com > ~/sanity_test/rs/tr_amazon.log
traceroute -A video-http.media-imdb.com > ~/sanity_test/rs/tr_imdb.log
traceroute -A github.com > ~/sanity_test/rs/tr_github.log
screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py github.com github; exec bash"