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
Jack::Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), void (* onReceiveAck) (JData *), long (* getTimestamp) ()) { //costruttore con mmJTM e funzione onRceive e OnReceiveAck
	
	//valori di default
	TIME_BEFORE_RESEND = 1000; //tempo reinvio ms
	
	SEND_ONE_TIME = 0; //inviare i pacchetti una volta sola
	
	stopPolling = 1;
	
	this->mmJTM = mmJTM;
	
	//creo buffer
	
	sendMessageBuffer = new HashMap<long, String>();
	sendMessageTimer = new HashMap<long, long>();
	sendMessageBufferJData = new HashMap<long, JData *>();
	sendAckBuffer = new HashMap<long, String>();
	
	idMessageReceived = new HashMap<long, long>();
	
	
	//imposto le funzioni
	
	this->onReceive = onReceive;
	
	this->onReceiveAck = onReceiveAck;
	
	this->getTimestamp = getTimestamp;
	
}

Jack::Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), void (* onReceiveAck) (JData *), long (* getTimestamp) (), long timeBeforeResend) { //tempo per il reinvio
	
	
	//valori di default
	TIME_BEFORE_RESEND = timeBeforeResend; //tempo reinvio ms
	
	SEND_ONE_TIME = 0; //inviare i pacchetti una volta sola
	
	stopPolling = 1;
	
	this->mmJTM = mmJTM;
	
	//creo buffer
	
	sendMessageBuffer = new HashMap<long, String>();
	sendMessageTimer = new HashMap<long, long>();
	sendMessageBufferJData = new HashMap<long, JData *>();
	sendAckBuffer = new HashMap<long, String>();
	
	idMessageReceived = new HashMap<long, long>();
	
	
	//imposto le funzioni
	
	this->onReceive = onReceive;
	
	this->onReceiveAck = onReceiveAck;
	
	this->getTimestamp = getTimestamp;
	
}
/*
Jack::Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), void (* onReceiveAck) (JData *), long (* getTimestamp) (), int sendOneTime) { //indica se effettuare il reinvio dei mex se non confermati
	
	//valori di default
	TIME_BEFORE_RESEND = 1000; //tempo reinvio ms
	
	SEND_ONE_TIME = sendOneTime;
	
	stopPolling = 1;
	
	this->mmJTM = mmJTM;
	
	//creo buffer
	
	sendMessageBuffer = new HashMap<long, String>();
	sendMessageTimer = new HashMap<long, long>();
	sendMessageBufferJData = new HashMap<long, JData *>();
	sendAckBuffer = new HashMap<long, String>();
	
	idMessageReceived = new HashMap<long, long>();
	
	
	//imposto le funzioni
	
	this->onReceive = onReceive;
	
	this->onReceiveAck = onReceiveAck;
	
	this->getTimestamp = getTimestamp;
}

*/

Jack::~Jack() {

	delete sendMessageBuffer; //buffer per i messaggi da inviare
	delete sendMessageTimer; //buffer per i timer per i mex da inviare
	delete sendMessageBufferJData; //buffer contenente il messaggi das inviare nel formato JData
		
	delete sendAckBuffer; //buffer degli ack da inviare
		
	delete idMessageReceived; //buffer contiene gli id dei messaggi già ricevuti
}


void Jack::start() { //avvia il polling

	stopPolling = 0;
}

void Jack::stop() { //stoppa il polling

	stopPolling = 1;
}
		

void Jack::flushBufferSend() { //cancella i buffer contenente i messaggi da inviare		
		
	sendMessageBuffer = new HashMap<long, String>();
	sendMessageBufferJData = new HashMap<long, JData *>();
}

void Jack::execute(String message) { //funzione che gestisce il protocollo
			
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
	
	/*conferma test memoria*/
			
	//Serial.print("memoria dopo conferma: ");
	//Serial.println(freeMemory());
			
	/*fine conferma test memoria*/
		
}

int Jack::checkMessageAlreadyReceived(JData * message) { //verifica se il messaggio è già stato ricevuto
		
	if (!message->containsKey(MESSAGE_ID)) { //validazione veloce
		return 1;
	}
	
	sendAck(message);
	
	if (!idMessageReceived->containsKey(message->getLong(MESSAGE_ID))) {
		
		idMessageReceived->put(message->getLong(MESSAGE_ID), 0); //da ottimizzare con una lista o un vettore
		
		return 0;
		
	} else {
		return 1;
	}
		
}

