void log_in(int conn_fd)
{
    while(1)
    {
        if((ret = recv(conn_fd,recv_buf,sizeof(recv_buf),0))<0)
        {
            perror("recv");
            exit(1);
        }
        recv_buf[ret-1] = '\0';

        if(flag_recv == USERNAME)
        {
            name_num = find_name(recv_buf);
            switch(name_num)
            {
            case -1:
                send_data(conn_fd,"n\n");
                break;
            case -2:
                exit(1);
                break;
            default:
                send_data(conn_fd,"y\n");
                flag_recv = PASSWORD;
                break; 
            }                    
        }
        else if(flag_recv == PASSWORD)
        {
            if(strcmp(users[name_num].password,recv_buf) == 0)
            {
                send_data(conn_fd,"y\n");
                send_data(conn_fd,"Welcome login my tcp server\n");
                printf("%s login\n",users[name_num].username);
                break;
            }
            else
            send_data(conn_fd,"n\n");
        } 
    }
}
