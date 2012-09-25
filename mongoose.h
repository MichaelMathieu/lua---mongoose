#ifndef __MONGOOSE_H_240912__
#define __MONGOOSE_H_240912__

#include<string>

const size_t BUFFER_SIZE = 1024;
const int LINE_SIZE = 43;

struct Mongoose {
  char buffer[BUFFER_SIZE];
  int i_buffer;
  FILE* file;

  inline Mongoose(std::string tty)
    :i_buffer(0) {
    memset(buffer, 0, sizeof(char)*BUFFER_SIZE);
    FILE* p = popen((std::string("stty -F ") + tty +
		     std::string(" hupcl && stty -F ") +
		     tty + std::string(" cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh ixon -crtscts")).c_str(), "");
    if (p)
      file = fopen(tty.c_str(), "rb");
    else
      file = NULL;
  }
  Mongoose(const Mongoose & source);//do not copy
  Mongoose & operator=(const Mongoose & source);//do not copy
  inline ~Mongoose() {
    fclose(file);
  }
  
};

#endif
