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

#ifndef JDATAWRAPPER_H
#define JDATAWRAPPER_H

#include <Arduino.h>
#include <Wrappers.h>
#include <JData.h>


//---CONSTANTI---

//JData Wrapper
#define WR_JDATA 4 //deve essere superiore a 3


//---JDATA WRAPPER---
//wrapper per jData
class JDataWrapper : public Wrapper {

	public:

		JDataWrapper(JData * value); //costruttore per riempire l'oggetto

		uint8_t type(); //funzione che restituisce il tipo di wrapper

		JData *getJData(); //funzione per estrarre il valore

	private:

		JData *_value; //contenitore del valore

};

#endif //JDATAWRAPPER_H
