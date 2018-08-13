screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/B56AQFGY2vBJpIKZQ/e08a09f6902f472a948fc66f4211b0aa/feedshare-mp4_3300/1488578169071-zmy00q?e=1534237200&v=beta&t=ey9e0fBdg1bt26y4Eev5Vksq09zxfnqRVWYT1dOAMXo' 0 clean linkedin;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' 0 clean amazon;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BYThkNmNlOWQtNDA3My00MjgzLWI2ODktNWQxODcwOGZlOGVkXkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1534142129&Signature=c7tkEzp4GJsz-fKhfLqDNFvCEOuuwOoOaBlC-jZWAAikDwQbLuIj5IvkbDB-uvK6X2JPrtCr2okCeEW1ezWYJRbNEMcfhZiRcfZvyJ9aonPOAS6n47Xmk32eTHzza9Vp~UBz6plcXOdCJ0835CC-Tb6AxJZK10C3~ISmcfXkKVs_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' 0 clean imdb;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/multipath-tcp/mptcp/archive/mptcp_v0.94.zip' 0 clean github;exec bash"
traceroute -A dms.licdn.com > /dev/null
traceroute -A dms.licdn.com > ~/sanity_test_results/tr_licdn.log
traceroute -A upload.wikimedia.org > ~/sanity_test_results/tr_wiki.log
traceroute -A m.media-amazon.com > ~/sanity_test_results/tr_amazon.log
traceroute -A video-http.media-imdb.com > ~/sanity_test_results/tr_imdb.log
traceroute -A github.com > ~/sanity_test_results/tr_github.log
screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py dms.licdn.com licdn; exec bash"