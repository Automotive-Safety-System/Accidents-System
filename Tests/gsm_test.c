usart2_init();


/* if event has occur */

// telephone number

char x[]="87564544534";

send_command_to_init_gsm();

//send telephone number

send_sms(x);

// send telephone number

gsm_call(x);



// to turn on gsm module

// call gsm_os_init from main
