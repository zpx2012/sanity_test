sudo apt-get update
sudo apt-get -y install git
git clone https://github.com/zpx2012/sanity_test.git ~/sanity_test
sh ~/sanity_test/install.sh
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'http://upload.wikimedia.org/wikipedia/commons/a/a8/LOC_Main_Reading_Room_Highsmith.jpg' 0 clean wiki_highsmith;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://dms.licdn.com/playback/C4E00AQEOf4NNZp_DSA/23033a33657347f2a6f2a1140146d656/feedshare-mp4_3300/1488578169071-zmy00q?e=1534143600&v=beta&t=NHHKtbIJ0ziALvVSHMeCAJ1lBOdLE_2ITDVji0X8j_w' 0 clean linkedin;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://m.media-amazon.com/images/G/01/IMDb/design/prototypes/raven/allornothing/allblacks.mp4' 0 clean amazon;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://video-http.media-imdb.com/MV5BYThkNmNlOWQtNDA3My00MjgzLWI2ODktNWQxODcwOGZlOGVkXkExMV5BbXA0XkFpbWRiLWV0cy10cmFuc2NvZGU@.mp4?Expires=1534142129&Signature=c7tkEzp4GJsz-fKhfLqDNFvCEOuuwOoOaBlC-jZWAAikDwQbLuIj5IvkbDB-uvK6X2JPrtCr2okCeEW1ezWYJRbNEMcfhZiRcfZvyJ9aonPOAS6n47Xmk32eTHzza9Vp~UBz6plcXOdCJ0835CC-Tb6AxJZK10C3~ISmcfXkKVs_&Key-Pair-Id=APKAILW5I44IHKUN2DYA' 0 clean imdb;exec bash"
screen -dmS test bash -c "python ~/sanity_test/curl_downloader.py 'https://github.com/multipath-tcp/mptcp' 0 clean github;exec bash"
traceroute -A dms.licdn.com > ~/sanity_test_results/tr_licdn.log
traceroute -A upload.wikimedia.org > ~/sanity_test_results/tr_wiki.log
traceroute -A m.media-amazon.com > ~/sanity_test_results/tr_amazon.log
traceroute -A video-http.media-imdb.com > ~/sanity_test_results/tr_imdb.log
traceroute -A github.com > ~/sanity_test_results/tr_github.log
screen -dmS test bash -c "python ~/sanity_test/mtr_runner.py upload.wikimedia.org; exec bash"