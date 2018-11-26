import csv

text = '''{
"server":"%s",
"server_port":8388,
"local_address":"127.0.0.1",
"local_port":%d,
"password":"wch92507@#",
"timeout":300,
"method":"aes-256-cfb"
}'''
with open('scripts/vultr.csv','r') as f:         
    domain_ip_list = list(csv.reader(f))
for i,line in enumerate(domain_ip_list):
    with open('shadowsocks/client_%s.json' % line[0],'w') as f,open('scripts/ss_vultr.csv','a') as f1:
        f.writelines(text % (line[1],1080+i))
        f1.writelines(','.join(line+[str(1080+i)])+'\n')
