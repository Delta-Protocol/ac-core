#ifndef USGOV_b31c225ca9360a034d03497ef22865c0b5c8d70c183eb98b4aedf857d721ab0a
#define USGOV_b31c225ca9360a034d03497ef22865c0b5c8d70c183eb98b4aedf857d721ab0a

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

namespace us { namespace gov {
namespace net {

using namespace std;

struct os {
        virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags)=0;
        virtual ssize_t write(int fd, const void *buf, size_t count)=0;
};

struct posix:os {
        virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) override { 
		auto r=::recv(sockfd,buf,len,flags); 
		//cout << "net: os(posix): recv from " << sockfd << " " << r << " bytes" << endl;
		return r;
	}
        virtual ssize_t write(int fd, const void *buf, size_t count) override {
		//cout << "net: os(posix): write " << count << " bytes" << endl;
		return ::write(fd,buf,count); 
	}
};

struct sim:os {
        virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) override {
                return ::recv(sockfd,buf,len,flags);
        }
        virtual ssize_t write(int fd, const void *buf, size_t count) override {
                return ::write(fd,buf,count);
        }
};

}
}}

#endif

