// insert the following in main

	char command[] = "AT\r\n";
	char expected[] ="OK";
	ESP_init();
	ESP_WIFIMode(ESP_MODE_BOTH_STA);

	if(ESP_connectAccessPoint("your ssid", "your pass"))
		STM_EVAL_LEDOn(LED3);

	//ESP_sendRequest(command, expected);
