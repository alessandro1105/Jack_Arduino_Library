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


//---CONSTANTI---

//JData Wrapper
#define WR_JDATA 4 //deve essere superiore a 3


//---JDATA ENUM---
typedef enum JDType {
	JD_LONG,
	JD_DOUBLE,
	JD_BOOLEAN,
	JD_STRING,
	JD_JDATA
};


//---JTRANSMISSION METHOD---
class JTransmissionMethod {

	public:
		
		virtual int receive(char *buffer, int size); //deve restituire il messaggio da passare a Jack
		virtual void send(char *message, int length); //invia il messaggio
		virtual int available(); //restituisce true se ci sono dati da ricevere nel buffer

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


// //---JACK---
// //classe Jack per il protocollo
// class Jack {
		
// 	public:
	
// 		Jack(JTrasmissionMethod *mmJTM, void (*onReceive) (JData *), void (*onReceiveAck) (JData *), long (* getTimestamp) ()); //costruttore con mmJTM e funzione onRceive e OnReceiveAck
		
// 		Jack(JTrasmissionMethod *mmJTM, void (*onReceive) (JData *), void (*onReceiveAck) (JData *), long (* getTimestamp) (), long timeBeforeResend); //tempo per il reinvio
		
// 		//Jack(JTrasmissionMethod * mmJTM, void (* onReceive) (JData *), void (* onReceiveAck) (JData *), long (* getTimestamp) (), int sendOneTime); //indica se effettuare il reinvio dei mex se non confermati
		
		
// 		~Jack(); //distruttore
		
		
// 		void start(); //avvia il polling
		
// 		void stop(); //stoppa il polling
		
// 		void flushBufferSend(); //cancella i buffer contenente i messaggi da inviare
		
// 		void send(JData * message); //invia il messaggio
		
// 		void loop(); //luppa per simulare il thread
		
// 		//void remove(JData * message); //elimina un messaggio cancellato

// 	private:
	
// 		long TIME_BEFORE_RESEND; //tempo reinvio ms
// 		int SEND_ONE_TIME; //inviare i pacchetti una volta sola
		
// 		static const int TIMER_POLLING = 100;
		
// 		static String MESSAGE_TYPE; //key tipo messaggio
// 		static String MESSAGE_DATA; //messaggio dati
		
// 		static String MESSAGE_ID; //id messaggio
		
// 		static String MESSAGE_TYPE_ACK; //tipo ack
// 		static String MESSAGE_TYPE_DATA; //tipo dati
		
// 		static String MESSAGE_BOOLEAN_TRUE; //simbolo invio boolean true
// 		static String MESSAGE_BOOLEAN_FALSE;//simbolo invio boolean falso
		
		
// 		JTrasmissionMethod *mmJTM; //contiene il metodo di trasmissione da usare
		
// 		HashMap<long, String> *sendMessageBuffer; //buffer per i messaggi da inviare
// 		HashMap<long, long> *sendMessageTimer; //buffer per i timer per i mex da inviare
		
// 		HashMap<long, JData *> *sendMessageBufferJData; //buffer contenente il messaggi das inviare nel formato JData
		
// 		HashMap<long, String> *sendAckBuffer; //buffer degli ack da inviare
		
// 		HashMap<long, long> *idMessageReceived; //buffer contiene gli id dei messaggi già ricevuti
		
// 		int stopPolling; //booloean che indica se stoppare il polling

// 		void (*onReceive) (JData *); //puntatore a funzione OnReceive
// 		void (*onReceiveAck) (JData *); //puntatore a funzione OnReceiveAck
		
// 		long (*getTimestamp) (); //puntatore a funzione per ottenere il timestamp in long
		
// 		void getMessagePollingFunction(); //funzione che sostituisce il thread per il get dei messaggi
// 		void sendMessagePollingFunction(); //" " " per inviare i messaggi
		
// 		void execute(String message); //funzione che gestisce il protocollo
		
// 		int checkMessageAlreadyReceived(JData * message); //verifica se il messaggio è già stato ricevuto
		
// 		int validate(String message); //verifica se il messaggio è conforme al protocollo
		
// 		JData *getJDataMessage(String message); //preleva i dati dal messaggio e crea il messaggio nel formato JData
		
// 		void sendAck(JData *message); //invia l'ack di conferma
		
// 		void checkAck(JData *message); //controlla l'ack
		
// };


#endif //JACK_H