////////////////////////////////////////////////////////////////////////////////
/// @brief input parsers
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Copyright 2009-2011, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_REST_INPUT_PARSER_H
#define TRIAGENS_FYN_REST_INPUT_PARSER_H 1

#include <Basics/Common.h>

namespace triagens {
  namespace basics {
    class VariantArray;
    class VariantBoolean;
    class VariantDouble;
    class VariantInt64;
    class VariantNull;
    class VariantObject;
    class VariantString;
    class VariantVector;
  }

  namespace rest {
    class HttpRequest;

    namespace InputParser {
      struct ObjectDescriptionImpl;

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief object description
      ////////////////////////////////////////////////////////////////////////////////

      class ObjectDescription {
        private:
          ObjectDescription (ObjectDescription const&);
          ObjectDescription& operator= (ObjectDescription const&);

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief creates a new description
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription ();

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief destroys a description
          ////////////////////////////////////////////////////////////////////////////////

          virtual ~ObjectDescription ();

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief loads an object
          ////////////////////////////////////////////////////////////////////////////////

          bool parse (basics::VariantObject*);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief returns the last error message
          ////////////////////////////////////////////////////////////////////////////////

          string const& lastError ();

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief applys transformations after parsing
          ////////////////////////////////////////////////////////////////////////////////

          virtual void transform ();

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an array attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantArray*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a boolean attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantBoolean*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a boolean attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, bool&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a double attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantDouble*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a double attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, double&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an integer attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantInt64*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an integer attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, int64_t&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an null attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantNull*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a string attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantString*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a string attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, string&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a string vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, vector<basics::VariantString*>&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a string vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, vector<string>&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds a vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& attribute (string const& name, basics::VariantVector*&);

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional attribute field
          ////////////////////////////////////////////////////////////////////////////////

          template<typename T>
          ObjectDescription& optional (string const& name, T& t, bool& hasAttribute) {
            return optional(name, t, &hasAttribute);
          }

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional attribute field
          ////////////////////////////////////////////////////////////////////////////////

          template<typename T>
          ObjectDescription& optional (string const& name, T& t) {
            return optional(name, t, 0);
          }

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional array attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantArray*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional boolean attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantBoolean*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional boolean attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, bool&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional double attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantDouble*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional double attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, double&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional integer attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantInt64*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional integer attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, int64_t&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional null attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantNull*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional string attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantString*&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional string attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, string&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional string vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, vector<basics::VariantString*>&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional string vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, vector<string>&, bool* hasAttribute);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an optional vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& optional (string const& name, basics::VariantVector*&, bool* hasAttribute);

        public:

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative array attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantArray*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative boolean attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantBoolean*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative double attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantDouble*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative integer attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantInt64*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative null attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantNull*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative string attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantString*&);

          ////////////////////////////////////////////////////////////////////////////////
          /// @brief adds an alternative vector attribute field
          ////////////////////////////////////////////////////////////////////////////////

          ObjectDescription& alternative (string const& name, basics::VariantVector*&);

        private:
          ObjectDescriptionImpl* impl;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief a json parser
      ////////////////////////////////////////////////////////////////////////////////

      basics::VariantObject* json (string const& input);

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief a json parser
      ////////////////////////////////////////////////////////////////////////////////

      basics::VariantObject* json (HttpRequest*);

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief a json parser for an array
      ////////////////////////////////////////////////////////////////////////////////

      basics::VariantArray* jsonArray (string const& input);

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief a json parser for an array
      ////////////////////////////////////////////////////////////////////////////////

      basics::VariantArray* jsonArray (HttpRequest*);

      ////////////////////////////////////////////////////////////////////////////////
      /// @ingroup Utilities
      /// @brief a json-to-object parser
      ////////////////////////////////////////////////////////////////////////////////

      bool json2object (ObjectDescription&, string const& input);
    }
  }
}

#endif
