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
#include "Jack.h"

//---JACK---

//---PUBLIC---

Jack::Jack(JTransmissionMethod &mmJTM, void (*onReceive)(JData &, long), void (*onReceiveAck)(long), long (*getMessageID)(), long timerSendMessage, long timerPolling) { //tempo per il reinvio
	
	//salvo il mezzo di trasmissione
	_mmJTM = &mmJTM;

	//imposto i valori dei timer
	_timerSendMessage = timerSendMessage;
	_timerPolling = timerPolling;

	//salvo i puntatori a funzioni
	_onReceive = onReceive;
	_onReceiveAck = onReceiveAck;
	_getMessageID = getMessageID;

	//inizializzo le variabili
	_timeLastPolling = 0;
	_timeLastSend = 0;


	//costruisco i contenitori
	_messageBuffer = new HashMap<long, char *>();
	
}

//costruttore ridotto
Jack::Jack(JTransmissionMethod &mmJTM, void (*onReceive)(JData &, long), void (*onReceiveAck)(long), long (*getMessageID)()): Jack(mmJTM, onReceive, onReceiveAck, getMessageID, JK_TIMER_RESEND_MESSAGE, JK_TIMER_POLLING) {} //costruttore con mmJTM, funzione onReceive e getMessageID


//distruttore
Jack::~Jack() {

	//elimino i buffer
	delete _messageBuffer; //buffer per i messaggi da inviare
}

//metodi per abilitare/disabilitare il polling
void Jack::start() { //avvia il polling
	_pollingEnabled = 1;
}

void Jack::stop() { //stoppa il polling
	_pollingEnabled = 0;
}
		

//svuota il buffer di invio
void Jack::flushBufferSend() { //cancella i buffer contenente i messaggi da inviare

	//elimino buffer
	delete _messageBuffer;

	//creo il nuovo buffer
	_messageBuffer = new HashMap<long, char *>();
}


//loop function
void Jack::loop() { //luppa per simulare il thread

	//se il polling è abilitato 

	if (_pollingEnabled && millis() - _timeLastPolling >= _timerPolling) {

		//ultimo polling
		_timeLastPolling = millis();

		//prelevo la lunghezza del messaggio
		int length = _mmJTM->available();

		//verifico se ci sono messaggi disponibili e ne salvo la lunghezza
		if (length) {

			//creo il buffer che conterrà il messaggio ricevuto da mmJTM
			char message[length +1];

			//recupero il messaggio e verifico se non è nullo
			if (_mmJTM->receive(message, length +1)) {

				//il messaggio è valido
				execute(message);
			}

		}
	
		//se ci sono messaggi da inviare ed è passato il tempo di pausa tra un invio e l'altro
		if (millis() - _timeLastSend >= _timerSendMessage && _messageBuffer->moveToFirst()) { //invio messaggi

			//ultimo invio
			_timeLastSend = millis();
			
			//scorro tutti i messaggi e li invio
			do {

				//prelevo il messaggio da inviare
				char *message = _messageBuffer->value();

				//invio il messaggio
				_mmJTM->send(message, strlen(message));
			
			} while (_messageBuffer->moveToNext());
		
		}
	}

}


//---PRIVATE---

void Jack::execute(char *json) { //funzione che gestisce il protocollo

	//creo memory pool
	StaticJsonBuffer<200> jsonBuffer;

	//creo la root a partire dal messaggio JSON
	JsonObject& root = jsonBuffer.parseObject(json);

	//verifo se il messaggio è un messaggio valido
	if (root.success()) {

		//ottengo il tipo del messaggio
		const char *type = root[JK_MESSAGE_TYPE];

		//tipo dati
		if (strcmp(type, JK_MESSAGE_TYPE_DATA) == 0) {

			//costruisco il messaggio JData
			JData message(root);

			//ottengo l'id del messaggio
			long id = root[JK_MESSAGE_ID];

			//confermo il messaggio
			sendAck(id);

			//chiamo la funzione di gestione definita dall'utenye
			(*_onReceive)(message, id);

		//tipo ACK
		} else if (strcmp(type, JK_MESSAGE_TYPE_ACK) == 0) {

			//ottengo l'id del messaggio
			long id = root[JK_MESSAGE_ID];

			//chiamo la funzione di gestione degli ack
			checkAck(id);
		}

	}
}


//invio ACK di conferma
void Jack::sendAck(long id) { //invia l'ack di conferma
	
	//creo memory pool
	StaticJsonBuffer<100> jsonBuffer;

	//creo root del messaggio
	JsonObject& root = jsonBuffer.createObject();

	//inserisco i dati
	root[JK_MESSAGE_ID] = id; //id del messaggio da confermare
	root[JK_MESSAGE_TYPE] = JK_MESSAGE_TYPE_ACK; //il messaggio è un ACK

	//verifico la dimensione del messaggio più il carattere di terminazione
	size_t length = root.measureLength() +1;

	//creo buffer per contenere il messaggio
	char message[length];

	//ottengo il messaggio in JSON
	root.printTo(message, length);

	//invio il messaggio
	_mmJTM->send(message, strlen(message));

}

//metodo che elimina il messaggio confermato dal buffer di invio
void Jack::checkAck(long id) { //controlla l'ack
	
	//se il buffer dei messaggi da inviare contiene il messaggio appena confermato lo elimino
	if (_messageBuffer->containsKey(id)) {

		//libero la memoria allocata per il messaggio stringa
		free(_messageBuffer->valueFor(id));

		//rimuovo il messaggio dal buffer di invio
		_messageBuffer->remove(id);

		//il messaggio è stato confermato, chiamo la funzione dell'utente
		(*_onReceiveAck)(id);
	}
		
}


//metodo che invia il messaggio
long Jack::send(JData &messageJData) { //invia il messaggio

	//prelevo la root del messaggio
	JsonObject *root = messageJData.getRoot();

	//ottengo l'id del messaggio
	long id = (*_getMessageID)();

	//aggiungo id e la tipologia del messaggio
	(*root)[JK_MESSAGE_ID] = id; //id del messaggio da confermare
	(*root)[JK_MESSAGE_TYPE] = JK_MESSAGE_TYPE_DATA; //il messaggio è un ACK

	//verifico la dimensione del messaggio più il carattere di terminazione
	size_t length = (*root).measureLength() +1;

	//creo buffer per contenere il messaggio malloc
	char *message = (char *) malloc(length * sizeof(char));

	//ottengo il messaggio in JSON
	(*root).printTo(message, length);

	//inserisco il messaggio nel buffer di invio
	_messageBuffer->put(id, message);

	//ritorno l'id del messaggio inserito nel buffer
	return id;
	
}