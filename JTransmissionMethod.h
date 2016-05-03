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

#ifndef JTRANSMISSIONMETHOD_H
#define JTRANSMISSIONMETHOD_H


//---JTRANSMISSION METHOD---
class JTransmissionMethod {

	public:
		
		virtual size_t receive(char *buffer, size_t size); //deve restituire il messaggio da passare a Jack
		virtual void send(char *message, size_t length); //invia il messaggio
		virtual size_t available(); //restituisce true se ci sono dati da ricevere nel buffer

};

#endif //JTRANSMISSIONMETHOD_H