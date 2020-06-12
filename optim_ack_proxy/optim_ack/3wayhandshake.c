
struct thread_data{
        int  raw_socket;
        int  len;
        unsigned char *buf;
};



void optimistic_ack(int local_port)
{
    unsigned int seq, ack;
    
    //local_port = rand() % 20000 + 30000; // generate random port (30000-49999)

    seq = rand();

    int retry = 4;
    while (retry) {
        send_SYN("", 0, seq, local_port);
        seq++;
        ack = wait_SYN_ACK(seq, 1, local_port);
        if (ack != 0) break;
        retry--;
    }
    if (retry == 0) {
        log_exp("Give up.");
        return;
    }
    ack++;
    send_ACK("", ack, seq, local_port);
    send_request(payload_sk, ack, seq, local_port);

    //Send Optim Acks, pacing
}