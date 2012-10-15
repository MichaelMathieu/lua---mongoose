#ifndef __MONGOOSE_H_240912__
#define __MONGOOSE_H_240912__

#include<string>
#include<fcntl.h>

const size_t BUFFER_SIZE = 1024;
const int LINE_SIZE = 43;

struct Mongoose {
  unsigned char buffer[BUFFER_SIZE];
  int i_buffer;
  FILE* file;

  inline Mongoose(std::string tty)
    :i_buffer(0) {
    memset(buffer, 0, sizeof(char)*BUFFER_SIZE);
    FILE* p = popen((std::string("stty -F ") + tty +
		     std::string(" hupcl && stty -F ") +
		     tty + std::string(" cs8 115200 ignbrk -brkint -icanon -icrnl -imaxbel -opost -onlcr -isig -iexten -echo -echoe -echok -echoctl -echoke -noflsh ixon -crtscts")).c_str(), "r");
    if (p) {
      pclose(p);
      file = fopen(tty.c_str(), "rb");
      int fd = fileno(file);
      int status = fcntl(fd, F_GETFL, NULL);
      if (status < 0) {
        file = NULL;
      } else {
	status |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, status) < 0) {
	  file = NULL;
	}
      }
    } else {
      file = NULL;
    }
  }
  Mongoose(const Mongoose & source);//do not copy
  Mongoose & operator=(const Mongoose & source);//do not copy
  inline ~Mongoose() {
    fclose(file);
  }
  
};

#endif
