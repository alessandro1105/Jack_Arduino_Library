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

#include <Arduino.h>
#include "JData.h"


//---JDATA---
JData::JData() { //costruttore
   
   //costruisco i contenitori dei dati
   _data = new HashMap<String, Wrapper *>();
   _indexes = new HashMap<int, String>();
   
   //azzero il numero di elementi memorizzati
   _size = 0;
   
}

JData::~JData() { //distruttore
   
   //elimino tutti gli elementi contenuti
   if (_data->moveToFirst()) {
      
      do {
         Wrapper *wrapper = _data->value();
         
         switch (wrapper->type()) {
      
            case WR_LONG:
               delete static_cast<LongWrapper *>(wrapper);
               break;
               
            case WR_DOUBLE:
               delete static_cast<DoubleWrapper *>(wrapper);
               break;
            
            case WR_BOOLEAN:
               delete static_cast<BooleanWrapper *>(wrapper);
               break;
               
            case WR_STRING:
               delete static_cast<StringWrapper *>(wrapper);
               break;
               
            case WR_JDATA:
               delete static_cast<JDataWrapper *>(wrapper);
               break;
         
   
         }
      
      } while (_data->moveToNext());
      
   }  

   //elimino i due contenitori
   delete _data; //elimino la struttura contenuta in HashMap
   delete _indexes; //elimino la struttura contenuta in HashMap
}

//add wrapper
void JData::addWrapper(String key, Wrapper * wrapper) {
   
   //inserisco il wrapper
   _data->put(key, wrapper);

   //inserisco l'indice
   _indexes->put(_size, key);
   
   //incremento la dimensione
   _size++;
   
};

//get wrapper
Wrapper * JData::getWrapper(String key) {
   return _data->valueFor(key);
}

Wrapper * JData::getWrapper(int index) {
   return getWrapper(getKey(index));
}
      
int JData::getWrapperType(String key) {
   return getWrapper(key)->type();
}

int JData::getWrapperType(int index) {
   return getWrapperType(getKey(index));
}

//adder
void JData::addLong(String key, long value) {

   LongWrapper * wrapper = new LongWrapper(value);
   
   addWrapper(key, wrapper);
}

void JData::addDouble(String key, double value) {
   
   DoubleWrapper * wrapper = new DoubleWrapper(value);
   
   addWrapper(key, wrapper);

}

void JData::addBoolean(String key, int value) {

   BooleanWrapper * wrapper = new BooleanWrapper(value);
   
   addWrapper(key, wrapper);

}

void JData::addString(String key, String value) {
   
   StringWrapper * wrapper = new StringWrapper(value);
   
   addWrapper(key, wrapper);
}

void JData::addJData(String key, JData * value) {
   
   JDataWrapper * wrapper = new JDataWrapper(value);
   
   addWrapper(key, wrapper);

}

//get 
long JData::getLong(String key) {
   return static_cast<LongWrapper *>(getWrapper(key))->getLong();
}

long JData::getLong(int index) {
   return getLong(getKey(index));
}
      
double JData::getDouble(String key) {
   return static_cast<DoubleWrapper *>(getWrapper(key))->getDouble();
}

double JData::getDouble(int index) {
   return getDouble(getKey(index));
}

String JData::getDoubleString(String key) {
   return static_cast<DoubleWrapper *>(getWrapper(key))->getString();
}

String JData::getDoubleString(int index) {
   return getDoubleString(getKey(index));
}
   
int JData::getBoolean(String key) {
   return static_cast<BooleanWrapper *>(getWrapper(key))->getBoolean();
}

int JData::getBoolean(int index) {
   return getBoolean(getKey(index));
}
      
String JData::getString(String key) {
   return static_cast<StringWrapper *>(getWrapper(key))->getString();
}

String JData::getString(int index) {
   return getString(getKey(index));
}

JData * JData::getJData(String key) {
   return static_cast<JDataWrapper *>(getWrapper(key))->getJData();
}

JData * JData::getJData(int index) {
   return getJData(getKey(index));
}
      
//get key
String JData::getKey(int index) {
   return _indexes->valueAt(index);
}
      
//get type stored
JDType JData::getType(String key) {
   
   switch (getWrapperType(key)) {
      
      case WR_LONG:
         return JD_LONG;
         break;
         
      case WR_DOUBLE:
         return JD_DOUBLE;
         break;
      
      case WR_BOOLEAN:
         return JD_BOOLEAN;
         break;
         
      case WR_STRING:
         return JD_STRING;
         break;
         
      case WR_JDATA:
         return JD_JDATA;
         break;
         
   
   }

}

JDType JData::getType(int index) {
   return getType(getKey(index));
}
      
//get size
int JData::length() {
   return _size;
}


int JData::containsKey(String key) {
   return _data->containsKey(key);
}