#ifndef PERFORMANCE_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define PERFORMANCE_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class QueryTimer {
 public:
  QueryTimer(const std::string& queryName) : name(queryName) {
    glGenQueries(1, &queryId);
  }

  ~QueryTimer() { glDeleteQueries(1, &queryId); }

  void startQuery() {
    glQueryCounter(queryId, GL_TIMESTAMP);
    GLint resultAvailable = 0;
    while (!resultAvailable) {
      glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
    }
    glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &startTime);
  }

  // Stop the query and print the elapsed time with its name
  void stopQuery() {
    glQueryCounter(queryId, GL_TIMESTAMP);
    GLint resultAvailable = 0;
    while (!resultAvailable) {
      glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
    }
    glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &endTime);
  }

  void printResult() const {
    std::cout << "Elapsed time for " << name << ": "
              << (endTime - startTime) / 1e6 << " ms" << std::endl;
  }

 private:
  GLuint queryId;
  std::string name;

  GLuint64 startTime;
  GLuint64 endTime;
};

#endif