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


#ifndef JACK_H
#define JACK_H


#include <Arduino.h>
#include <HashMap.h>
#include <Wrappers.h>
#include "JData.h"
#include "JDataWrappers.h"
#include "JTransmissionMethod.h"


//---CONSTANTI---

//Jack
#define JK_MESSAGE_TYPE "message_type" //key tipo messaggio
#define JK_MESSAGE_DATA "message_data" //messaggio dati
		
#define JK_MESSAGE_ID "id" //id messaggio
		
#define JK_MESSAGE_TYPE_ACK "ack" //tipo ack
#define JK_MESSAGE_TYPE_DATA "values" //tipo dati


//---JACK---
//classe Jack per il protocollo
class Jack {
		
	public:
	
		Jack(JTransmissionMethod *mmJTM, void (*onReceive) (JData *), void (*onReceiveAck) (JData *), long (* getTimestamp) ()); //costruttore con mmJTM e funzione onRceive e OnReceiveAck
		
		Jack(JTransmissionMethod *mmJTM, void (*onReceive) (JData *), void (*onReceiveAck) (JData *), long (* getTimestamp) (), long timeBeforeResend); //tempo per il reinvio
		
		~Jack(); //distruttore
		
		
		void start(); //avvia il polling
		
		void stop(); //stoppa il polling
		
		void flushBufferSend(); //cancella i buffer contenente i messaggi da inviare
		
		void send(JData * message); //invia il messaggio
		
		void loop(); //luppa per simulare il thread
		
		//void remove(JData * message); //elimina un messaggio cancellato

	private:
	
		long TIME_BEFORE_RESEND; //tempo reinvio ms
		int SEND_ONE_TIME; //inviare i pacchetti una volta sola
		
		static const int TIMER_POLLING = 100;
		
		static String MESSAGE_TYPE; //key tipo messaggio
		static String MESSAGE_DATA; //messaggio dati
		
		static String MESSAGE_ID; //id messaggio
		
		static String MESSAGE_TYPE_ACK; //tipo ack
		static String MESSAGE_TYPE_DATA; //tipo dati
		
		static String MESSAGE_BOOLEAN_TRUE; //simbolo invio boolean true
		static String MESSAGE_BOOLEAN_FALSE;//simbolo invio boolean falso
		
		
		JTransmissionMethod *mmJTM; //contiene il metodo di trasmissione da usare
		
		HashMap<long, String> *sendMessageBuffer; //buffer per i messaggi da inviare
		HashMap<long, long> *sendMessageTimer; //buffer per i timer per i mex da inviare
		
		HashMap<long, JData *> *sendMessageBufferJData; //buffer contenente il messaggi das inviare nel formato JData
		
		HashMap<long, String> *sendAckBuffer; //buffer degli ack da inviare
		
		HashMap<long, long> *idMessageReceived; //buffer contiene gli id dei messaggi già ricevuti
		
		int stopPolling; //booloean che indica se stoppare il polling

		void (*onReceive) (JData *); //puntatore a funzione OnReceive
		void (*onReceiveAck) (JData *); //puntatore a funzione OnReceiveAck
		
		long (*getTimestamp) (); //puntatore a funzione per ottenere il timestamp in long
		
		void getMessagePollingFunction(); //funzione che sostituisce il thread per il get dei messaggi
		void sendMessagePollingFunction(); //" " " per inviare i messaggi
		
		void execute(String message); //funzione che gestisce il protocollo
		
		int checkMessageAlreadyReceived(JData * message); //verifica se il messaggio è già stato ricevuto
		
		int validate(String message); //verifica se il messaggio è conforme al protocollo
		
		JData *getJDataMessage(String message); //preleva i dati dal messaggio e crea il messaggio nel formato JData
		
		void sendAck(JData *message); //invia l'ack di conferma
		
		void checkAck(JData *message); //controlla l'ack
		
};


#endif //JACK_H