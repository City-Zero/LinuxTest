online chat
=
基于 Linux 平台的文字聊天室,服务器端启动后可以接受多个客户端连接,实现客户端之间的通讯。

依赖环境及使用说明
-
#ubuntu系
安装`libmysqlclient-dev`

#fedora系
安装`mysql-devel`

- 安装mysql及上面的包
- 在ser/makefile 设置好正确的mysql路径
- 创建一个数据库，使用`create table info ( name VARCHAR(20), pass VARCHAR(20));`建立基础表
- 在ser/conf.h中设置正确的数据库信息
- ser/ cli/目录下运行make命令
- 开启一个终端启动 ser
- 开启数个终端运行 cli
