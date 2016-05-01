/*
   Copyright 2016 Alessandro Pasqualini

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
   @author         Alessandro Pasqualini <alessandro.pasqualini.1105@gmail.com>
   @url            https://github.com/alessandro1105
*/

#ifndef JDATA_H
#define JDATA_H

#include <Arduino.h>
#include <HashMap.h>
#include <Wrappers.h>


//---JDATA ENUM---
typedef enum JDType {
   JD_LONG,
   JD_DOUBLE,
   JD_BOOLEAN,
   JD_STRING,
   JD_JDATA
};


//---JDATA---
//classe usata come contenitore per i messaggi
class JData {

   public:
      
      static const int LONG = 0;
      static const int DOUBLE = 1;
      static const int BOOLEAN = 2;
      static const int STRING = 3;
      static const int JDATA = 4;
      
      JData();
      ~JData(); //distruttore
      
      //add
      void addLong(String key, long value);
      void addDouble(String key, double value);
      void addBoolean(String key, int value);
      void addString(String key, String value);
      void addJData(String key, JData * value);
      
      //get 
      long getLong(String key);
      long getLong(int index);
      
      double getDouble(String key);
      double getDouble(int index);
      
      String getDoubleString(String key);
      String getDoubleString(int index);
      
      int getBoolean(String key);
      int getBoolean(int index);
      
      String getString(String key);
      String getString(int index);
      
      JData *getJData(String key);
      JData *getJData(int index);
      
      //get key
      String getKey(int index);
      
      //get type stored
      JDType getType(String key);
      JDType getType(int index);
      
      //get size
      int length();
      
      //contains
      int containsKey(String key);

   private:

      //numero elementi memorizzati
      int _size;
      
      //contenitori dei dati
      HashMap<String, Wrapper *> *_data;
      HashMap<int, String> *_indexes;
      
      //
      void addWrapper(String key, Wrapper * wrapper);
      
      Wrapper *getWrapper(String key);
      Wrapper *getWrapper(int index);
      
      int getWrapperType(String key);
      int getWrapperType(int index);

};


#endif //JDATA