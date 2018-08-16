kill -9 $(pgrep screen)
screen -wipe
cd ~/sanity_test/
git pull
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://upload.wikimedia.org/wikipedia/commons/a/a8/LOC_Main_Reading_Room_Highsmith.jpg' 0 clean wiki_highsmith;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/C4E00AQEOf4NNZp_DSA/23033a33657347f2a6f2a1140146d656/feedshare-mp4_3300/1488578169071-zmy00q?e=1534489200&v=beta&t=cxxWeYDQT7zXvxIpJkeJ4OyUatuRQtldTQhDVziHi_A' 0 clean linkedin;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' 0 clean amazon;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BNDU3MGU1MmYtZDExNS00NTQ5LTk1ZGEtZDAxMzgxNWNhZDc0XkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1534487350&Signature=WIE-i34ndYRieMnsrFdxiHKg92GBoSKEdpWDfqPt~LuOUhXZHv3O62b3-oNPfnkliDNZ0k3EeuZIt-v-Qvib3v1MjF8Z9uWlBHE9K5ETmAdhiP0Fvp8enoL2tlPAixbPZCfT4tRy4H3zTqh~xoZsKuZTgLvoqc1Py-JxqDk0EUg_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' 0 clean imdb;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/multipath-tcp/mptcp/archive/mptcp_v0.94.zip' 0 clean github;exec bash"
traceroute -A dms.licdn.com > /dev/null
traceroute -A dms.licdn.com > ~/sanity_test_results/tr_licdn.log
traceroute -A upload.wikimedia.org > ~/sanity_test_results/tr_wiki.log
traceroute -A m.media-amazon.com > ~/sanity_test_results/tr_amazon.log
traceroute -A video-http.media-imdb.com > ~/sanity_test_results/tr_imdb.log
traceroute -A github.com > ~/sanity_test_results/tr_github.log
screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py github.com github; exec bash"