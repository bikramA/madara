

#include "madara/knowledge_engine/File_Header.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

#include "madara/utility/Utility.h"

#include <stdio.h>
#include <iostream>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/containers/String_Vector.h"
#include "madara/logger/Global_Logger.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
namespace logger = Madara::Logger;

typedef  Madara::Knowledge_Record::Integer  Integer;

void test_copy (void)
{
  engine::Knowledge_Base source, dest;
  containers::String_Vector kids ("kids", source, 1);
  engine::Copy_Set copy_set;

  // create some information in the source knowledge base
  source.set ("name", "John Smith");
  source.set ("age", Integer (20));
  source.set ("occupation", "Explorer");
  source.set ("spouse", "Pocahontas");
  kids.set (0, "Thomas Rolfe");

  // TEST 1: Copy everything to destination
  dest.copy (source, copy_set, false);
  if (dest.get ("name") == "John Smith" &&
      dest.get ("age") == Integer (20) &&
      dest.get ("occupation") == "Explorer" &&
      dest.get ("spouse") == "Pocahontas" &&
      dest.get ("kids.0") == "Thomas Rolfe")
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 1: Full copy is SUCCESS.\n");
  }
  else
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 1: Full copy is FAIL.\n");
    dest.print ();
  }

  // TEST 2: Copy name to destination
  copy_set.clear ();
  copy_set["name"] = true;

  dest.copy (source, copy_set, true);
  if (dest.get ("name") == "John Smith" &&
      !dest.exists ("age") &&
      !dest.exists ("occupation") &&
      !dest.exists ("spouse") &&
      !dest.exists ("kids.0"))
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 2: Lone name copy is SUCCESS.\n");
  }
  else
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 2: Lone name copy is FAIL.\n");
    dest.print ();
  }

  // TEST 3: Add age to destination
  copy_set.clear ();
  copy_set["age"] = true;

  dest.copy (source, copy_set, false);
  if (dest.get ("name") == "John Smith" &&
      dest.get ("age") == Integer (20) &&
      !dest.exists ("occupation") &&
      !dest.exists ("spouse") &&
      !dest.exists ("kids.0"))
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 3: Add age to copy is SUCCESS.\n");
  }
  else
  {
    logger::global_logger->log (logger::LOG_ALWAYS,
      "TEST 3: Add age to copy is FAIL.\n");
    dest.print ();
  }

  logger::global_logger->log (logger::LOG_ALWAYS,
    "\nEND OF TESTS CONTEXT CONTENTS:\n"
    "\nSource:\n");
  source.print ();

  logger::global_logger->log (logger::LOG_ALWAYS, "\nDest:\n");
  dest.print ();
}

int main (int, char **)
{
  test_copy ();

  return 0;
}
