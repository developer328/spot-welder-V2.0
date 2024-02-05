#ifndef FUNCS_H
#define FUNCS_H

#define HEAD_FONT u8g2_font_9x15B_tf //u8g2_font_7x13B_tf
#define PAGE_FONT u8g2_font_7x13B_tf

u8g2_t u8g2;

void io_setup(void){
	DDRD |= (0<<2) | (0<<3) | (0<<4) | (0<<5); //pin 2,3,5,6
	DDRB |= (1<<1); //pin9
}

void int_setup(void){
	EICRA |= (1<<0) | (1<<1) | (0<<2) | (1<<3); //ISC00=1,ISC01=1(rising),ISC10=1,ISC11=1(falling)
	EIMSK |= (1<<1) | (1<<0); //enable INT0(pin2) and INT1(pin3)
}

void pcint_setup(void){
	PCICR |= (1<<2); //PCIE2(PCINT23_14)
	PCMSK2 |= (1<<5) | (1<<6); //PCINT21, PCINT22
}


void pcint_deactive(void){
	PCICR &= (0<<2); //PCIE2(PCINT23_14)
	PCMSK2 &= (0<<5) & (0<<6); //PCINT21, PCINT22
}

void timer1_setup(void){
	TCCR1A = 0;
	TCCR1B = (1<<3) | (1<<2);//WGM12(CTC-OCR1A)
	//TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = 63;
	//OCR1B = 0;
	TIMSK1 = (1<<1);//ocr1a
	//GTCCR = 0;
}

uint16_t adc_read(uint8_t pin){
	uint16_t adval = 0;
	
	ADMUX |= (0<<7) | (1<<6) | pin; //1.1REF
	ADCSRA |= (1<<7) | (1<<6) | (1<<0); //ADEN, ADSRA,  presc=2
	
	while(ADCSRA & (1<<6));

	adval = ADCL | (ADCH << 8);
	
	ADMUX &= 0;

	return adval;
}

uint16_t temp_convert(uint16_t ADC_val)
{
  float Temp;
  //Temp = log(10000.0 / (1023.0 / ADC_val - 1));
  Temp = log(10000.0 * ((1024.0 / ADC_val) - 1)); // for pull-up configuration
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
  Temp = Temp - 273.15; // Convert Kelvin to Celsius
  // Temp = (Temp * 9.0) / 5.0 + 32.0; // Convert Celsius to Fahrenheit
  _delay_ms(1);
  return (uint16_t)Temp;
}

int range(uint8_t loop, int min, int max, int value){

	if(value > max){
		value = (!loop) ? max : min;
	}else if(value < min){	
		value = (!loop) ? min : max;
	}
	return value;
}


void weld_page(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_fub42_tr);//u8g2_font_7Segments_26x42_mn);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	sprintf(R_count_str, "%d", R_count);		
	u8g2_DrawStr(&u8g2, 10, 20, R_count_str);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);	
	u8g2_DrawStr(&u8g2, 8, 0, "Miliseconds:");
	u8g2_SendBuffer(&u8g2);
}

void main_menu(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	sprintf(R_count_str, "MENU: %d/5", R_count);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 20, 0, R_count_str);
	u8g2_SetFont(&u8g2, PAGE_FONT);

	uint8_t equal = 0;
	for(uint8_t i = 0; i < 4; i++){
		uint8_t item = initial_item+i;
		uint8_t item_posY = (i*10)+17;
		uint8_t cursor_posY = item_posY+1;

		if((menu_last_cnt-initial_item) == i){
			u8g2_SetDrawColor(&u8g2, 1);
			u8g2_DrawBox(&u8g2, 0, cursor_posY, 127, 11);
			u8g2_SetDrawColor(&u8g2, 0);
			equal = 1;
		}else{
			u8g2_SetDrawColor(&u8g2, 1);
		}
		u8g2_DrawStr(&u8g2, SCREEN_XA, item_posY, 
				main_items[item]);
	}
	if(!equal){
		initial_item++;
		if(initial_item > 2){
			initial_item = 0;
		}
	}
	u8g2_SendBuffer(&u8g2);
}

