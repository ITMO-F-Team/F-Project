#pragma once

namespace flang {
enum TokenType {
  tkLPAREN,
  tkRPAREN,
  tkQUOTE,
  tkQUOTEMARK,
  tkSETQ,
  tkFUNC,
  tkLAMBDA,
  tkPROG,
  tkCOND,
  tkWHILE,
  tkRETURN,
  tkBREAK,
  tkIDENTIFIER,
  tkINTEGER,
  tkREAL,
  // tkNULL // TODO: Add null as token (?)
};
}
