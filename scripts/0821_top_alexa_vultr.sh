kill -9 $(pgrep screen)
screen -wipe
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://upload.wikimedia.org/wikipedia/commons/a/a8/LOC_Main_Reading_Room_Highsmith.jpg' '208.80.153.240' https wiki_highsmith;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/multipath-tcp/mptcp/archive/mptcp_v0.94.zip' '192.30.255.112' https github;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' '23.38.126.22' https amazon;exec bash"

screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/C4E00AQEOf4NNZp_DSA/23033a33657347f2a6f2a1140146d656/feedshare-mp4_3300/1488578169071-zmy00q?e=1535007600&v=beta&t=y5405te03Whmkw5VVtAcAOxXytB3Gi0niTMQJwHYVEQ' '23.38.127.219' https linkedin;exec bash"

screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BY2FmN2I2NGQtMzI2NS00YzdiLTk2Y2YtOWIyZTEzZmJhYzg5XkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1535007546&Signature=xREp~NQN~YzLBIHoE6bGHB6cnZtidF0NMRE1ZE7NX67DWsW2gvK1D~W22D3mSFv4pxq6PvhoBQ8nABxwDttt4dSRyKUAzGBNpF5Gh8E8LKZ6RCNr63h044XGqhKaYUExui~SIP7xutsJK4env1JWB6dZkM-~AFlKVmk8g1LNZ3I_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' '54.192.232.51' https imdb;exec bash"

screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://mirror.math.princeton.edu/pub/ubuntu-iso/ubuntu-core/16/ubuntu-core-16-pi2.img.xz' '128.112.18.21' 'clean' 'princeton'"

screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 
'https://mirror.tuna.tsinghua.edu.cn/ubuntu-releases/ubuntu-core/16/ubuntu-core-16-pi2.img.xz' '101.6.8.193' clean tsinghua"

screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py '192.30.255.112' github; exec bash"