void jump_page(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	if(jump_last_cnt > 0){
		uint8_t X_val = 6+(42*jump_last_cnt)-42;
		//u8g2_DrawBox(&u8g2, X_val, 16, 35, 2);
		//u8g2_DrawBox(&u8g2, X_val, 63, 35, 2);
		u8g2_DrawFrame(&u8g2, X_val, 16, 38, 48);
		if(int1_val > 0){
			//uint8_t X_val = 9+(42*jump_last_cnt)-42;
			u8g2_DrawBox(&u8g2, X_val, 16, 38, 48);
		}
	}
	u8g2_DrawStr(&u8g2, 30, 0, "Jump ms");
	u8g2_SetFont(&u8g2, u8g2_font_fub42_tr); //u8g2_font_fub35_tn);
	u8g2_SetDrawColor(&u8g2, hundred_cl);
	sprintf(R_count_str, "%d", hundred);	
	u8g2_DrawStr(&u8g2, 8, 18, R_count_str);
	u8g2_SetDrawColor(&u8g2, ten_cl);
	sprintf(R_count_str, "%d", ten);	
	u8g2_DrawStr(&u8g2, 50, 18, R_count_str);	
	u8g2_SetDrawColor(&u8g2, unit_cl);
	sprintf(R_count_str, "%d", unit);	
	u8g2_DrawStr(&u8g2, 92, 18, R_count_str);
	u8g2_SendBuffer(&u8g2);
}

void system_page(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 29, 0, "System:");
	u8g2_SetFont(&u8g2, PAGE_FONT);
	u8g2_DrawStr(&u8g2, 5, 16, "MCU: ATmega328p");
	u8g2_DrawStr(&u8g2, 5, 30, "Relay: SSR-40DA");
	u8g2_SendBuffer(&u8g2);	
}

void shutdown_page(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 5, 0, "Save&shutdown");
	u8g2_SendBuffer(&u8g2);	
}

void info_page(void){
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 15, 0, "Information");
	u8g2_SetFont(&u8g2, PAGE_FONT);
	u8g2_DrawStr(&u8g2, 5, 16, "Github:");
	u8g2_DrawStr(&u8g2, 5, 26, "developer328");
	u8g2_DrawStr(&u8g2, 5, 40, "Youtube:");
	u8g2_DrawStr(&u8g2, 5, 50, "dev328p");
	u8g2_SendBuffer(&u8g2);	
}

void temps_page(void){
	char temp_str[20];
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, HEAD_FONT);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 5, 0, "Temperatures");
	u8g2_SetFont(&u8g2, PAGE_FONT);

	sprintf(temp_str, "Transformer: %dC", temp_convert(adc_read(0)));
	u8g2_DrawStr(&u8g2, 5, 16, temp_str);
	
	sprintf(temp_str, "Relay: %dC", temp_convert(adc_read(1)));
	u8g2_DrawStr(&u8g2, 5, 32, temp_str);
	
//	sprintf(temp_str, "MCU: %dC", interrnal_temp());
//	u8g2_DrawStr(&u8g2, 5, 48, temp_str);
	u8g2_SendBuffer(&u8g2);	
}

void cut_off(void){
	
}

void save_on_eeprom(void){

}

void shut_down(void){

}


void menu_pages(void){
	switch(page){
		case 0:	
			weld_mode = 1;
		break;
		case 1:
			hundred_cl = 1;
			ten_cl = 1;
			unit_cl = 1;

			hundred = (weld_last_cnt % 1000)/100;
			ten = (weld_last_cnt % 100)/10;
			unit = (weld_last_cnt % 10);

			if(int1_val == 1){
				R_count = range(0, 0, 9, R_count);
				if(jump_last_cnt == 1){
					if(once_val){
						R_count = hundred;
						once_val = 0;
					}
					hundred = R_count;
					hundred_cl = 0;
				}else if(jump_last_cnt == 2){
					if(once_val){
						R_count = ten;
						once_val = 0;
					}
					ten = R_count; 
					ten_cl = 0;
				}else if(jump_last_cnt == 3){
					if(once_val){
						R_count = unit;
						once_val = 0;
					}
					unit = R_count;
					unit_cl = 0;
				}
				weld_last_cnt = ((hundred*100)+(ten*10)+(unit));
				once_sel = 1;
			}else if(int1_val == 0){
				if(once_sel){
					R_count = jump_last_cnt;
					once_sel = 0;
				}
				once_val = 1;
				R_count = range(0, 0, 3, R_count);
				jump_last_cnt = R_count;
			}else{
				int1_val = 0;
			}
			jump_page();
		break;
		case 2:
			system_page();
		break;
		case 3:
			info_page();
		break;
		case 4:
			shutdown_page();//fix
		break;
		case 5:
			temps_page();
		break;
		case 6:
			R_count = menu_last_cnt;
			RON_mode = 1;
			main_menu();
			menu_last_cnt = range(1, 0, 5, R_count);
		break;
		default:
			page = 6;
		break;
	}
}


#endif
