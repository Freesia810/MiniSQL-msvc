#include <cstdio>
#include "execute_engine.h"
#include "syntax_tree_printer.h"
#include <iostream>
#include <windows.h>
#include <chrono>

extern "C" {
int yyparse(void);
#include "minisql_lex.h"
#include "parser.h"
}


void InputCommand(char *input, const int len) {
  memset(input, 0, len);
  printf("minisql > ");
  int i = 0;
  char ch;
  while ((ch = getchar()) != ';') {
    input[i++] = ch;
  }
  input[i] = ch;    // ;
  getchar();        // remove enter
}


int main(int argc, char **argv) {
  // command buffer
  const int buf_size = 1024;
  char cmd[buf_size];
  // execute engine
  ExecuteEngine engine;

  while (1) {
    // read from buffer
    InputCommand(cmd, buf_size);
    // create buffer for sql input
    YY_BUFFER_STATE bp = yy_scan_string(cmd);
    if (bp == nullptr) {
      exit(1);
    }
    yy_switch_to_buffer(bp);

    // init parser module
    MinisqlParserInit();

    // parse
    yyparse();


    // parse result handle
    if (MinisqlParserGetError()) {
      // error
      printf("%s\n", MinisqlParserGetErrorMessage());
    }
    pSyntaxNode p = MinisqlGetParserRootNode();

    ExecuteContext context;
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();//计时开始
    engine.Execute(MinisqlGetParserRootNode(), &context);
    auto t2 = Clock::now();//计时结束
    std::cout << "Execute time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1e+6 << "ms" << std::endl;

    // clean memory after parse
    MinisqlParserFinish();
    yy_delete_buffer(bp);
    yylex_destroy();

    // quit condition
    if (context.flag_quit_) {
      printf("bye!\n");
      break;
    }

  }
  return 0;
}