int Jack::validate(String message) { //verifica se il messaggio è conforme al protocollo
		
	//da implementare
		
	return 1;
		
}

void Jack::sendAck(JData * message) { //invia l'ack di conferma

	String messageString = "{\"";
	messageString += MESSAGE_ID;
	messageString += "\":";
	messageString += message->getLong(MESSAGE_ID);
	messageString += ",\"";
	messageString += MESSAGE_TYPE_ACK;
	messageString += "\":1}";
	
	sendAckBuffer->put(message->getLong(MESSAGE_ID), messageString);
}

void Jack::checkAck(JData * message) { //controlla l'ack
	
	long id = message->getLong(MESSAGE_ID);
	
	if (sendMessageBufferJData->length() > 0) {
		
		if (sendMessageBufferJData->containsKey(id)) {
			
			if (sendMessageBuffer->containsKey(id)) {
				sendMessageBuffer->remove(id);
			}
			
			if (sendMessageTimer->containsKey(id)) { //rimuovo il timer se presente3
				sendMessageTimer->remove(id);
			}
			
			Serial.println("message Confirmed");
			//Serial.println("CONFIRM RICEVUTO trigger");
			(* onReceiveAck) (sendMessageBufferJData->getValue(id));
			
			//Serial.println("elimino JData messaggio inviato");
			delete sendMessageBufferJData->getValue(id);
			
			sendMessageBufferJData->remove(id);			
			
		}
		
	}
		
}

void Jack::loop() { //luppa per simulare il thread

	if (mmJTM->available()) {
		getMessagePollingFunction();
	}
	
	sendMessagePollingFunction();

}

void Jack::getMessagePollingFunction() { //funzione che sostituisce il thread per il get dei messaggi

	if (!stopPolling) {
		
		String message = mmJTM->receive();
		
		if (message.length() > 0) {
			
			Serial.println("message received: " + message);
			
			execute(message);
			
		}
		
	}
	
}


void Jack::sendMessagePollingFunction() { //" " " per inviare i messaggi

	if (!stopPolling) {
	
		if (sendAckBuffer->moveToFirst()) { //invio ack
		
			do {
			
				mmJTM->send(sendAckBuffer->getValue());
				
				sendAckBuffer->remove();
			
			} while (sendAckBuffer->moveToNext());
			
		}
		
		if (sendMessageBuffer->moveToFirst()) { //invio messaggi
			
			do {
				
				long key = sendMessageBuffer->getKey(); //prelevo la chiave (id)
				
				if (sendMessageTimer->containsKey(key)) { //controllo se il messaggio è già stato inviato (presenza del buffer)
				
					if ((millis() - sendMessageTimer->getValue(key)) > TIME_BEFORE_RESEND) { //controllo se è scaduto il tempo di attesa prima di reinviare il messaggio
						
						mmJTM->send(sendMessageBuffer->getValue()); //invio il messaggio
						
						
						//Serial.println("Elimino timer resend");
						sendMessageTimer->remove(key);
						//Serial.println("Eliminato");
						
						sendMessageTimer->put(key, millis());
						
					}
				
				
				} else { //messaggio da inviare per la prima volta
					//Serial.println("First send");
					mmJTM->send(sendMessageBuffer->getValue()); //invio il messaggio
					
					if (!SEND_ONE_TIME) {//controllo se non è da inviare una volta sola
						
						sendMessageTimer->put(key, millis());
						
					} else { //messaggio da inviare una sola volta
					
						sendMessageBuffer->remove(key);
						
					}
				
				}
			
			} while (sendMessageBuffer->moveToNext());
			
		
		}
	}

}


void Jack::send(JData * message) { //invia il messaggio
	
	long id = (* getTimestamp) ();
	
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

JData * Jack::getJDataMessage(String message) { //preleva i dati dal messaggio e crea il messaggio nel formato JData
	
	//Serial.print("JDATAGET: ");
	//Serial.println(freeMemory());
	
	
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