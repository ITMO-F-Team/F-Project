#pragma once

namespace flang {
enum TokenType {
  tkINTEGER,
  tkREAL,
  tkLPAREN,
  tkRPAREN,
  tkQUOTE,
  tkSETQ,
  tkFUNC,
  tkLAMBDA,
  tkPROG,
  tkCOND,
  tkWHILE,
  tkRETURN,
  tkBREAK,
  tkNULL,
  tkIDENTIFIER
};
}