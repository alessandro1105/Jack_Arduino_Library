/*

TIPOLOGIA MESSAGGI JACK

	
	//MESSAGGIO DATI
	{
		"id": 123,
		"type": "data",
		"val": { //NESTED OBJECT
					"TIMESTAMP": timestamp,
					"GSR":	gsr,
					"TEMPERATURE": temperature
		}
	}

	//MESSAGGIO ACK
	{
		"id": 123,
		"type": "ack",
	}

	Payload è dentro val:
*/




JsonVariant get(char *key) {

	return _message['val'][key];

}


//tanti add in overload
void add(char *key, JsonVariant value) {

	//alla prima chiamata creiamo l'oggetto nested "val"

	//inserisco il valore
	_message['val'][value];
}