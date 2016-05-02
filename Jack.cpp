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

Jack::Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), long (* getTimestamp) (), long timerSendMessage, long timerPolling) { //tempo per il reinvio
	
	//salvo il mezzo di trasmissione
	_mmJTM = mmJTM;

	//imposto i valori dei timer
	_timerSendMessage = timerSendMessage;
	_timerPolling = timerPolling;

	//salvo i puntatori a funzioni
	_onReceive = onReceive;
	_getTimestamp = getTimestamp;

	//inizializzo le variabili
	_timeLastPolling = 0;
	_timeLastSend = 0;


	//costruisco i contenitori
	_messageBuffer = new HashMap<long, char *>();
	
}

//costruttore ridotto
Jack::Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), long (* getTimestamp) ()): Jack(mmJTM, onReceive, getTimestamp) {} //costruttore con mmJTM, funzione onReceive e getTimestamp


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

		//verifico se ci sono messaggi disponibili e ne salvo la lunghezza
		if (_pollingEnabled && int length = _mmJTM->available()) {

			//creo il buffer che conterrà il messaggio ricevuto da mmJTM
			char message[length];

			//recupero il messaggio e verifico se non è nullo
			if (_mmJTM->receive(message, length)) {

				//il messaggio è valido
				execute(message);
			}

		}
	
		//se ci sono messaggi da inviare ed è passato il tempo di pausa tra un invio e l'altro
		if (millis() - _timeLastSend >= _timerSendMessage && sendMessageBuffer->moveToFirst()) { //invio messaggi
			
			//scorro tutti i messaggi e li invio
			do {

				//prelevo il messaggio da inviare
				char *message = _messageBuffer->value();

				//invio il messaggio
				_mmJTM->send(message, strlen(message));
			
			} while (sendMessageBuffer->moveToNext());
		
		}
	}

}


//---PRIVATE---

//DA RIVEDERE
void Jack::execute(char *messageString) { //funzione che gestisce il protocollo

	//se il messaggio è in formato valido
	if (JData *message = getMessageJData(messageString)) {

		if (message->getChar(MESSAGE_TYPE).equals(MESSAGE_TYPE_DATA))

	}


	//codice vecchio			
	if (validate(message)) {
		
		JData * messageJData = getJDataMessage(message);
		
		if (messageJData->getString(MESSAGE_TYPE).equals(MESSAGE_TYPE_DATA)) {
		
			//Serial.println("check MEX GIA RICEVUTO mex ricevuto");
		
			if (!checkMessageAlreadyReceived(messageJData)) {
			
				//Serial.println("MEX RICEVUTO trigger");
				
				(* onReceive) (messageJData->getJData(MESSAGE_DATA));
			}
			
		} else {
			//Serial.println("CHECK mex ricevuto");
			checkAck(messageJData);
		}
		
		
		//Serial.println("elimino JData mex ricevuto");
		delete messageJData;
		
	}
		
}


//invio ACK di conferma
void Jack::sendAck(long id) { //invia l'ack di conferma
	
	//creo memory pool
	StaticJsonBuffer<200> jsonBuffer;

	//creo root del messaggio
	JsonObject& root = jsonBuffer.createObject();

	//inserisco i dati
	root[JK_MESSAGE_ID] = id; //id del messaggio da confermare
	root[JK_MESSAGE_TYPE] = JK_MESSAGE_ACK; //il messaggio è un ACK

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
		_messageBuffer->remove(id);
	}
		
}


//DA RIVEDERE
void Jack::send(JData * message) { //invia il messaggio

	//creo memory pool
	StaticJsonBuffer<500> jsonBuffer;

	//creo root del messaggio
	JsonObject& root = jsonBuffer.createObject();
	
	//ottengo il timestamp da usare come id del messaggio
	long id = (* getTimestamp) ();
		
	//scorro i dati contenuti in JData
	for (int i = 0; i < message.length; i++) {

		JDType type message->getType(i);

		switch (type) {
			case JD_LONG:
				root[message->getKey(i)]
				break;

			case JD_DOUBLE:
				break;

			case JD_BOOLEAN:
				break;

			case JD_STRING:
				break;

			case JD_JDATA:
				break;

		}

	}


	String messageString = "{\"" + MESSAGE_ID + "\":";
	messageString += id;
	messageString += ",\"" + MESSAGE_TYPE_DATA + "\":[{";
	
	for(int i = 0; i < message->length(); i++) {
		
		messageString += "\"";
		messageString += message->getKey(i);
		messageString += "\":";
		
		int type = message->getType(i);
		
		if (type == JData::LONG) {
		
			messageString += message->getLong(i);
		
		} else if (type == JData::DOUBLE) {
			
			messageString += message->getDoubleString(i);
		
		} else if (type == JData::BOOLEAN) {
			
			if (message->getBoolean(i)) {
				messageString += MESSAGE_BOOLEAN_TRUE;
			} else {
				messageString += MESSAGE_BOOLEAN_FALSE;
			}
		
		} else if (type == JData::STRING) {
		
			messageString += "\"";
			messageString += message->getString(i);
			messageString += "\"";
			
		}
		
		messageString += ",";
		
	}
	
	
	messageString = messageString.substring(0, messageString.length() -1);
	
	messageString += "}]}";
	
	
	sendMessageBuffer->put(id, messageString);
	
	sendMessageBufferJData->put (id, message);
	
}

//DA RIVEDERE
JData * Jack::getMessageJData(String message) { //preleva i dati dal messaggio e crea il messaggio nel formato JData
	
	//Serial.print("JDATAGET: ");
	//Serial.println(freeMemory());

	//DA JSON deve ritornare un oggetto JDATA
	
	
	JData * messageJData = new JData();
	
	String temp = "";
	String temp2 = "";
	
	int nChar = 0;
	
	int value;
	
	message = message.substring(2, message.length());
	
	for(int i = 0; i < 2; i++) {
	
		temp = "";
		
		//Serial.println(message);
		
		if (message.startsWith(MESSAGE_ID)) { //id
			
			message = message.substring(MESSAGE_ID.length() + 2, message.length());
			
			for(int x = 0; message.charAt(x) != ','; x++) { //prelevo l'id dal messaggio
				
				temp += message.charAt(x);
				
			}
			
			if (i < 1) {
				message = message.substring(temp.length() + 2, message.length());
			}
			
			LongWrapper lw(temp);
			
			messageJData->addLong(MESSAGE_ID, lw.getLong());
			
			//Serial.println("id: " + temp);
			
			
		} else if (message.startsWith(MESSAGE_TYPE_ACK)) { //ack
		
			messageJData->addString(MESSAGE_TYPE, MESSAGE_TYPE_ACK);
			
			if (i < 1) {
				message = message.substring(MESSAGE_TYPE_DATA.length() + 5, message.length());
			}
			
			//Serial.println("ack");
		}
	
	}
	
	return messageJData;

}